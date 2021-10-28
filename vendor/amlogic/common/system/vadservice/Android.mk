LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    log.cpp \
    VadTinymix.c \
    VadService.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils  \
    libtinyalsa \
    libbase

LOCAL_MODULE:= vadservice
LOCAL_INIT_RC := vadservice.rc
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
