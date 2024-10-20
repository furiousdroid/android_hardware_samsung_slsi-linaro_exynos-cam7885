/*
**
** Copyright 2016, Samsung Electronics Co. LTD
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/* #define LOG_NDEBUG 0 */
#define LOG_TAG "ExynosCameraPipeVRA"
#include <cutils/log.h>

#include "ExynosCameraPipeVRA.h"
#include "ExynosCameraPipeGSC.h"

namespace android {

ExynosCameraPipeVRA::~ExynosCameraPipeVRA()
{
    this->destroy();
}

status_t ExynosCameraPipeVRA::create(int32_t *sensorIds)
{
    status_t ret = NO_ERROR;

    ret = ExynosCameraPipe::create(sensorIds);

    int32_t nodeNum[1] = {PREVIEW_GSC_NODE_NUM};
    m_gscPipe = (ExynosCameraPipe*)new ExynosCameraPipeGSC(m_cameraId, m_parameters, true, nodeNum);
    m_gscPipe->setPipeId(PIPE_GSC_VRA);
    m_gscPipe->setPipeName("PIPE_GSC_VRA");

    ret = m_gscPipe->create(sensorIds);
    if (ret != NO_ERROR) {
        CLOGE("Internal GSC Pipe creation fail!");
        return ret;
    }

    m_gscThread = ExynosCameraThreadFactory::createThread(this, &ExynosCameraPipeVRA::m_gscThreadFunc, "gscThread");

    m_gscFrameQ = new frame_queue_t;
    m_gscFrameDoneQ = new frame_queue_t;

    return ret;
}

status_t ExynosCameraPipeVRA::destroy(void)
{
    status_t ret = NO_ERROR;

    ret = ExynosCameraPipe::destroy();
    if (ret != NO_ERROR)
        CLOGE("Destroy fail!");

    ret = m_gscPipe->destroy();
    if (ret != NO_ERROR)
        CLOGE("Internal GSC Pipe detstroy fail!");

    return ret;
}

status_t ExynosCameraPipeVRA::stop(void)
{
    status_t ret = NO_ERROR;

    m_gscThread->requestExitAndWait();

    m_gscFrameQ->release();
    m_gscFrameDoneQ->release();

    ret = ExynosCameraPipe::stop();
    if (ret != NO_ERROR)
        CLOGE("Pipe stop fail!");

    ret = m_gscPipe->stop();
    if (ret != NO_ERROR)
        CLOGE("Internal GSC Pipe stop fail!");

    return ret;
}

status_t ExynosCameraPipeVRA::startThread(void)
{
    status_t ret = NO_ERROR;

    if (m_gscThread->isRunning() == false) {
        ret = m_gscThread->run(m_name);
        if (ret != NO_ERROR)
            CLOGE("Internal GSC Pipe startThread fail!");
        else
            CLOGI("Internal GSC Pipe startThread is succeed");
    } else {
        CLOGW("Internal GSC Thread is already running");
    }

    return ret;
}

status_t ExynosCameraPipeVRA::stopThread(void)
{
    status_t ret = NO_ERROR;

    m_gscThread->requestExit();
    m_gscFrameQ->sendCmd(WAKE_UP);

    ret = ExynosCameraPipe::stopThread();
    if (ret != NO_ERROR)
        CLOGE("Internal GSC Pipe stopThread fail!");

    return ret;
}

status_t ExynosCameraPipeVRA::getInputFrameQ(frame_queue_t **inputFrameQ)
{
    *inputFrameQ = m_gscFrameQ;

    if (*inputFrameQ == NULL)
        CLOGE("inputFrameQ is NULL");

    return NO_ERROR;
}

status_t ExynosCameraPipeVRA::m_runScaler(void)
{
    status_t ret = NO_ERROR;
    ExynosCameraFrameSP_sptr_t newFrame = NULL;
    ExynosCameraFrameSP_sptr_t doneFrame = NULL;
    ExynosCameraBuffer srcBuf, dstBuf;
    ExynosRect srcRect, dstRect;
    struct camera2_stream *streamMeta = NULL;
    uint32_t *mcscOutputCrop = NULL;
    struct camera2_shot_ext *shot_ext;

    int dstBufIndex = -2;
    int gscPipeId = PIPE_GSC_VRA;

    int waitCount = 0;
    int vraWidth = 0, vraHeight = 0;
    int32_t previewFormat = m_parameters->getHwPreviewFormat();
    m_parameters->getHwVraInputSize(&vraWidth, &vraHeight);

    while (m_gscFrameQ->getSizeOfProcessQ() > 1) {
        ret = m_gscFrameQ->popProcessQ(&newFrame);
        if (ret != NO_ERROR) {
            CLOGE("wait and pop fail, ret(%d)", ret);
            return ret;
        }

        ret = newFrame->setEntityState(getPipeId(), ENTITY_STATE_FRAME_DONE);
        if (ret != NO_ERROR) {
            CLOGE("set entity state fail, ret(%d)", ret);
            /* TODO: doing exception handling */
            return INVALID_OPERATION;
        }

        m_outputFrameQ->pushProcessQ(&newFrame);
    }

    ret = m_gscFrameQ->waitAndPopProcessQ(&newFrame);
    if (m_flagTryStop == true) {
        CLOGD("m_flagTryStop(%d)", m_flagTryStop);
        goto FUNC_EXIT;
    }
    if (ret != NO_ERROR) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
#ifdef USE_CAMERA2_API_SUPPORT
            /*
             * TIMEOUT log print
             * condition 1 : it is not reprocessing
             * condition 2 : if it is reprocessing, but m_timeLogCount is equals or lower than 0
             */
            if (!(m_parameters->isReprocessing() == true && m_timeLogCount <= 0)) {
                m_timeLogCount--;
#endif
            CLOGW("wait timeout");
            m_mainNode->dumpState();
#ifdef USE_CAMERA2_API_SUPPORT
            }
#endif
        } else {
            CLOGE("wait and pop fail, ret(%d)", ret);
            /* TODO: doing exception handling */
        }
        goto FUNC_EXIT;
    }

    if (newFrame == NULL) {
        CLOGE("newFrame is NULL");
        goto FUNC_EXIT;
    }

    /* Get scaler source buffer */
    ret = newFrame->getSrcBuffer(getPipeId(), &srcBuf);
    if (ret != NO_ERROR) {
        CLOGE("getSrcBuffer fail, ret(%d)", ret);
        /* TODO: doing exception handling */
        goto FUNC_EXIT;
    }

    shot_ext = (camera2_shot_ext*)srcBuf.addr[srcBuf.planeCount-1];
    entity_buffer_state_t srcBufferState;
    ret = newFrame->getSrcBufferState(getPipeId(), &srcBufferState);
    if (srcBuf.index < 0
        || shot_ext->fd_bypass == true
        || srcBufferState == ENTITY_BUFFER_STATE_ERROR) {
        if (m_mainThread->isRunning() == true)
            m_mainThread->requestExit();

        goto FUNC_EXIT;
    }

    ret = newFrame->getDstBuffer(getPipeId(), &dstBuf);
    if (ret != NO_ERROR) {
        CLOGE("getDstBuffer fail. pipeId(%d), frameCount(%d), ret(%d)",
                getPipeId(), newFrame->getFrameCount(), ret);
        goto FUNC_EXIT;
    }

    if (dstBuf.index < 0) {
        ret = m_bufferManager[OUTPUT_NODE]->getBuffer(&dstBufIndex, EXYNOS_CAMERA_BUFFER_POSITION_IN_HAL, &dstBuf);
        if (ret != NO_ERROR) {
            CLOGE("Buffer manager getBuffer fail, manager(%d), frameCount(%d), ret(%d)",
                    OUTPUT_NODE, newFrame->getFrameCount(), ret);
            goto FUNC_EXIT;
        }
    }

    /* Get size from metadata */
    streamMeta = (struct camera2_stream*)srcBuf.addr[srcBuf.planeCount-1];
    if (streamMeta == NULL) {
        CLOGE("srcBuf.addr is NULL, srcBuf.addr(0x%x)", srcBuf.addr[srcBuf.planeCount-1]);
        goto FUNC_EXIT;
    }

    /* Set size to GSC frame */
    mcscOutputCrop = streamMeta->output_crop_region;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = mcscOutputCrop[2];
    srcRect.h = mcscOutputCrop[3];
    srcRect.fullW = mcscOutputCrop[2];
    srcRect.fullH = mcscOutputCrop[3];
    srcRect.colorFormat = previewFormat;

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = vraWidth;
    dstRect.h = vraHeight;
    dstRect.fullW = vraWidth;
    dstRect.fullH = vraHeight;
    dstRect.colorFormat = m_parameters->getHwVraInputFormat();

    ret = newFrame->setSrcRect(gscPipeId, srcRect);
    ret = newFrame->setDstRect(gscPipeId, dstRect);

    /* set buffers */
    ret = newFrame->setSrcBuffer(gscPipeId, srcBuf);
    if (ret != NO_ERROR) {
        CLOGE("setSrcBuffer fail, pipeId(%d), ret(%d)", gscPipeId, ret);
        goto FUNC_EXIT;
    }

    ret = newFrame->setDstBuffer(gscPipeId, dstBuf);
    if (ret != NO_ERROR) {
        CLOGE("setDstBuffer fail, pipeId(%d), ret(%d)", gscPipeId, ret);
        goto FUNC_EXIT;
    }

    m_gscPipe->setOutputFrameQ(m_gscFrameDoneQ);
    m_gscPipe->pushFrame(newFrame);


    /* Wait and Pop frame from GSC output Q */
    CLOGV("wait GSC output");

    waitCount = 0;
    do {
        ret = m_gscFrameDoneQ->waitAndPopProcessQ(&doneFrame);
        waitCount++;

    } while (ret == TIMED_OUT && waitCount < 10);

    if (ret != NO_ERROR)
        CLOGW("GSC wait and pop error, ret(%d)", ret);

    if (doneFrame == NULL) {
        CLOGE("gscFrame is NULL");
        goto FUNC_EXIT;
    }

    if (newFrame->getFrameCount() != doneFrame->getFrameCount()) {
        CLOGW("FrameCount mismatch, Push(%d) Pop(%d)",
                newFrame->getFrameCount(), doneFrame->getFrameCount());
    }

    CLOGV("Get frame from GSC Pipe, frameCount(%d)", newFrame->getFrameCount());

    memcpy(dstBuf.addr[dstBuf.planeCount-1], srcBuf.addr[srcBuf.planeCount-1], dstBuf.size[dstBuf.planeCount-1]);

    camera2_node_group node_group_info;
    memset(&node_group_info, 0x0, sizeof(camera2_node_group));

    node_group_info.leader.request = 1;
    node_group_info.leader.input.cropRegion[0] = 0;
    node_group_info.leader.input.cropRegion[1] = 0;
    node_group_info.leader.input.cropRegion[2] = vraWidth;
    node_group_info.leader.input.cropRegion[3] = vraHeight;
    node_group_info.leader.output.cropRegion[0] = 0;
    node_group_info.leader.output.cropRegion[1] = 0;
    node_group_info.leader.output.cropRegion[2] = node_group_info.leader.input.cropRegion[2];
    node_group_info.leader.output.cropRegion[3] = node_group_info.leader.input.cropRegion[3];

    newFrame->storeNodeGroupInfo(&node_group_info, PERFRAME_INFO_VRA);

    ret = newFrame->setDstBuffer(getPipeId(), dstBuf);
    if (ret != NO_ERROR) {
        CLOGE("setDstBuffer fail, pipeId(%d), ret(%d)", gscPipeId, ret);
        goto FUNC_EXIT;
    }

    m_inputFrameQ->pushProcessQ(&newFrame);

    if (m_mainThread->isRunning() == false) {
        m_mainThread->run(m_name);
        CLOGI("startThread is succeed (%d)", getPipeId());
    }

    return NO_ERROR;

