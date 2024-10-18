/*
 * Copyright (C) 2014, Samsung Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* #define LOG_NDEBUG 0 */
#define LOG_TAG "ExynosCameraRequestManager"

#include "ExynosCameraRequestManager.h"

namespace android {

ExynosCamera3RequestResult::ExynosCamera3RequestResult(uint32_t key, uint32_t frameCount, EXYNOS_REQUEST_RESULT::TYPE type, camera3_capture_result *captureResult, camera3_notify_msg_t *notityMsg) : 
    ExynosCameraRequestResult::ExynosCameraRequestResult(key, frameCount, key, type){
{
    m_init();

    m_key = key;
    m_type = type;
    m_frameCount = frameCount;

    switch (type) {
    case EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY:
        if (notityMsg == NULL) {
            CLOGE2("CALLBACK_NOTIFY_ONLY frameCount(%u) notityMsg is NULL notityMsg(%p) ",
                frameCount, notityMsg);
        } else {
            memcpy(&m_notityMsg, notityMsg, sizeof(camera3_notify_msg_t));
        }
        break;

    case EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY:
        if (captureResult != NULL) {
            memcpy(&m_captureResult, captureResult, sizeof(camera3_capture_result_t));
        }
        break;

    case EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA:
        if (captureResult == NULL) {
            CLOGE2("CALLBACK_PARTIAL_3AA frameCount(%u) captureResult is NULL captureResult(%p) ",
                frameCount, captureResult);
        } else {
            memcpy(&m_captureResult, captureResult, sizeof(camera3_capture_result_t));
        }
        break;

    case EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT:
        if (captureResult == NULL) {
            CLOGE2("CALLBACK_ALL_RESULT frameCount(%u) captureResult is NULL captureResult(%p) ",
                frameCount, captureResult);
        } else {
            memcpy(&m_captureResult, captureResult, sizeof(camera3_capture_result_t));
        }
        break;

    case EXYNOS_REQUEST_RESULT::CALLBACK_INVALID:
    default:
        CLOGE2("type have INVALID value type(%d) frameCount(%u) ", type, frameCount);

        break;
    }
   }
}

ExynosCamera3RequestResult::~ExynosCamera3RequestResult()
{
    m_deinit();
}

status_t ExynosCamera3RequestResult::m_init()
{
    status_t ret = NO_ERROR;
    m_key = 0;
    m_type = EXYNOS_REQUEST_RESULT::CALLBACK_INVALID;
    m_frameCount = 0;
    memset(&m_captureResult, 0x00, sizeof(camera3_capture_result_t));
    memset(&m_notityMsg, 0x00, sizeof(camera3_notify_msg_t));

    m_streamBufferList.clear();

    return ret;
}

status_t ExynosCamera3RequestResult::m_deinit()
{
    status_t ret = NO_ERROR;
    m_key = 0;
    m_type = EXYNOS_REQUEST_RESULT::CALLBACK_INVALID;
    m_frameCount = 0;
    memset(&m_captureResult, 0x00, sizeof(camera3_capture_result_t));
    memset(&m_notityMsg, 0x00, sizeof(camera3_notify_msg_t));

    {
        Mutex::Autolock l(m_streamBufferListLock);
        StreamBufferList::iterator iter;
        camera3_stream_buffer_t *obj = NULL;
        for(iter = m_streamBufferList.begin(); iter != m_streamBufferList.end();)
        {
            obj = *iter;
            m_streamBufferList.erase(iter++);
            if (obj != NULL) {
                delete obj;
            }
        }
    }

    m_streamBufferList.clear();

    return ret;
}

uint32_t ExynosCamera3RequestResult::getKey()
{
    return m_key;
}

uint32_t ExynosCamera3RequestResult::getFrameCount()
{
    return m_frameCount;
}

EXYNOS_REQUEST_RESULT::TYPE ExynosCamera3RequestResult::getType()
{
    EXYNOS_REQUEST_RESULT::TYPE ret = EXYNOS_REQUEST_RESULT::CALLBACK_INVALID;
    switch (m_type) {
    case EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY:
    case EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY:
    case EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT:
    case EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA:
        ret = m_type;
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_INVALID:
    default:
        CLOGE2("getResultType type have INVALID value type(%d) key(%u) frameCount(%u) ",
            m_type, m_key, m_frameCount);
        break;
    }
    return ret;
}

status_t ExynosCamera3RequestResult::getCaptureResult(camera3_capture_result_t *captureResult)
{
    memcpy(captureResult, &m_captureResult, sizeof(camera3_capture_result_t));
    return OK;
}

status_t ExynosCamera3RequestResult::getNofityMsg(camera3_notify_msg_t *notifyResult)
{
    memcpy(notifyResult, &m_notityMsg, sizeof(camera3_notify_msg_t));
    return OK;
}

status_t ExynosCamera3RequestResult::pushStreamBuffer(camera3_stream_buffer_t *streamBuffer)
{
    status_t ret = NO_ERROR;
    if (streamBuffer == NULL){
        CLOGE2("pushStreamBuffer is failed streamBuffer == NULL streamBuffer(%p)", streamBuffer);
        ret = INVALID_OPERATION;
        return ret;
    }

    ret = m_pushBuffer(streamBuffer, &m_streamBufferList, &m_streamBufferListLock);
    if (ret < 0){
        CLOGE2("m_pushBuffer is failed ");
        ret = INVALID_OPERATION;
        return ret;
    }

    return ret;
}

status_t ExynosCamera3RequestResult::getStreamBuffer(camera3_stream_buffer_t *streamBuffer)
{
    status_t ret = NO_ERROR;
    if (streamBuffer == NULL){
        CLOGE2("getStreamBufferis failed streamBuffer == NULL streamBuffer(%p)", streamBuffer);
        ret = INVALID_OPERATION;
        return ret;
    }

    ret = m_getBuffer(streamBuffer, &m_streamBufferList, &m_streamBufferListLock);
    if (ret < 0){
        CLOGE2("m_getBufferis failed ");
        ret = INVALID_OPERATION;
        return ret;
    }

    return ret;
}

status_t ExynosCamera3RequestResult::popStreamBuffer(camera3_stream_buffer_t *streamBuffer)
{
    status_t ret = NO_ERROR;
    if (streamBuffer == NULL){
        CLOGE2("popStreamBuffer is failed streamBuffer == NULL streamBuffer(%p)", streamBuffer);
        ret = INVALID_OPERATION;
        return ret;
    }

    ret = m_popBuffer(streamBuffer, &m_streamBufferList, &m_streamBufferListLock);
    if (ret < 0){
        CLOGE2("m_popBuffer is failed ");
        ret = INVALID_OPERATION;
        return ret;
    }

    return ret;
}

int ExynosCamera3RequestResult::getNumOfStreamBuffer()
{
     return m_getNumOfBuffer(&m_streamBufferList, &m_streamBufferListLock);
}

status_t ExynosCamera3RequestResult::m_pushBuffer(camera3_stream_buffer_t *src, StreamBufferList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    camera3_stream_buffer_t *dst = new camera3_stream_buffer_t;
    memcpy(dst, src, sizeof(camera3_stream_buffer_t));

    lock->lock();
    list->push_back(dst);
    lock->unlock();
    return ret;
}

status_t ExynosCamera3RequestResult::m_getBuffer(camera3_stream_buffer_t *dst, StreamBufferList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    camera3_stream_buffer_t *src = NULL;

    lock->lock();
    if (list->size() > 0) {
        src = list->front();
        lock->unlock();
    } else {
        lock->unlock();
        CLOGE2("failed, size(%zu)", list->size());
        ret = INVALID_OPERATION;
        return ret;
    }

    memcpy(dst, src, sizeof(camera3_stream_buffer_t));
    return ret;
}

status_t ExynosCamera3RequestResult::m_popBuffer(camera3_stream_buffer_t *dst, StreamBufferList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    camera3_stream_buffer_t *src = NULL;

    lock->lock();
    if (list->size() > 0) {
        src = list->front();
        list->pop_front();
        lock->unlock();
    } else {
        lock->unlock();
        CLOGE2("failed, size(%zu)", list->size());
        ret = INVALID_OPERATION;
        return ret;
    }

    memcpy(dst, src, sizeof(camera3_stream_buffer_t));

    if (src != NULL) {
        delete src;
        src = NULL;
    }
    return ret;
}

int ExynosCamera3RequestResult::m_getNumOfBuffer(StreamBufferList *list, Mutex *lock)
{
    int count = 0;
    lock->lock();
    count = list->size();
    lock->unlock();
    return count;
}
#if 0
ExynosCameraCbRequest::ExynosCameraCbRequest(uint32_t frameCount)
{
    m_init();
    m_frameCount = frameCount;
}

ExynosCameraCbRequest::~ExynosCameraCbRequest()
{
    m_deinit();
}

uint32_t ExynosCameraCbRequest::getFrameCount()
{
    return m_frameCount;
}

status_t ExynosCameraCbRequest::pushRequest(ResultRequest result)
{
    status_t ret = NO_ERROR;
    ret = m_push(result, &m_callbackList, &m_callbackListLock);
    if (ret < 0) {
        CLOGE2("push request is failed, result frameCount(%u) type(%d)", result->getFrameCount(), result->getType());
    }

    return ret;
}

status_t ExynosCameraCbRequest::popRequest(EXYNOS_REQUEST_RESULT::TYPE reqType, ResultRequestList *resultList)
{
    status_t ret = NO_ERROR;

    resultList->clear();

    ret = m_pop(reqType, &m_callbackList, resultList, &m_callbackListLock);
    if (ret < 0) {
        CLOGE2("m_pop request is failed, request type(%d) resultSize(%d)", reqType, resultList->size());
    }

    return ret;
}

status_t ExynosCameraCbRequest::getRequest(EXYNOS_REQUEST_RESULT::TYPE reqType, ResultRequestList *resultList)
{
    status_t ret = NO_ERROR;
    ret = m_get(reqType, &m_callbackList, resultList, &m_callbackListLock);
    if (ret < 0) {
        CLOGE2("m_get request is failed, request type(%d) resultSize(%d)", reqType, resultList->size());
    }

    return ret;
}

status_t ExynosCameraCbRequest::setCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType, bool flag)
{
    status_t ret = NO_ERROR;
    ret = m_setCallbackDone(reqType, flag, &m_statusLock);
    if (ret < 0) {
        CLOGE2("m_get request is failed, request type(%d) ", reqType);
    }

    return ret;
}

bool ExynosCameraCbRequest::getCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType)
{
    bool ret = false;
    ret = m_getCallbackDone(reqType, &m_statusLock);
    return ret;
}

bool ExynosCameraCbRequest::isComplete()
{
    bool ret = false;
    bool notify = false;
    bool capture = false;

    notify = m_getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY, &m_statusLock);
    capture = m_getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT, &m_statusLock);

    if (notify == true && capture == true) {
        ret = true;
    }

    return ret;
}

status_t ExynosCameraCbRequest::m_init()
{
    status_t ret = NO_ERROR;

    m_callbackList.clear();
    for (int i = 0 ; i < EXYNOS_REQUEST_RESULT::CALLBACK_MAX ; i++) {
        m_status[i] = false;
    }
    return ret;
}

status_t ExynosCameraCbRequest::m_deinit()
{
    status_t ret = NO_ERROR;
    ResultRequestListIterator iter;
    ResultRequest result;

    if (m_callbackList.size() > 0) {
        CLOGE2("delete cb objects, but result size is not ZERO frameCount(%u) result size(%u)", m_frameCount, m_callbackList.size());
        for (iter = m_callbackList.begin(); iter != m_callbackList.end();) {
            result = *iter;
            CLOGE2("delete cb objects, frameCount(%u) type(%d)", result->getFrameCount(), result->getType());
            m_callbackList.erase(iter++);
            result = NULL;
        }
    }

    m_callbackList.clear();
    for (int i = 0 ; i < EXYNOS_REQUEST_RESULT::CALLBACK_MAX ; i++) {
        m_status[i] = false;
    }

    return ret;
}

status_t ExynosCameraCbRequest::m_push(ResultRequest result, ResultRequestList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    if (result->getFrameCount() != m_frameCount) {
        CLOGE2("m_push, check m_frame(%u) getFrameCount(%u)", m_frameCount, result->getFrameCount());
    }

    lock->lock();
    list->push_back(result);
    lock->unlock();
    return ret;
}

status_t ExynosCameraCbRequest::m_pop(EXYNOS_REQUEST_RESULT::TYPE reqType, ResultRequestList *list, ResultRequestList *resultList, Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestListIterator iter;

    ResultRequest obj;
    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin() ; iter != list->end() ;) {
            obj = *iter;
            if (obj->getType() == reqType) {
                resultList->push_back(obj);
                list->erase(iter++);
            } else {
                iter++;
            }
        }
    }

    lock->unlock();
    return ret;
}

status_t ExynosCameraCbRequest::m_get(EXYNOS_REQUEST_RESULT::TYPE reqType, ResultRequestList *list, ResultRequestList *resultList, Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestListIterator iter;

    ResultRequest obj;
    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin() ; iter != list->end() ;iter++) {
            obj = *iter;
            if (obj->getType() == reqType) {
                resultList->push_back(obj);
            }
        }
    } else {
        obj = NULL;
        ret = INVALID_OPERATION;
        CLOGE2("m_getCallbackResults failed, size is ZERO, reqType(%d) size(%d)", reqType, list->size());
    }

    lock->unlock();
    return ret;
}

status_t ExynosCameraCbRequest::m_setCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType, bool flag, Mutex *lock)
{
    status_t ret = NO_ERROR;
    if (reqType >= EXYNOS_REQUEST_RESULT::CALLBACK_MAX) {
        CLOGE2("m_setCallback failed, status erray out of bounded reqType(%d)", reqType);
        ret = INVALID_OPERATION;
        return ret;
    }

    lock->lock();
    m_status[reqType] = flag;
    lock->unlock();
    return ret;
}

bool ExynosCameraCbRequest::m_getCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType, Mutex *lock)
{
    bool ret = false;
    if (reqType >= EXYNOS_REQUEST_RESULT::CALLBACK_MAX) {
        CLOGE2("m_getCallback failed, status erray out of bounded reqType(%d)", reqType);
        return ret;
    }

    lock->lock();
    ret = m_status[reqType];
    lock->unlock();
    return ret;
}
#endif

