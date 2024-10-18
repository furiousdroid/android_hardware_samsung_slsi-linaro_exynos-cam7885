/*
 * Copyright 2017, Samsung Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed toggle an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*!
 * \file      SecCameraConfig-common.h
 * \brief     hearder file for SecCameraConfig
 * \author    Pilsun Jang(pilsun.jang@samsung.com)
 * \date      2013/7/15
 *
 */

#ifndef SEC_CAMERA_CONFIG_H__
#define SEC_CAMERA_CONFIG_H__

#define USE_VENDOR_SPECIFIC_CONFIG_HEADER

/* frame with smart pointer */
#define USE_SMARTPOINTER_FRAME

#define SUPPORT_64BITS

/******************************
 * Debugging Configurations *
 ******************************/
#define MONITOR_LOG_SYNC
#define MONITOR_LOG_SYNC_INTERVAL 100

/* #define PREVIEW_DURATION_DEBUG */
/* #define DEBUG_RAWDUMP */
/* #define YUV_DUMP */
/* #define DEBUG_STREAM_CONFIGURATIONS */

/******************************
 * MCD Feature Configurations *
 ******************************/
#define SAMSUNG_TN_FEATURE

#define SAMSUNG_CONTROL_METERING
#define SAMSUNG_OIS
//#define SAMSUNG_MANUAL_FOCUS
#define SAMSUNG_READ_ROM

//#define USE_AE_CONVERGED_UDM
#define USE_LIMITATION_FOR_THIRD_PARTY

#define SAMSUNG_FRONT_LCD_FLASH

#ifdef SAMSUNG_READ_ROM
#define SAMSUNG_READ_ROM_REAR   (true)
#define SAMSUNG_READ_ROM_FRONT  (true)
#endif

//#define SUPPORT_MULTI_AF
//#define SAMSUNG_DOF
//#define SAMSUNG_FIXED_FACE_FOCUS
#define LLS_CAPTURE
//#define SAMSUNG_FOOD_MODE
#define CAPTURE_FD_SYNC_WITH_PREVIEW
#define SAMSUNG_FEATURE_SHUTTER_NOTIFICATION
//#define SAMSUNG_SSRM
#define SAMSUNG_RTHDR
//#define SAMSUNG_PAF

#ifdef LLS_CAPTURE
#define SET_LLS_CAPTURE_SETFILE
#define LLS_HISTORY_COUNT			4
#endif

#define BURSTSHOT_OFF_FPS               (0)
#define BURSTSHOT_MIN_FPS               (10)
#define BURSTSHOT_MAX_FPS               (20)
#define BURSTSHOT_AGIF_FPS              (5)

#define USE_BV_FOR_VARIABLE_BURST_FPS
#ifdef USE_BV_FOR_VARIABLE_BURST_FPS
#define LLS_BV                          (-4*256)
#endif /* USE_BV_FOR_VARIABLE_BURST_FPS */

#define CAMERA_PREVIEW_EXPOSURE_TIME_LIMIT          (100000)
#define PERFRAME_CONTROL_CAMERA_EXPOSURE_TIME_MAX   (300000)
#ifdef USE_EXPOSURE_DYNAMIC_SHOT
#define CAMERA_SENSOR_EXPOSURE_TIME_MAX             (2000000)
#else
#define CAMERA_SENSOR_EXPOSURE_TIME_MAX             (500000)
#endif
#define CAMERA_ADD_BAYER_ENABLE

#ifdef CAMERA_GED_FEATURE
#else
#define SENSOR_FW_GET_FROM_FILE
#endif

#ifdef SAMSUNG_OIS
#define OIS_CAPTURE
#define USE_EXPOSURE_DYNAMIC_SHOT
#endif

#define SAMSUNG_MTF

/* UniSys */
#define SAMSUNG_UNIPLUGIN

/* This index may be changed  according to the performance of the sensor  */
#define BINNING_SETFILE_INDEX		ISS_SUB_SCENARIO_FHD_240FPS

#define SAMSUNG_UNI_API

#define MAX_PIPE_UNI_NUM (2)

#ifdef SAMSUNG_UNIPLUGIN
//#define SAMSUNG_SW_VDIS /* SW VDIS (former SUPPORT_SW_VDIS) */
//#define SAMSUNG_OT  /* Object Tracking */
//#define SAMSUNG_HLV /* Highlight Video */
//#define SAMSUNG_BD /* Blur detection for Burstshot + BestPhoto */
#define SAMSUNG_LLS_DEBLUR
//#define SAMSUNG_LENS_DC /* Lens Distortion Correction */
//#define SAMSUNG_FOCUS_PEAKING
//#define SAMSUNG_STR_CAPTURE /* Skin Tone Reproduction Capture */
//#define SAMSUNG_STR_PREVIEW /* Skin Tone Reproduction Preview */
//#define SAMSUNG_IDDQD /* Dirty Lens Detection(Image Driven Dirty Quick Detection) */
#endif
#ifdef SAMSUNG_BD
#define MAX_BD_BUFF_NUM            (4)
#endif
#ifdef SAMSUNG_LLS_DEBLUR
#define MAX_LD_CAPTURE_COUNT     (4)
#define LD_CAPTURE_COUNT_MULTI1  (2)
#define LD_CAPTURE_COUNT_MULTI2  (3)
#define LD_CAPTURE_COUNT_MULTI3  (4)
#define LD_CAPTURE_COUNT_MULTI4  (5)
#define SAMSUNG_LDCAPTURE_REAR   (true)
#define SAMSUNG_LDCAPTURE_FRONT  (true)
#endif

#ifdef SAMSUNG_LLS_DEBLUR
#define LLS_SETFILE_VERSION (2)
#else
#define LLS_SETFILE_VERSION (1)
#endif

#ifdef SAMSUNG_LLS_DEBLUR
//#define SAMSUNG_HIFI_CAPTURE
#endif

/* #define SAMSUNG_SENSOR_LISTENER */

