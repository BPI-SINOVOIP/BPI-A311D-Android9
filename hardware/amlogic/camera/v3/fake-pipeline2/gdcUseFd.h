#ifndef _GDC_V2_H_
#define _GDC_V2_H_

#include "gdc_api.h"
#include "IGdc.h"
struct ion_buffer {
 int share_fd;
 uint8_t* addr;
 size_t size;
};

class gdcUseFd:public IGdc{
    public:
        gdcUseFd();
        ~gdcUseFd();
        int gdc_init(size_t width, size_t height, int gdc_format , int plane_number) override;
        void gdc_exit() override;
        void gdc_do_fisheye_correction(struct param* p) override;
    public:
        struct gdc_usr_ctx_s *mGDCContext;
        struct ion_buffer mConfig;
    private:
        void set_input_buffer(int in_fd);
        void set_output_buffer(int out_fd);
        bool load_config_file(size_t width, size_t height,int plane_number);
};

#endif

