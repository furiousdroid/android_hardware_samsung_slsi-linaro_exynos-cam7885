# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)
CAMERA_PATH := $(TOP)/hardware/samsung_slsi-linaro/exynos/libcamera
CAMERA_SRC_PATH := ..

#################
# libexynoscamera3

include $(CLEAR_VARS)

ifndef TARGET_SOC_BASE
TARGET_SOC_BASE = $(TARGET_SOC)
endif

######## System LSI ONLY ########
BOARD_CAMERA_GED_FEATURE := true
#################################
# LOCAL_CFLAGS += -DUSE_CAMERA_EXYNOS7885_META

LOCAL_PRELINK_MODULE := false
LOCAL_PROPRIETARY_MODULE := true
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw

# LOCAL_STATIC_LIBRARIES := android.hardware.camera.common@1.0-helper
LOCAL_SHARED_LIBRARIES:= libutils libcutils liblog libui libcamera_metadata libutilscallstack libgui_vendor libbinder libbase libbinder_ndk libhidlbase
LOCAL_SHARED_LIBRARIES += android.hardware.graphics.bufferqueue@1.0 android.hardware.graphics.bufferqueue@2.0 android.hidl.token@1.0-utils android.frameworks.cameraservice.service@2.0
LOCAL_SHARED_LIBRARIES += libexynosutils libhwjpeg libexynosv4l2 libsync libcsc libacryl
LOCAL_SHARED_LIBRARIES += libdl libexynosgraphicbuffer
LOCAL_HEADER_LIBRARIES := libhardware_headers libcameraservice_headers media_plugin_headers libgui_aidl_headers libacryl_headers libgui_flags libbase_headers
ifeq ($(BOARD_CAMERA_USE_HFD), true)
LOCAL_SHARED_LIBRARIES_arm := libhfd
endif

LOCAL_CFLAGS += -Wno-error=date-time
LOCAL_CFLAGS += -Wno-overloaded-virtual
LOCAL_CFLAGS += -Wno-unused-parameter
LOCAL_CFLAGS += -Wno-date-time
LOCAL_CFLAGS += -Wno-unused-variable
LOCAL_CFLAGS += -Wno-implicit-fallthrough
LOCAL_CFLAGS += -Wno-error=implicit-fallthrough
LOCAL_CFLAGS += -Wno-error=macro-redefined
LOCAL_CFLAGS += -Wno-error=integer-overflow
LOCAL_CFLAGS += -Wno-error=braced-scalar-init

