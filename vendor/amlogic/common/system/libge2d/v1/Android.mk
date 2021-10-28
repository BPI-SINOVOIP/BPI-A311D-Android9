LOCAL_PATH := $(call my-dir)

# libge2d
include $(CLEAR_VARS)
LOCAL_SRC_FILES := ge2d_port.c aml_ge2d.c IONmem.c
LOCAL_MODULE := libge2d
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog libion
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/kernel-headers

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
     $(LOCAL_PATH)/kernel-headers \
     system/core/libion/include/ \
     system/core/libion/kernel-headers \
     system/core/include/ion

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include $(LOCAL_PATH)/kernel-headers
LOCAL_CFLAGS := -Werror

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 26 && echo OK),OK)
LOCAL_PROPRIETARY_MODULE := true
endif

include $(BUILD_SHARED_LIBRARY)

# ge2d_feature_test
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE:= false

LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES:= aml_ge2d.c IONmem.c ge2d_feature_test.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/kernel-headers \
	system/core/libion/include/ \
	system/core/libion/kernel-headers \
	system/core/include/ion

LOCAL_SHARED_LIBRARIES := \
    liblog \
	libge2d \
	libion

LOCAL_CFLAGS +=-g
LOCAL_CPPFLAGS := -g

LOCAL_MODULE := ge2d_feature_test
LOCAL_MODULE_TAGS := eng

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 26 && echo OK),OK)
LOCAL_PROPRIETARY_MODULE := true
endif

include $(BUILD_EXECUTABLE)

# ge2d_load_test
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE:= false

LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES:= aml_ge2d.c IONmem.c ge2d_load_test.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/kernel-headers \
	system/core/libion/include/ \
	system/core/libion/kernel-headers \
	system/core/include/ion

LOCAL_SHARED_LIBRARIES := \
    liblog \
	libge2d \
	libion

LOCAL_CFLAGS +=-g
LOCAL_CPPFLAGS := -g

LOCAL_MODULE := ge2d_load_test
LOCAL_MODULE_TAGS := eng

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 26 && echo OK),OK)
LOCAL_PROPRIETARY_MODULE := true
endif

include $(BUILD_EXECUTABLE)

# ge2d_chip_check
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE:= false

LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES:= aml_ge2d.c IONmem.c ge2d_chip_check.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/kernel-headers \
	system/core/libion/include/ \
	system/core/libion/kernel-headers \
	system/core/include/ion

LOCAL_SHARED_LIBRARIES := \
    liblog \
	libge2d \
	libion

LOCAL_CFLAGS +=-g
LOCAL_CPPFLAGS := -g

LOCAL_MODULE := ge2d_chip_check
LOCAL_MODULE_TAGS := eng

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 26 && echo OK),OK)
LOCAL_PROPRIETARY_MODULE := true
endif

include $(BUILD_EXECUTABLE)
