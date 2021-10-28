#define LOG_NDEBUG 0
#define LOG_TAG "CaptureUseGe2d"
#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL | ATRACE_TAG_ALWAYS)
#include <utils/Trace.h>
#include "CaptureUseGe2d.h"
#include "ge2d_stream.h"

namespace android {
    CaptureUseGe2d::CaptureUseGe2d(MIPIVideoInfo* info) {
        mInfo = info;
        mCameraUtil = new CameraUtil();
    }

    CaptureUseGe2d::~CaptureUseGe2d() {
        if (mCameraUtil) {
            delete mCameraUtil;
            mCameraUtil = nullptr;
        }
    }

    int CaptureUseGe2d::captureRGBframe(uint8_t *img, struct data_in* in) {
            uint8_t* src = nullptr;
            uint32_t width = mInfo->picture.format.fmt.pix.width;
            uint32_t height = mInfo->picture.format.fmt.pix.height;
            uint32_t format = mInfo->picture.format.fmt.pix.pixelformat;

            src = (uint8_t *)mInfo->get_picture();
            if (nullptr == src) {
                usleep(5000);
                return -1;
            }

            switch (format) {
                case V4L2_PIX_FMT_YUYV:
                    if (mInfo->picture.buf.length == mInfo->picture.buf.bytesused)
                        mCameraUtil->yuyv422_to_rgb24(src,img,width,height);
                    break;
                case V4L2_PIX_FMT_RGB24:
                    if (mInfo->picture.buf.length == width * height * 3) {
                        memcpy(img, src, mInfo->picture.buf.length);
                    } else {
                        mCameraUtil->rgb24_memcpy(img, src, width, height);
                    }
                    break;
                case V4L2_PIX_FMT_NV21:
                    mCameraUtil->nv21_to_rgb24(src, img, width,height);
                    break;

                default:
                    break;
            }
            return 0;
    }

    int CaptureUseGe2d::captureYUYVframe(uint8_t *img, struct data_in* in) {
            uint8_t* src = nullptr;
            uint32_t format = mInfo->preview.format.fmt.pix.pixelformat;
            src = in->src;
            if (src) {
                switch (format) {
                    case V4L2_PIX_FMT_YUYV:
                        break;
                    default:
                        ALOGE("Unable known sensor format: %d", format);
                        break;
                }
                return 0;
            }

            src = (uint8_t *)mInfo->get_frame();
            if (nullptr == src) {
                ALOGV("get frame NULL, sleep 5ms");
                usleep(5000);
                return -1;
            }

            if (format == V4L2_PIX_FMT_YUYV)
                memcpy(img, src, mInfo->preview.buf.length);

            return 0;
    }

    int CaptureUseGe2d::captureNV21frame(StreamBuffer b, struct data_in* in) {
			ATRACE_CALL();
            //uint32_t width = mInfo->preview.format.fmt.pix.width;
            //uint32_t height = mInfo->preview.format.fmt.pix.height;
            uint32_t format = mInfo->preview.format.fmt.pix.pixelformat;

            int dmabuf_fd = -1;
            //int tempfd = in->share_fd;
            /*if (tempfd != -1) {
                switch (format) {
                    case V4L2_PIX_FMT_NV21:
                        if ((width == b.width) && (height == b.height))
                            ge2dDevice::ge2d_copy(b.share_fd,tempfd,b.stride,b.height);
                        break;
                    default:
                        ALOGE("Unable known sensor format: %d", format);
                        break;
                }
                return 0;
            }*/
            struct VideoInfoBuffer vb;
            int ret = mInfo->get_frame_buffer(&vb);
            dmabuf_fd = vb.dma_fd;
            if (-1 == ret || -1 == dmabuf_fd) {
                ALOGV("%s:get frame fd fail!, sleep 5ms",__FUNCTION__);
                usleep(5000);
                return -1;
            }

            switch (format) {
                case V4L2_PIX_FMT_NV21:
                    if (mInfo->preview.buf.length == b.width * b.height * 3/2) {
                        ALOGV("%s:dma buffer fd = %d \n",__FUNCTION__,dmabuf_fd);
                        ge2dDevice::ge2d_copy(b.share_fd,dmabuf_fd,b.stride,b.height,ge2dDevice::NV12);
                    }
                    break;
                default:
                    break;
            }
            in->dmabuf_fd = dmabuf_fd;
            return 0;
    }

    int CaptureUseGe2d::captureYV12frame(StreamBuffer b, struct data_in* in) {
			ATRACE_CALL();
            int dmabuf_fd = -1;
            uint32_t format = mInfo->preview.format.fmt.pix.pixelformat;

            struct VideoInfoBuffer vb;
            int ret = mInfo->get_frame_buffer(&vb);
            if (-1 == ret) {
                ALOGV("get frame NULL, sleep 5ms");
                usleep(5000);
                return -1;
            }
            dmabuf_fd = vb.dma_fd;
            switch (format) {
                case V4L2_PIX_FMT_YVU420:
                    if (mInfo->preview.buf.length == b.width * b.height * 3/2) {
                        ge2dDevice::ge2d_copy(b.share_fd,dmabuf_fd,b.stride,b.height,ge2dDevice::NV12);
                    }
                    break;
                default:
                    break;
                }
                return 0;
     }

}
