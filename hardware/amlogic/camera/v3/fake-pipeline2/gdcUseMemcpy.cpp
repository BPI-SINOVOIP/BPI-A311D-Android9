#define LOG_NDEBUG 0
#define LOG_TAG "gdc"
#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL | ATRACE_TAG_ALWAYS)
#include <utils/Trace.h>

#include "gdcUseMemcpy.h"
#include "IONmem.h"

namespace android {
gdcUseMemcpy::gdcUseMemcpy() {
    ALOGD("%s: E \n",__FUNCTION__);
    memset(&mGdcCtx, 0, sizeof(mGdcCtx));
}

gdcUseMemcpy::~gdcUseMemcpy() {
    ALOGD("%s: E \n",__FUNCTION__);
}

int gdcUseMemcpy::gdc_init(size_t width, size_t height, int gdc_format , int plane_number) {
    struct gdc_param g_param;
    int ret = 0;
    g_param.i_width = width;
    g_param.i_height = height;
    g_param.o_width = width;
    g_param.o_height = height;
    g_param.format = gdc_format;

    ALOGE("%s: E \n",__FUNCTION__);
    mGdcCtx.plane_number = plane_number;
    mGdcCtx.custom_fw = 0;
    mGdcCtx.mem_type = AML_GDC_MEM_ION;
    if ((width ==4096 ) && (height == 2160)) {
        ret = gdc_init_cfg(&mGdcCtx,&g_param,"/data/4k_config.bin");
    } else if ((width == 1920) && (height == 1080)) {
        ret = gdc_init_cfg(&mGdcCtx, &g_param, "/data/1080p_config.bin");
    } else if ((width == 1280) && (height == 720)) {
        ret = gdc_init_cfg(&mGdcCtx, &g_param, "/data/720p_config.bin");
    } else if ((width == 640) && (height == 480)) {
        ret = gdc_init_cfg(&mGdcCtx, &g_param, "/data/vga_config.bin");
    } else {
        ret = gdc_init_cfg(&mGdcCtx, &g_param, "/data/1080p_config.bin");
    }

    if (ret < 0) {
        ALOGE("%s:Error gdc init\n",__FUNCTION__);
        gdc_destroy_ctx(&mGdcCtx);
        return -1;
    }
    return ret;
}

void gdcUseMemcpy::gdc_exit() {
    gdc_destroy_ctx(&mGdcCtx);
}

void gdcUseMemcpy::gdc_set_input_buffer(uint8_t *img) {
    int i;
    uint8_t *p = img;

    if (img == NULL) {
        ALOGE("%s, Error input param\n", __FUNCTION__);
        return ;
    }

    for (i = 0; i < mGdcCtx.plane_number; i++) {
        if (mGdcCtx.i_buff[i] == NULL || mGdcCtx.i_len[i] == 0) {
            ALOGE("%s, Error input param, plane_id=%d\n", __FUNCTION__, i);
            return ;
        }
        memcpy(mGdcCtx.i_buff[i], p , mGdcCtx.i_len[i]);
        p += mGdcCtx.i_len[i];
    }
}

void gdcUseMemcpy::gdc_set_output_buffer(uint8_t *img) {
    int i;
    uint8_t *p = img;

    if (img == NULL) {
        ALOGE("%s, Error output param\n", __FUNCTION__);
        return ;
    }

    for (i = 0; i < mGdcCtx.plane_number; i++) {
        if (mGdcCtx.o_buff[i] == NULL || mGdcCtx.o_len[i] == 0) {
            ALOGE("%s, Error output param, plane_id=%d\n", __FUNCTION__, i);
            return ;
        }
        memcpy(p, mGdcCtx.o_buff[i], mGdcCtx.o_len[i]);
        p += mGdcCtx.o_len[i];
    }
}

void gdcUseMemcpy::gdc_do_fisheye_correction(struct param* p) {
    gdc_set_input_buffer(p->img);
    do {
        ALOGD("gdc process \n");
        int ret = gdc_process(&mGdcCtx);
        if (ret < 0) {
            ALOGE("gdc ioctl failed\n");
            gdc_destroy_ctx(&mGdcCtx);
            break;
        }
        gdc_set_output_buffer(p->img);
    }while(0);
}

}


