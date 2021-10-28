#ifndef __CAPTURE_IF__
#define __CAPTURE_IF__

#include "Base.h"

namespace android {
    struct data_in {
		/*in*/
        uint8_t* src;
		/*in*/
        int share_fd;
		/*out*/
        int dmabuf_fd;
    };
    class ICapture {
        public:
             ICapture(){};
             virtual ~ICapture(){};
        public:
            virtual int captureRGBframe(uint8_t *img, struct data_in* in)=0;
            virtual int captureYUYVframe(uint8_t *img, struct data_in* in)=0;
            virtual int captureNV21frame(StreamBuffer b, struct data_in* in)=0;
            virtual int captureYV12frame(StreamBuffer b, struct data_in* in)=0;
    };
}
#endif
