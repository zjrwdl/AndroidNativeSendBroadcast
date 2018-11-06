LOCAL_PATH:= $(call my-dir)
  
include $(CLEAR_VARS)
  
LOCAL_SRC_FILES:= NativeBroadCast.cpp NativeBroadCastHandler.cpp \
  
LOCAL_SHARED_LIBRARIES:= libutils libutils libbinder liblog
  
LOCAL_C_INCLUDES := $(TOP)/frameworks/base/include/ \
					$(LOCAL_PATH)/ \
	
					
LOCAL_MODULE:= nativeBroadcast
  
LOCAL_PRELINK_MODULE:= false
  
include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
