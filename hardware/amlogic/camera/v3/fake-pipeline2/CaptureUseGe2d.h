#ifndef __CAPTUREUSEGE2D_H__
#define __CAPTUREUSEGE2D_H__
#include "Base.h"
#include "ICapture.h"
#include "MIPICameraIO.h"
#include "CameraUtil.h"
namespace android {
    class CaptureUseGe2d:public ICapture {
        protected:
            MIPIVideoInfo* mInfo;
            CameraUtil* mCameraUtil;
        public:
            CaptureUseGe2d(MIPIVideoInfo* info);
            virtual ~CaptureUseGe2d();
            int captureRGBframe(uint8_t *img, struct data_in* in) override;
            int captureYUYVframe(uint8_t *img, struct data_in* in) override;
            int captureNV21frame(StreamBuffer b, struct data_in* in) override;
            int captureYV12frame(StreamBuffer b, struct data_in* in) override;
    };
}
#endif
