#define LOG_TAG "ISP Library"

#include "Isp3a.h"
#include <utils/Log.h>
#include "ispaaalib.h"

namespace android{

isp3a* isp3a::mInstance = nullptr;

isp3a* isp3a::get_instance(void) {
    if (mInstance == nullptr) {
        mInstance = new isp3a();
        return mInstance;
    }
    else
        return mInstance;
}

isp3a::isp3a() {
    ALOGD("%s:create isp3a object",__FUNCTION__);
    mIsOpened = false;
    mOpenCount = 0;
}
isp3a::~isp3a() {
    ALOGD("%s:release isp3a object",__FUNCTION__);
}
void isp3a::open_isp3a_library() {
    ALOGD("%s:E",__FUNCTION__);
#ifdef ISP_ENABLE
    if (mIsOpened == false && mOpenCount == 0) {
        ALOGD("%s:enable isp lib",__FUNCTION__);
        isp_lib_enable();
        mIsOpened = true;
    } else {
        ALOGD("%s:isp lib has been opened",__FUNCTION__);
    }
    mOpenCount += 1;
#endif
}

void isp3a::close_isp3a_library() {
     ALOGD("%s:E",__FUNCTION__);
#ifdef ISP_ENABLE
    mOpenCount -= 1;
    if (mIsOpened && mOpenCount == 0) {
        ALOGD("%s:disable isp lib",__FUNCTION__);
        isp_lib_disable();
        mIsOpened = false;
    }
#endif
}

void isp3a::print_status() {
    if (mIsOpened == false) {
        ALOGD("isp lib is closed");
        ALOGD("current open count is %d",mOpenCount);
    } else {
        ALOGD("isp lib has opend");
        ALOGD("current open count is %d",mOpenCount);
    }
}
}