LOCAL_CFLAGS += -DMAIN_CAMERA_SENSOR_NAME=$(BOARD_BACK_CAMERA_SENSOR)
$(warning MAIN_CAMERA_SENSOR_NAME is $(BOARD_BACK_CAMERA_SENSOR))
LOCAL_CFLAGS += -DFRONT_CAMERA_SENSOR_NAME=$(BOARD_FRONT_CAMERA_SENSOR)
$(warning FRONT_CAMERA_SENSOR_NAME is $(BOARD_FRONT_CAMERA_SENSOR))
LOCAL_CFLAGS += -DSECURE_CAMERA_SENSOR_NAME=$(BOARD_SECURE_CAMERA_SENSOR)
LOCAL_CFLAGS += -DUSE_CAMERA_ESD_RESET
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)
LOCAL_CFLAGS += -DSECURE_ROTATION=$(BOARD_SECURE_CAMERA_ROTATION)
ifneq ($(BOARD_BACK_1_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DBACK_1_CAMERA_SENSOR_NAME=$(BOARD_BACK_1_CAMERA_SENSOR)
$(warning BACK_1_CAMERA_SENSOR_NAME is $(BOARD_BACK_1_CAMERA_SENSOR))
endif
ifneq ($(BOARD_FRONT_1_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DFRONT_1_CAMERA_SENSOR_NAME=$(BOARD_FRONT_1_CAMERA_SENSOR)
endif
ifneq ($(BOARD_BACK_2_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DBACK_2_CAMERA_SENSOR_NAME=$(BOARD_BACK_2_CAMERA_SENSOR)
$(warning BOARD_BACK_2_CAMERA_SENSOR is $(BOARD_BACK_2_CAMERA_SENSOR))
endif
ifneq ($(BOARD_FRONT_2_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DFRONT_2_CAMERA_SENSOR_NAME=$(BOARD_FRONT_2_CAMERA_SENSOR)
endif
ifneq ($(BOARD_BACK_3_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DBACK_3_CAMERA_SENSOR_NAME=$(BOARD_BACK_3_CAMERA_SENSOR)
endif
ifneq ($(BOARD_FRONT_3_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DFRONT_3_CAMERA_SENSOR_NAME=$(BOARD_FRONT_3_CAMERA_SENSOR)
endif

ifeq ($(BOARD_BACK_1_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_1=2
endif
ifeq ($(BOARD_FRONT_1_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_1=3
endif
ifeq ($(BOARD_BACK_2_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_2=4
endif
ifeq ($(BOARD_FRONT_2_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_2=5
endif
ifeq ($(BOARD_BACK_3_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_3=6
endif
ifeq ($(BOARD_FRONT_3_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_3=7
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_CFLAGS += -DUSE_DUAL_CAMERA
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_ZOOM_MASTER)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_ZOOM_MASTER=$(BOARD_DUAL_CAMERA_REAR_ZOOM_MASTER)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_ZOOM_SLAVE)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_ZOOM_SLAVE=$(BOARD_DUAL_CAMERA_REAR_ZOOM_SLAVE)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_PORTRAIT_MASTER)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_PORTRAIT_MASTER=$(BOARD_DUAL_CAMERA_REAR_PORTRAIT_MASTER)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_PORTRAIT_SLAVE)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_PORTRAIT_SLAVE=$(BOARD_DUAL_CAMERA_REAR_PORTRAIT_SLAVE)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_MASTER)))
LOCAL_CFLAGS += -DDUAL_CAMERA_FRONT_PORTRAIT_MASTER=$(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_MASTER)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_SLAVE)))
LOCAL_CFLAGS += -DDUAL_CAMERA_FRONT_PORTRAIT_SLAVE=$(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_SLAVE)
endif
endif

ifeq ($(BOARD_CAMERA_USES_PIPE_HANDLER), true)
LOCAL_CFLAGS += -DUSE_PIPE_HANDLER
endif

ifeq ($(BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS), true)
LOCAL_CFLAGS += -DUSES_SW_VDIS
endif

ifeq ($(BOARD_CAMERA_USES_REMOSAIC_SENSOR), true)
LOCAL_CFLAGS += -DUSE_REMOSAIC_SENSOR
endif

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	system/media/camera/include \
	system/core/libsync/include \
	$(CAMERA_PATH)/7885/hal1 \
	$(CAMERA_PATH)/7885/hal3 \
	$(CAMERA_PATH)/7885/JpegEncoderForCamera \
	$(CAMERA_PATH)/common_v3_7885 \
	$(CAMERA_PATH)/common_v3_7885/Include \
	$(CAMERA_PATH)/common_v3_7885/Activities \
	$(CAMERA_PATH)/common_v3_7885/Buffers \
	$(CAMERA_PATH)/common_v3_7885/Ged \
	$(CAMERA_PATH)/common_v3_7885/MCPipes \
	$(CAMERA_PATH)/common_v3_7885/Pipes2 \
	$(CAMERA_PATH)/common_v3_7885/PostProcessing \
	$(CAMERA_PATH)/common_v3_7885/Sec \
	$(CAMERA_PATH)/common_v3_7885/SensorInfos \
	$(CAMERA_PATH)/common_v3_7885/SizeTables \
	hardware/samsung_slsi-linaro/exynos/include \
	hardware/samsung_slsi-linaro/exynos5/include \
	hardware/samsung_slsi-linaro/graphics/base/libion/include \
	hardware/libhardware_legacy/include/hardware_legacy \
	bionic \
	external/expat/lib \
	frameworks/native/libs/binder/include \
	frameworks/native/libs/ui/include \
	external/libcxx/include \
	frameworks/native/include \
	frameworks/native/libs/arect/include \
	frameworks/native/libs/nativebase/include \
	frameworks/av/include/ 

ifeq ($(BOARD_CAMERA_USES_EFD), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_EARLY_FD
endif

ifeq ($(BOARD_CAMERA_USES_3AA_DNG), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_3AA_DNG
endif

ifeq ($(BOARD_CAMERA_USES_BAYER_COMPRESSION), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_USES_BAYER_COMPRESSION
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Vendor/$(LOCAL_PROJECT_DIR)
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/SensorInfos
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_C_INCLUDES += \
	$(CAMERA_PATH)/common_v3_7885/Fusion \
	$(CAMERA_PATH)/common_v3_7885/Fusion/DofLut
endif

# Camera2 HAL3.2
LOCAL_CFLAGS += -DUSE_CAMERA2_API_SUPPORT
# Use legacy ExynosJpegEncoderForCamera
LOCAL_CFLAGS += -DEXYNOS_JPEG_ENCODER_FOR_CAMERA_GLUE

LOCAL_SRC_FILES = \
	$(CAMERA_SRC_PATH)/7885/JpegEncoderForCamera/ExynosJpegEncoderForCamera.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraFrame.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraMemory.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraFrameManager.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraUtils.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraNode.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraNodeJpegHAL.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraFrameSelector.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraCallbackHooker.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraRequestManager.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraStreamManager.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraMetadataConverter.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraTimeLogger.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/SensorInfos/ExynosCameraSensorInfoBase.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/SensorInfos/ExynosCamera3SensorInfoBase.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/PostProcessing/ExynosCameraPP.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/PostProcessing/ExynosCameraPPLibcsc.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/PostProcessing/ExynosCameraPPGDC.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/PostProcessing/ExynosCameraPPLibacryl.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/PostProcessing/ExynosCameraPPFactory.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/MCPipes/ExynosCameraMCPipe.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipe.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraSWPipe.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeFlite.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeGSC.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeJpeg.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeUVS.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeVRAGroup.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeVRA.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipePP.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeSTK_PICTURE.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeSTK_PREVIEW.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Buffers/ExynosCameraBufferManager.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Buffers/ExynosCameraBufferLocker.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Activities/ExynosCameraActivityBase.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Activities/ExynosCameraActivityAutofocus.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Activities/ExynosCameraActivityFlash.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Activities/ExynosCameraActivitySpecialCapture.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Activities/ExynosCameraActivityUCTL.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/ExynosCameraActivityAutofocusVendor.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/ExynosCameraActivityFlashVendor.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/ExynosCameraFrameSelectorVendor.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/ExynosCameraMetadataConverterVendor.cpp \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/ExynosJpegEncoderForCameraVendor.cpp \
	$(CAMERA_SRC_PATH)/7885/ExynosCameraActivityControl.cpp \
	$(CAMERA_SRC_PATH)/7885/ExynosCameraScalableSensor.cpp \
	$(CAMERA_SRC_PATH)/7885/ExynosCameraUtilsModule.cpp \
	$(CAMERA_SRC_PATH)/7885/hal3/ExynosCameraSizeControl.cpp \
	$(CAMERA_SRC_PATH)/7885/hal3/ExynosCamera3.cpp \
	$(CAMERA_SRC_PATH)/7885/hal3/ExynosCamera3Parameters.cpp \
	$(CAMERA_SRC_PATH)/7885/hal3/ExynosCamera3FrameFactory.cpp \
	$(CAMERA_SRC_PATH)/7885/hal3/ExynosCamera3FrameFactoryPreview.cpp \
	$(CAMERA_SRC_PATH)/7885/hal3/ExynosCamera3FrameReprocessingFactory.cpp 

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_SRC_FILES += \
    $(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraSyncFrameSelector.cpp \
    $(CAMERA_SRC_PATH)/common_v3_7885/ExynosCameraDualFrameSelector.cpp \
    $(CAMERA_SRC_PATH)/common_v3_7885/ExynosCamera3DualInterface.cpp \
    $(CAMERA_SRC_PATH)/common_v3_7885/Pipes2/ExynosCameraPipeSync.cpp
endif

ifeq ($(BOARD_CAMERA_USES_REMOSAIC_SENSOR), true)
LOCAL_SRC_FILES += \
	$(CAMERA_SRC_PATH)/common_v3_7885/PostProcessing/ExynosCameraPPLibRemosaic.cpp
endif

ifneq ($(filter eng userdebug, $(TARGET_BUILD_VARIANT)),)
ifeq ($(BOARD_CAMERA_USES_DEBUG_PROPERTY), true)
LOCAL_CFLAGS += -DUSE_DEBUG_PROPERTY
endif
endif

ifeq ($(BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS), true)
LOCAL_SRC_FILES += \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/SecCameraSWVdis_3_0.cpp
endif

ifeq ($(BOARD_CAMERA_USES_SLSI_VENDOR_TAGS), true)
LOCAL_CFLAGS += -DUSE_SLSI_VENDOR_TAGS
LOCAL_SRC_FILES += \
	$(CAMERA_SRC_PATH)/common_v3_7885/Sec/SecCameraVendorTags.cpp
endif

# TODO -- add sensorlistener source
ifeq ($(BOARD_CAMERA_USES_SENSOR_LISTENER), true)
LOCAL_CFLAGS += -DUSES_SENSOR_LISTENER

LOCAL_SHARED_LIBRARIES += \
    libhardware \
    libsensorndkbridge

LOCAL_C_INCLUDES += \
	$(CAMERA_PATH)/core/src/common_v2/SensorListener

LOCAL_SRC_FILES += \
	$(CAMERA_SRC_PATH)/core/src/common_v2/SensorListener/ExynosCameraSensorListener.cpp \
	$(CAMERA_SRC_PATH)/core/src/common_v2/SensorListener/ExynosCameraSensorListenerDummy.cpp \
	$(CAMERA_SRC_PATH)/core/src/common_v2/SensorListener/ExynosCameraSensorListenerASensor.cpp \
	$(CAMERA_SRC_PATH)/core/src/common_v2/SensorListener/ExynosCameraSensorListenerWrapper.cpp
endif

ifeq ($(BOARD_CAMERA_USES_EXYNOS_APP_FLM), true)
LOCAL_CFLAGS += -DEXYNOS_APP_FLM
endif

ifeq ($(BOARD_CAMERA_USES_EXYNOS_HAL_FLM), true)
LOCAL_CFLAGS += -DEXYNOS_HAL_FLM
endif

$(foreach file,$(LOCAL_SRC_FILES),$(warning $(file)))

LOCAL_LDFLAGS :=  -Wl,-Bsymbolic

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libexynoscamera3

include $(TOP)/hardware/samsung_slsi-linaro/exynos/BoardConfigCFlags.mk
include $(BUILD_SHARED_LIBRARY)


#################
# camera.exynos7885.so

include $(CLEAR_VARS)

ifndef TARGET_SOC_BASE
TARGET_SOC_BASE = $(TARGET_SOC)
endif

######## System LSI ONLY ########
BOARD_CAMERA_GED_FEATURE := true
#################################
LOCAL_CFLAGS += -DUSE_CAMERA_EXYNOS7885_META

# HAL module implemenation stored in
# hw/<COPYPIX_HARDWARE_MODULE_ID>.<ro.product.board>.so
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(TOP)/system/media/camera/include \
	$(TOP)/system/core/libsync/include \
	$(TOP)/system/core/libion/kernel-headers \
	$(CAMERA_PATH)/7885 \
	$(CAMERA_PATH)/7885/hal1 \
	$(CAMERA_PATH)/7885/hal3 \
	$(CAMERA_PATH)/common_v3_7885 \
	$(CAMERA_PATH)/common_v3_7885/Include \
	$(CAMERA_PATH)/common_v3_7885/Activities \
	$(CAMERA_PATH)/common_v3_7885/Buffers \
	$(CAMERA_PATH)/common_v3_7885/Ged \
	$(CAMERA_PATH)/common_v3_7885/MCPipes \
	$(CAMERA_PATH)/common_v3_7885/Pipes2 \
	$(CAMERA_PATH)/common_v3_7885/PostProcessing \
	$(CAMERA_PATH)/common_v3_7885/Sec \
	$(CAMERA_PATH)/common_v3_7885/SensorInfos \
	$(CAMERA_PATH)/common_v3_7885/SizeTables \
	$(TOP)/hardware/samsung_slsi-linaro/exynos/include \
	$(TOP)/hardware/samsung_slsi-linaro/exynos5/include \
	$(TOP)/hardware/samsung_slsi-linaro/graphics/libion/include/hardware/exynos \
	$(TOP)/bionic \
	$(TOP)/external/expat/lib \
	$(TOP)/external/libcxx/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/frameworks/native/libs/arect/include \
	$(TOP)/frameworks/native/libs/nativebase/include \
	$(TOP)/frameworks/av/include 

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_C_INCLUDES += \
	$(CAMERA_PATH)/common_v3_7885/Fusion \
	$(CAMERA_PATH)/common_v3_7885/Fusion/DofLut
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Vendor/$(LOCAL_PROJECT_DIR)
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/SensorInfos
endif

# Camera2 HAL3.2
LOCAL_CFLAGS += -DUSE_CAMERA2_API_SUPPORT

LOCAL_SRC_FILES:= \
	$(CAMERA_SRC_PATH)/common_v3_7885/ExynosCamera3Interface.cpp

$(foreach file,$(LOCAL_SRC_FILES),$(warning $(file)))
$(warning $(CAMERA_PATH))

ifneq ($(filter eng userdebug, $(TARGET_BUILD_VARIANT)),)
ifeq ($(BOARD_CAMERA_USES_DEBUG_PROPERTY), true)
LOCAL_CFLAGS += -DUSE_DEBUG_PROPERTY
endif
endif
LOCAL_CFLAGS += -Wno-error=date-time
LOCAL_CFLAGS += -Wno-overloaded-virtual
LOCAL_CFLAGS += -Wno-unused-parameter
LOCAL_CFLAGS += -Wno-date-time
LOCAL_CFLAGS += -Wno-unused-variable
LOCAL_CFLAGS += -Wno-implicit-fallthrough
LOCAL_CFLAGS += -Wno-error=implicit-fallthrough
LOCAL_CFLAGS += -Wno-error=macro-redefined
LOCAL_CFLAGS += -Wno-error=gnu-designator
LOCAL_CFLAGS += -Wno-error=braced-scalar-init

LOCAL_CFLAGS += -DMAIN_CAMERA_SENSOR_NAME=$(BOARD_BACK_CAMERA_SENSOR)
$(warning MAIN_CAMERA_SENSOR_NAME is $(BOARD_BACK_CAMERA_SENSOR))
LOCAL_CFLAGS += -DFRONT_CAMERA_SENSOR_NAME=$(BOARD_FRONT_CAMERA_SENSOR)
$(warning FRONT_CAMERA_SENSOR_NAME is $(BOARD_FRONT_CAMERA_SENSOR))
LOCAL_CFLAGS += -DSECURE_CAMERA_SENSOR_NAME=$(BOARD_SECURE_CAMERA_SENSOR)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_0=0
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_0=1
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)
LOCAL_CFLAGS += -DSECURE_ROTATION=$(BOARD_SECURE_CAMERA_ROTATION)
ifneq ($(BOARD_BACK_1_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DBACK_1_CAMERA_SENSOR_NAME=$(BOARD_BACK_1_CAMERA_SENSOR)
$(warning BACK_1_CAMERA_SENSOR_NAME is $(BOARD_BACK_1_CAMERA_SENSOR))
endif
ifneq ($(BOARD_FRONT_1_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DFRONT_1_CAMERA_SENSOR_NAME=$(BOARD_FRONT_1_CAMERA_SENSOR)
endif
ifneq ($(BOARD_BACK_2_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DBACK_2_CAMERA_SENSOR_NAME=$(BOARD_BACK_2_CAMERA_SENSOR)
$(warning BOARD_BACK_2_CAMERA_SENSOR is $(BOARD_BACK_2_CAMERA_SENSOR))
endif
ifneq ($(BOARD_FRONT_2_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DFRONT_2_CAMERA_SENSOR_NAME=$(BOARD_FRONT_2_CAMERA_SENSOR)
endif
ifneq ($(BOARD_BACK_3_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DBACK_3_CAMERA_SENSOR_NAME=$(BOARD_BACK_3_CAMERA_SENSOR)
endif
ifneq ($(BOARD_FRONT_3_CAMERA_SENSOR), )
LOCAL_CFLAGS += -DFRONT_3_CAMERA_SENSOR_NAME=$(BOARD_FRONT_3_CAMERA_SENSOR)
endif

ifeq ($(BOARD_BACK_1_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_1=2
endif
ifeq ($(BOARD_FRONT_1_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_1=3
endif
ifeq ($(BOARD_BACK_2_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_2=4
endif
ifeq ($(BOARD_FRONT_2_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_2=5
endif
ifeq ($(BOARD_BACK_3_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_BACK_3=6
endif
ifeq ($(BOARD_FRONT_3_CAMERA_SENSOR_OPEN), true)
LOCAL_CFLAGS += -DCAMERA_OPEN_ID_FRONT_3=7
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_CFLAGS += -DUSE_DUAL_CAMERA
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_ZOOM_MASTER)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_ZOOM_MASTER=$(BOARD_DUAL_CAMERA_REAR_ZOOM_MASTER)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_ZOOM_SLAVE)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_ZOOM_SLAVE=$(BOARD_DUAL_CAMERA_REAR_ZOOM_SLAVE)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_PORTRAIT_MASTER)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_PORTRAIT_MASTER=$(BOARD_DUAL_CAMERA_REAR_PORTRAIT_MASTER)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_REAR_PORTRAIT_SLAVE)))
LOCAL_CFLAGS += -DDUAL_CAMERA_REAR_PORTRAIT_SLAVE=$(BOARD_DUAL_CAMERA_REAR_PORTRAIT_SLAVE)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_MASTER)))
LOCAL_CFLAGS += -DDUAL_CAMERA_FRONT_PORTRAIT_MASTER=$(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_MASTER)
endif
ifneq (, $(findstring CAMERA_ID, $(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_SLAVE)))
LOCAL_CFLAGS += -DDUAL_CAMERA_FRONT_PORTRAIT_SLAVE=$(BOARD_DUAL_CAMERA_FRONT_PORTRAIT_SLAVE)
endif
endif

ifeq ($(BOARD_CAMERA_USES_EFD), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_EARLY_FD
endif

ifeq ($(BOARD_CAMERA_USES_3AA_DNG), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_3AA_DNG
endif

ifeq ($(BOARD_CAMERA_USES_SLSI_VENDOR_TAGS), true)
LOCAL_CFLAGS += -DUSE_SLSI_VENDOR_TAGS
endif

# TODO -- implement sensorlistener
ifeq ($(BOARD_CAMERA_USES_SENSOR_LISTENER), true)
LOCAL_CFLAGS += -DUSES_SENSOR_LISTENER

LOCAL_C_INCLUDES += \
	$(CAMERA_PATH)/core/src/common_v2/SensorListener
endif

LOCAL_CFLAGS += -DUSE_CAMERA_ESD_RESET

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA_SOLUTION_FAKE), true)
	LOCAL_CFLAGS += -DUSES_DUAL_CAMERA_SOLUTION_FAKE
endif

ifeq ($(BOARD_CAMERA_USES_PIPE_HANDLER), true)
	LOCAL_CFLAGS += -DUSE_PIPE_HANDLER
endif

ifeq ($(BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS), true)
	LOCAL_CFLAGS += -DUSES_SW_VDIS
endif

ifeq ($(BOARD_CAMERA_USES_REMOSAIC_SENSOR), true)
	LOCAL_CFLAGS += -DUSE_REMOSAIC_SENSOR
endif

ifeq ($(BOARD_CAMERA_USES_SENSOR_GYRO_FACTORY_MODE), true)
LOCAL_CFLAGS += -DUSES_SENSOR_GYRO_FACTORY_MODE
endif

ifeq ($(BOARD_CAMERA_USES_BAYER_COMPRESSION), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_USES_BAYER_COMPRESSION
endif

ifeq ($(BOARD_CAMERA_GED_FEATURE), true)
LOCAL_CFLAGS += -DCAMERA_GED_FEATURE
endif

ifeq ($(BOARD_CAMERA_USE_HFD), true)
LOCAL_CFLAGS += -DUSE_SUPPORT_HFD
endif

ifeq ($(BOARD_SECURE_CAMERA_SUPPORT), true)
LOCAL_CFLAGS += -DBOARD_SECURE_CAMERA_SUPPORT
endif

ifeq ($(BOARD_CAMERA_USES_SLSI_VENDOR_TAGS), true)
LOCAL_CFLAGS += -DUSE_SLSI_VENDOR_TAGS
endif

LOCAL_SHARED_LIBRARIES:= liblog libhardware libutils libion_exynos libhwjpeg libnativewindow libprocessgroup libui libgui_vendor libbinder libexpat libbase libbinder_ndk
LOCAL_SHARED_LIBRARIES += libexynoscamera3
LOCAL_SHARED_LIBRARIES += android.hardware.graphics.allocator@2.0
LOCAL_HEADER_LIBRARIES := media_plugin_headers

#$(foreach file,$(LOCAL_SRC_FILES),$(shell touch '$(LOCAL_PATH)/$(file)'))

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := camera.$(TARGET_SOC)

include $(TOP)/hardware/samsung_slsi-linaro/exynos/BoardConfigCFlags.mk
include $(BUILD_SHARED_LIBRARY)

$(warning #############################################)
$(warning ########       libcamera3       #############)
$(warning #############################################)

#include $(call all-makefiles-under, $(LOCAL_PATH))

