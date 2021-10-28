#ifndef _GDC_H_
#define _GDC_H_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "gdc_api.h"
#include "IGdc.h"

namespace android {
    class gdcUseMemcpy :public IGdc{
        public:
           gdcUseMemcpy();
            ~gdcUseMemcpy();
            int gdc_init(size_t width, size_t height,int gdc_format,int plane_number) override;
            void gdc_exit() override;
            void gdc_do_fisheye_correction(struct param*) override;
		private:
			struct gdc_usr_ctx_s mGdcCtx;
        private:
            void gdc_set_input_buffer(uint8_t *img);
            void gdc_set_output_buffer(uint8_t *img);
    };
}
#endif
