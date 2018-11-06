#ifndef NATIVECRASHHANDLER_NATIVEBROADCASTHANDLER_H
#define NATIVECRASHHANDLER_NATIVEBROADCASTHANDLER_H
#include <jni.h>
#include<stdlib.h>
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MCA", __VA_ARGS__)
#define  LOGW(...) __android_log_print(ANDROID_LOG_WARN, "MCA", __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "MCA", __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
namespace android{
	void sendBroadcast(const char* action_str, const char* uri_str);
}

#ifdef __cplusplus
}
#endif

#endif //NATIVECRASHHANDLER_NATIVEBROADCASTHANDLER_H
