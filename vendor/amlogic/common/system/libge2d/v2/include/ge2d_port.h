/*
 * Copyright (c) 2014 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */

#ifndef GE2D_PORT_H_
#define GE2D_PORT_H_

#define GE2D_FAIL    -1
#define GE2D_SUCCESS  0

#define OSD0        0
#define OSD1        1

#define SRC1_GB_ALPHA_ENABLE 0x80000000

#if defined (__cplusplus)
extern "C" {
#endif

#define MAX_PLANE  4
typedef enum {
    GE2D_CANVAS_OSD0 = 0,
    GE2D_CANVAS_OSD1,
    GE2D_CANVAS_ALLOC,
    GE2D_CANVAS_TYPE_INVALID,
} ge2d_canvas_t;

enum ge2d_memtype_s {
    AML_GE2D_MEM_ION,
    AML_GE2D_MEM_DMABUF,
    AML_GE2D_MEM_INVALID,
};

typedef enum {
    LAYER_MODE_INVALID = 0,
    LAYER_MODE_NON = 1,
    LAYER_MODE_PREMULTIPLIED = 2,
    LAYER_MODE_COVERAGE = 3,
} layer_mode_t;

/* Blend modes, settable per layer */
typedef enum {
    BLEND_MODE_INVALID = 0,

    /* colorOut = colorSrc */
    BLEND_MODE_NONE = 1,

    /* colorOut = colorSrc + colorDst * (1 - alphaSrc) */
    BLEND_MODE_PREMULTIPLIED = 2,

    /* colorOut = colorSrc * alphaSrc + colorDst * (1 - alphaSrc) */
    BLEND_MODE_COVERAGE = 3,
} blend_mode_t;

/**
 * pixel format definitions
 */
typedef enum  {
    PIXEL_FORMAT_RGBA_8888          = 1,
    PIXEL_FORMAT_RGBX_8888          = 2,
    PIXEL_FORMAT_RGB_888            = 3,
    PIXEL_FORMAT_RGB_565            = 4,
    PIXEL_FORMAT_BGRA_8888          = 5,
    PIXEL_FORMAT_YV12               = 0x32315659, // YCrCb 4:2:0 Planar  YYYY......  U......V......
    PIXEL_FORMAT_Y8                 = 0x20203859, // YYYY
    PIXEL_FORMAT_YCbCr_422_SP       = 0x10, // NV16   YYYY.....UVUV....
    PIXEL_FORMAT_YCrCb_420_SP       = 0x11, // NV21   YYYY.....UV....
    PIXEL_FORMAT_YCbCr_422_UYVY        = 0x14,     // UYVY   U0-Y0-V0-Y1 U2-Y2-V2-Y3 U4 ...
    PIXEL_FORMAT_BGR_888,
    PIXEL_FORMAT_YCbCr_420_SP_NV12,                // NV12 YCbCr YYYY.....UV....
}pixel_format_t;

/* if customized matrix is used, set this flag in format */
#define MATRIX_CUSTOM               (0x80000000)

/* if customized stride is used, set this flag in format */
#define STRIDE_CUSTOM               (0x40000000)

/* capability flags */
#define CANVAS_STATUS   ((1 << 5) | (1 << 6))
#define HAS_SELF_POWER  (1 << 4)
#define DEEP_COLOR      (1 << 3)
#define ADVANCED_MATRIX (1 << 2)
#define SRC2_REPEAT     (1 << 1)
#define SRC2_ALPHA      (1 << 0)

typedef enum {
    GE2D_ROTATION_0,
    GE2D_ROTATION_90,
    GE2D_ROTATION_180,
    GE2D_ROTATION_270,
    GE2D_MIRROR_X,
    GE2D_MIRROR_Y,
} GE2D_ROTATION;

typedef enum {
    AML_GE2D_FILLRECTANGLE,
    AML_GE2D_BLEND,
    AML_GE2D_STRETCHBLIT,
    AML_GE2D_BLIT,
    AML_GE2D_NONE,
} GE2DOP;

typedef struct{
    int x;
    int y;
    int w;
    int h;
}rectangle_t;

typedef struct buffer_info {
    unsigned int mem_alloc_type;
    unsigned int memtype;
    char* vaddr[MAX_PLANE];
    unsigned long offset[MAX_PLANE];
    unsigned int canvas_w;
    unsigned int canvas_h;
    rectangle_t rect;
    int format;
    unsigned int rotation;
    int shared_fd[MAX_PLANE];
    unsigned char plane_alpha;
    unsigned char layer_mode;
    unsigned char fill_color_en;
    unsigned int  def_color;
    int plane_number;
} buffer_info_t;

struct ge2d_matrix_s {
    unsigned int pre_offset0;
    unsigned int pre_offset1;
    unsigned int pre_offset2;
    unsigned int coef0;
    unsigned int coef1;
    unsigned int coef2;
    unsigned int coef3;
    unsigned int coef4;
    unsigned int coef5;
    unsigned int coef6;
    unsigned int coef7;
    unsigned int coef8;
    unsigned int offset0;
    unsigned int offset1;
    unsigned int offset2;
    /* input y/cb/cr saturation enable */
    unsigned char sat_in_en;
};

struct ge2d_stride_s {
	unsigned int src1_stride[MAX_PLANE];
	unsigned int src2_stride[MAX_PLANE];
	unsigned int dst_stride[MAX_PLANE];
};

typedef struct aml_ge2d_info {
    int ge2d_fd; /* ge2d_fd */
    int ion_fd;  /* ion_fd */
    unsigned int offset;
    unsigned int blend_mode;
    GE2DOP ge2d_op;
    buffer_info_t src_info[2];
    buffer_info_t dst_info;
    unsigned int color;
    unsigned int gl_alpha;
    unsigned int const_color;
    /* means do multi ge2d op */
    unsigned int dst_op_cnt;
    int cap_attr;
    int b_src_swap;
    struct ge2d_matrix_s matrix_custom;
    struct ge2d_stride_s stride_custom;
    unsigned int reserved;
} aml_ge2d_info_t;

int ge2d_open(void);
int ge2d_close(int fd);
int ge2d_get_cap(int fd);
int ge2d_process(int fd,aml_ge2d_info_t *pge2dinfo);
int ge2d_process_ion(int fd,aml_ge2d_info_t *pge2dinfo);
#if defined (__cplusplus)
}
#endif

#endif