ExynosCamera3Request::ExynosCamera3Request(camera3_capture_request_t* request, CameraMetadata previousMeta)
{
    ExynosCameraStream *stream = NULL;
    int streamId = -1;

    if (request == NULL) {
        ALOGE("ERR(%s[%d]):capture_request is NULL", __FUNCTION__, __LINE__);
        return;
    }

    m_init();

    m_request = new camera3_capture_request_t;
    memcpy(m_request, request, sizeof(camera3_capture_request_t));
    for (int i = 0; i < HAL_STREAM_ID_MAX; i++)
        m_streamIdList[i] = -1;

    /* Deep copy the input buffer object, because the Camera sevice can reuse it
       in successive request with different contents.
    */
    if(request->input_buffer != NULL) {
        CLOGD2("Allocating input buffer (%p)", request->input_buffer);
        m_request->input_buffer = new camera3_stream_buffer_t();
        memcpy(m_request->input_buffer, request->input_buffer, sizeof(camera3_stream_buffer_t));
    }

    m_key = m_request->frame_number;
    m_numOfOutputBuffers = request->num_output_buffers;
    m_isNeedInternalFrame = false;

    /* Deep copy the output buffer objects as well */
    if ((request->output_buffers != NULL) && (m_numOfOutputBuffers > 0)) {
        camera3_stream_buffer_t* newOutputBuffers = NULL;
        newOutputBuffers = new camera3_stream_buffer_t[m_numOfOutputBuffers];
        memcpy(newOutputBuffers, request->output_buffers, sizeof(camera3_stream_buffer_t) * m_numOfOutputBuffers);
        /* Nasty casting to assign a value to const pointer */
        *(camera3_stream_buffer_t**)(&m_request->output_buffers) = newOutputBuffers;

        for (int i = 0; i < m_numOfOutputBuffers; i++) {
            streamId = -1;
            stream = static_cast<ExynosCameraStream *>(request->output_buffers[i].stream->priv);
            stream->getID(&streamId);
            m_streamIdList[i] = streamId;
        }
    }

    if (request->settings != NULL) {
        m_serviceMeta = request->settings;
        m_resultMeta = request->settings;
    } else {
        CLOGV2("serviceMeta is NULL, use previousMeta");
        if (previousMeta.isEmpty()) {
            CLOGE2("previous meta is empty, ERROR ");
        } else {
            m_serviceMeta = previousMeta;
            m_resultMeta = previousMeta;
        }
    }

    if (m_serviceMeta.exists(ANDROID_CONTROL_CAPTURE_INTENT)) {
        m_captureIntent = m_resultMeta.find(ANDROID_CONTROL_CAPTURE_INTENT).data.u8[0];
        CLOGV2("ANDROID_CONTROL_CAPTURE_INTENT is (%d)", m_captureIntent);
    }

    CLOGV2("key(%d), request->frame_count(%d), num_output_buffers(%d)",
        m_key, request->frame_number, request->num_output_buffers);

/*    m_resultMeta = request->settings;*/
}

ExynosCamera3Request::~ExynosCamera3Request()
{
    m_deinit();
}

uint32_t ExynosCamera3Request::getKey()
{
    return m_key;
}

void ExynosCamera3Request::setFrameCount(uint32_t frameCount)
{
    m_frameCount = frameCount;
}

status_t ExynosCamera3Request::m_init()
{
    status_t ret = NO_ERROR;

    m_key = 0;
    m_request = NULL;
    m_requestId = 0;
    m_captureIntent = 0;
    m_frameCount = 0;
    m_serviceMeta.clear();
    memset(&m_serviceShot, 0x00, sizeof(struct camera2_shot_ext));
#ifdef USE_VRA_GROUP
    for (int i = 0; i < INTERFACE_TYPE_MAX; i++) {
        m_serviceShot.shot.uctl.scalerUd.mcsc_sub_blk_port[i] = MCSC_PORT_NONE;
    }
#endif
    m_resultMeta.clear();
    memset(&m_resultShot, 0x00, sizeof(struct camera2_shot_ext));
    memset(&m_prevShot, 0x00, sizeof(struct camera2_shot_ext));
    m_requestState = EXYNOS_REQUEST::SERVICE;
    m_factoryMap.clear();
    m_resultList.clear();
    m_numOfCompleteBuffers = 0;
    m_numOfcallbackBuffers = 0;
    m_numOfDuplicateBuffers = 0;
    m_pipelineDepth = 0;

    for (int i = 0 ; i < EXYNOS_REQUEST_RESULT::CALLBACK_MAX ; i++) {
        m_resultStatus[i] = false;
    }

    return ret;
}

status_t ExynosCamera3Request::m_deinit()
{
    status_t ret = NO_ERROR;

    if (m_request != NULL) {
        if (m_request->input_buffer != NULL) {
            delete m_request->input_buffer;
        }
        if (m_request->output_buffers != NULL) {
            delete[] m_request->output_buffers;
        }
        delete m_request;
        m_request = NULL;
    }

    m_frameCount = 0;
    m_serviceMeta = NULL;
    memset(&m_serviceShot, 0x00, sizeof(struct camera2_shot_ext));
    m_resultMeta = NULL;
    memset(&m_resultShot, 0x00, sizeof(struct camera2_shot_ext));
    memset(&m_prevShot, 0x00, sizeof(struct camera2_shot_ext));
    m_requestState = EXYNOS_REQUEST::SERVICE;
    m_resultList.clear();
    resetCompleteBufferCount();
    resetDuplicateBufferCount();

    for (int i = 0 ; i < EXYNOS_REQUEST_RESULT::CALLBACK_MAX ; i++) {
        m_resultStatus[i] = false;
    }

    return ret;
}


uint32_t ExynosCamera3Request::getFrameCount()
{
    return m_frameCount;
}

uint8_t ExynosCamera3Request::getCaptureIntent()
{
    return m_captureIntent;
}

camera3_capture_request_t* ExynosCamera3Request::getService()
{
    if (m_request == NULL)
    {
        CLOGE2("getService is NULL m_request(%p) ", m_request);
    }
    return m_request;
}

uint32_t ExynosCamera3Request::setServiceMeta(CameraMetadata request)
{
    Mutex::Autolock l(m_serviceMetaLock);
    status_t ret = NO_ERROR;
    m_serviceMeta = request;
    return ret;
}

CameraMetadata ExynosCamera3Request::getServiceMeta()
{
    Mutex::Autolock l(m_serviceMetaLock);
    return m_serviceMeta;
}

status_t ExynosCamera3Request::setServiceShot(struct camera2_shot_ext *metadata)
{
    Mutex::Autolock l(m_serviceShotLock);
    status_t ret = NO_ERROR;
    memcpy(&m_serviceShot, metadata, sizeof(struct camera2_shot_ext));

    return ret;
}

status_t ExynosCamera3Request::getServiceShot(struct camera2_shot_ext *metadata)
{
    Mutex::Autolock l(m_serviceShotLock);
    status_t ret = NO_ERROR;
    memcpy(metadata, &m_serviceShot, sizeof(struct camera2_shot_ext));

    return ret;
}


status_t ExynosCamera3Request::setResultMeta(CameraMetadata request)
{
    Mutex::Autolock l(m_resultMetaLock);
    status_t ret = NO_ERROR;
    m_resultMeta = request;
    return ret;
}

CameraMetadata ExynosCamera3Request::getResultMeta()
{
    Mutex::Autolock l(m_resultMetaLock);
    return m_resultMeta;
}


status_t ExynosCamera3Request::setResultShot(struct camera2_shot_ext *metadata)
{
    Mutex::Autolock l(m_resultShotLock);
    status_t ret = NO_ERROR;
    memcpy(&m_resultShot, metadata, sizeof(struct camera2_shot_ext));

    return ret;
}

status_t ExynosCamera3Request::getResultShot(struct camera2_shot_ext *metadata)
{
    Mutex::Autolock l(m_resultShotLock);
    status_t ret = NO_ERROR;
    memcpy(metadata, &m_resultShot, sizeof(struct camera2_shot_ext));

    return ret;
}

status_t ExynosCamera3Request::setPrevShot(struct camera2_shot_ext *metadata)
{
    Mutex::Autolock l(m_prevShotLock);
    status_t ret = NO_ERROR;
    if (metadata == NULL) {
        ret = INVALID_OPERATION;
        CLOGE2("setPrevShot metadata is NULL ret(%d) ", ret);
    } else {
        memcpy(&m_prevShot, metadata, sizeof(struct camera2_shot_ext));
    }

    return ret;
}

status_t ExynosCamera3Request::getPrevShot(struct camera2_shot_ext *metadata)
{
    Mutex::Autolock l(m_prevShotLock);
    status_t ret = NO_ERROR;
    if (metadata == NULL) {
        ret = INVALID_OPERATION;
        CLOGE2("getPrevShot metadata is NULL ret(%d) ", ret);
    } else {
        memcpy(metadata, &m_prevShot, sizeof(struct camera2_shot_ext));
    }

    return ret;
}

status_t ExynosCamera3Request::setRequestState(EXYNOS_REQUEST::STATE state)
{
    status_t ret = NO_ERROR;
    switch(state) {
    case EXYNOS_REQUEST::SERVICE:
    case EXYNOS_REQUEST::RUNNING:
        m_requestState = state;
        break;
    default:
        CLOGE2("setState is invalid newstate(%d) ", state);
        break;
    }

    return ret;
}

EXYNOS_REQUEST::STATE ExynosCamera3Request::getRequestState()
{
    EXYNOS_REQUEST::STATE ret = EXYNOS_REQUEST::INVALID;
    switch(m_requestState) {
    case EXYNOS_REQUEST::SERVICE:
    case EXYNOS_REQUEST::RUNNING:
        ret = m_requestState;
        break;
    default:
        CLOGE2("getState is invalid curstate(%d) ", m_requestState);
        break;
    }

    return ret;
}

uint32_t ExynosCamera3Request::getNumOfInputBuffer()
{
    uint32_t numOfInputBuffer = 0;
    if (m_request->input_buffer != NULL) {
        numOfInputBuffer = 1;
    }
    return numOfInputBuffer;
}

camera3_stream_buffer_t* ExynosCamera3Request::getInputBuffer()
{
    if (m_request == NULL){
        CLOGE2("getInputBuffer m_request is NULL m_request(%p) ", m_request);
        return NULL;
    }

    if (m_request->input_buffer == NULL){
        CLOGV2("getInputBuffer input_buffer is NULL m_request(%p) ", m_request->input_buffer);
    }

    return m_request->input_buffer;
}

status_t ExynosCamera3Request::setSensorTimestamp(uint64_t timestamp)
{
    Mutex::Autolock l(m_sensorTimeStampLock);
    m_resultShot.shot.udm.sensor.timeStampBoot = timestamp;
    m_sensorTimeStamp = timestamp;
    return NO_ERROR;
}

uint64_t ExynosCamera3Request::getSensorTimestamp()
{
    Mutex::Autolock l(m_sensorTimeStampLock);
    if (m_resultShot.shot.udm.sensor.timeStampBoot != m_sensorTimeStamp) {
          CLOGE2("getSensorTimestamp is different m_frameCount(%d) timeStampBoot(%llu) m_sensorTimeStamp(%llu)",
            m_frameCount, m_resultShot.shot.udm.sensor.timeStampBoot, m_sensorTimeStamp);
    }

    return m_sensorTimeStamp;
}

uint32_t ExynosCamera3Request::getNumOfOutputBuffer()
{
    return m_numOfOutputBuffers;
}

void ExynosCamera3Request::increasecallbackBufferCount(void)
{
    m_resultcallbackLock.lock();
    m_numOfcallbackBuffers++;
    m_resultcallbackLock.unlock();
}

void ExynosCamera3Request::increaseCompleteBufferCount(void)
{
    m_resultStatusLock.lock();
    m_numOfCompleteBuffers++;
    m_resultStatusLock.unlock();
}

void ExynosCamera3Request::resetCompleteBufferCount(void)
{
    m_resultStatusLock.lock();
    m_numOfCompleteBuffers = 0;
    m_resultStatusLock.unlock();
}

void ExynosCamera3Request::resetcallbackBufferCount(void)
{
    m_resultcallbackLock.lock();
    m_numOfcallbackBuffers = 0;
    m_resultcallbackLock.unlock();
}

int ExynosCamera3Request::getCompleteBufferCount(void)
{
     return m_numOfCompleteBuffers;
}

int ExynosCamera3Request::getcallbackBufferCount(void)
{
     return m_numOfcallbackBuffers;
}

void ExynosCamera3Request::increaseDuplicateBufferCount(void)
{
    m_resultStatusLock.lock();
    m_numOfDuplicateBuffers++;
    m_resultStatusLock.unlock();
}

void ExynosCamera3Request::resetDuplicateBufferCount(void)
{
    m_resultStatusLock.lock();
    m_numOfDuplicateBuffers = 0;
    m_resultStatusLock.unlock();
}
int ExynosCamera3Request::getDuplicateBufferCount(void)
{
     return m_numOfDuplicateBuffers;
}

const camera3_stream_buffer_t* ExynosCamera3Request::getOutputBuffers()
{
    if (m_request == NULL){
        CLOGE2("utputBuffer m_request is NULL m_request(%p) ", m_request);
        return NULL;
    }

    if (m_request->output_buffers == NULL){
        CLOGE2("getNumOfOutputBuffer output_buffers is NULL m_request(%p) ",
            m_request->output_buffers);
        return NULL;
    }

    CLOGV2("m_request->output_buffers(%p)", m_request->output_buffers);

    return m_request->output_buffers;
}

status_t ExynosCamera3Request::pushToResultList(ResultRequest result)
{
    status_t ret = NO_ERROR;

    switch(result->getType()) {
    case EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY:
    case EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY:
    case EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT:
    case EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA:
        ret = m_pushResult(result, &m_resultList, &m_resultListLock);
        if (ret < 0){
            CLOGE2("pushResult is failed request - Key(%u) frameCount(%u) /  result - Key(%u) frameCount(%u)",
                m_key, m_frameCount, result->getKey(), result->getFrameCount());
            ret = INVALID_OPERATION;
        }
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_INVALID:
    default:
        ret = INVALID_OPERATION;
        CLOGE2("getResultType type have INVALID value type(%d) key(%u) frameCount(%u)",
            result->getType(), m_key, m_frameCount);
        break;
    }


    return ret;
}

void ExynosCamera3Request::setRequestId(int reqId) {
    m_requestId = reqId;
}

int ExynosCamera3Request::getRequestId(void) {
    return m_requestId;
}

ResultRequest ExynosCamera3Request::popResult(uint32_t resultKey)
{
    ResultRequest result = NULL;
    status_t ret = NO_ERROR;
    
    result = m_popResult(resultKey, &m_resultList, &m_resultListLock);
    if (ret < 0){
        CLOGE2("popResult is failed request - Key(%u) frameCount(%u) /  result - Key(%u) frameCount(%u)",
            m_key, m_frameCount, result->getKey(), result->getFrameCount());
        result = NULL;
    }

    return result;
}