#ifdef SAMSUNG_SENSOR_LISTENER
#define SAMSUNG_HRM            /* SamsungCamera only. vendor specific operation */
#define SAMSUNG_LIGHT_IR       /* SamsungCamera only. vendor specific operation */
#define SAMSUNG_GYRO
#define SAMSUNG_ACCELEROMETER
#define SAMSUNG_ROTATION
#endif

#define SAMSUNG_COLOR_IRIS

#ifdef SAMSUNG_SW_VDIS
//#define SAMSUNG_OIS_VDIS
//#define SAMSUNG_PREVIEW_VDIS
#endif

/***********************
 * Size Configurations *
 ***********************/
#define LCD_SIZE_DEFAULT                (0)
#define LCD_SIZE_800_480                (1)
#define LCD_SIZE_1280_720               (2)
#define LCD_SIZE_1920_1080              (3)
#define LCD_SIZE_2560_1440              (4)

#define CAMERA_LCD_SIZE                 LCD_SIZE_2560_1440

#define PIP_CAMERA_SUPPORTED
#define PIP_CAMERA_MAX_YUV_STREAM       (1)
#define PIP_CAMERA_MAX_YUV_HEIGHT       (1080)
#define PIP_CAMERA_MAX_YUV_SIZE         (1920 * PIP_CAMERA_MAX_YUV_HEIGHT)

#define CAMERA_BCROP_ALIGN              (2)
#define CAMERA_MCSC_ALIGN               (2)
#define CAMERA_TPU_CHUNK_ALIGN_W        (64)
#define CAMERA_TPU_CHUNK_ALIGN_H        (4)
#define CAMERA_16PX_ALIGN               (16)

/* This value for GSC alignment refer to "csc.h" */
#define GSCALER_IMG_ALIGN               (2)
/* This value for MFC alignment */
#define MFC_ALIGN                       (16)

#define USE_WQHD_RECORDING
#define USE_UHD_RECORDING
#define USE_ISP_BUFFER_SIZE_TO_BDS

#define SUPPORT_X8_ZOOM
#define SCALER_MAX_SCALE_UP_RATIO       (8)

/* Following flags are used in Single chain configuration.
   Defined here to prevent compilation error */
#define M2M_SCALER_MAX_DOWNSCALE_RATIO  (16)
#define MCSC_DOWN_RATIO_SMALL_YUV       (2)

#define USE_CAMERA_SIZE_TABLE           (true)

#define HIGH_RESOLUTION_MIN_PIXEL_SIZE  (12 * 1024 * 1024) /* 12MP pixel */
#define FHD_PIXEL_SIZE                  (1920 * 1080)

/* UHD 4:3 */
#define UHD_DVFS_CEILING_WIDTH          (3840)
#define UHD_DVFS_CEILING_HEIGHT         (2880)

#define YUVSTALL_DSCALED_SIZE_W         (1920)
#define YUVSTALL_DSCALED_SIZE_H         (1080)

/**************************
 * Control Configurations *
 **************************/
#define USE_SUBDIVIDED_EV

/* #define SET_SETFILE_BY_SHOT */
#define SET_SETFILE_BY_SET_CTRL
#ifdef SET_SETFILE_BY_SET_CTRL
#define SET_SETFILE_BY_SET_CTRL_3AA     (false)
#define SET_SETFILE_BY_SET_CTRL_ISP     (true)
#endif

/********************************
 * User Scenario Configurations *
 ********************************/
#define DUAL_CAMERA_SUPPORTED
#define USE_HIGHSPEED_RECORDING         (true)

/* #define USE_DUAL_CAMERA */
#ifdef USE_DUAL_CAMERA
#define USE_DUAL_PREVIEW_HW             (false)
#define USE_DUAL_PREVIEW_SW             (true)
#if (USE_DUAL_PREVIEW_SW)
#define NUM_FUSION_BUFFERS              (14)
#else
#define NUM_FUSION_BUFFERS              (0)
#endif
#define DUAL_TRANSITION_FRAME_COUNT     (30)
#endif

/*********************************
 * Internal Logic Configurations *
 *********************************/
#define INITIAL_SKIP_FRAME              (8)
#define VISION_SKIP_FRAME               (4)

#define USE_CAMERA2_USE_FENCE
#define USE_SW_FENCE

//#define SENSOR_NAME_GET_FROM_FILE

/* #define RESERVED_MEMORY_ENABLE */
#define RESERVED_BUFFER_COUNT_MAX       (0)

/* #define FPS_CHECK */
#define FIRST_PREVIEW_TIME_CHECK /* to get time processCaptureRequest() ~ first preview result to frameworks */

#define USE_FD_AE
#define FD_ROTATION                     (true)

#define SUPPORT_BACK_HW_VDIS                (false)
#define SUPPORT_FRONT_HW_VDIS               (false)

#define HW_VDIS_W_RATIO                 (1.2f)
#define HW_VDIS_H_RATIO                 (1.2f)

#define USE_FASTEN_AE_STABLE            (false)
#define USE_FASTEN_AE_STABLE_FRONT      (false)

#define FASTEN_AE_FPS                   (120)
#define FASTEN_AE_FPS_FRONT             (112)
#define SUPPORT_HFR_BATCH_MODE
#define MULTI_BUFFER_BASE_FPS           (60)
#define DEFAULT_BINNING_RATIO           (1)

#ifdef SAMSUNG_COLOR_IRIS
#define FASTEN_AE_SIZE_INDEX_COLOR_IRIS (1)
#define FASTEN_AE_FPS_COLOR_IRIS_FRONT  (102)
#endif

#ifdef SENSOR_NAME_GET_FROM_FILE
#define SENSOR_NAME_PATH_BACK "/sys/class/camera/rear/rear_sensorid"
#define SENSOR_NAME_PATH_BACK_1 "/sys/class/camera/rear/rear2_sensorid"
#define SENSOR_NAME_PATH_FRONT "/sys/class/camera/front/front_sensorid"
#define SENSOR_NAME_PATH_SECURE "/sys/class/camera/secure/secure_sensorid"
#endif
#define SENSOR_FW_PATH_BACK "/sys/class/camera/rear/rear_camfw"
#define SENSOR_FW_PATH_BACK_1 "/sys/class/camera/rear/rear2_camfw"
#define SENSOR_FW_PATH_FRONT "/sys/class/camera/front/front_camfw"

