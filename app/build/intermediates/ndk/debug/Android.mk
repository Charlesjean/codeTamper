LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := dexfileparser
LOCAL_LDLIBS := \
	-llog \

LOCAL_SRC_FILES := \
	/Users/duanjin/Documents/personal/project/hello-jni3/app/src/main/jni/Android.mk \
	/Users/duanjin/Documents/personal/project/hello-jni3/app/src/main/jni/Application.mk \
	/Users/duanjin/Documents/personal/project/hello-jni3/app/src/main/jni/DexClass.cpp \
	/Users/duanjin/Documents/personal/project/hello-jni3/app/src/main/jni/DexFileParser.cpp \
	/Users/duanjin/Documents/personal/project/hello-jni3/app/src/main/jni/Leb128.cpp \

LOCAL_C_INCLUDES += /Users/duanjin/Documents/personal/project/hello-jni3/app/src/main/jni
LOCAL_C_INCLUDES += /Users/duanjin/Documents/personal/project/hello-jni3/app/src/debug/jni

include $(BUILD_SHARED_LIBRARY)