ResultRequest ExynosCamera3Request::getResult(uint32_t resultKey)
{
    ResultRequest result = NULL;
    status_t ret = NO_ERROR;
    
    result = m_getResult(resultKey, &m_resultList, &m_resultListLock);
    if (ret < 0){
        CLOGE2("popResult is failed request - Key(%u) frameCount(%u) /  result - Key(%u) frameCount(%u)",
            m_key, m_frameCount, result->getKey(), result->getFrameCount());
        result = NULL;
    }

    return result;
}

status_t ExynosCamera3Request::m_pushResult(ResultRequest result, ResultRequestMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<ResultRequestMap::iterator,bool> listRet;
    lock->lock();
    listRet = list->insert(pair<uint32_t, ResultRequest>(result->getKey(), result));
    if (listRet.second == false) {
        ret = INVALID_OPERATION;
        CLOGE2("m_push failed, request already exist!! Request frameCnt( %d )",
            result->getFrameCount());
    }
    lock->unlock();

    return ret;
}

ResultRequest ExynosCamera3Request::m_popResult(uint32_t key, ResultRequestMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<ResultRequestMap::iterator,bool> listRet;
    ResultRequestMapIterator iter;
    ResultRequest result = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        result = iter->second;
        list->erase( iter );
    } else {
        CLOGE2("m_pop failed, request is not EXIST Request frameCnt( %d )",
            result->getFrameCount());
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return result;
}

ResultRequest ExynosCamera3Request::m_getResult(uint32_t key, ResultRequestMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<ResultRequestMap::iterator,bool> listRet;
    ResultRequestMapIterator iter;
    ResultRequest result = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        result = iter->second;
    } else {
        CLOGE2("m_getResult failed, request is not EXIST Request frameCnt( %d )",
            result->getFrameCount());
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return result;
}

status_t ExynosCamera3Request::m_getAllResultKeys(ResultRequestkeys *keylist, ResultRequestMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestMapIterator iter;

    lock->lock();
    for (iter = list->begin(); iter != list->end() ; iter++) {
        keylist->push_back(iter->first);
    }
    lock->unlock();
    return ret;
}

status_t ExynosCamera3Request::m_getResultKeys(ResultRequestkeys *keylist, ResultRequestMap *list, EXYNOS_REQUEST_RESULT::TYPE type, Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestMapIterator iter;
    ResultRequest result;
    camera3_capture_result_t capture_result;

    lock->lock();
    /* validation check */
    if ((type < EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) ||
        (type >= EXYNOS_REQUEST_RESULT::CALLBACK_MAX)) {
        CLOGE2("INVALID value type(%d)", type);
        lock->unlock();
        return BAD_VALUE;
    }

    for (iter = list->begin(); iter != list->end() ; iter++) {
        result = iter->second;

        CLOGV2("result->getKey(%d)", result->getKey());

        if (type == result->getType())
            keylist->push_back(iter->first);
    }
    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::m_push(int key, ExynosCamera3FrameFactory* item, FrameFactoryMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<FrameFactoryMap::iterator,bool> listRet;
    lock->lock();
    listRet = list->insert( pair<uint32_t, ExynosCamera3FrameFactory*>(key, item));
    if (listRet.second == false) {
        ret = INVALID_OPERATION;
        CLOGE2("m_push failed, request already exist!! Request frameCnt( %d )",
            key);
    }
    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::m_pop(int key, ExynosCamera3FrameFactory** item, FrameFactoryMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<FrameFactoryMap::iterator,bool> listRet;
    FrameFactoryMapIterator iter;
    ExynosCamera3FrameFactory *factory = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        factory = iter->second;
        *item = factory;
        list->erase( iter );
    } else {
        CLOGE2("m_pop failed, factory is not EXIST Request frameCnt( %d )",
            key);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::m_get(int streamID, ExynosCamera3FrameFactory** item, FrameFactoryMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<FrameFactoryMap::iterator,bool> listRet;
    FrameFactoryMapIterator iter;
    ExynosCamera3FrameFactory *factory = NULL;

    lock->lock();
    iter = list->find(streamID);
    if (iter != list->end()) {
        factory = iter->second;
        *item = factory;
    } else {
        CLOGE2("m_pop failed, request is not EXIST Request streamID( %d )",
            streamID);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

bool ExynosCamera3Request::m_find(int streamID, FrameFactoryMap *list, Mutex *lock)
{
    bool ret = false;
    pair<FrameFactoryMap::iterator,bool> listRet;
    FrameFactoryMapIterator iter;

    lock->lock();
    iter = list->find(streamID);
    if (iter != list->end()) {
        ret = true;
    }
    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::m_getList(FrameFactoryList *factorylist, FrameFactoryMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    FrameFactoryMapIterator iter;
    ExynosCamera3FrameFactory *factory = NULL;

    lock->lock();
    for (iter = list->begin(); iter != list->end() ; iter++) {
        factory = iter->second;
        factorylist->push_back(factory);
    }
    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::getAllResultKeys(ResultRequestkeys *keys)
{
    status_t ret = NO_ERROR;

    keys->clear();

    ret = m_getAllResultKeys(keys, &m_resultList, &m_resultListLock);
    if (ret < 0){
        CLOGE2("m_getAllResultKeys is failed Request-Key(%u) frameCount(%u) / m_resultList.Size(%zu)",
            m_key, m_frameCount, m_resultList.size());
    }

    return ret;
}

status_t ExynosCamera3Request::getResultKeys(ResultRequestkeys *keys, EXYNOS_REQUEST_RESULT::TYPE type)
{
    status_t ret = NO_ERROR;

    keys->clear();

    ret = m_getResultKeys(keys, &m_resultList, type, &m_resultListLock);
    if (ret < 0){
        CLOGE2("getResultKeys is failed Request-Key(%u) frameCount(%u) / m_resultList.Size(%zu)",
            m_key, m_frameCount, m_resultList.size());
    }

    return ret;
}

status_t ExynosCamera3Request::pushFrameFactory(int StreamID, ExynosCamera3FrameFactory* factory)
{
    status_t ret = NO_ERROR;
    ret = m_push(StreamID % HAL_STREAM_ID_MAX, factory, &m_factoryMap, &m_factoryMapLock);
    if (ret < 0) {
        CLOGE2("pushFrameFactory is failed StreamID(%d) factory(%p)",
            StreamID, factory);
    }

    return ret;
}

ExynosCamera3FrameFactory* ExynosCamera3Request::popFrameFactory(int streamID)
{
    status_t ret = NO_ERROR;
    ExynosCamera3FrameFactory* factory = NULL;

    ret = m_pop(streamID % HAL_STREAM_ID_MAX, &factory, &m_factoryMap, &m_factoryMapLock);
    if (ret < 0) {
        CLOGE2("popFrameFactory is failed StreamID(%d) factory(%p)",
            streamID, factory);
    }
    return factory;
}

ExynosCamera3FrameFactory* ExynosCamera3Request::getFrameFactory(int streamID)
{
    status_t ret = NO_ERROR;
    ExynosCamera3FrameFactory* factory = NULL;

    ret = m_get(streamID % HAL_STREAM_ID_MAX, &factory, &m_factoryMap, &m_factoryMapLock);
    if (ret < 0) {
        CLOGE2("getFrameFactory is failed StreamID(%d) factory(%p)",
            streamID, factory);
    }

    return factory;
}

bool ExynosCamera3Request::isFrameFactory(int streamID)
{
    return m_find(streamID % HAL_STREAM_ID_MAX, &m_factoryMap, &m_factoryMapLock);
}

status_t ExynosCamera3Request::getFrameFactoryList(FrameFactoryList *list)
{
    status_t ret = NO_ERROR;

    ret = m_getList(list, &m_factoryMap, &m_factoryMapLock);
    if (ret < 0) {
        CLOGE2("getFrameFactoryList is failed");
    }

    return ret;
}

status_t ExynosCamera3Request::getAllRequestOutputStreams(List<int> **list)
{
    status_t ret = NO_ERROR;

    CLOGV2("m_requestOutputStreamList.size(%zu)",
        m_requestOutputStreamList.size());

    /* lock->lock(); */
    *list = &m_requestOutputStreamList;
    /* lock->unlock(); */

    return ret;
}

status_t ExynosCamera3Request::pushRequestOutputStreams(int requestStreamId)
{
    status_t ret = NO_ERROR;

    /* lock->lock(); */
    m_requestOutputStreamList.push_back(requestStreamId);
    /* lock->unlock(); */

    return ret;
}

status_t ExynosCamera3Request::getAllRequestInputStreams(List<int> **list)
{
    status_t ret = NO_ERROR;

    CLOGV2("m_requestOutputStreamList.size(%zu)", m_requestInputStreamList.size());

    /* lock->lock(); */
    *list = &m_requestInputStreamList;
    /* lock->unlock(); */

    return ret;
}

status_t ExynosCamera3Request::pushRequestInputStreams(int requestStreamId)
{
    status_t ret = NO_ERROR;

    /* lock->lock(); */
    m_requestInputStreamList.push_back(requestStreamId);
    /* lock->unlock(); */

    return ret;
}

status_t ExynosCamera3Request::popAndEraseResultsByType(EXYNOS_REQUEST_RESULT::TYPE reqType, ResultRequestList *resultList)
{
    status_t ret = NO_ERROR;

    resultList->clear();

    ret = m_popAndEraseResultsByType(reqType, &m_resultList, resultList, &m_resultListLock);
    if (ret < 0) {
        CLOGE2("m_pop request is failed, request type(%d) resultSize(%zu)",
            reqType, resultList->size());
    }

    return ret;
}

status_t ExynosCamera3Request::popAndEraseResultsByTypeAndStreamId(EXYNOS_REQUEST_RESULT::TYPE reqType, int streamId, ResultRequestList *resultList)
{
    status_t ret = NO_ERROR;

    resultList->clear();

    ret = m_popAndEraseResultsByTypeAndStreamId(reqType, streamId, &m_resultList, resultList, &m_resultListLock);
    if (ret < 0) {
        ALOGE("ERR(%s[%d]):m_pop request is failed, request type(%d) streamId(%d) resultSize(%zu)",
            __FUNCTION__, __LINE__, reqType, streamId, resultList->size());
    }

    return ret;
}

status_t ExynosCamera3Request::popResultsByType(EXYNOS_REQUEST_RESULT::TYPE reqType, ResultRequestList *resultList)
{
    status_t ret = NO_ERROR;

    resultList->clear();

    ret = m_popResultsByType(reqType, &m_resultList, resultList, &m_resultListLock);
    if (ret < 0) {
        CLOGE2("m_pop request is failed, request type(%d) resultSize(%zu)",
            reqType, resultList->size());
    }

    return ret;
}

status_t ExynosCamera3Request::setCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType, bool flag)
{
    status_t ret = NO_ERROR;
    ret = m_setCallbackDone(reqType, flag, &m_resultStatusLock);
    if (ret < 0) {
        CLOGE2("m_get request is failed, request type(%d) ", reqType);
    }

    return ret;
}

bool ExynosCamera3Request::getCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType)
{
    bool ret = false;
    ret = m_getCallbackDone(reqType, &m_resultStatusLock);
    return ret;
}

bool ExynosCamera3Request::isComplete()
{
    bool ret = false;
    bool notify = false;
    bool partial = false;
    bool capture = false;

    notify = m_getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY, &m_resultStatusLock);
    partial = m_getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA, &m_resultStatusLock);
    capture = m_getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT, &m_resultStatusLock);

    if (notify == true && capture == true && partial == true) {
        ret = true;
    }

    return ret;
}

int ExynosCamera3Request::getStreamId(int bufferIndex)
{
    if (bufferIndex < 0 || bufferIndex >= m_numOfOutputBuffers) {
        CLOGE2("Invalid buffer index(%d), outputBufferCount(%d)",
                bufferIndex, m_numOfOutputBuffers);
        return -1;
    }

    if (m_streamIdList[bufferIndex] < 0) {
        CLOGE2("m_streamIdList is NULL, index(%d)", bufferIndex);
        return -1;
    }

    return m_streamIdList[bufferIndex];
}

bool ExynosCamera3Request::isExistStream(int streamId)
{
    ExynosCameraStream *stream = NULL;
    bool ret = false;
    int id = 0;

    /* 1. check output stream */
    for (int i = 0 ; i < m_numOfOutputBuffers; i++) {
        if (streamId == m_streamIdList[i]) {
            ret = true;
            break;
        }
    }

    if (ret == true) {
        return ret;
    }

    if (m_request->input_buffer != NULL) {
        stream = static_cast<ExynosCameraStream *>(m_request->input_buffer->stream->priv);
        stream->getID(&id);
        if (streamId == id) {
            ret = true;
        }
    }

    return ret;
}

void ExynosCamera3Request::setNeedInternalFrame(bool isNeedInternalFrame)
{
    m_isNeedInternalFrame = isNeedInternalFrame;
}

bool ExynosCamera3Request::getNeedInternalFrame(void)
{
    return m_isNeedInternalFrame;
}

void ExynosCamera3Request::increasePipelineDepth(void)
{
    m_pipelineDepth++;
}

void ExynosCamera3Request::updatePipelineDepth(void)
{
    Mutex::Autolock l(m_resultMetaLock);
    const uint8_t pipelineDepth = m_pipelineDepth;

    m_resultShot.shot.dm.request.pipelineDepth = m_pipelineDepth;
    m_resultMeta.update(ANDROID_REQUEST_PIPELINE_DEPTH, &pipelineDepth, 1);
    CLOGV2("ANDROID_REQUEST_PIPELINE_DEPTH(%d)", pipelineDepth);
}

CameraMetadata ExynosCamera3Request::get3AAResultMeta(void)
{
    Mutex::Autolock l(m_resultMetaLock);
    uint8_t entry;
    CameraMetadata minimal_resultMeta;

    if (m_resultMeta.exists(ANDROID_CONTROL_AF_MODE)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AF_MODE).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AF_MODE, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AF_MODE is (%d)", __FUNCTION__, __LINE__, entry);
    }

    if (m_resultMeta.exists(ANDROID_CONTROL_AWB_MODE)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AWB_MODE).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AWB_MODE, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AWB_MODE is (%d)", __FUNCTION__, __LINE__, entry);
    }

    if (m_resultMeta.exists(ANDROID_CONTROL_AE_STATE)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AE_STATE).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AE_STATE, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AE_MODE is (%d)", __FUNCTION__, __LINE__, entry);
    }

    if (m_resultMeta.exists(ANDROID_CONTROL_AF_STATE)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AF_STATE).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AF_STATE, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AF_STATE is (%d)", __FUNCTION__, __LINE__, entry);
    }

    if (m_resultMeta.exists(ANDROID_CONTROL_AWB_STATE)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AWB_STATE).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AWB_STATE, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AWB_STATE is (%d)", __FUNCTION__, __LINE__, entry);
    }

    if (m_resultMeta.exists(ANDROID_CONTROL_AF_TRIGGER_ID)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AF_TRIGGER_ID).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AF_TRIGGER_ID, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AF_TRIGGER_ID is (%d)", __FUNCTION__, __LINE__, entry);
    }

    if (m_resultMeta.exists(ANDROID_CONTROL_AE_PRECAPTURE_ID)) {
        entry = m_resultMeta.find(ANDROID_CONTROL_AE_PRECAPTURE_ID).data.u8[0];
        minimal_resultMeta.update(ANDROID_CONTROL_AE_PRECAPTURE_ID, &entry, 1);
        ALOGV("DEBUG(%s[%d]):ANDROID_CONTROL_CONTROL_AE_PRECAPTURE_ID is (%d)", __FUNCTION__, __LINE__, entry);
    }
    return minimal_resultMeta;
}

