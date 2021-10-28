#ifndef __CAPTUREUSEMEMCPY_H__
#define __CAPTUREUSEMEMCPY_H__
#include "MIPICameraIO.h"
#include "CameraUtil.h"
#include "ICapture.h"
#include "Base.h"

namespace android {
    class CaptureUseMemcpy:public ICapture {
        protected:
            MIPIVideoInfo* mInfo;
            CameraUtil* mCameraUtil;
        public:
            CaptureUseMemcpy(MIPIVideoInfo* info);
            virtual ~CaptureUseMemcpy();
            int captureRGBframe(uint8_t *img, struct data_in* in) override;
            int captureYUYVframe(uint8_t *img, struct data_in* in) override;
            int captureNV21frame(StreamBuffer b, struct data_in* in) override;
            int captureYV12frame(StreamBuffer b, struct data_in* in) override;
    };
}
#endif
