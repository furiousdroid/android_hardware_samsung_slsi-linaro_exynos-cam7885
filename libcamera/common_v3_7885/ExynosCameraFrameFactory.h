/*
**
** Copyright 2013, Samsung Electronics Co. LTD
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

#ifndef EXYNOS_CAMERA_FRAME_FACTORY_H
#define EXYNOS_CAMERA_FRAME_FACTORY_H

#include "ExynosCameraFrame.h"

#include "ExynosCameraPipe.h"
#include "ExynosCameraMCPipe.h"
#include "ExynosCameraPipeFlite.h"
#include "ExynosCameraPipeGSC.h"
#include "ExynosCameraPipeJpeg.h"
#ifdef USE_DUAL_CAMERA
#include "ExynosCameraPipeFusion.h"
#include "ExynosCameraPipeSync.h"
#include "ExynosCameraSyncFrameSelector.h"
#include "ExynosCameraDualFrameSelector.h"
#endif
#include "ExynosCameraFrameManager.h"
#include "ExynosCameraStreamMutex.h"

#include "ExynosCamera1Parameters.h"
#ifdef USE_VRA_GROUP
#include "ExynosCameraPipeVRAGroup.h"
#endif

namespace android {

#define SET_OUTPUT_DEVICE_BASIC_INFO(perframeInfo) \
    pipeInfo[nodeType].rectInfo = tempRect;\
    pipeInfo[nodeType].bufInfo.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;\
    pipeInfo[nodeType].bufInfo.memory = V4L2_CAMERA_MEMORY_TYPE;\
    pipeInfo[nodeType].perFrameNodeGroupInfo.perframeSupportNodeNum = CAPTURE_NODE_MAX;\
    pipeInfo[nodeType].perFrameNodeGroupInfo.perFrameLeaderInfo.perframeInfoIndex = perframeInfo;\
    pipeInfo[nodeType].perFrameNodeGroupInfo.perFrameLeaderInfo.perFrameNodeType = PERFRAME_NODE_TYPE_LEADER;\
    pipeInfo[nodeType].perFrameNodeGroupInfo.perFrameLeaderInfo.perFrameVideoID = (m_deviceInfo[INDEX(pipeId)].nodeNum[nodeType] - FIMC_IS_VIDEO_BAS_NUM);
#define SET_CAPTURE_DEVICE_BASIC_INFO() \
    pipeInfo[nodeType].rectInfo = tempRect;\
    pipeInfo[nodeType].bufInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;\
    pipeInfo[nodeType].bufInfo.memory = V4L2_CAMERA_MEMORY_TYPE;\
    pipeInfo[leaderNodeType].perFrameNodeGroupInfo.perFrameCaptureInfo[perFramePos].perFrameNodeType = PERFRAME_NODE_TYPE_CAPTURE;\
    pipeInfo[leaderNodeType].perFrameNodeGroupInfo.perFrameCaptureInfo[perFramePos].perFrameVideoID = (m_deviceInfo[INDEX(pipeId)].nodeNum[nodeType] - FIMC_IS_VIDEO_BAS_NUM);\

class ExynosCameraFrameFactory {
public:
    ExynosCameraFrameFactory()
    {
        m_init();
    }

    ExynosCameraFrameFactory(int cameraId, ExynosCamera1Parameters *param)
    {
        m_init();

        m_cameraId = cameraId;
        m_parameters = param;
        m_activityControl = m_parameters->getActivityControl();
    }

public:
    virtual ~ExynosCameraFrameFactory();

    virtual status_t        create(bool active = true) = 0;
    virtual status_t        precreate(void);
    virtual status_t        postcreate(void);
    virtual status_t        destroy(void);
    virtual bool            isCreated(void);
    virtual bool            isSwitching(void);

    virtual status_t        initPipes(void) = 0;
    virtual status_t        mapBuffers(void);
    virtual status_t        preparePipes(void) = 0;
    virtual status_t        startPipes(void) = 0;
    virtual status_t        startSensor3AAPipe(void) = 0;
    virtual status_t        stopPipes(void) = 0;
    virtual status_t        startInitialThreads(void) = 0;

    virtual status_t        fastenAeStable(int32_t numFrames, ExynosCameraBuffer *buffers);

    virtual status_t        setStopFlag(void);
    virtual status_t        stopPipe(uint32_t pipeId);
    virtual status_t        startThread(uint32_t pipeId);
    virtual status_t        stopThread(uint32_t pipeId);
    virtual status_t        stopThreadAndWait(uint32_t pipeId, int sleep = 5, int times = 40);
    virtual bool            checkPipeThreadRunning(uint32_t pipeId);
    virtual void            setThreadOneShotMode(uint32_t pipeId, bool enable);

    virtual status_t        setFrameManager(ExynosCameraFrameManager *manager);
    virtual status_t        getFrameManager(ExynosCameraFrameManager **manager);

    virtual status_t        setBufferManagerToPipe(ExynosCameraBufferManager **bufferManager, uint32_t pipeId);

    virtual status_t        setOutputFrameQToPipe(frame_queue_t *outputQ, uint32_t pipeId);
    virtual status_t        getOutputFrameQToPipe(frame_queue_t **outputQ, uint32_t pipeId);
    virtual status_t        setFrameDoneQToPipe(frame_queue_t *frameDoneQ, uint32_t pipeId);
    virtual status_t        getFrameDoneQToPipe(frame_queue_t **frameDoneQ, uint32_t pipeId);
    virtual status_t        getInputFrameQToPipe(frame_queue_t **inputFrameQ, uint32_t pipeId);

    virtual status_t        pushFrameToPipe(ExynosCameraFrameSP_dptr_t newFrame, uint32_t pipeId);

    virtual status_t        setParam(struct v4l2_streamparm *streamParam, uint32_t pipeId);
    virtual status_t        setControl(int cid, int value, uint32_t pipeId);
    virtual status_t        setControl(int cid, int value, uint32_t pipeId, enum NODE_TYPE nodeType);
    virtual status_t        getControl(int cid, int *value, uint32_t pipeId);
    virtual status_t        setExtControl(struct v4l2_ext_controls *ctrl, uint32_t pipeId);

    virtual void            setRequest(int pipeId, bool enable);
    virtual void            setRequestFLITE(bool enable);
    virtual void            setRequestBayer(bool enable);
    virtual void            setRequest3AC(bool enable);
    virtual void            setRequestISPC(bool enable);
    virtual void            setRequestISPP(bool enable);
    virtual void            setRequestSCC(bool enable);
    virtual void            setRequestDIS(bool enable);

    virtual status_t        getThreadState(int **threadState, uint32_t pipeId);
    virtual status_t        getThreadInterval(uint64_t **threadInterval, uint32_t pipeId);
    virtual status_t        getThreadRenew(int **threadRenew, uint32_t pipeId);
    virtual status_t        incThreadRenew(uint32_t pipeId);

    virtual int             getRunningFrameCount(uint32_t pipeId);

    virtual enum NODE_TYPE  getNodeType(uint32_t pipeId);

    virtual ExynosCameraFrameSP_sptr_t createNewFrameOnlyOnePipe(int pipeId, int frameCnt=-1, uint32_t frameType = 0, ExynosCameraFrameSP_sptr_t refFrame = NULL);
    virtual ExynosCameraFrameSP_sptr_t createNewFrameVideoOnly(void);
    virtual ExynosCameraFrameSP_sptr_t createNewFrame(ExynosCameraFrameSP_sptr_t refFrame = NULL) = 0;

    virtual status_t        switchSensorMode(void);
    virtual status_t        finishSwitchSensorMode(void);

    virtual void            dump(void);
    /* only for debugging */
    virtual status_t        dumpFimcIsInfo(uint32_t pipeId, bool bugOn);