#ifdef USE_VRA_GROUP
void ExynosCamera3Request::setDsInputPortId(int dsInputPortId)
{
    m_dsInputPortId = dsInputPortId;
}

int ExynosCamera3Request::getDsInputPortId(void)
{
    return m_dsInputPortId;
}
#endif

status_t ExynosCamera3Request::m_setCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType, bool flag, Mutex *lock)
{
    status_t ret = NO_ERROR;
    if (reqType >= EXYNOS_REQUEST_RESULT::CALLBACK_MAX) {
        CLOGE2("m_setCallback failed, status erray out of bounded reqType(%d)",
            reqType);

        ret = INVALID_OPERATION;
        return ret;
    }

    lock->lock();
    m_resultStatus[reqType] = flag;
    lock->unlock();
    return ret;
}

bool ExynosCamera3Request::m_getCallbackDone(EXYNOS_REQUEST_RESULT::TYPE reqType, Mutex *lock)
{
    bool ret = false;
    if (reqType >= EXYNOS_REQUEST_RESULT::CALLBACK_MAX) {
        CLOGE2("m_getCallback failed, status erray out of bounded reqType(%d)",
            reqType);

        return ret;
    }

    lock->lock();
    ret = m_resultStatus[reqType];
    lock->unlock();
    return ret;
}

void ExynosCamera3Request::printCallbackDoneState()
{
    for (int i = 0 ; i < EXYNOS_REQUEST_RESULT::CALLBACK_MAX ; i++)
        CLOGD2("m_key(%d), m_resultStatus[%d](%d)",
            m_key, i, m_resultStatus[i]);
}

status_t ExynosCamera3Request::m_popAndEraseResultsByType(EXYNOS_REQUEST_RESULT::TYPE reqType,
                                                            ResultRequestMap *list,
                                                            ResultRequestList *resultList,
                                                            Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestMapIterator iter;
    ResultRequest obj;

    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin(); iter != list->end();) {
            obj = iter->second;
            if (obj->getType() == reqType) {
                resultList->push_back(obj);
                list->erase(iter++);
            } else {
                ++iter;
            }
        }
    }

    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::m_popAndEraseResultsByTypeAndStreamId(EXYNOS_REQUEST_RESULT::TYPE reqType,
                                                                      int streamID,
                                                                      ResultRequestMap *list,
                                                                      ResultRequestList *resultList,
                                                                      Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestMapIterator iter;
    ResultRequest obj;
    camera3_stream_buffer_t streamBuffer;
    ExynosCameraStream *privStreamInfo = NULL;
    camera3_stream_t *newStream = NULL;
    int id = 0;

    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin(); iter != list->end();) {
            obj = iter->second;
            if (obj->getType() == reqType) {
                switch(reqType) {
                case EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY:
                case EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT:
                case EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA:
                    resultList->push_back(obj);
                    list->erase(iter++);
                    break;
                case EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY:
                    obj->getStreamBuffer(&streamBuffer);
                    newStream = streamBuffer.stream;
                    privStreamInfo = static_cast<ExynosCameraStream*>(newStream->priv);
                    privStreamInfo->getID(&id);
                    if (id == streamID) {
                        resultList->push_back(obj);
                        list->erase(iter++);
                    } else {
                        iter++;
                    }
                    break;
                case EXYNOS_REQUEST_RESULT::CALLBACK_INVALID:
                default:
                    ret = INVALID_OPERATION;
                    iter++;
                    CLOGE2("getResultTypeAndStreamId type have INVALID "
                        "value type(%d) streamId(%d) key(%u) frameCount(%u)",
                        obj->getType(), streamID, m_key, m_frameCount);
                    break;
                }
            } else {
                iter++;
            }
        }
    }

    lock->unlock();

    return ret;
}

status_t ExynosCamera3Request::m_popResultsByType(EXYNOS_REQUEST_RESULT::TYPE reqType,
                                                    ResultRequestMap *list,
                                                    ResultRequestList *resultList,
                                                    Mutex *lock)
{
    status_t ret = NO_ERROR;
    ResultRequestMapIterator iter;
    ResultRequest obj;

    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin(); iter != list->end(); iter++) {
            obj = iter->second;
            if (obj->getType() == reqType) {
                resultList->push_back(obj);
            }
        }
    }

    lock->unlock();

    return ret;
}

ExynosCameraRequestManager::ExynosCameraRequestManager(int cameraId, ExynosCameraParameters *param)
{
    CLOGD("Create-ID(%d)",cameraId);

    m_cameraId = cameraId;
    m_parameters = param;
    m_converter = NULL;
    m_callbackOps = NULL;
    m_requestKey = 0;
    m_requestResultKey = 0;
    memset(&m_dummyShot, 0x00, sizeof(struct camera2_shot_ext));
    memset(&m_currShot, 0x00, sizeof(struct camera2_shot_ext));
    memset(m_name, 0x00, sizeof(m_name));
    memset(&m_streamPrevPos, 0x00, sizeof(m_streamPrevPos));

    for (int i = 0; i < CAMERA3_TEMPLATE_COUNT; i++)
        m_defaultRequestTemplate[i] = NULL;

    m_factoryMap.clear();
    m_zslFactoryMap.clear();

    m_callbackSequencer = new ExynosCameraCallbackSequencer();

    m_preShot = NULL;
    m_preShot = new struct camera2_shot_ext;
    m_callbackTraceCount = 0;

    for (int i = 0 ; i < (HAL_STREAM_ID_MAX * 5); i++) {
        m_threadStreamDoneQ[i] = NULL;
        m_streamPrevPos[i] = 0;
    }
}

ExynosCameraRequestManager::~ExynosCameraRequestManager()
{
    CLOGD("");

    memset(&m_dummyShot, 0x00, sizeof(struct camera2_shot_ext));
    memset(&m_currShot, 0x00, sizeof(struct camera2_shot_ext));

    m_deleteStreamThread(&m_streamthreadMap, &m_streamthreadMapLock);
    m_streamthreadMap.clear();

    for (int i = 0 ; i < (HAL_STREAM_ID_MAX * 5); i++) {
        if (m_threadStreamDoneQ[i] != NULL) {
            delete m_threadStreamDoneQ[i];
            m_threadStreamDoneQ[i] = NULL;
        }
        m_streamPrevPos[i] = 0;
    }

    if (m_notifyCallbackThread != NULL) {
        m_notifyCallbackThread->requestExit();
        m_notifyQ.wakeupAll();
        m_notifyCallbackThread->requestExitAndWait();
        m_notifyQ.release();
    }

    if (m_partial3AACallbackThread != NULL) {
        m_partial3AACallbackThread->requestExit();
        m_partial3AAQ.wakeupAll();
        m_partial3AACallbackThread->requestExitAndWait();
        m_partial3AAQ.release();
    }

    if (m_metaCallbackThread != NULL) {
        m_metaCallbackThread->requestExit();
        m_metaQ.wakeupAll();
        m_metaCallbackThread->requestExitAndWait();
        m_metaQ.release();
    }

    if (m_requestDeleteThread != NULL) {
        m_requestDeleteThread->requestExit();
        m_requestDeleteQ.wakeupAll();
        m_requestDeleteThread->requestExitAndWait();
        m_requestDeleteQ.release();
    }

    if (m_callbackSequencer != NULL) {
        m_callbackSequencer->flush();
        delete m_callbackSequencer;
        m_callbackSequencer= NULL;
    }

    for (int i = 0; i < CAMERA3_TEMPLATE_COUNT; i++) {
        if (m_defaultRequestTemplate[i]) {
            free(m_defaultRequestTemplate[i]);
        }
    }

    for (int i = 0 ; i < EXYNOS_REQUEST_TYPE::MAX ; i++) {
        m_serviceRequests[i].clear();
        m_runningRequests[i].clear();
    }

    m_requestFrameCountMap.clear();

    m_factoryMap.clear();
    m_zslFactoryMap.clear();

    if (m_preShot != NULL) {
        delete m_preShot;
        m_preShot = NULL;
    }

    m_callbackTraceCount = 0;
}

status_t ExynosCameraRequestManager::setMetaDataConverter(ExynosCameraMetadataConverter *converter)
{
    status_t ret = NO_ERROR;
    if (m_converter != NULL)
        CLOGD("m_converter is not NULL(%p)", m_converter);

    m_converter = converter;
    return ret;
}

ExynosCameraMetadataConverter* ExynosCameraRequestManager::getMetaDataConverter()
{
    if (m_converter == NULL)
        CLOGD("m_converter is NULL(%p)", m_converter);

    return m_converter;
}

status_t ExynosCameraRequestManager::setRequestsInfo(int key, ExynosCamera3FrameFactory *factory, ExynosCamera3FrameFactory *zslFactory)
{
    status_t ret = NO_ERROR;
    if (factory == NULL) {
        CLOGE("m_factory is NULL key(%d) factory(%p)",
            key, factory);

        ret = INVALID_OPERATION;
        return ret;
    }
    /* zslFactory can be NULL. In this case, use factory insted.
       (Same frame factory for both normal capture and ZSL input)
    */
    if (zslFactory == NULL) {
        zslFactory = factory;
    }

    ret = m_pushFactory(key ,factory, &m_factoryMap, &m_factoryMapLock);
    if (ret < 0) {
        CLOGE("m_pushFactory is failed key(%d) factory(%p)",
            key, factory);

        ret = INVALID_OPERATION;
        return ret;
    }
    ret = m_pushFactory(key ,zslFactory, &m_zslFactoryMap, &m_zslFactoryMapLock);
    if (ret < 0) {
        CLOGE("m_pushFactory is failed key(%d) zslFactory(%p)",
            key, factory);

        ret = INVALID_OPERATION;
        return ret;
    }

    return ret;
}

ExynosCamera3FrameFactory* ExynosCameraRequestManager::getFrameFactory(int key)
{
    status_t ret = NO_ERROR;
    ExynosCamera3FrameFactory *factory = NULL;
    if (key < 0) {
        CLOGE("getFrameFactory, type is invalid key(%d)",
            key);

        ret = INVALID_OPERATION;
        return NULL;
    }

    ret = m_popFactory(key ,&factory, &m_factoryMap, &m_factoryMapLock);
    if (ret < 0) {
        CLOGE("m_popFactory is failed key(%d) factory(%p)",
            key, factory);

        ret = INVALID_OPERATION;
        return NULL;
    }
    return factory;
}

status_t ExynosCameraRequestManager::isPrevRequest()
{
    if (m_previousMeta.isEmpty())
        return BAD_VALUE;
    else
        return OK;
}

status_t ExynosCameraRequestManager::clearPrevRequest()
{
    m_previousMeta.clear();
    return OK;
}

status_t ExynosCameraRequestManager::clearPrevShot()
{
    status_t ret = NO_ERROR;
    if (m_preShot == NULL) {
        ret = INVALID_OPERATION;
        CLOGE("clearPrevShot previous meta is NULL ret(%d)", ret);
    } else {
        memset(m_preShot, 0x00, sizeof(struct camera2_shot_ext));
    }
    return ret;
}

status_t ExynosCameraRequestManager::constructDefaultRequestSettings(int type, camera_metadata_t **request)
{
    Mutex::Autolock l(m_requestLock);

    CLOGD("Type = %d", type);

    if (m_defaultRequestTemplate[type]) {
        *request = m_defaultRequestTemplate[type];
        return OK;
    }

    m_converter->constructDefaultRequestSettings(type, request);

    m_defaultRequestTemplate[type] = *request;

    /* create default shot */
    m_converter->initShotData(&m_dummyShot);

    CLOGD("Registered default request template(%d)", type);
    return OK;
}

status_t ExynosCameraRequestManager::m_pushBack(ExynosCameraRequestSP_sprt_t item, RequestInfoList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    lock->lock();
    list->push_back(item);
    lock->unlock();
    return ret;
}

status_t ExynosCameraRequestManager::m_popBack(ExynosCameraRequestSP_dptr_t item, RequestInfoList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    lock->lock();
    if (list->size() > 0) {
        item = list->back();
        list->pop_back();
    } else {
        CLOGE("m_popBack failed, size(%zu)", list->size());
        ret = INVALID_OPERATION;
    }
    lock->unlock();
    return ret;
}

status_t ExynosCameraRequestManager::m_pushFront(ExynosCameraRequestSP_sprt_t item, RequestInfoList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    lock->lock();
    list->push_back(item);
    lock->unlock();
    return ret;
}

