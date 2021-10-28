#define LOG_NDEBUG 0
#define LOG_TAG "gdcUseFd"
#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL | ATRACE_TAG_ALWAYS)
#include <utils/Trace.h>
#include "IONmem.h"
#include "gdcUseFd.h"
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "IGdc.h"

gdcUseFd:: ~gdcUseFd() {
    ALOGD("%s: E \n",__FUNCTION__);
    if (mGDCContext) {
        free(mGDCContext);
        mGDCContext = nullptr;
    }
}

gdcUseFd::gdcUseFd() {
    ALOGD("%s: E \n",__FUNCTION__);
	ATRACE_CALL();
    mGDCContext = nullptr;
    mConfig.addr = nullptr;
    mConfig.share_fd = -1;
    mConfig.size = 0;
    //---create gdc context
    if (!mGDCContext) {
        mGDCContext = (struct gdc_usr_ctx_s*)malloc(sizeof(struct gdc_usr_ctx_s));
        if (gdc_create_ctx(mGDCContext) < 0) {
            ALOGE("failed to gdc_create_ctx");
            return ;
        }
    }
}

int gdcUseFd::gdc_init(uint32_t width, uint32_t height, int format,int plane_number) {
	ATRACE_CALL();
    ALOGD("%s: E \n",__FUNCTION__);
    uint32_t i_c_stride;
    uint32_t o_c_stride;
    uint32_t i_y_stride = AXI_WORD_ALIGN(width);
    uint32_t o_y_stride = AXI_WORD_ALIGN(width);
    switch (format) {
        case NV12:
            i_c_stride = AXI_WORD_ALIGN(width);
            o_c_stride = AXI_WORD_ALIGN(width);
            break;
        default:
            return -1;
    }

    struct gdc_settings_ex *gdc_gs = &mGDCContext->gs_ex;
    gdc_gs->gdc_config.input_width = width;
    gdc_gs->gdc_config.input_height = height;
    gdc_gs->gdc_config.input_y_stride = i_y_stride;
    gdc_gs->gdc_config.input_c_stride = i_c_stride;
    gdc_gs->gdc_config.output_width = width;
    gdc_gs->gdc_config.output_height = height;
    gdc_gs->gdc_config.output_y_stride = o_y_stride;
    gdc_gs->gdc_config.output_c_stride = o_c_stride;
    gdc_gs->gdc_config.format = NV12;
    gdc_gs->magic = sizeof(*gdc_gs);

    //-----load gdc config
    if (!load_config_file(width, height,plane_number)) {
        ALOGE("failed to load gdc config");
        return -1;
    }
    return 0;
}

void gdcUseFd::set_input_buffer(int in_fd) {
	ATRACE_CALL();
    struct gdc_settings_ex *gdc_gs = &mGDCContext->gs_ex;
    gdc_gs->input_buffer.plane_number = 1;
    gdc_gs->input_buffer.shared_fd = in_fd;
    gdc_gs->input_buffer.mem_alloc_type = AML_GDC_MEM_ION ;
}

void gdcUseFd::set_output_buffer(int out_fd) {
	ATRACE_CALL();
    struct gdc_settings_ex *gdc_gs = &mGDCContext->gs_ex;
    gdc_gs->output_buffer.plane_number = 1;
    gdc_gs->output_buffer.shared_fd = out_fd;
    gdc_gs->output_buffer.mem_alloc_type = AML_GDC_MEM_ION;
}

void gdcUseFd::gdc_exit() {
    ALOGD("%s: E \n",__FUNCTION__);
	ATRACE_CALL();
    if (mConfig.addr) {
        munmap(mConfig.addr,mConfig.size);
        mConfig.addr = nullptr;
        mConfig.size = 0;
        close(mConfig.share_fd);
        mConfig.share_fd = 0;
    }
    mGDCContext->gs_ex.config_buffer.shared_fd = -1;
    mGDCContext->gs_ex.input_buffer.shared_fd = -1;
    mGDCContext->gs_ex.output_buffer.shared_fd = -1;
    mGDCContext->plane_number = 0;
    gdc_destroy_ctx(mGDCContext);
}

//------loading config file for gdc
bool gdcUseFd::load_config_file(size_t width, size_t height,int plane_number) {
    ALOGD("%s: E \n",__FUNCTION__);
	ATRACE_CALL();
    struct gdc_settings_ex *gdc_gs = &mGDCContext->gs_ex;
    std::string fileName;

    //---read bin file to memory
    if ((width ==4096 ) && (height == 2160)) {
        fileName = "/data/4k_config.bin";
    } else if ((width == 1920) && (height == 1080)) {
        fileName = "/data/1080p_config.bin";
    } else if ((width == 640) && (height == 480)) {
        fileName = "/data/vga_config.bin";
    } else {
        fileName = "/data/1080p_config.bin";
    }

     FILE* fp = fopen(fileName.c_str(), "rb");
    if (fp == nullptr) {
        ALOGE ("failed to open gdc config file");
        return false;
    }
    //---get file length
    fseek(fp, 0L, SEEK_END);
    int filelen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    mConfig.size = filelen;
    ALOGI("config file size is %d",filelen);
    if (mConfig.addr == nullptr) {
        IONMEM_AllocParams ion_alloc_params;
        //----alloc uncached memory
        bool cache_flag = false;
        int ret = gdc_ion_mem_alloc(filelen, &ion_alloc_params, cache_flag);
        if (ret < 0) {
          ALOGE("failed to allocate config buffer %d",filelen);
          fclose(fp);
          return false;
        }
        mConfig.share_fd = ion_alloc_params.mImageFd;
        mConfig.addr = (uint8_t*)mmap(NULL,filelen,PROT_READ|PROT_WRITE,MAP_SHARED,mConfig.share_fd,0);
        int r_size = fread(mConfig.addr,1,filelen,fp);
        ALOGI("file size is %d , read size is %d \n",filelen, r_size);
        fclose(fp);
    }
  //-----set config file content in
  gdc_gs->gdc_config.config_size = filelen / 4;
  gdc_gs->config_buffer.shared_fd = mConfig.share_fd;
  gdc_gs->config_buffer.plane_number = plane_number;
  gdc_gs->config_buffer.mem_alloc_type = AML_GDC_MEM_ION;

  return true;
}

void gdcUseFd::gdc_do_fisheye_correction(struct param* p) {
	ATRACE_CALL();
    do {
        ALOGD("gdc process \n");
        set_input_buffer(p->input_fd);
        set_output_buffer(p->output_fd);
        int ret = gdc_process(mGDCContext);
        if (ret < 0) {
            ALOGE("gdc ioctl failed\n");
            gdc_exit();
            break;
        }
    }while(0);
}