#ifdef MONITOR_LOG_SYNC
    virtual status_t        syncLog(uint32_t pipeId, uint32_t syncId);
#endif
    virtual status_t        sensorStandby(__unused bool standby) { return NO_ERROR; };

protected:
    /* flite pipe setting */
    virtual status_t        m_initFlitePipe(int sensorW, int sensorH, uint32_t frameRate);

    virtual status_t        m_setupRequestFlags(void) = 0;
    virtual status_t        m_initFrameMetadata(ExynosCameraFrameSP_sptr_t frame);
    virtual status_t        m_initPipelines(ExynosCameraFrameSP_sptr_t frame);
    virtual status_t        m_checkPipeInfo(uint32_t srcPipeId, uint32_t dstPipeId);

    virtual int             m_getSensorId(unsigned int nodeNum, bool reprocessing);
    virtual int             m_getSensorId(unsigned int nodeNum, unsigned int connectionMode, bool flagLeader, bool reprocessing, int sensorScenario = SENSOR_SCENARIO_NORMAL);

    virtual void            m_initDeviceInfo(int pipeId);

    virtual status_t        m_transitState(frame_factory_state_t state);

    virtual status_t        m_setSensorSize(int pipeId, int sensorW, int sensorH);

private:
    void                    m_init(void);

