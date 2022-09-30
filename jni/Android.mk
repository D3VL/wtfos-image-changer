LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -fPIC -std=c99
LOCAL_LDFLAGS += -fPIC 
LOCAL_LDLIBS := -llog

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_MODULE    := draw_to_splash
LOCAL_SRC_FILES := draw_to_splash.c

include $(BUILD_EXECUTABLE)