#ifdef SAMSUNG_OIS
#define OIS_EXIF_PATH_BACK "/sys/class/camera/ois/ois_exif"
#endif

#define USE_MCPIPE_SERIALIZATION_MODE
#ifdef USE_MCPIPE_SERIALIZATION_MODE
#define SUPPORT_HWFC_SERIALIZATION
#endif

/***********************
 * Time Configurations *
 ***********************/
#define HDR_DELAY                       (3)
#define SENSOR_REQUEST_DELAY            (2)

#define CORRECT_TIMESTAMP_FOR_SENSORFUSION
#ifdef CORRECT_TIMESTAMP_FOR_SENSORFUSION
#define CORRECTION_SENSORFUSION         (3000000) /* + 3.0ms */
#endif

/********************************
 * H/W Scenario Configurations *
 ********************************/
#define USE_BINNING_MODE

#define USE_YSUM_RECORDING

#define SUPPORT_DEPTH_MAP
#ifdef SUPPORT_DEPTH_MAP
#define NUM_DEPTHMAP_BUFFERS             (6)
#define MAX_DEPTH_SIZE_W (504)
#define MAX_DEPTH_SIZE_H (378)
#endif

#define USE_JPEG_HWFC                   (true)
#define USE_JPEG_HWFC_ONDEMAND          (true)

/* #define SUPPORT_HW_GDC               (true) */

#define USE_PURE_BAYER_REPROCESSING                    (true)
#define USE_PURE_BAYER_REPROCESSING_ON_RECORDING       (true)
#define USE_PURE_BAYER_REPROCESSING_ON_DUAL            (true)
#define USE_PURE_BAYER_REPROCESSING_ON_DUAL_RECORDING  (true)

#define USE_PURE_BAYER_REPROCESSING_FRONT              (true)
#define USE_PURE_BAYER_REPROCESSING_FRONT_ON_RECORDING (true)
#define USE_PURE_BAYER_REPROCESSING_FRONT_ON_DUAL      (true)
#define USE_PURE_BAYER_REPROCESSING_FRONT_ON_DUAL_RECORDING  (true)

/* This USE_DYNAMIC_BAYER define is for default scenario.
 * See <ExynosCameraParameter.cpp> for details of dynamic bayer setting
 */
#define USE_DYNAMIC_BAYER               (true)
#define USE_DYNAMIC_BAYER_60FPS         (true)
#define USE_DYNAMIC_BAYER_120FPS        (false)
#define USE_DYNAMIC_BAYER_240FPS        (false)

#define USE_DYNAMIC_BAYER_FRONT         (true)
#define USE_DYNAMIC_BAYER_60FPS_FRONT   (true)
#define USE_DYNAMIC_BAYER_120FPS_FRONT  (false)
#define USE_DYNAMIC_BAYER_240FPS_FRONT  (false)

enum REPROCESSING_BAYER_MODE {
    REPROCESSING_BAYER_MODE_NONE            = 0, /* This means capture do not use reprocessing */
    REPROCESSING_BAYER_MODE_PURE_ALWAYS_ON,
    REPROCESSING_BAYER_MODE_DIRTY_ALWAYS_ON,
    REPROCESSING_BAYER_MODE_PURE_DYNAMIC,
    REPROCESSING_BAYER_MODE_DIRTY_DYNAMIC,
    REPROCESSING_BAYER_MODE_MAX,
};

#define USE_DYNAMIC_SCC_REAR                (false)
#define USE_DYNAMIC_SCC_FRONT               (false)

#define USE_GSC_FOR_CAPTURE_BACK            (false)
#define USE_GSC_FOR_CAPTURE_FRONT           (false)

/* back */
#define MAIN_CAMERA_FLITE_NUM               FIMC_IS_VIDEO_SS0_NUM
#define MAIN_1_CAMERA_FLITE_NUM             FIMC_IS_VIDEO_SS2_NUM
#define MAIN_CAMERA_DEPTH_VC_NUM            FIMC_IS_VIDEO_SS0VC1_NUM
#define MAIN_CAMERA_HAS_OWN_SCC             (false)

#define MAIN_CAMERA_SINGLE_FLITE_3AA_OTF    (true)
#define MAIN_CAMERA_DUAL_FLITE_3AA_OTF      (true)

#define MAIN_CAMERA_SINGLE_3AA_ISP_OTF      (false)
#define MAIN_CAMERA_DUAL_3AA_ISP_OTF        (false)

#define MAIN_CAMERA_SINGLE_ISP_TPU_OTF      (false)
#define MAIN_CAMERA_DUAL_ISP_TPU_OTF        (false)

#define MAIN_CAMERA_SINGLE_ISP_MCSC_OTF     (true)
#define MAIN_CAMERA_DUAL_ISP_MCSC_OTF       (true)

#define MAIN_CAMERA_SINGLE_TPU_MCSC_OTF     (true)
#define MAIN_CAMERA_DUAL_TPU_MCSC_OTF       (false)

#define MAIN_CAMERA_SINGLE_MCSC_VRA_OTF    (false)
#define MAIN_CAMERA_DUAL_MCSC_VRA_OTF      (false)

#define MAIN_CAMERA_SINGLE_REPROCESSING     (true)
#define MAIN_CAMERA_SINGLE_SCC_CAPTURE      (false)

#define MAIN_CAMERA_DUAL_REPROCESSING       (true)
#define MAIN_CAMERA_DUAL_SCC_CAPTURE        (false)

#define MAIN_CAMERA_SINGLE_3AA_ISP_OTF_REPROCESSING     (true)
#define MAIN_CAMERA_DUAL_3AA_ISP_OTF_REPROCESSING       (true) /* need to be same with FRONT_CAMERA_DUAL_3AA_ISP_OTF on dual case */

#define MAIN_CAMERA_SINGLE_ISP_TPU_OTF_REPROCESSING     (false)
#define MAIN_CAMERA_DUAL_ISP_TPU_OTF_REPROCESSING       (false)