protected:
    int                         m_cameraId;
    uint32_t                    m_frameCount;

    char                        m_name[EXYNOS_CAMERA_NAME_STR_SIZE];
    int32_t                     m_nodeNums[MAX_NUM_PIPES][MAX_NODE];
    int32_t                     m_sensorIds[MAX_NUM_PIPES][MAX_NODE];
    camera_device_info_t        m_deviceInfo[MAX_NUM_PIPES];

    ExynosCameraPipe           *m_pipes[MAX_NUM_PIPES];
    ExynosCamera1Parameters     *m_parameters;
    ExynosCameraFrameManager   *m_frameMgr;
    ExynosCameraActivityControl *m_activityControl;

    Mutex                       m_frameLock;

    uint32_t                    m_requestVC0;
    uint32_t                    m_requestVC1;
    uint32_t                    m_requestVC2;
    uint32_t                    m_requestVC3;
    uint32_t                    m_request3AC;
    uint32_t                    m_request3AP;
    uint32_t                    m_requestISP;
    uint32_t                    m_requestISPC;
    uint32_t                    m_requestISPP;
    uint32_t                    m_requestDIS;
    uint32_t                    m_requestSCC;
    uint32_t                    m_requestSCP;
    uint32_t                    m_requestMCSC0;
    uint32_t                    m_requestMCSC1;
    uint32_t                    m_requestMCSC2;
    uint32_t                    m_requestMCSC3;
    uint32_t                    m_requestMCSC4;
#ifdef USE_VRA_GROUP
    uint32_t                    m_requestVRA;
#endif
    uint32_t                    m_requestJPEG;
    uint32_t                    m_requestThumbnail;

    bool                        m_bypassDRC;
    bool                        m_bypassDIS;
    bool                        m_bypassDNR;
    bool                        m_bypassFD;

    bool                        m_flagFlite3aaOTF;
    bool                        m_flag3aaIspOTF;
    bool                        m_flagIspTpuOTF;
    bool                        m_flagIspMcscOTF;
    bool                        m_flagTpuMcscOTF;
    bool                        m_flagMcscVraOTF;

    bool                        m_supportReprocessing;
    bool                        m_flagReprocessing;
    bool                        m_supportPureBayerReprocessing;
    bool                        m_supportSCC;
    bool                        m_supportSingleChain;

    frame_factory_state_t       m_state;
    Mutex                       m_stateLock;

#ifdef USE_DUAL_CAMERA
    sync_type_t m_preSyncType;
    int         m_preZoom;
    Mutex       m_sensorStandbyLock;
#endif
private:
    struct camera2_shot_ext     *m_shot_ext;
};

}; /* namespace android */

#endif
