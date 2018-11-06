#include <fcntl.h>
#include <unistd.h>

#include <binder/MemoryHeapBase.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include "NativeBroadCastHandler.h"

namespace android
{

    namespace {
        sp<IBinder>              gActivityManagerService;
        const int                kActivityManagerServicePollDelay = 500000;      // 0.5s
        const char*              kActivityManagerServiceName      = "activity";  

        Mutex                    gLock;

        class DeathNotifier : public IBinder::DeathRecipient
        {
        public:
            DeathNotifier() {
            }

            virtual void binderDied(const wp<IBinder>& who) {
                ALOGV("binderDied");
                Mutex::Autolock _l(gLock);
                gActivityManagerService.clear();
                ALOGW("ActivityManager service died!");
            }
        };

        sp<DeathNotifier>         gDeathNotifier;
    };


    const sp<IBinder>& getActivityManagerService()
    {
        Mutex::Autolock _l(gLock);
        if (gActivityManagerService.get() == 0) {
            sp<IServiceManager> sm = defaultServiceManager();
            sp<IBinder> binder;
            do {
                binder = sm->getService(String16(kActivityManagerServiceName));
                if (binder != 0) {
                    break;
                }
                ALOGW("ActivityManagerService not published, waiting...");
                usleep(kActivityManagerServicePollDelay);
            } while(true);
            if (gDeathNotifier == NULL) {
                gDeathNotifier = new DeathNotifier();
            }
            binder->linkToDeath(gDeathNotifier);
            gActivityManagerService = binder;
        }
        ALOGE_IF(gActivityManagerService == 0, "no ActivityManagerService!?");
        return gActivityManagerService;
    }


    typedef enum URI_TYPE_ID
    {
        NULL_TYPE_ID    = 0,
        StringUri       = 1,
        OpaqueUri       = 2,
        HierarchicalUri = 3,
    };


    typedef enum VALUE_TYPE
    {
        VAL_NULL = -1,
        VAL_STRING = 0,
        VAL_INTEGER = 1,
        VAL_MAP = 2,
        VAL_BUNDLE = 3,
        VAL_PARCELABLE = 4,
        VAL_SHORT = 5,
        VAL_LONG = 6,
        VAL_FLOAT = 7,
        VAL_DOUBLE = 8,
        VAL_BOOLEAN = 9,
        VAL_CHARSEQUENCE = 10,
        VAL_LIST  = 11,
        VAL_SPARSEARRAY = 12,
        VAL_BYTEARRAY = 13,
        VAL_STRINGARRAY = 14,
        VAL_IBINDER = 15,
        VAL_PARCELABLEARRAY = 16,
        VAL_OBJECTARRAY = 17,
        VAL_INTARRAY = 18,
        VAL_LONGARRAY = 19,
        VAL_BYTE = 20,
        VAL_SERIALIZABLE = 21,
        VAL_SPARSEBOOLEANARRAY = 22,
        VAL_BOOLEANARRAY = 23,
        VAL_CHARSEQUENCEARRAY = 24,
    };


    const uint32_t BROADCAST_INTENT_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 13;

    void sendBroadcast(const char* action_str, const char* uri_str)
    {
        Parcel data, reply;

        sp<IBinder> am = getActivityManagerService();
        if(am == NULL)
        {
            ALOGE("sendBroadcast(%s) get ActivityManager failed\n", String8(uri_str).string());
            return;
        }

        data.writeInterfaceToken(String16("android.app.IActivityManager"));
        data.writeStrongBinder(NULL);             /* caller */
        /* intent */
        data.writeString16(String16(action_str)); /* action */
        if(uri_str == NULL)
        {
            data.writeInt32(NULL_TYPE_ID);
        }
        else
        {
            // only support StringUri type
            data.writeInt32(StringUri);             /* Uri - type */
            data.writeString16(String16(uri_str));  /* uri string if URI_TYPE_ID set */
        }
        data.writeString16(NULL, 0);              /* type */
        data.writeInt32(0);                       /* flags */
        data.writeString16(NULL, 0);              /* package name */
        data.writeString16(NULL, 0);              /* ComponentName */
        data.writeInt32(0);                       /* source bound - size */
        data.writeInt32(0);                       /* Categories - size */
        data.writeInt32(0);                       /* selector - size */
        data.writeInt32(0);                       /* ClipData */
        data.writeInt32(-1);                      /* bundle(extras) size */
        if(1)
        {
            data.writeInt32(0x4C444E42); // 'B' 'N' 'D' 'L'
            int oldPos = data.dataPosition();
            { /* writeMapInternal */
                data.writeInt32(1); /* size */
                data.writeInt32(VAL_STRING);
                data.writeString16(String16("status"));
                data.writeInt32(VAL_STRING);
                data.writeString16(String16("OFF"));
            }
            int newPos = data.dataPosition();
            data.setDataPosition(oldPos - 8);
            data.writeInt32(newPos - oldPos); /* length */
            data.setDataPosition(newPos);
        }
        /* end of intent */
        data.writeString16(NULL, 0);  /* resolvedType */
        data.writeStrongBinder(NULL); /* resultTo */
        data.writeInt32(-1);          /* result code */
        data.writeString16(NULL, 0);  /* result data */
        data.writeInt32(-1);          /* no result extra */
        data.writeString16(NULL, 0);  /* permission */
        data.writeInt32(false);       /* app operation in AppOpsManager */
        data.writeInt32(false);       /* serialized */
        data.writeInt32(false);       /* sticky */
        data.writeInt32(false);       /* userid */
		LOGI("am->transact");
        status_t ret = am->transact(BROADCAST_INTENT_TRANSACTION, data, &reply);
        if (ret == NO_ERROR)
        {
            int32_t exceptionCode = reply.readExceptionCode();
            if (!exceptionCode)
            {
                ALOGD("sendBroadcast(%s) succeed\n", String8(action_str).string());
            }
            else
            {
                // An exception was thrown back; fall through to return failure
                ALOGE("sendBroadcast(%s) caught exception %d\n", String8(action_str).string(), exceptionCode);
            }
        }
    }
  }