#define MAIN_CAMERA_SINGLE_ISP_MCSC_OTF_REPROCESSING    (true)
#define MAIN_CAMERA_DUAL_ISP_MCSC_OTF_REPROCESSING      (true)

#define MAIN_CAMERA_SINGLE_TPU_MCSC_OTF_REPROCESSING    (false)
#define MAIN_CAMERA_DUAL_TPU_MCSC_OTF_REPROCESSING      (false)

#define MAIN_CAMERA_SINGLE_MCSC_VRA_OTF_REPROCESSING    (false)
#define MAIN_CAMERA_DUAL_MCSC_VRA_OTF_REPROCESSING      (false)

/* front */
#define FRONT_CAMERA_FLITE_NUM              FIMC_IS_VIDEO_SS1_NUM
#define FRONT_1_CAMERA_FLITE_NUM            FIMC_IS_VIDEO_SS3_NUM
#define FRONT_CAMERA_DEPTH_VC_NUM           FIMC_IS_VIDEO_SS1VC1_NUM
#define FRONT_CAMERA_HAS_OWN_SCC            (false)

#define FRONT_CAMERA_SINGLE_FLITE_3AA_OTF   (true)
#define FRONT_CAMERA_DUAL_FLITE_3AA_OTF     (false)

#define FRONT_CAMERA_SINGLE_3AA_ISP_OTF     (false)
#define FRONT_CAMERA_DUAL_3AA_ISP_OTF       (true)

#define FRONT_CAMERA_SINGLE_ISP_TPU_OTF     (false)
#define FRONT_CAMERA_DUAL_ISP_TPU_OTF       (false)

#define FRONT_CAMERA_SINGLE_ISP_MCSC_OTF    (true)
#define FRONT_CAMERA_DUAL_ISP_MCSC_OTF      (true)

#define FRONT_CAMERA_SINGLE_TPU_MCSC_OTF    (true)
#define FRONT_CAMERA_DUAL_TPU_MCSC_OTF      (false)

#define FRONT_CAMERA_SINGLE_MCSC_VRA_OTF    (false)
#define FRONT_CAMERA_DUAL_MCSC_VRA_OTF      (false)

#define FRONT_CAMERA_SINGLE_REPROCESSING    (true)
#define FRONT_CAMERA_SINGLE_SCC_CAPTURE     (false)

#define FRONT_CAMERA_DUAL_REPROCESSING      (true)
#define FRONT_CAMERA_DUAL_SCC_CAPTURE       (false)

#define FRONT_CAMERA_SINGLE_3AA_ISP_OTF_REPROCESSING    (true)
#define FRONT_CAMERA_DUAL_3AA_ISP_OTF_REPROCESSING      (true)

#define FRONT_CAMERA_SINGLE_ISP_TPU_OTF_REPROCESSING    (false)
#define FRONT_CAMERA_DUAL_ISP_TPU_OTF_REPROCESSING      (false)

#define FRONT_CAMERA_SINGLE_ISP_MCSC_OTF_REPROCESSING   (true)
#define FRONT_CAMERA_DUAL_ISP_MCSC_OTF_REPROCESSING     (true)

#define FRONT_CAMERA_SINGLE_TPU_MCSC_OTF_REPROCESSING   (false)
#define FRONT_CAMERA_DUAL_TPU_MCSC_OTF_REPROCESSING     (false)

#define FRONT_CAMERA_SINGLE_MCSC_VRA_OTF_REPROCESSING   (false)
#define FRONT_CAMERA_DUAL_MCSC_VRA_OTF_REPROCESSING     (false)

/* Vision, Secure Camera */
#define VISION_CAMERA_FLITE_NUM  FIMC_IS_VIDEO_SS1_NUM
#define SECURE_CAMERA_FLITE_NUM  FIMC_IS_VIDEO_SS3_NUM  //IRIS use FIMC_IS_VIDEO_SS3_NUM

#define SCENARIO_SHIFT              (28)
#define REPROCESSING_SHIFT          (28)
#define SSX_VINDEX_SHIFT            (16)

#define SENSOR_INSTANT_SHIFT        (16)

#define INPUT_SENSOR_MASK   0xFC000000 /* sensor scenario */
#define INPUT_SENSOR_SHIFT   26
#define INPUT_STREAM_MASK   0x03000000 /* stream type : 1 (reprocessing) */
#define INPUT_STREAM_SHIFT   24
#define INPUT_MODULE_MASK   0x00FF0000 /* module id : unique sensor id */
#define INPUT_MODULE_SHIFT   16
#define INPUT_VINDEX_MASK   0x0000FF00 /* video node index : connected node */
#define INPUT_VINDEX_SHIFT   8
#define INPUT_MEMORY_MASK   0x000000F0 /* memory interface : 0 (on-the-fly) */
#define INPUT_MEMORY_SHIFT   4
#define INPUT_LEADER_MASK   0x0000000F /* leader : 1 (leader video node) */
#define INPUT_LEADER_SHIFT   0

#define PREVIEW_GSC_NODE_NUM            (4)  /* 4 = MSC from Exynos5420 */
#define PICTURE_GSC_NODE_NUM            (5)  /* 0,1,2 = GSC */
#define VIDEO_GSC_NODE_NUM              (4)
enum vendor_node {
    VENDOR_NODE_NUM = 500,
    UNIPLUGIN_NODE_NUM = VENDOR_NODE_NUM,
    VENDOR_NODE_MAX,
};

/*************************
 * Buffer Configurations *
 *************************/
#define NUM_REQUEST_RAW_BUFFER              (5)
#define NUM_REQUEST_PREVIEW_BUFFER          (7)
#define NUM_REQUEST_CALLBACK_BUFFER         (7)
#define NUM_REQUEST_VIDEO_BUFFER            (7)
#define NUM_REQUEST_JPEG_BUFFER             (5)

#define MAX_BUFFERS                     (VIDEO_MAX_FRAME)
#define NUM_FASTAESTABLE_BUFFERS        (10)
#define NUM_SENSOR_BUFFERS              (4 + REPROCESSING_BAYER_HOLD_COUNT + SENSOR_REQUEST_DELAY)
#define NUM_3AA_BUFFERS                 (6 + SENSOR_REQUEST_DELAY)
#define NUM_HW_DIS_BUFFERS              (NUM_3AA_BUFFERS + 1)
#define NUM_VRA_BUFFERS                     (10)

