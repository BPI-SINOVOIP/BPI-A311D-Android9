#ifndef _IGDC_H_
#define _IGDC_H_

struct param {
    uint8_t* img;
    int input_fd;
    int output_fd;
};
class IGdc {
public:
    IGdc(){};
    virtual ~IGdc(){};
public:
    virtual int gdc_init(size_t width, size_t height, int gdc_format , int plane_number) = 0;
    virtual void gdc_exit() = 0;
    virtual void gdc_do_fisheye_correction(struct param* p) = 0;

};


#endif