status_t ExynosCameraRequestManager::m_popFront(ExynosCameraRequestSP_dptr_t item, RequestInfoList *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    lock->lock();
    if (list->size() > 0) {
        item = list->front();
        list->pop_front();
    } else {
        CLOGE("m_popFront failed, size(%zu)", list->size());
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_get(uint32_t frameCount,
                                        ExynosCameraRequestSP_dptr_t item,
                                        RequestInfoList *list,
                                        Mutex *lock)
{
    status_t ret = INVALID_OPERATION;
    RequestInfoListIterator iter;
    ExynosCameraRequestSP_sprt_t request = NULL;

    if (item == NULL) {
        CLOGE("item is NULL");
        return BAD_VALUE;
    }

    if (list == NULL) {
        CLOGE("list is NULL");
        return BAD_VALUE;
    }

    if (lock == NULL) {
        CLOGE("lock is NULL");
        return BAD_VALUE;
    }

    lock->lock();
    for (iter = list->begin(); iter != list->end(); ++iter) {
        request = *iter;
        if (request->getKey() == frameCount) {
            ret = NO_ERROR;
            break;
        }
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_push(ExynosCameraRequestSP_sprt_t request, RequestInfoMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<RequestInfoMap::iterator,bool> listRet;
    lock->lock();
    listRet = list->insert( pair<uint32_t, ExynosCameraRequestSP_sprt_t>(request->getKey(), request));
    if (listRet.second == false) {
        ret = INVALID_OPERATION;
        CLOGE("m_push failed, request already exist!! Request frameCnt( %d )",
            request->getFrameCount());
    }
    lock->unlock();

    //m_printAllRequestInfo(list, lock);

    return ret;
}

status_t ExynosCameraRequestManager::m_pop(uint32_t key, ExynosCameraRequestSP_dptr_t item,
                                            RequestInfoMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<RequestInfoMap::iterator,bool> listRet;
    RequestInfoMapIterator iter;
    ExynosCameraRequestSP_sprt_t request = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        request = iter->second;
        item = request;
        list->erase( iter );
    } else {
        CLOGE("m_pop failed, request is not EXIST Request key(%d)", key);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_get(uint32_t key, ExynosCameraRequestSP_dptr_t item,
                                            RequestInfoMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<RequestInfoMap::iterator,bool> listRet;
    RequestInfoMapIterator iter;
    ExynosCameraRequestSP_sprt_t request = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        request = iter->second;
        item = request;
    } else {
        CLOGE("m_pop failed, request is not EXIST Request key(%d)", key);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    //m_printAllRequestInfo(list, lock);

    return ret;
}

void ExynosCameraRequestManager::m_printAllRequestInfo(RequestInfoMap *map, Mutex *lock)
{
    RequestInfoMapIterator iter;
    ExynosCameraRequestSP_sprt_t request = NULL;
    ExynosCameraRequestSP_sprt_t item = NULL;
    camera3_capture_request_t *serviceRequest = NULL;

    lock->lock();
    iter = map->begin();

    while(iter != map->end()) {
        request = iter->second;
        item = request;

        serviceRequest = item->getService();
#if 0
        CLOGE("key(%d), serviceFrameCount(%d), (%p) frame_number(%d), outputNum(%d)",
            request->getKey(),
            request->getFrameCount(),
            serviceRequest,
            serviceRequest->frame_number,
            serviceRequest->num_output_buffers);
#endif
        iter++;
    }
    lock->unlock();
}

status_t ExynosCameraRequestManager::m_delete(ExynosCameraRequestSP_sprt_t item)
{
    status_t ret = NO_ERROR;

    CLOGV("m_delete -> delete request(%d)", item->getFrameCount());

//    delete item;
//    item = NULL;

    return ret;
}

status_t ExynosCameraRequestManager::m_pushFactory(int key,
                                                    ExynosCamera3FrameFactory* item,
                                                    FrameFactoryMap *list,
                                                    Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<FrameFactoryMap::iterator,bool> listRet;
    lock->lock();
    listRet = list->insert(pair<uint32_t, ExynosCamera3FrameFactory*>(key, item));
    if (listRet.second == false) {
        ret = INVALID_OPERATION;
        CLOGE("m_push failed, request already exist!! Request frameCnt( %d )",
            key);
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_popFactory(int key, ExynosCamera3FrameFactory** item, FrameFactoryMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<FrameFactoryMap::iterator,bool> listRet;
    FrameFactoryMapIterator iter;
    ExynosCamera3FrameFactory *factory = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        factory = iter->second;
        *item = factory;
        list->erase( iter );
    } else {
        CLOGE("m_pop failed, factory is not EXIST Request frameCnt( %d )",
            key);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_getFactory(int key, ExynosCamera3FrameFactory** item, FrameFactoryMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<FrameFactoryMap::iterator,bool> listRet;
    FrameFactoryMapIterator iter;
    ExynosCamera3FrameFactory *factory = NULL;
    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        factory = iter->second;
        *item = factory;
    } else {
        CLOGE("m_pop failed, request is not EXIST Request frameCnt( %d )",
            key);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

ExynosCameraRequestSP_sprt_t ExynosCameraRequestManager::registerToServiceList(camera3_capture_request_t *srcRequest)
{
    status_t ret = NO_ERROR;
    ExynosCameraRequestSP_sprt_t request = NULL;
    struct camera2_shot_ext shot_ext;
    CameraMetadata meta;
    int32_t captureIntent = 0;
    uint32_t bufferCnt = 0;
    camera3_stream_buffer_t *inputbuffer = NULL;
    const camera3_stream_buffer_t *outputbuffer = NULL;
    ExynosCameraStream *stream = NULL;
    ExynosCamera3FrameFactory *factory = NULL;
    int32_t streamID = 0;
    int32_t factoryID = 0;
    bool needDummyStream = true;
    bool isZslInput = false;
#ifdef USE_VRA_GROUP
    int previewPortId = MCSC_PORT_NONE;
    int yuvPortId = MCSC_PORT_NONE;
    int tempPortId = MCSC_PORT_NONE;
    int jpegPortId = MCSC_PORT_NONE;
    int yuvStallPortId = MCSC_PORT_NONE;
#endif

    if (srcRequest->settings == NULL) {
        meta = m_previousMeta;
    } else {
        meta = srcRequest->settings;
    }

    if (meta.exists(ANDROID_CONTROL_CAPTURE_INTENT)) {
        captureIntent = meta.find(ANDROID_CONTROL_CAPTURE_INTENT).data.u8[0];
        CLOGV("ANDROID_CONTROL_CAPTURE_INTENT is (%d)",
            captureIntent);
    }

    /* Check whether the input buffer (ZSL input) is specified.
       Use zslFramFactory in the following section if ZSL input is used
    */
    request = new ExynosCamera3Request(srcRequest, m_previousMeta);
    bufferCnt = request->getNumOfInputBuffer();
    inputbuffer = request->getInputBuffer();
    for(uint32_t i = 0 ; i < bufferCnt ; i++) {
        stream = static_cast<ExynosCameraStream*>(inputbuffer[i].stream->priv);
        stream->getID(&streamID);
        factoryID = streamID % HAL_STREAM_ID_MAX;

        /* Stream ID validity */
        if(factoryID == HAL_STREAM_ID_ZSL_INPUT) {
            isZslInput = true;
        } else {
            /* Ignore input buffer */
            CLOGE("Invalid input streamID. captureIntent(%d) streamID(%d)",
                    captureIntent, streamID);
        }
    }

    bufferCnt = request->getNumOfOutputBuffer();
    outputbuffer = request->getOutputBuffers();
    for(uint32_t i = 0 ; i < bufferCnt ; i++) {
        stream = static_cast<ExynosCameraStream*>(outputbuffer[i].stream->priv);
        stream->getID(&streamID);
        factoryID = streamID % HAL_STREAM_ID_MAX;
        switch(streamID % HAL_STREAM_ID_MAX) {
        case HAL_STREAM_ID_PREVIEW:
        case HAL_STREAM_ID_VIDEO:
        case HAL_STREAM_ID_CALLBACK:
        case HAL_STREAM_ID_RAW:
        case HAL_STREAM_ID_ZSL_OUTPUT:
            needDummyStream = false;
            break;
        default:
            break;
        }

        if (m_parameters->isReprocessing() == false) {
            needDummyStream = false;
        }

        /* If current request has ZSL Input stream buffer,
         * CALLBACK stream must be processed by reprocessing stream.
         */
        if (isZslInput == true && factoryID == HAL_STREAM_ID_CALLBACK) {
            CLOGV("[R%d]CALLBACK stream will be replaced with CALLBACK_STALL stream",
                request->getKey());

            factoryID = HAL_STREAM_ID_CALLBACK_STALL;
        }
        ret = m_getFactory(factoryID, &factory, &m_factoryMap, &m_factoryMapLock);
        if (ret < 0) {
            CLOGD("[R:%d]m_getFactory is failed captureIntent(%d) streamID(%d)",
                request->getKey(), captureIntent, streamID);
        }

#ifdef USE_VRA_GROUP
        switch(factoryID % HAL_STREAM_ID_MAX) {
        case HAL_STREAM_ID_PREVIEW:
            stream->getOutputPortId(&tempPortId);
            if (previewPortId < 0) {
                previewPortId = tempPortId;
            } else {
                previewPortId = (previewPortId > tempPortId) ? tempPortId : previewPortId;
            }
            break;
        case HAL_STREAM_ID_VIDEO:
        case HAL_STREAM_ID_CALLBACK:
            stream->getOutputPortId(&tempPortId);
            if (yuvPortId < 0) {
                yuvPortId = tempPortId;
            } else {
                yuvPortId = (yuvPortId > tempPortId) ? tempPortId : yuvPortId;
            }
            break;
        case HAL_STREAM_ID_CALLBACK_STALL:
            stream->getOutputPortId(&tempPortId);
            if (yuvStallPortId < 0) {
                yuvStallPortId = tempPortId;
                yuvStallPortId = yuvStallPortId + MCSC_PORT_MAX;
            } else {
                yuvStallPortId = yuvStallPortId - MCSC_PORT_MAX;
                yuvStallPortId = (yuvStallPortId > tempPortId) ? tempPortId : yuvStallPortId;
                yuvStallPortId = yuvStallPortId + MCSC_PORT_MAX;
            }
            break;
        case HAL_STREAM_ID_JPEG:
            jpegPortId = MCSC_PORT_3 + MCSC_PORT_MAX;
            break;
        }
#endif
        request->pushFrameFactory(streamID, factory);
        request->pushRequestOutputStreams(streamID);
    }

#if !defined(ENABLE_FULL_FRAME)
    /* attach dummy stream to this request if this request needs dummy stream */
    request->setNeedInternalFrame(needDummyStream);
#endif

    m_converter->setPreviousMeta(&m_previousMeta);

    request->getServiceShot(&shot_ext);
    meta = request->getServiceMeta();
    m_converter->initShotData(&shot_ext);

    CLOGV("m_currReqeustList size(%zu), fn(%d)",
            m_serviceRequests[EXYNOS_REQUEST_TYPE::PREVIEW].size(), request->getFrameCount());

    if (meta.isEmpty()) {
        CLOGD("meta is EMPTY");
    } else {
        CLOGV("meta is NOT EMPTY");

    }

    int reqId;
    ret = m_converter->convertRequestToShot(meta, &shot_ext, &reqId);
    request->setRequestId(reqId);

    request->setServiceShot(&shot_ext);
    request->setPrevShot(m_preShot);

    m_previousMeta = meta;

#ifdef USE_VRA_GROUP
    request->getServiceShot(&shot_ext);

    if (shot_ext.shot.ctl.stats.faceDetectMode == FACEDETECT_MODE_OFF) {
        request->setDsInputPortId(MCSC_PORT_NONE);
    } else {
        if (previewPortId >= 0) {
            request->setDsInputPortId(previewPortId);
        } else if (yuvPortId >= 0) {
            request->setDsInputPortId(yuvPortId);
        } else if (jpegPortId >= 0) {
            request->setDsInputPortId(jpegPortId);
        } else {
            request->setDsInputPortId(yuvStallPortId);
        }
    }
#endif

    memcpy(m_preShot, &shot_ext, sizeof(struct camera2_shot_ext));

    if (m_parameters->getRestartStream() == false) {
        ret = m_pushBack(request, &m_serviceRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
        if (ret < 0){
            CLOGE("request m_pushBack is failed request(%d)", request->getFrameCount());

            request = NULL;
            return NULL;
        }
    } else {
        CLOGD("restartStream flag checked, request(%d)", request->getFrameCount());
    }

    return request;
}

ExynosCameraRequestSP_sprt_t ExynosCameraRequestManager::registerToServiceList(ExynosCameraRequestSP_sprt_t request)
{
    status_t ret = NO_ERROR;

    ret = m_pushBack(request, &m_serviceRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < 0){
        CLOGE("request m_pushBack is failed request(%d)", request->getFrameCount());

        request = NULL;
        return NULL;
    }

    return request;
}

status_t ExynosCameraRequestManager::getPreviousShot(struct camera2_shot_ext *pre_shot_ext)
{
    memcpy(pre_shot_ext, m_preShot, sizeof(struct camera2_shot_ext));

    return NO_ERROR;
}

uint32_t ExynosCameraRequestManager::getAllRequestCount(void)
{
    Mutex::Autolock l(m_requestLock);
    return m_serviceRequests[EXYNOS_REQUEST_TYPE::PREVIEW].size() + m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW].size();
}

uint32_t ExynosCameraRequestManager::getServiceRequestCount(void)
{
    Mutex::Autolock lock(m_requestLock);
    return m_serviceRequests[EXYNOS_REQUEST_TYPE::PREVIEW].size();
}

ExynosCameraRequestSP_sprt_t ExynosCameraRequestManager::registerToRunningList()
{
    status_t ret = NO_ERROR;
    ExynosCameraRequestSP_sprt_t request = NULL;

    ret = m_popFront(request, &m_serviceRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < 0){
        CLOGE("request m_popFront is failed request");
        ret = INVALID_OPERATION;
        return NULL;
    }

    ret = m_push(request, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < 0){
        CLOGE("request m_push is failed request");
        ret = INVALID_OPERATION;
        return NULL;
    }

    ret = m_increasePipelineDepth(&m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret != NO_ERROR)
        CLOGE("Failed to increase the pipeline depth");

    m_callbackSequencer->pushToRunningKeyList(request->getKey());

    return request;
}

status_t ExynosCameraRequestManager::removeFromRunningList(uint32_t frameCount)
{
    status_t ret = NO_ERROR;
    uint32_t key = 0;
    ExynosCameraRequestSP_sprt_t request = NULL;

    ret = m_popKey(&key, frameCount);
    if (ret < NO_ERROR) {
        CLOGE("Failed to m_popKey. frameCount %d",
                frameCount);
        return INVALID_OPERATION;
    }

    ret = m_pop(key, request, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < 0){
        ret = INVALID_OPERATION;
        CLOGE("request m_popFront is failed request");
    } else {
//        m_delete(request);
        request = NULL;
    }

    return ret;
}

ExynosCameraRequestSP_sprt_t ExynosCameraRequestManager::getRunningRequest(uint32_t frameCount)
{
    status_t ret = NO_ERROR;
    uint32_t key = 0;
    ExynosCameraRequestSP_sprt_t request = NULL;

    ret = m_getKey(&key, frameCount);
    if (ret < NO_ERROR) {
        CLOGE("Failed to m_popKey. frameCount %d",
                frameCount);
        return NULL;
    }

    ret = m_get(key, request, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < 0) {
        ret = INVALID_OPERATION;
        CLOGE("request m_popFront is failed request");
    }

    return request;
}

status_t ExynosCameraRequestManager::flush()
{
    status_t ret = NO_ERROR;
    int count = 0;

    while (true) {
        count++;
        if (m_serviceRequests[0].size() == 0 && m_runningRequests[0].size() == 0)
            break;
        CLOGD("m_serviceRequest size(%d) m_runningRequest size(%d) count(%d)",
               m_serviceRequests[0].size(), m_runningRequests[0].size(), count);

        usleep(50000);

        if (count == 200)
            break;
    }

    for (int i = 0 ; i < EXYNOS_REQUEST_TYPE::MAX ; i++) {
        m_serviceRequests[i].clear();
        m_runningRequests[i].clear();
    }

    m_requestFrameCountMap.clear();

    m_callbackFlushTimer.start();
    m_deleteStreamThread(&m_streamthreadMap, &m_streamthreadMapLock);
    m_streamthreadMap.clear();

    for (int i = 0 ; i < (HAL_STREAM_ID_MAX * 5); i++) {
        if (m_threadStreamDoneQ[i] != NULL) {
            delete m_threadStreamDoneQ[i];
            m_threadStreamDoneQ[i] = NULL;
        }
        m_streamPrevPos[i] = 0;
    }

    m_callbackFlushTimer.stop();
    long long FlushTime = m_callbackFlushTimer.durationMsecs();
    CLOGV("flush time(%lld)", FlushTime);

    if (m_notifyCallbackThread != NULL) {
        m_notifyCallbackThread->requestExit();
        m_notifyQ.wakeupAll();
        m_notifyCallbackThread->requestExitAndWait();
        m_notifyQ.release();
    }

    if (m_partial3AACallbackThread != NULL) {
        m_partial3AACallbackThread->requestExit();
        m_partial3AAQ.wakeupAll();
        m_partial3AACallbackThread->requestExitAndWait();
        m_partial3AAQ.release();
    }

    if (m_metaCallbackThread != NULL) {
        m_metaCallbackThread->requestExit();
        m_metaQ.wakeupAll();
        m_metaCallbackThread->requestExitAndWait();
        m_metaQ.release();
    }

    if (m_callbackSequencer != NULL)
        m_callbackSequencer->flush();
    return ret;
}

status_t ExynosCameraRequestManager::m_getKey(uint32_t *key, uint32_t frameCount)
{
    status_t ret = NO_ERROR;
    RequestFrameCountMapIterator iter;

    m_requestFrameCountMapLock.lock();
    iter = m_requestFrameCountMap.find(frameCount);
    if (iter != m_requestFrameCountMap.end()) {
        *key = iter->second;
    } else {
        CLOGE("get request key is failed. request for framecount(%d) is not EXIST",
                frameCount);
        ret = INVALID_OPERATION;
    }
    m_requestFrameCountMapLock.unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_popKey(uint32_t *key, uint32_t frameCount)
{
    status_t ret = NO_ERROR;
    RequestFrameCountMapIterator iter;

    m_requestFrameCountMapLock.lock();
    iter = m_requestFrameCountMap.find(frameCount);
    if (iter != m_requestFrameCountMap.end()) {
        *key = iter->second;
        m_requestFrameCountMap.erase(iter);
    } else {
        CLOGE("get request key is failed. request for framecount(%d) is not EXIST",
                frameCount);
        ret = INVALID_OPERATION;
    }
    m_requestFrameCountMapLock.unlock();

    return ret;
}

uint32_t ExynosCameraRequestManager::m_generateResultKey()
{
    m_requestResultKeyLock.lock();
    uint32_t key = m_requestResultKey++;
    m_requestResultKeyLock.unlock();
    return key;
}

uint32_t ExynosCameraRequestManager::m_getResultKey()
{
    m_requestResultKeyLock.lock();
    uint32_t key = m_requestResultKey;
    m_requestResultKeyLock.unlock();
    return key;
}

ResultRequest ExynosCameraRequestManager::createResultRequest(uint32_t frameCount,
                                                            EXYNOS_REQUEST_RESULT::TYPE type,
                                                            camera3_capture_result_t *captureResult,
                                                            camera3_notify_msg_t *notifyMsg)
{
    status_t ret = NO_ERROR;
    uint32_t key = 0;
    ResultRequest request;

    ret = m_getKey(&key, frameCount);
    if (ret < NO_ERROR) {
        CLOGE("m_getKey is failed. framecount(%d)", frameCount);
        return NULL;
    }

    request = new ExynosCamera3RequestResult(m_generateResultKey(), key, type, captureResult, notifyMsg);

    return request;
}

status_t ExynosCameraRequestManager::setCallbackOps(const camera3_callback_ops *callbackOps)
{
    status_t ret = NO_ERROR;
    m_callbackOps = callbackOps;
    return ret;
}

status_t ExynosCameraRequestManager::registerToResultList(ResultRequest result)
{
    status_t ret = NO_ERROR;

    ExynosCameraRequestSP_sprt_t request = NULL;
    int id = 0;
    uint key = 0;

    ret = m_get(result->getFrameCount(), request, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < NO_ERROR) {
        CLOGE("m_get is failed. requestKey(%d)",
                result->getFrameCount());
        return ret;
    }
    CLOGV("type(%d) key(%u) frameCount(%u) ",
            result->getType(), result->getKey(), result->getFrameCount());

    switch(result->getType()){
    case EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY:
        request->pushToResultList(result);
        m_notifyQ.pushProcessQ(&key);
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY:
        /* Output Buffers will be updated finally */
        request->pushToResultList(result);
        m_notifyQ.pushProcessQ(&key);
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA:
        request->pushToResultList(result);
        m_notifyQ.pushProcessQ(&key);
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT:
        request->pushToResultList(result);
        m_notifyQ.pushProcessQ(&key);
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_INVALID:
    default:
        CLOGE("callbackRequest type have INVALID value type(%d) key(%u) frameCount(%u) ",
                result->getType(), result->getKey(), result->getFrameCount());
        break;
    }

    return ret;
}

void ExynosCameraRequestManager::callbackSequencerLock()
{
    /* do not use camera sequence lock / capture sequence change thread logic  */
    /* m_callbackSequencerLock.lock(); */
}

void ExynosCameraRequestManager::callbackSequencerUnlock()
{
    /* do not use camera sequence lock / capture sequence change thread logic  */
    /* m_callbackSequencerLock.unlock(); */
}

status_t ExynosCameraRequestManager::setFrameCount(uint32_t frameCount, uint32_t requestKey)
{
    status_t ret = NO_ERROR;
    pair<RequestFrameCountMapIterator, bool> listRet;
    ExynosCameraRequestSP_sprt_t request = NULL;

    m_requestFrameCountMapLock.lock();
    listRet = m_requestFrameCountMap.insert(pair<uint32_t, uint32_t>(frameCount, requestKey));
    if (listRet.second == false) {
        ret = INVALID_OPERATION;
        CLOGE("Failed, requestKey(%d) already exist!!",
                frameCount);
    }
    m_requestFrameCountMapLock.unlock();

    ret = m_get(requestKey, request, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
    if (ret < 0)
        CLOGE("m_get is failed. requestKey(%d)", requestKey);

    request->setFrameCount(frameCount);

    return ret;
}

// Count number of invocation and print FPS for every 30 frames.
void ExynosCameraRequestManager::m_debugCallbackFPS() {
#ifdef CALLBACK_FPS_CHECK
    m_callbackFrameCnt++;
    if(m_callbackFrameCnt == 1) {
        // Initial invocation
        m_callbackDurationTimer.start();
    } else if(m_callbackFrameCnt >= CALLBACK_FPS_CHECK+1) {
        m_callbackDurationTimer.stop();
        long long durationTime = m_callbackDurationTimer.durationMsecs();
/*
        float meanInterval = durationTime / (float)CALLBACK_FPS_CHECK;
        CLOGI("CALLBACK_FPS_CHECK, duration %lld / 30 = %.2f ms. %.2f fps",
             durationTime, meanInterval, 1000 / meanInterval);
*/
        m_callbackFrameCnt = 0;
    }
#endif
}
status_t ExynosCameraRequestManager::m_sendCallbackResult(ResultRequest result)
{
    status_t ret = NO_ERROR;
    camera3_capture_result_t capture_result;
    camera3_notify_msg_t notify_msg;

    CLOGV("type(%d) key(%u) frameCount(%u) ",
             result->getType(), result->getKey(), result->getFrameCount());

    switch(result->getType()){
    case EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY:
        result->getNotifyMsg(&notify_msg);
        m_callbackOpsNotify(&notify_msg);
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY:
        result->getCaptureResult(&capture_result);
        m_callbackOpsCaptureResult(&capture_result);

#if 1
        if (capture_result.output_buffers != NULL) {
            delete[] capture_result.output_buffers;
            capture_result.output_buffers = NULL;
        }
#endif
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA:
        result->getCaptureResult(&capture_result);
        m_callbackOpsCaptureResult(&capture_result);

#if 1
        if (capture_result.result != NULL)
            free((camera_metadata_t *)capture_result.result);

        if (capture_result.output_buffers != NULL) {
            delete[] capture_result.output_buffers;
            capture_result.output_buffers = NULL;
        }

#endif
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT:
        result->getCaptureResult(&capture_result);
        m_callbackOpsCaptureResult(&capture_result);

#if 1
        free((camera_metadata_t *)capture_result.result);
        if (capture_result.output_buffers != NULL) {
            delete[] capture_result.output_buffers;
            capture_result.output_buffers = NULL;
        }
#endif
        break;
    case EXYNOS_REQUEST_RESULT::CALLBACK_INVALID:
    default:
        ret = BAD_VALUE;
        CLOGE("callbackRequest type have INVALID value type(%d) key(%u) frameCount(%u) ",
                 result->getType(), result->getKey(), result->getFrameCount());
        break;
    }

    return ret;
}

status_t ExynosCameraRequestManager::m_callbackOpsCaptureResult(camera3_capture_result_t *result)
{
    status_t ret = NO_ERROR;

#ifdef DEBUG_STREAM_CONFIGURATIONS
    CLOGD("DEBUG_STREAM_CONFIGURATIONS:frame number(%d), #out(%d)",
             result->frame_number, result->num_output_buffers);
#else
    CLOGV("frame number(%d), #out(%d)",
             result->frame_number, result->num_output_buffers);
#endif

    m_callbackOps->process_capture_result(m_callbackOps, result);
    return ret;
}

status_t ExynosCameraRequestManager::m_callbackOpsNotify(camera3_notify_msg_t *msg)
{
    status_t ret = NO_ERROR;

    switch (msg->type) {
    case CAMERA3_MSG_ERROR:
        CLOGW("msg frame(%d) type(%d) errorCode(%d)",
                 msg->message.error.frame_number, msg->type, msg->message.error.error_code);
        m_callbackOps->notify(m_callbackOps, msg);
        break;
    case CAMERA3_MSG_SHUTTER:
        CLOGV("msg frame(%d) type(%d) timestamp(%llu)",
                 msg->message.shutter.frame_number, msg->type, msg->message.shutter.timestamp);
        m_callbackOps->notify(m_callbackOps, msg);
        break;
    default:
        CLOGE("Msg type is invalid (%d)", msg->type);
        ret = BAD_VALUE;
        break;
    }
    return ret;
}

/* Increase the pipeline depth value from each request in running request map */
status_t ExynosCameraRequestManager::m_increasePipelineDepth(RequestInfoMap *map, Mutex *lock)
{
    status_t ret = NO_ERROR;
    RequestInfoMapIterator requestIter;
    ExynosCameraRequestSP_sprt_t request = NULL;
    struct camera2_shot_ext shot_ext;

    lock->lock();
    if (map->size() < 1) {
        CLOGV("map is empty. Skip to increase the pipeline depth");
        ret = NO_ERROR;
        goto func_exit;
    }

    requestIter = map->begin();
    while (requestIter != map->end()) {
        request = requestIter->second;

        request->increasePipelineDepth();
        requestIter++;
    }

func_exit:
    lock->unlock();
    return ret;
}

status_t ExynosCameraRequestManager::createCallbackThreads(camera3_stream_configuration *stream_list)
{
    m_createCallbackThreads(stream_list);

    return OK;
}

status_t ExynosCameraRequestManager::m_createCallbackThreads(camera3_stream_configuration *stream_list)
{
    status_t ret = 0;
    int id = 0;
    int stream_num = 0;
    sp<callbackThread> m_CallbackThread = NULL;
    for (size_t i = 0; i < stream_list->num_streams; i++) {
        id = -1;
        camera3_stream_t *newStream = stream_list->streams[i];
        ExynosCameraStream *privStreamInfo = static_cast<ExynosCameraStream*>(newStream->priv);
        privStreamInfo->getID(&id);
        stream_num = (id / HAL_STREAM_ID_MAX);
        switch (id % HAL_STREAM_ID_MAX) {
        case HAL_STREAM_ID_RAW:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_rawCallbackThreadFunc0, "RawCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_rawCallbackThreadFunc1, "RawCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_rawCallbackThreadFunc2, "RawCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_rawCallbackThreadFunc3, "RawCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_rawCallbackThreadFunc4, "RawCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):Rawstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_ZSL_OUTPUT:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc0, "ZslOutputCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc1, "ZslOutputCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc2, "ZslOutputCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc3, "ZslOutputCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc4, "ZslOutputCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):ZslOutputstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_ZSL_INPUT:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslInputCallbackThreadFunc0, "ZslInputCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslInputCallbackThreadFunc1, "ZslInputCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslInputCallbackThreadFunc2, "ZslInputCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslInputCallbackThreadFunc3, "ZslInputCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_zslInputCallbackThreadFunc4, "ZslInputCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):ZslInputstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_PREVIEW:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_previewCallbackThreadFunc0, "PreviewCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_previewCallbackThreadFunc1, "PreviewCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_previewCallbackThreadFunc2, "PreviewCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_previewCallbackThreadFunc3, "PreviewCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_previewCallbackThreadFunc4, "PreviewCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):Previewstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_VIDEO:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_videoCallbackThreadFunc0, "VideoCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_videoCallbackThreadFunc1, "VideoCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_videoCallbackThreadFunc2, "VideoCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_videoCallbackThreadFunc3, "VideoCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_videoCallbackThreadFunc4, "VideoCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):Videotream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_JPEG:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_jpegCallbackThreadFunc0, "JpegCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_jpegCallbackThreadFunc1, "JpegCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_jpegCallbackThreadFunc2, "JpegCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_jpegCallbackThreadFunc3, "JpegCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_jpegCallbackThreadFunc4, "JpegCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):Jpegstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_CALLBACK:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackThreadFunc0, "YuvCallbackThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackThreadFunc1, "YuvCallbackThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackThreadFunc2, "YuvCallbackThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackThreadFunc3, "YuvCallbackThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackThreadFunc4, "YuvCallbackThread4");
            }
            CLOGV("DEBUG(%s[%d]):Callbackstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        case HAL_STREAM_ID_CALLBACK_STALL:
            if (stream_num == 0) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc0, "YuvCallbackStallThread0");
            } else if (stream_num == 1) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc1, "YuvCallbackStallThread1");
            } else if (stream_num == 2) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc2, "YuvCallbackStallThread2");
            } else if (stream_num == 3) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc3, "YuvCallbackStallThread3");
            } else if (stream_num == 4) {
                m_CallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc4, "YuvCallbackStallThread4");
            }
            CLOGV("DEBUG(%s[%d]):Callbackstream Callbackthread-%d created", __FUNCTION__, __LINE__, stream_num);
            break;
        }
        m_threadStreamDoneQ[id] = new stream_callback_queue_t;
        m_threadStreamDoneQ[id]->setWaitTime(100000000 * 100); // 2 sec
        m_streamPrevPos[id] = 0;
        ret = m_pushStreamThread(id, m_CallbackThread, &m_streamthreadMap, &m_streamthreadMapLock);
        if (ret < 0) {
            CLOGE("ERR(%s[%d]):Stream Number %d is not pushed", __FUNCTION__, __LINE__, id);
            break;
        }
        m_CallbackThread->run(PRIORITY_URGENT_DISPLAY);
    }

    m_notifyCallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_notifyCallbackThreadFunc, "NotifyCallbackThread");
    m_partial3AACallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_partial3AACallbackThreadFunc, "partial3AACallbackThread");
    m_metaCallbackThread = new callbackThread(this, &ExynosCameraRequestManager::m_metaCallbackThreadFunc, "MetaCallbackThread");
    m_requestDeleteThread = new callbackThread(this, &ExynosCameraRequestManager::m_requestDeleteThreadFunc, "RequestDeleteThread");

    m_notifyCallbackThread->run(PRIORITY_URGENT_DISPLAY);
    m_partial3AACallbackThread->run(PRIORITY_URGENT_DISPLAY);
    m_metaCallbackThread->run(PRIORITY_URGENT_DISPLAY);
    m_requestDeleteThread->run(PRIORITY_URGENT_DISPLAY);

    return OK;
}