#define NUM_REPROCESSING_BUFFERS        (4)
#define NUM_PICTURE_BUFFERS             (5)

#define NUM_BAYER_BUFFERS               (5 + REPROCESSING_BAYER_HOLD_COUNT)
#define NUM_PREVIEW_BUFFERS             (9)
#define NUM_RECORDING_BUFFERS           (8)

#define PIPE_FLITE_PREPARE_COUNT        (2)
#define PIPE_3AA_PREPARE_COUNT          (2)

#ifdef DEBUG_RAWDUMP
#define RESERVED_NUM_BAYER_BUFFERS          (0)
#define RESERVED_NUM_ISP_BUFFERS            (0)
#define FRONT_RESERVED_NUM_BAYER_BUFFERS    (0)
#define FRONT_RESERVED_NUM_ISP_BUFFERS      (0)
#else
#define RESERVED_NUM_BAYER_BUFFERS          (NUM_BAYER_BUFFERS)
#define RESERVED_NUM_ISP_BUFFERS            (0)
#define FRONT_RESERVED_NUM_BAYER_BUFFERS    (6)
#define FRONT_RESERVED_NUM_ISP_BUFFERS      (0)
#endif /* DEBUG_RAWDUMP */
#define RESERVED_NUM_SECURE_BUFFERS     (5)

/* TO DO : will remove */
#define REPROCESSING_BAYER_HOLD_COUNT   (1)

#if (USE_HIGHSPEED_RECORDING)
/* HIGHSPEED_120 */
#define FPS120_NUM_REQUEST_RAW_BUFFER        (NUM_REQUEST_RAW_BUFFER)
#define FPS120_NUM_REQUEST_PREVIEW_BUFFER    (NUM_REQUEST_PREVIEW_BUFFER)
#define FPS120_NUM_REQUEST_CALLBACK_BUFFER   (NUM_REQUEST_CALLBACK_BUFFER * 3)
#define FPS120_NUM_REQUEST_VIDEO_BUFFER      (NUM_REQUEST_VIDEO_BUFFER * 3)
#define FPS120_NUM_REQUEST_JPEG_BUFFER       (NUM_REQUEST_JPEG_BUFFER)

#define FPS120_NUM_SENSOR_BUFFERS            (NUM_SENSOR_BUFFERS * 3)
#define FPS120_NUM_3AA_BUFFERS               (NUM_3AA_BUFFERS * 3)
#define FPS120_NUM_HW_DIS_BUFFERS            (NUM_HW_DIS_BUFFERS * 3)

#define FPS120_NUM_BAYER_BUFFERS             (NUM_BAYER_BUFFERS * 3)
#define FPS120_NUM_PREVIEW_BUFFERS           (NUM_PREVIEW_BUFFERS * 3)
#define FPS120_NUM_RECORDING_BUFFERS         (NUM_RECORDING_BUFFERS * 3)

#define FPS120_PIPE_FLITE_PREPARE_COUNT      (PIPE_FLITE_PREPARE_COUNT * 3)
#define FPS120_PIPE_3AA_PREPARE_COUNT        (PIPE_3AA_PREPARE_COUNT * 3)

/* HIGHSPEED_240 */
#define FPS240_NUM_REQUEST_RAW_BUFFER       	(NUM_REQUEST_RAW_BUFFER)
#define FPS240_NUM_REQUEST_PREVIEW_BUFFER   	(NUM_REQUEST_PREVIEW_BUFFER)
#define FPS240_NUM_REQUEST_CALLBACK_BUFFER  	(NUM_REQUEST_CALLBACK_BUFFER * 3)
#define FPS240_NUM_REQUEST_VIDEO_BUFFER     	(NUM_REQUEST_VIDEO_BUFFER * 3)
#define FPS240_NUM_REQUEST_JPEG_BUFFER      	(NUM_REQUEST_JPEG_BUFFER)

#define FPS240_NUM_SENSOR_BUFFERS           	(NUM_SENSOR_BUFFERS * 3)
#define FPS240_NUM_3AA_BUFFERS              	(32)   /* VIDEO_MAX_FRAME : 32 */
#define FPS240_NUM_HW_DIS_BUFFERS           	(NUM_HW_DIS_BUFFERS * 3)

#define FPS240_NUM_BAYER_BUFFERS        	    (NUM_BAYER_BUFFERS * 3)
#define FPS240_NUM_PREVIEW_BUFFERS          	(NUM_PREVIEW_BUFFERS * 3)
#define FPS240_NUM_RECORDING_BUFFERS        	(NUM_RECORDING_BUFFERS * 3)

#define FPS240_PIPE_FLITE_PREPARE_COUNT         (PIPE_FLITE_PREPARE_COUNT * 3)
#define FPS240_PIPE_3AA_PREPARE_COUNT           (PIPE_3AA_PREPARE_COUNT * 3)
#endif

#ifdef SAMSUNG_SW_VDIS
#define VDIS_NUM_INTERNAL_BUFFERS       (27)
#endif

#define EXYNOS_CAMERA_BUFFER_MAX_PLANES (17)     /* img buffer 4 + metadata 1 */
#define EXYNOS_CAMERA_META_PLANE_SIZE   (36 * 1024)
#define EXYNOS_CAMERA_YSUM_PLANE_SIZE   sizeof(ExynosVideoMeta)
#define GRALLOC_LOCK_FOR_CAMERA         (GRALLOC_SET_USAGE_FOR_CAMERA)

/* For use ExynosVideoMeta, gralloc always allocate plane to video buffer. */
#define GRALLOC_ALWAYS_ALLOC_VIDEO_META

/*******************************
 * Color Format Configurations *
 *******************************/
#ifndef DEBUG_RAWDUMP
#define CAMERA_PACKED_BAYER_ENABLE
#endif
/* #define CAMERA_PACKED_BAYER_ENABLE */
#define USE_BUFFER_WITH_STRIDE

