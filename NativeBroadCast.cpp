#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <private/android_filesystem_config.h>
#include "NativeBroadCastHandler.h"
using namespace android;


int main(int argc, char *argv[])
{
	sp<ProcessState> proc(ProcessState::self());
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
	
    const char ACTION_POWER_DOWN_NOW[] = "cn.native.broadcasst.test end";
    sendBroadcast(ACTION_POWER_DOWN_NOW, NULL);
	return 0;
}








