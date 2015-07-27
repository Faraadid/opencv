LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := OpenCL
LOCAL_SRC_FILES := $(OPENCL_SDK)/lib/$(TARGET_ARCH_ABI)/libOpenCL.so
LOCAL_EXPORT_C_INCLUDES := $(OPENCL_SDK)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OPENCV_INSTALL_MODULES:=on
ifeq ($(O4A_SDK_ROOT),)
    include ../../sdk/native/jni/OpenCV.mk
else
    include $(O4A_SDK_ROOT)/sdk/native/jni/OpenCV.mk
endif

LOCAL_MODULE    := JNIrender
LOCAL_SRC_FILES := jni.c GLrender.cpp CLprocessor.cpp
LOCAL_LDLIBS    += -llog -lGLESv2 -lEGL
LOCAL_SHARED_LIBRARIES += OpenCL
include $(BUILD_SHARED_LIBRARY)