#define V4L2_CAMERA_MEMORY_TYPE         (V4L2_MEMORY_DMABUF) /* (V4L2_MEMORY_USERPTR) */
#define SCC_OUTPUT_COLOR_FMT            (V4L2_PIX_FMT_YUYV)
#define JPEG_INPUT_COLOR_FMT            (SCC_OUTPUT_COLOR_FMT)

#ifdef CAMERA_PACKED_BAYER_ENABLE
#define CAMERA_FLITE_BAYER_FORMAT               (V4L2_PIX_FMT_SBGGR12)
#define CAMERA_3AC_BAYER_FORMAT                 (V4L2_PIX_FMT_SBGGR12)
#define CAMERA_3AP_BAYER_FORMAT                 (V4L2_PIX_FMT_SBGGR12)
#define CAMERA_3AC_REPROCESSING_BAYER_FORMAT    (V4L2_PIX_FMT_SBGGR16)
#else
#define CAMERA_FLITE_BAYER_FORMAT               (V4L2_PIX_FMT_SBGGR16)
#define CAMERA_3AC_BAYER_FORMAT                 (V4L2_PIX_FMT_SBGGR16)
#define CAMERA_3AP_BAYER_FORMAT                 (V4L2_PIX_FMT_SBGGR16)
#define CAMERA_3AC_REPROCESSING_BAYER_FORMAT    (V4L2_PIX_FMT_SBGGR16)
#endif

#define DEPTH_MAP_FORMAT                (V4L2_PIX_FMT_SBGGR16)
#define CAMERA_DUMP_BAYER_FORMAT        (V4L2_PIX_FMT_SBGGR16)

/***********
 * Defines *
 ***********/
#define ERROR_POLLING_DETECTED          (-1001)
#define ERROR_DQ_BLOCKED_DETECTED       (-1002)
#define ERROR_DQ_BLOCKED_COUNT          (20)
#define MONITOR_THREAD_INTERVAL         (200000)

#define EXYNOS_CAMERA_PREVIEW_FPS_REFERENCE  (60)

#define  NUM_OF_DETECTED_FACES           (16)
#define  NUM_OF_DETECTED_FACES_THRESHOLD (0)

#define PERFRAME_NODE_GROUP_MAX          (6)

#define PERFRAME_INFO_INDEX_MAX          (6)
#define PERFRAME_INFO_INDEX_0            (0)
#define PERFRAME_INFO_INDEX_1            (1)
#define PERFRAME_INFO_INDEX_2            (2)
#define PERFRAME_INFO_INDEX_3            (3)
#define PERFRAME_INFO_INDEX_4            (4)
#define PERFRAME_INFO_INDEX_5            (5)

#define PERFRAME_CONTROL_PIPE                   PIPE_3AA
#define PERFRAME_CONTROL_REPROCESSING_PIPE      PIPE_3AA_REPROCESSING

#define PERFRAME_INFO_3AA                       PERFRAME_INFO_INDEX_0
#define PERFRAME_INFO_ISP                       PERFRAME_INFO_INDEX_1
#define PERFRAME_INFO_TPU                       PERFRAME_INFO_INDEX_2
#define PERFRAME_INFO_DIS                       PERFRAME_INFO_TPU
#define PERFRAME_INFO_MCSC                      PERFRAME_INFO_INDEX_3
#define PERFRAME_INFO_FLITE                     PERFRAME_INFO_INDEX_4
#define PERFRAME_INFO_VRA                       PERFRAME_INFO_INDEX_5

#define PERFRAME_INFO_DIRTY_REPROCESSING_ISP    PERFRAME_INFO_INDEX_0
#define PERFRAME_INFO_DIRTY_REPROCESSING_MCSC   PERFRAME_INFO_INDEX_1
#define PERFRAME_INFO_DIRTY_REPROCESSING_VRA    PERFRAME_INFO_INDEX_2

#define PERFRAME_INFO_PURE_REPROCESSING_3AA     PERFRAME_INFO_INDEX_0
#define PERFRAME_INFO_PURE_REPROCESSING_ISP     PERFRAME_INFO_INDEX_1
#define PERFRAME_INFO_PURE_REPROCESSING_MCSC    PERFRAME_INFO_INDEX_2
#define PERFRAME_INFO_PURE_REPROCESSING_VRA     PERFRAME_INFO_INDEX_3

#define PERFRAME_BACK_VC0_POS           (0)
#define PERFRAME_BACK_3AC_POS           (1)
#define PERFRAME_BACK_3AP_POS           (2)
#define PERFRAME_BACK_ISPC_POS          (3)
#define PERFRAME_BACK_ISPP_POS          (4)
#define PERFRAME_BACK_DCPS0_POS         (5)
#define PERFRAME_BACK_DCPS1_POS         (5)
#define PERFRAME_BACK_DCPC0_POS         (7)
#define PERFRAME_BACK_DCPC1_POS         (8)
#define PERFRAME_BACK_CIPC0_POS         (9)
#define PERFRAME_BACK_CIPC1_POS         (10)
#define PERFRAME_BACK_SCC_POS           (0)
#define PERFRAME_BACK_SCP_POS           (4)
#define PERFRAME_BACK_MCSC0_POS         PERFRAME_BACK_SCP_POS
#define PERFRAME_BACK_MCSC1_POS         (1)
#define PERFRAME_BACK_MCSC2_POS         (2)
#define PERFRAME_BACK_MCSC5_POS         (3)

#define PERFRAME_FRONT_VC0_POS          (PERFRAME_BACK_VC0_POS)
#define PERFRAME_FRONT_3AC_POS          (PERFRAME_BACK_3AC_POS)
#define PERFRAME_FRONT_3AP_POS          (PERFRAME_BACK_3AP_POS)
#define PERFRAME_FRONT_ISPC_POS         (PERFRAME_BACK_ISPC_POS)
#define PERFRAME_FRONT_ISPP_POS         (PERFRAME_BACK_ISPP_POS)
#define PERFRAME_FRONT_SCC_POS          (PERFRAME_BACK_SCC_POS)
#define PERFRAME_FRONT_SCP_POS          (PERFRAME_BACK_SCP_POS)