FUNC_EXIT:
    if (newFrame != NULL) {
        ret = newFrame->setEntityState(getPipeId(), ENTITY_STATE_FRAME_DONE);
        if (ret != NO_ERROR) {
            CLOGE("set entity state fail, ret(%d)", ret);
            /* TODO: doing exception handling */
            return INVALID_OPERATION;
        }

        m_outputFrameQ->pushProcessQ(&newFrame);
    }

    return NO_ERROR;
}

status_t ExynosCameraPipeVRA::m_putBuffer(void)
{
    status_t ret = NO_ERROR;
    ExynosCameraFrameSP_sptr_t newFrame = NULL;

    while (m_inputFrameQ->getSizeOfProcessQ() > 1) {
        ret = m_inputFrameQ->popProcessQ(&newFrame);
        if (ret != NO_ERROR) {
            CLOGE("wait and pop fail, ret(%d)", ret);
            return ret;
        }

        ret = newFrame->setEntityState(getPipeId(), ENTITY_STATE_FRAME_DONE);
        if (ret != NO_ERROR) {
            CLOGE("set entity state fail, ret(%d)", ret);
            /* TODO: doing exception handling */
            return INVALID_OPERATION;
        }

        m_outputFrameQ->pushProcessQ(&newFrame);
    }

    ret = ExynosCameraPipe::m_putBuffer();
    if (ret != NO_ERROR)
        CLOGE("m_putBuffer() fail!");

    return ret;
}

