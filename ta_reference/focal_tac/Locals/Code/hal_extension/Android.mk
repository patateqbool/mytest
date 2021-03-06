LOCAL_PATH := $(call my-dir)
# -----------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE    := libFocalHalExt

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_SRC_FILES :=  $(LOCAL_PATH)/lib_interface.c  \
                    $(LOCAL_PATH)/sensor.c         \
                    $(LOCAL_PATH)/sensor_irq.c     \
                    $(LOCAL_PATH)/sensor_ctrl.c    \
                    $(LOCAL_PATH)/util.c

LOCAL_CFLAGS := -Wall -std=c99 -DLOG_TAG='"FocalHalExtension"'

include $(BUILD_STATIC_LIBRARY)

# -----------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE    := focal_fake_hal

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_SRC_FILES :=  $(LOCAL_PATH)/main_test.c  \
                    $(LOCAL_PATH)/sensor.c     \
                    $(LOCAL_PATH)/sensor_irq.c \
                    $(LOCAL_PATH)/sensor_ctrl.c\
                    $(LOCAL_PATH)/util.c

LOCAL_CFLAGS := -Wall -std=c99 -DLOG_TAG='"FocalFakeHal"'

LOCAL_PRELINK_MODULE := false

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

# -----------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE    := focalHalExt_libtest

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_SRC_FILES :=  $(LOCAL_PATH)/main_lib_test.c

LOCAL_CFLAGS := -Wall -std=c99 -DLOG_TAG='"FocalHalExt_libtest"'

LOCAL_PRELINK_MODULE := false

LIB_PATH := $(LOCAL_PATH)/out/focal_fake_hal/libs/arm64-v8a

LOCAL_LDLIBS := -llog

LOCAL_STATIC_LIBRARIES := libFocalHalExt

include $(BUILD_EXECUTABLE)

# -----------------------------------------------------------------------------