#define PERFRAME_REPROCESSING_3AC_POS     (0)
#define PERFRAME_REPROCESSING_3AP_POS     (0)
#define PERFRAME_REPROCESSING_SCC_POS     (2)
#define PERFRAME_REPROCESSING_ISPC_POS    (2)
#define PERFRAME_REPROCESSING_ISPP_POS    (2)
#define PERFRAME_REPROCESSING_DCPS0_POS   (3)
#define PERFRAME_REPROCESSING_DCPS1_POS   (4)
#define PERFRAME_REPROCESSING_DCPC0_POS   (5)
#define PERFRAME_REPROCESSING_DCPC1_POS   (6)
#define PERFRAME_REPROCESSING_CIPS0_POS   (7)
#define PERFRAME_REPROCESSING_CIPS1_POS   (8)
#define PERFRAME_REPROCESSING_MCSC0_POS   (1)
#define PERFRAME_REPROCESSING_MCSC1_POS   (2)
#define PERFRAME_REPROCESSING_MCSC2_POS   (3)
#define PERFRAME_REPROCESSING_MCSC3_POS   (4)
#define PERFRAME_REPROCESSING_MCSC4_POS   (5)
#define PERFRAME_REPROCESSING_MCSC5_POS   (6)

#define WAITING_TIME                     (5000)   /* 5msec */
#define TOTAL_WAITING_TIME               (3000 * 1000)  /* 3000msec */
#define TOTAL_WAITING_COUNT              (3)
#define TOTAL_FLASH_WATING_COUNT         (10)
#define CAPTURE_WAITING_COUNT            (15)

#define DM_WAITING_TIME                  (30 * 1000) /* 30msec */
#define DM_WAITING_COUNT                 (10)

/*vision */
/* #define VISION_DUMP */
#define VISION_WIDTH                     (320)
#define VISION_HEIGHT                    (180)

/* Secure Camera */
#define SECURE_CAMERA_WIDTH              (2400)
#define SECURE_CAMERA_HEIGHT             (2400)

/* MCSC Restriction*/
#define NO_MCSC_RESTRICTION

#define USE_MEM2MEM_GSC
#ifdef USE_MEM2MEM_GSC
#define GRALLOC_SET_USAGE_FOR_CAMERA \
    (GRALLOC_USAGE_SW_READ_OFTEN | \
     GRALLOC_USAGE_SW_WRITE_OFTEN | \
     GRALLOC_USAGE_HW_TEXTURE | \
     GRALLOC_USAGE_HW_COMPOSER | \
     GRALLOC_USAGE_EXTERNAL_DISP | \
     GRALLOC_USAGE_HW_CAMERA_MASK)
#else
#define GRALLOC_SET_USAGE_FOR_CAMERA \
    (GRALLOC_USAGE_SW_READ_OFTEN | \
     GRALLOC_USAGE_SW_WRITE_OFTEN | \
     GRALLOC_USAGE_HW_TEXTURE | \
     GRALLOC_USAGE_HW_COMPOSER | \
     GRALLOC_USAGE_EXTERNAL_DISP)
#endif

enum YUV_RANGE {
    YUV_FULL_RANGE = 0,
    YUV_LIMITED_RANGE = 1,
};

enum pipeline {
    PIPE_FLITE                  = 0,
    PIPE_VC0,
    PIPE_VC1, // DEPTH MAP
    PIPE_VC2,
    PIPE_VC3,
    PIPE_3AA,
    PIPE_3AC,
    PIPE_3AP,
    PIPE_ISP,
    PIPE_ISPC,
    PIPE_ISPP,
    PIPE_DCPS0,
    PIPE_DCPS1,
    PIPE_DCPC0,
    PIPE_DCPC1,
    PIPE_CIPS0,
    PIPE_CIPS1,
    PIPE_DIS,
    PIPE_TPU = PIPE_DIS,
    PIPE_TPUP,
    PIPE_MCSC,
    PIPE_SCP,
    PIPE_MCSC0 = PIPE_SCP,
    PIPE_MCSC1,
    PIPE_MCSC2,
    PIPE_MCSC3,
    PIPE_MCSC4,
    PIPE_MCSC5,
    PIPE_VRA,
    PIPE_3AA_ISP,
    PIPE_POST_3AA_ISP,
    PIPE_SCC,
    PIPE_GDC,
    PIPE_GSC,
    PIPE_GSC_VIDEO,
    PIPE_GSC_PICTURE,
    PIPE_JPEG,
    PIPE_SYNC,
    PIPE_FUSION,
    PIPE_PP_VENDOR,
    PIPE_PP_UNI = PIPE_PP_VENDOR,
    PIPE_PP_UNI2,
    MAX_PIPE_NUM,

    /*
     * PIPE_XXX_FRONT are deprecated define.
     * Don't use this. (just let for common code compile)
     */
    PIPE_FLITE_FRONT = 100,
    PIPE_3AA_FRONT,
    PIPE_3AC_FRONT,
    PIPE_3AP_FRONT,
    PIPE_ISP_FRONT,
    PIPE_ISPC_FRONT,
    PIPE_ISPP_FRONT,
    PIPE_SCP_FRONT,
    PIPE_3AA_ISP_FRONT,
    PIPE_POST_3AA_ISP_FRONT,
    PIPE_DIS_FRONT,
    PIPE_SCC_FRONT,
    PIPE_GSC_FRONT,
    PIPE_GSC_VIDEO_FRONT,
    PIPE_GSC_PICTURE_FRONT,
    PIPE_JPEG_FRONT,
    MAX_PIPE_NUM_FRONT,