bool ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_CALLBACK_STALL;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("wait timeout");
        } else {
            CLOGE("wait and pop fail, ret(%d)", ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_CALLBACK_STALL;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_CALLBACK_STALL;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_CALLBACK_STALL;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackStallThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_CALLBACK_STALL;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_rawCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_RAW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_rawCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_RAW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_rawCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_RAW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_rawCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_RAW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_rawCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_RAW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_ZSL_OUTPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_ZSL_OUTPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_ZSL_OUTPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_ZSL_OUTPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslOutputCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_ZSL_OUTPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslInputCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_ZSL_INPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslInputCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_ZSL_INPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslInputCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_ZSL_INPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslInputCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_ZSL_INPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_zslInputCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_ZSL_INPUT;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_previewCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_PREVIEW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_previewCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_PREVIEW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_previewCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_PREVIEW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_previewCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_PREVIEW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_previewCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_PREVIEW;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_jpegCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_JPEG;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_jpegCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_JPEG;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_jpegCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_JPEG;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_jpegCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_JPEG;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_jpegCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_JPEG;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_videoCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_VIDEO;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_videoCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_VIDEO;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_videoCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_VIDEO;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_videoCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_VIDEO;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_videoCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_VIDEO;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackThreadFunc0(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_CALLBACK;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackThreadFunc1(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX + HAL_STREAM_ID_CALLBACK;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackThreadFunc2(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 2 + HAL_STREAM_ID_CALLBACK;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackThreadFunc3(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 3 + HAL_STREAM_ID_CALLBACK;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_yuvCallbackThreadFunc4(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    int index = HAL_STREAM_ID_MAX * 4 + HAL_STREAM_ID_CALLBACK;
    ret = m_threadStreamDoneQ[index]->waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            CLOGV("WARN(%s):wait timeout", __FUNCTION__);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    return m_streamCallbackThreadFunc(index);
}

bool ExynosCameraRequestManager::m_streamCallbackThreadFunc(int id, bool testflag)
{
    status_t ret = NO_ERROR;
    ResultRequest result;
    ResultRequest temp;
    uint32_t key = 0;
    ResultRequestList resultList;
    ResultRequestListIterator resultListIter;
    EXYNOS_REQUEST_RESULT::TYPE cbType;

    ExynosCameraRequestSP_sprt_t reqeust = NULL;

    CallbackListkeys keyList;
    CallbackListkeysIter keyListIter;

    resultList.clear();

    ExynosCameraRequestSP_sprt_t camera3Request = NULL;

    m_callbackSequencer->getRunningKeyList(&keyList);

    keyListIter = keyList.begin();
    while (keyListIter != keyList.end()) {
        CLOGV("(*keyListIter)(%d) size(%d) STID(%d)",
            (uint32_t)(*keyListIter), keyList.size(), id);

        key = (uint32_t)(*keyListIter);
        if (m_streamPrevPos[id] > key) {
            keyListIter++;
            continue;
        }

        ret = m_get(key, reqeust, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
        if (ret < 0) {
            CLOGE("m_get is failed. requestKey(%d)", key);
            keyListIter++;
            continue;
        }
        camera3Request = reqeust;

        /* Check BUFFER Case */
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY;
        if (reqeust->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) == true) {
            /* Output Buffers will be updated finally */

            /* 1. check request by streamId */
            if (reqeust->isExistStream(id)) {
                ret = reqeust->popAndEraseResultsByTypeAndStreamId(cbType, id, &resultList);
                if (ret < 0) {
                    CLOGE("popRequest frameCount(%u) type(%d) resultList Size(%zu)",
                            reqeust->getFrameCount(), cbType, resultList.size());
                }

                if (resultList.size() > 0) {
                    CLOGV("frameCount(%u), size of list(%zu)",
                        reqeust->getFrameCount(), resultList.size());
                } else {
                    CLOGV("callbackList.size error frameCount(%u), size of list(%zu)",
                        reqeust->getFrameCount(), resultList.size());
                    m_streamPrevPos[id] = key;
                    break;
                }

                for (resultListIter = resultList.begin(); resultListIter != resultList.end(); resultListIter++) {
                    result = *resultListIter;
                    /* Output Buffers will be updated finally */
                    CLOGV("callbackType(%d) frameCount(%u) streamId(%d)",
                            cbType, reqeust->getFrameCount(), id);
                    camera3Request->increasecallbackBufferCount();
                    temp = m_callbackOutputBuffers(reqeust, result);
                    m_sendCallbackResult(temp);
                    m_streamPrevPos[id] = key;
                    camera3Request->increaseCompleteBufferCount();
                    reqeust->setCallbackDone(cbType, true);
                    m_requestDeleteQ.pushProcessQ(&key);
                }
            } else {
                CLOGV("streamId(%d) is not exist in callbackReq", id);
                m_streamPrevPos[id] = key;
            }
        } else {
            CLOGV("NotifyCallback is not delivered");
            m_streamPrevPos[id] = key;
            break;
        }
        keyListIter++;
    }
    return true;
}

status_t ExynosCameraRequestManager::m_pushStreamThread(int key,
                                                    sp<callbackThread> item,
                                                    StreamThreadMap *list,
                                                    Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<StreamThreadMap::iterator,bool> listRet;
    lock->lock();
    CLOGI("key(%d) ", key);
    listRet = list->insert( pair<uint32_t, sp<callbackThread>>(key, item));
    if (listRet.second == false) {
        ret = INVALID_OPERATION;
        CLOGE("m_push failed, request already exist!! Request frameCnt( %d )", key);
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_popStreamThread(int key, sp<callbackThread> item, StreamThreadMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    pair<StreamThreadMap::iterator,bool> listRet;
    StreamThreadMapIterator iter;
    sp<callbackThread> callbackthread = NULL;

    lock->lock();
    iter = list->find(key);
    if (iter != list->end()) {
        callbackthread = iter->second;
        item = callbackthread;
        list->erase( iter );
    } else {
        CLOGE("m_pop failed, factory is not EXIST Request frameCnt( %d )", key);
        ret = INVALID_OPERATION;
    }
    lock->unlock();

    return ret;
}

status_t ExynosCameraRequestManager::m_deleteStreamThread(StreamThreadMap *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    StreamThreadMapIterator iter;
    sp<callbackThread> temp;
    int id = 0;

    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin(); iter != list->end();) {
            temp = iter->second;
            temp->requestExit();

            id = iter->first;
            m_threadStreamDoneQ[id]->wakeupAll();

            temp->requestExitAndWait();
            list->erase(iter++);
            CLOGD("FLUSH list size(%zu)",list->size());
        }
    } else {
        ret = INVALID_OPERATION;
        CLOGE("failed, StreamThreadMap size is ZERO, size(%zu)",
            list->size());
    }

    lock->unlock();

    CLOGV("size(%zu)", list->size());

    return ret;
}

ResultRequest ExynosCameraRequestManager::m_callbackOutputBuffers(ExynosCameraRequestSP_sprt_t request, ResultRequest result)
{
    status_t ret = NO_ERROR;
    camera3_stream_buffer_t *output_buffers;
    int bufferIndex = -2;
    ResultRequestkeys keys;
    camera3_stream_buffer_t streamBuffer;
    camera3_capture_result_t requestResult;
    camera3_stream_t *newStream = NULL;
    ExynosCameraStream *privStreamInfo = NULL;
    int id = 0;

    CLOGV("frameCount(%d), EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ALL",
            request->getFrameCount());

    /* make output stream buffers */
    output_buffers = new camera3_stream_buffer[request->getNumOfOutputBuffer()];
    request->getResultKeys(&keys, EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY);
    bufferIndex = 0;

    result->getStreamBuffer(&streamBuffer);
    output_buffers[bufferIndex] = streamBuffer;

    newStream = streamBuffer.stream;
    privStreamInfo = static_cast<ExynosCameraStream*>(newStream->priv);
    privStreamInfo->getID(&id);

    /* update pipeline depth */
    request->updatePipelineDepth();

    /* construct result for service */
    requestResult.frame_number = request->getKey();
    requestResult.result = NULL;
    requestResult.num_output_buffers = 1;
    requestResult.output_buffers = output_buffers;
    requestResult.input_buffer = request->getInputBuffer();
    requestResult.partial_result = 1;

    ResultRequest resultRequest = NULL;
    CLOGV("frame number(%d), #out(%d)",
            requestResult.frame_number, requestResult.num_output_buffers);

    resultRequest = this->createResultRequest(request->getFrameCount(),
                                            EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY,
                                            &requestResult,
                                            NULL);
    return resultRequest;
}

bool ExynosCameraRequestManager::m_notifyCallbackThreadFunc(void)
{
    m_NotifyCallback();
    usleep(1*1000);

    return true;
}

status_t ExynosCameraRequestManager::m_NotifyCallback()
{
    status_t ret = NO_ERROR;
    ResultRequest result;
    uint32_t key = 0;
    int32_t id = 0;
    ResultRequestList resultList;
    ResultRequestListIterator resultListIter;
    EXYNOS_REQUEST_RESULT::TYPE cbType;

    ExynosCameraRequestSP_sprt_t request = NULL;

    CallbackListkeys keyList;
    CallbackListkeysIter keyListIter;

    resultList.clear();

    camera3_stream_t *newStream;
    camera3_stream_buffer_t streamBuffer;

    ExynosCameraStream *privStreamInfo = NULL;

    /* m_callbackSequencerLock.lock(); */

    /* m_callbackSequencer->dumpList(); */

    if (m_callbackSequencer == NULL) {
        CLOGE("m_callbackSequencer is NULL");
        return ret;
    }

    if (m_callbackSequencer->getRunningKeyListSize() <= 0) {
        CLOGV("m_callbackSequencer is empty");
        return ret;
    }

    m_callbackSequencer->getRunningKeyList(&keyList);

    keyListIter = keyList.begin();
    while (keyListIter != keyList.end()) {

        key = (uint32_t)(*keyListIter);
        ret = m_get(key, request, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
        if (ret < 0) {
            CLOGE("m_get is failed. requestKey(%d)", key);
            keyListIter++;
            continue;
        }

        CLOGV("frameCount(%u)", request->getFrameCount());
        /* Check NOTIFY Case */
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY;
        if (request->getCallbackDone(cbType) == true) {
            CLOGV("notify already callback done frameCount(%u)", request->getFrameCount());
            //keyListIter++;
            //continue;
        } else {
            ret = request->popAndEraseResultsByType(cbType, &resultList);
            if (ret < 0) {
                CLOGE("popRequest type(%d) resultList Size(%zu)",
                        cbType, resultList.size());
            }

            if (resultList.size() > 0) {
                CLOGV("frameCount(%u), size of list(%zu)",request->getFrameCount(), resultList.size());
                resultListIter = resultList.begin();
                result = *resultListIter;

                m_sendCallbackResult(result);
                request->setCallbackDone(cbType, true);
                m_requestDeleteQ.pushProcessQ(&key);
            } else {
                CLOGV("resultList.size error frameCount(%u), size of list(%zu)",
                    request->getFrameCount(), resultList.size());
                break;
            }
         }

#if 1 // partial 3aa trigger
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA;
        if (request->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) == true) {
            ret = request->popResultsByType(cbType, &resultList);
            if (ret < 0) {
                CLOGE("popRequest type(%d) callbackList Size(%zu)",
                        cbType, resultList.size());
            }

            if (resultList.size() > 0) {
                m_partial3AAQ.pushProcessQ(&key);
            }
        }
#endif

#if 1 // meta trigger
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT;
        if (request->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) == true) {
            ret = request->popResultsByType(cbType, &resultList);
            if (ret < 0) {
                CLOGE("popRequest type(%d) resultList Size(%zu)",
                        cbType, resultList.size());
            }

            if (resultList.size() > 0) {
                m_metaQ.pushProcessQ(&key);
            }
        }
#endif

#if 1 // buffer trigger
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_BUFFER_ONLY;
        if (request->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) == true) {
            /* Output Buffers will be updated finally */

            /* 1. check request by streamId */
            ret = request->popResultsByType(cbType, &resultList);
            if (ret < 0) {
                CLOGE("popRequest frameCount(%u) type(%d) resultList Size(%zu)",
                        request->getFrameCount(), cbType, resultList.size());
            }

            for (resultListIter = resultList.begin(); resultListIter != resultList.end(); resultListIter++) {
                result = *resultListIter;
                /* Output Buffers will be updated finally */
                // check streamType
                result->getStreamBuffer(&streamBuffer);
                newStream = streamBuffer.stream;
                privStreamInfo = static_cast<ExynosCameraStream*>(newStream->priv);
                privStreamInfo->getID(&id);
                switch (id % HAL_STREAM_ID_MAX) {
                    case HAL_STREAM_ID_RAW:
                    case HAL_STREAM_ID_ZSL_OUTPUT:
                    case HAL_STREAM_ID_ZSL_INPUT:
                    case HAL_STREAM_ID_PREVIEW:
                    case HAL_STREAM_ID_VIDEO:
                    case HAL_STREAM_ID_JPEG:
                    case HAL_STREAM_ID_CALLBACK:
                    case HAL_STREAM_ID_CALLBACK_STALL:
                        CLOGV("streamdId(%d) is running", id);
                        m_threadStreamDoneQ[id]->pushProcessQ(&key);
                        break;
                }
            }
        }
#endif
        keyListIter++;
    }
    return ret;
}

bool ExynosCameraRequestManager::m_partial3AACallbackThreadFunc(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    ret = m_partial3AAQ.waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            /* CLOGW("WARN(%s):wait timeout", __FUNCTION__); */
            CLOGV("ERR(%s):fail, ret(%d)", __FUNCTION__, ret);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    m_3AAMetaCallback();
    return true;
}

status_t ExynosCameraRequestManager::m_3AAMetaCallback()
{
    status_t ret = NO_ERROR;
    ResultRequest result;
    uint32_t key = 0;
    ResultRequestList resultList;
    ResultRequestListIterator resultListIter;
    EXYNOS_REQUEST_RESULT::TYPE cbType;

    ExynosCameraRequestSP_sprt_t reqeust = NULL;

    CallbackListkeys keyList;
    CallbackListkeysIter keyListIter;

    resultList.clear();

    ExynosCameraRequestSP_sprt_t camera3Request = NULL;
    camera3_stream_t *newStream;
    camera3_stream_buffer_t streamBuffer;

    /* m_callbackSequencerLock.lock(); */

    /* m_callbackSequencer->dumpList(); */

    m_callbackSequencer->getRunningKeyList(&keyList);

    keyListIter = keyList.begin();
    while (keyListIter != keyList.end()) {
        CLOGV("(*keyListIter)(%d) size(%d)",(uint32_t)(*keyListIter), keyList.size());

        key = (uint32_t)(*keyListIter);
        ret = m_get(key, reqeust, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
        if (ret < 0) {
            CLOGE("m_get is failed. requestKey(%d)", key);
            keyListIter++;
            continue;
//            break;
        }

        CLOGV("frameCount(%u)",  reqeust->getFrameCount());

        /* Check ALL_RESULT Case */
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA;
        if (reqeust->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) == true) {
            if (reqeust->getCallbackDone(cbType) == false) {
                ret = reqeust->popAndEraseResultsByType(cbType, &resultList);
                if (ret < 0) {
                    CLOGE("popRequest type(%d) resultList Size(%zu)",
                            cbType, resultList.size());
                }

                if (resultList.size() > 0) {
                    CLOGV("frameCount(%u), size of list(%zu)",reqeust->getFrameCount(), resultList.size());
                } else {
                    CLOGV("resultList.size error frameCount(%u), size of list(%zu)",
                        reqeust->getFrameCount(), resultList.size());
                    break;
                }

                for (resultListIter = resultList.begin(); resultListIter != resultList.end(); resultListIter++) {
                    CLOGV("callbackType(%d) frameCount(%u)", cbType, reqeust->getFrameCount());
                    result = *resultListIter;
                    m_sendCallbackResult(result);
                    reqeust->setCallbackDone(cbType, true);
                    m_requestDeleteQ.pushProcessQ(&key);
                }
            }
        } else {
            CLOGV("NotifyCallback is not delivered");
            break;
        }
        keyListIter++;
    }
    return ret;
}

bool ExynosCameraRequestManager::m_requestDeleteThreadFunc(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    ret = m_requestDeleteQ.waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            /* CLOGW("WARN(%s):wait timeout", __FUNCTION__); */
            CLOGV("fail, ret(%d)", ret);
        } else {
            CLOGE("wait and pop fail, ret(%d)", ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    ExynosCameraRequestSP_sprt_t reqeust = NULL;

    CallbackListkeys keyList;
    CallbackListkeysIter keyListIter;

    ExynosCameraRequestSP_sprt_t camera3Request = NULL;

    m_callbackSequencer->getRunningKeyList(&keyList);

    keyListIter = keyList.begin();
    while (keyListIter != keyList.end()) {
        CLOGV("(*keyListIter)(%d) size(%d)", (uint32_t)(*keyListIter), keyList.size());

        key = (uint32_t)(*keyListIter);
        ret = m_get(key, reqeust, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
        if (ret < 0) {
            CLOGE("m_get is failed. requestKey(%d)", key);
            keyListIter++;
            continue;
        }

        camera3Request = reqeust;
        CLOGV("frameCount(%u)", reqeust->getFrameCount());
        keyListIter++;
#if 1
        if (reqeust->isComplete() &&
          (int)camera3Request->getNumOfOutputBuffer() == camera3Request->getCompleteBufferCount()) {
            CLOGV("removeFromRunningList frameCount(%u)", reqeust->getFrameCount());
            m_callbackSequencer->deleteRunningKey(camera3Request->getKey());
            removeFromRunningList(camera3Request->getFrameCount());
            m_callbackTraceCount = 0;
        } else {
            CLOGV("Not delete status frameCount(%u)", reqeust->getFrameCount());
            //break;
        }
#endif
    }

    return true;
}

bool ExynosCameraRequestManager::m_metaCallbackThreadFunc(void)
{
    status_t ret = NO_ERROR;
    uint32_t key;
    ret = m_metaQ.waitAndPopProcessQ(&key);
    if (ret < 0) {
        /* TODO: We need to make timeout duration depends on FPS */
        if (ret == TIMED_OUT) {
            /* CLOGW("WARN(%s):wait timeout", __FUNCTION__); */
            CLOGV("ERR(%s):fail, ret(%d)", __FUNCTION__, ret);
        } else {
            CLOGE("ERR(%s):wait and pop fail, ret(%d)", __FUNCTION__, ret);
            /* TODO: doing exception handling */
        }
        return true;
    }

    m_MetaCallback();
    return true;
}

status_t ExynosCameraRequestManager::m_MetaCallback()
{
    status_t ret = NO_ERROR;
    ResultRequest result;
    uint32_t key = 0;
    ResultRequestList resultList;
    ResultRequestListIterator resultListIter;
    EXYNOS_REQUEST_RESULT::TYPE cbType;

    ExynosCameraRequestSP_sprt_t reqeust = NULL;

    CallbackListkeys keyList;
    CallbackListkeysIter keyListIter;

    resultList.clear();

    camera3_stream_t *newStream;
    camera3_stream_buffer_t streamBuffer;

    /* m_callbackSequencerLock.lock(); */

    /* m_callbackSequencer->dumpList(); */

    m_callbackSequencer->getRunningKeyList(&keyList);

    keyListIter = keyList.begin();
    while (keyListIter != keyList.end()) {
        CLOGV("(*keyListIter)(%d) size(%d)", (uint32_t)(*keyListIter), keyList.size());

        key = (uint32_t)(*keyListIter);
        ret = m_get(key, reqeust, &m_runningRequests[EXYNOS_REQUEST_TYPE::PREVIEW], &m_requestLock);
        if (ret < 0) {
            CLOGE("m_get is failed. requestKey(%d)", key);
            keyListIter++;
            continue;
        }

        /* Check ALL_RESULT Case */
        cbType = EXYNOS_REQUEST_RESULT::CALLBACK_ALL_RESULT;
        if (reqeust->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_NOTIFY_ONLY) == true &&
            reqeust->getCallbackDone(EXYNOS_REQUEST_RESULT::CALLBACK_PARTIAL_3AA) == true) {
            if (reqeust->getCallbackDone(cbType) == false) {
                ret = reqeust->popAndEraseResultsByType(cbType, &resultList);
                if (ret < 0) {
                    CLOGE("popRequest type(%d) resultList Size(%zu)",
                            cbType, resultList.size());
                }

                if (resultList.size() > 0) {
                    CLOGV("frameCount(%u), size of list(%zu)",
                        reqeust->getFrameCount(), resultList.size());
                } else {
                    CLOGV("resultList.size error frameCount(%u), size of list(%zu)",
                        reqeust->getFrameCount(), resultList.size());
                    break;
                }

                for (resultListIter = resultList.begin(); resultListIter != resultList.end(); resultListIter++) {
                    CLOGV("callbackType(%d) frameCount(%u)",
                        cbType, reqeust->getFrameCount());
                    result = *resultListIter;
                    m_sendCallbackResult(result);
                    reqeust->setCallbackDone(cbType, true);
                    m_requestDeleteQ.pushProcessQ(&key);
                }
            }
        } else {
            CLOGV("NotifyCallback and 3AA callback is not delivered");
            break;
        }
        keyListIter++;
    }

    return ret;
}

status_t ExynosCameraRequestManager::waitforRequestflush()
{
    int count = 0;
    while (true) {
        count++;
        if (m_serviceRequests[0].size() == 0 && m_runningRequests[0].size() == 0)
            break;

        ALOGV("DEBUG(%s[%d]):m_serviceRequest size(%d) m_runningRequest size(%d) count(%d)",
                __FUNCTION__, __LINE__, m_serviceRequests[0].size(), m_runningRequests[0].size(), count);

        usleep(50000);

        if (count == 200)
            break;
    }

    return 0;
}

ExynosCameraCallbackSequencer::ExynosCameraCallbackSequencer()
{
    Mutex::Autolock lock(m_requestCbListLock);
    m_runningRequestKeys.clear();
}

ExynosCameraCallbackSequencer::~ExynosCameraCallbackSequencer()
{
    if (m_runningRequestKeys.size() > 0) {
        CLOGE2("destructor size is not ZERO(%zu)",
                 m_runningRequestKeys.size());
    }
}

uint32_t ExynosCameraCallbackSequencer::popFromRunningKeyList()
{
    status_t ret = NO_ERROR;
    uint32_t obj;

    obj = m_pop(EXYNOS_LIST_OPER::SINGLE_FRONT, &m_runningRequestKeys, &m_requestCbListLock);
    if (ret < 0){
        CLOGE2("m_get failed");
        return 0;
    }

    return obj;
}

status_t ExynosCameraCallbackSequencer::pushToRunningKeyList(uint32_t key)
{
    status_t ret = NO_ERROR;

    ret = m_push(EXYNOS_LIST_OPER::SINGLE_BACK, key, &m_runningRequestKeys, &m_requestCbListLock);
    if (ret < 0){
        CLOGE2("m_push failed, key(%d)",
                 key);
    }

    return ret;
}

uint32_t ExynosCameraCallbackSequencer::getRunningKeyListSize()
{
    Mutex::Autolock lock(m_requestCbListLock);
    return m_runningRequestKeys.size();
}

status_t ExynosCameraCallbackSequencer::getRunningKeyList(CallbackListkeys *list)
{
    status_t ret = NO_ERROR;

    list->clear();

    m_requestCbListLock.lock();
    std::copy(m_runningRequestKeys.begin(), m_runningRequestKeys.end(), std::back_inserter(*list));
    m_requestCbListLock.unlock();
    return ret;
}

status_t ExynosCameraCallbackSequencer::deleteRunningKey(uint32_t key)
{
    status_t ret = NO_ERROR;

    ret = m_delete(key, &m_runningRequestKeys, &m_requestCbListLock);
    if (ret < 0){
        CLOGE2("m_push failed, key(%d)", key);
    }

    return ret;
}

void ExynosCameraCallbackSequencer::dumpList()
{
    CallbackListkeysIter iter;
    CallbackListkeys *list = &m_runningRequestKeys;

    m_requestCbListLock.lock();

    if (list->size() > 0) {
        for (iter = list->begin(); iter != list->end();) {
            CLOGE2("key(%d), size(%zu)", *iter, list->size());
            iter++;
        }
    } else {
        CLOGE2("m_getCallbackResults failed, size is ZERO, size(%zu)",
                 list->size());
    }

    m_requestCbListLock.unlock();

}

status_t ExynosCameraCallbackSequencer::flush()
{
    Mutex::Autolock lock(m_requestCbListLock);
    status_t ret = NO_ERROR;

    m_runningRequestKeys.clear();
    return ret;
}

status_t ExynosCameraCallbackSequencer::m_init()
{
    Mutex::Autolock lock(m_requestCbListLock);
    status_t ret = NO_ERROR;

    m_runningRequestKeys.clear();
    return ret;
}

status_t ExynosCameraCallbackSequencer::m_deinit()
{
    Mutex::Autolock lock(m_requestCbListLock);
    status_t ret = NO_ERROR;

    m_runningRequestKeys.clear();
    return ret;
}

status_t ExynosCameraCallbackSequencer::m_push(EXYNOS_LIST_OPER::MODE operMode,
                                                uint32_t key,
                                                CallbackListkeys *list,
                                                Mutex *lock)
{
    status_t ret = NO_ERROR;

    lock->lock();

    switch (operMode) {
    case EXYNOS_LIST_OPER::SINGLE_BACK:
        list->push_back(key);
        break;
    case EXYNOS_LIST_OPER::SINGLE_FRONT:
        list->push_front(key);
        break;
    case EXYNOS_LIST_OPER::SINGLE_ORDER:
    case EXYNOS_LIST_OPER::MULTI_GET:
    default:
        ret = INVALID_OPERATION;
        CLOGE2("m_push failed, mode(%d) size(%zu)",
                 operMode, list->size());
        break;
    }

    lock->unlock();

    CLOGV2("m_push(%d), size(%zu)", key, list->size());

    return ret;
}

uint32_t ExynosCameraCallbackSequencer::m_pop(EXYNOS_LIST_OPER::MODE operMode, CallbackListkeys *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    CallbackListkeysIter iter;
    uint32_t obj = 0;

    lock->lock();

    switch (operMode) {
    case EXYNOS_LIST_OPER::SINGLE_BACK:
        if (list->size() > 0) {
            obj = list->back();
            list->pop_back();
        } else {
            CLOGE2("m_pop failed, size(%zu)", list->size());
            ret = INVALID_OPERATION;
        }
        break;
    case EXYNOS_LIST_OPER::SINGLE_FRONT:
        if (list->size() > 0) {
            obj = list->front();
            list->pop_front();
        } else {
            CLOGE2("m_pop failed, size(%zu)", list->size());
            ret = INVALID_OPERATION;
        }
        break;
    case EXYNOS_LIST_OPER::SINGLE_ORDER:
    case EXYNOS_LIST_OPER::MULTI_GET:
    default:
        ret = INVALID_OPERATION;
        obj = 0;
        CLOGE2("m_push failed, mode(%d) size(%zu)",
                 operMode, list->size());
        break;
    }

    lock->unlock();

    CLOGI2("m_pop(%d), size(%zu)", obj, list->size());

    return obj;
}

status_t ExynosCameraCallbackSequencer::m_delete(uint32_t key, CallbackListkeys *list, Mutex *lock)
{
    status_t ret = NO_ERROR;
    CallbackListkeysIter iter;

    lock->lock();

    if (list->size() > 0) {
        for (iter = list->begin(); iter != list->end();) {
            if (key == (uint32_t)*iter) {
                list->erase(iter++);
                CLOGV2("key(%d), size(%zu)",
                         key, list->size());
            } else {
                iter++;
            }
        }
    } else {
        ret = INVALID_OPERATION;
        CLOGE2("m_getCallbackResults failed, size is ZERO, size(%zu)",
                 list->size());
    }

    lock->unlock();

    CLOGV2("size(%zu)", list->size());

    return ret;
}

}; /* namespace android */