bool ExynosCameraPipeVRA::m_gscThreadFunc(void)
{
    status_t ret = NO_ERROR;

    if (m_flagTryStop == true) {
        usleep(5000);
        return true;
    }

    /*
     * Cycle is below
     * m_gscFrameQ -> m_runScaler()(GSC) ->
     * m_inputFrameQ -> m_putBuffer()(VRA) -> m_getBuffer()(VRA) ->
     * m_outputFrameQ
     */

    ret = m_runScaler();
    if (ret != NO_ERROR) {
        if (ret == TIMED_OUT)
            return true;

        CLOGE("m_runScaler fail, ret(%d)", ret);
        /* TODO: doing exception handling */
        return false;
    }

    return true;
}

bool ExynosCameraPipeVRA::m_mainThreadFunc(void)
{
    status_t ret = NO_ERROR;

    if (m_flagTryStop == true) {
        usleep(5000);
        return true;
    }

    ret = m_putBuffer();
    if (ret != NO_ERROR) {
        if (ret == TIMED_OUT)
            return true;

        CLOGE("m_putBuffer fail, ret(%d)", ret);
        /* TODO: doing exception handling */
        return false;
    }

    ret = m_getBuffer();
    if (ret != NO_ERROR) {
        if (ret == TIMED_OUT)
            return true;

        CLOGE("m_getBuffer fail, ret(%d)", ret);
        /* TODO: doing exception handling */
        return false;
    }

    return m_checkThreadLoop();
}

void ExynosCameraPipeVRA::m_init(void)
{
    m_gscFrameQ = NULL;
    m_gscFrameDoneQ = NULL;
    m_gscPipe = NULL;
}

}; /* namespace android */