    PIPE_FLITE_REPROCESSING     = 200,
    PIPE_VC0_REPROCESSING,
    PIPE_3AA_REPROCESSING,
    PIPE_3AC_REPROCESSING,
    PIPE_3AP_REPROCESSING,
    PIPE_ISP_REPROCESSING,
    PIPE_ISPC_REPROCESSING,
    PIPE_ISPP_REPROCESSING,
    PIPE_DCPS0_REPROCESSING,
    PIPE_DCPS1_REPROCESSING,
    PIPE_DCPC0_REPROCESSING,
    PIPE_DCPC1_REPROCESSING,
    PIPE_CIPS0_REPROCESSING,
    PIPE_CIPS1_REPROCESSING,
    PIPE_TPU_REPROCESSING,
    PIPE_TPUP_REPROCESSING,
    PIPE_MCSC_REPROCESSING,
    PIPE_MCSC0_REPROCESSING,
    PIPE_MCSC1_REPROCESSING,
    PIPE_MCSC2_REPROCESSING,
    PIPE_MCSC3_REPROCESSING,
    PIPE_MCSC4_REPROCESSING,
    PIPE_MCSC5_REPROCESSING,
    PIPE_VRA_REPROCESSING,
    PIPE_SCC_REPROCESSING,
    PIPE_SCP_REPROCESSING,
    PIPE_GSC_REPROCESSING,
    PIPE_GSC_REPROCESSING2,
    PIPE_GSC_REPROCESSING3,
#ifdef UVS
    PIPE_UVS_REPROCESSING,
#endif
    PIPE_JPEG_REPROCESSING,
    PIPE_HWFC_JPEG_SRC_REPROCESSING,
    PIPE_HWFC_THUMB_SRC_REPROCESSING,
    PIPE_HWFC_JPEG_DST_REPROCESSING,
    PIPE_HWFC_THUMB_DST_REPROCESSING,
    PIPE_SYNC_REPROCESSING,
    PIPE_FUSION_REPROCESSING,
    PIPE_PP_VENDOR_REPROCESSING,
    PIPE_PP_UNI_REPROCESSING = PIPE_PP_VENDOR_REPROCESSING,
    PIPE_PP_UNI_REPROCESSING2,
    MAX_PIPE_NUM_REPROCESSING
};

enum fimc_is_video_dev_num {
    FIMC_IS_VIDEO_BAS_NUM = 100,
    FIMC_IS_VIDEO_SS0_NUM = 101,
    FIMC_IS_VIDEO_SS1_NUM,
    FIMC_IS_VIDEO_SS2_NUM,
    FIMC_IS_VIDEO_SS3_NUM,
    FIMC_IS_VIDEO_BNS_NUM = 107,
    FIMC_IS_VIDEO_PRE_NUM = 109,
    FIMC_IS_VIDEO_30S_NUM = 110,
    FIMC_IS_VIDEO_30C_NUM,
    FIMC_IS_VIDEO_30P_NUM,
    FIMC_IS_VIDEO_31S_NUM = 120,
    FIMC_IS_VIDEO_31C_NUM,
    FIMC_IS_VIDEO_31P_NUM,
    FIMC_IS_VIDEO_I0S_NUM = 130,
    FIMC_IS_VIDEO_I0C_NUM,
    FIMC_IS_VIDEO_I0P_NUM,
    FIMC_IS_VIDEO_I1S_NUM = 140,
    FIMC_IS_VIDEO_I1C_NUM,
    FIMC_IS_VIDEO_I1P_NUM,
    FIMC_IS_VIDEO_TPU_NUM = 150,
    FIMC_IS_VIDEO_DCP0S_NUM,
    FIMC_IS_VIDEO_DCP1S_NUM,
    FIMC_IS_VIDEO_DCP0C_NUM,
    FIMC_IS_VIDEO_DCP1C_NUM,
    FIMC_IS_VIDEO_GDC_NUM = 155,
    FIMC_IS_VIDEO_SCC_NUM,
    FIMC_IS_VIDEO_SCP_NUM,
    FIMC_IS_VIDEO_M0S_NUM = 160,
    FIMC_IS_VIDEO_M1S_NUM,
    FIMC_IS_VIDEO_M0P_NUM = 170,
    FIMC_IS_VIDEO_M1P_NUM,
    FIMC_IS_VIDEO_M2P_NUM,
    FIMC_IS_VIDEO_M3P_NUM,
    FIMC_IS_VIDEO_M4P_NUM,
    FIMC_IS_VIDEO_M5P_NUM,
    FIMC_IS_VIDEO_VRA_NUM = 180,
    FIMC_IS_VIDEO_D0S_NUM = 190,
    FIMC_IS_VIDEO_D0C_NUM,
    FIMC_IS_VIDEO_D1S_NUM,
    FIMC_IS_VIDEO_D1C_NUM,
    FIMC_IS_VIDEO_HWFC_JPEG_NUM = 200,
    FIMC_IS_VIDEO_HWFC_THUMB_NUM = 201,
    FIMC_IS_VIDEO_CIP0S_NUM = 205,
    FIMC_IS_VIDEO_CIP1S_NUM = 206,
    FIMC_IS_VIDEO_SS0VC0_NUM = 210,
    FIMC_IS_VIDEO_SS0VC1_NUM,
    FIMC_IS_VIDEO_SS0VC2_NUM,
    FIMC_IS_VIDEO_SS0VC3_NUM,
    FIMC_IS_VIDEO_SS1VC0_NUM,
    FIMC_IS_VIDEO_SS1VC1_NUM,
    FIMC_IS_VIDEO_SS1VC2_NUM,
    FIMC_IS_VIDEO_SS1VC3_NUM,
    FIMC_IS_VIDEO_SS2VC0_NUM,
    FIMC_IS_VIDEO_SS2VC1_NUM,
    FIMC_IS_VIDEO_SS2VC2_NUM = 220,
    FIMC_IS_VIDEO_SS2VC3_NUM,
    FIMC_IS_VIDEO_SS3VC0_NUM,
    FIMC_IS_VIDEO_SS3VC1_NUM,
    FIMC_IS_VIDEO_SS3VC2_NUM,
    FIMC_IS_VIDEO_SS3VC3_NUM = 225,
    FIMC_IS_VIDEO_MAX_NUM
};

/* overflow handling */
#define SENSOR_OVERFLOW_CHECK
#define AVOID_ASSERT_FRAME

#endif /* EXYNOS_CAMERA_CONFIG_H__ */

