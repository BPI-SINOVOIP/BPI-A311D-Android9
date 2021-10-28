#ifndef __MIPI_CAMERA_IO__
#define __MIPI_CAMERA_IO__

#define LOG_NDEBUG 0
#define LOG_TAG "MIPI_Camera_IO"
#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL | ATRACE_TAG_ALWAYS)
#include <utils/Trace.h>

#include <errno.h>
#include <cutils/properties.h>
#include "MIPICameraIO.h"


namespace android {

MIPIVideoInfo::MIPIVideoInfo() {
    ALOGD("mipi video info constructer!");

}

MIPIVideoInfo::~MIPIVideoInfo() {
    ALOGD("mipi video info destructer!");
    for (uint32_t i = 0; i < mem.size(); i++) {
        if (mem[i].dma_fd != -1)
            close(mem[i].dma_fd);
    }
}

//----get dmabuf file descritor according to index
int MIPIVideoInfo::export_dmabuf_fd(int v4lfd, int index, int* dmafd)
{
    struct v4l2_exportbuffer expbuf;
    memset(&expbuf,0,sizeof(expbuf));
    expbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    expbuf.index = index;
    expbuf.flags = 0;
    expbuf.fd = -1;
    if (ioctl(v4lfd,VIDIOC_EXPBUF,&expbuf) == -1) {
        ALOGE("export buffer fail:%s",strerror(errno));
        return -1;
    } else {
        ALOGD("dma buffer fd = %d \n",expbuf.fd);
        *dmafd = expbuf.fd;
    }
    return 0;
}

//
int MIPIVideoInfo::start_capturing(void)
{
        int ret = 0;
        int i;
        enum v4l2_buf_type type;
        struct  v4l2_buffer buf;

        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }

        if (isStreaming)
            ALOGD("already stream on\n");

        //----allocate memory
        CLEAR(preview.rb);
        mem.resize(IO_PREVIEW_BUFFER);
        preview.rb.count = IO_PREVIEW_BUFFER;
        preview.rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        preview.rb.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd, VIDIOC_REQBUFS, &preview.rb);
        if (ret < 0) {
            DBG_LOGB("camera idx:%d does not support "
                      "memory mapping, errno=%d\n", idx, errno);
        }
        if (preview.rb.count < 2) {
            DBG_LOGB( "Insufficient buffer memory on /dev/video%d, errno=%d\n",
                            idx, errno);
            return -EINVAL;
        }

        //----map memory to user space
        for (i = 0; i < (int)preview.rb.count; ++i) {
            CLEAR(preview.buf);
            preview.buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            preview.buf.memory      = V4L2_MEMORY_MMAP;
            preview.buf.index       = i;

            if (ioctl(fd, VIDIOC_QUERYBUF, &preview.buf) < 0) {
                    DBG_LOGB("VIDIOC_QUERYBUF, errno=%d", errno);
            }

            mem[i].size = preview.buf.length;
            mem[i].addr = mmap(NULL, // start anywhere
                                mem[i].size,
                                PROT_READ | PROT_WRITE, // required
                                MAP_SHARED, // recommended
                                fd,        //video device fd
                                preview.buf.m.offset);

            if (MAP_FAILED == mem[i].addr) {
                ALOGE("mmap failed,%s\n", strerror(errno));
            }
            int dma_fd = -1;
            int ret = export_dmabuf_fd(fd,i, &dma_fd);
            if (ret) {
                ALOGE("export dma fd failed,%s\n", strerror(errno));
            } else {
                mem[i].dma_fd = dma_fd;
                ALOGD("index = %d, dma_fd = %d \n",i,mem[i].dma_fd);
            }
        }
        //----queue buffer to driver's video buffer queue
        for (i = 0; i < (int)preview.rb.count; ++i) {

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
                        DBG_LOGB("VIDIOC_QBUF failed, errno=%d\n", errno);
        }
        //----stream on----
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if ((preview.format.fmt.pix.width != 0) &&
               (preview.format.fmt.pix.height != 0)) {
             if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
                  DBG_LOGB("VIDIOC_STREAMON, errno=%d\n", errno);
        }

        isStreaming = true;
        return 0;
}

int MIPIVideoInfo::stop_capturing(void)
{
        enum v4l2_buf_type type;
        int res = 0;
        int i;
        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }

        if (!isStreaming)
                return -1;

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
                DBG_LOGB("VIDIOC_STREAMOFF, errno=%d", errno);
                res = -1;
        }

        if (!preview.buf.length) {
            preview.buf.length = tempbuflen;
        }

        for (i = 0; i < (int)preview.rb.count; ++i) {
                if (munmap(mem[i].addr, mem[i].size) < 0) {
                        DBG_LOGB("munmap failed errno=%d", errno);
                        res = -1;
                }
        }

        if (strstr((const char *)cap.driver, "ARM-camera-isp")) {
            preview.format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            preview.rb.memory = V4L2_MEMORY_MMAP;
            preview.rb.count = 0;

            res = ioctl(fd, VIDIOC_REQBUFS, &preview.rb);
            if (res < 0) {
                DBG_LOGB("VIDIOC_REQBUFS failed: %s", strerror(errno));
            }else{
                DBG_LOGA("VIDIOC_REQBUFS delete buffer success\n");
            }
        }

        isStreaming = false;
        return res;
}

int MIPIVideoInfo::releasebuf_and_stop_capturing(void)
{
        enum v4l2_buf_type type;
        int res = 0 ,ret;
        int i;

        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }
        if (!isStreaming)
                return -1;

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if ((preview.format.fmt.pix.width != 0) &&
               (preview.format.fmt.pix.height != 0)) {
            if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
                 DBG_LOGB("VIDIOC_STREAMOFF, errno=%d", errno);
                 res = -1;
            }
        }
        if (!preview.buf.length) {
            preview.buf.length = tempbuflen;
        }
        for (i = 0; i < (int)preview.rb.count; ++i) {
                if (munmap(mem[i].addr, mem[i].size) < 0) {
                        DBG_LOGB("munmap failed errno=%d", errno);
                        res = -1;
                }
        }
        isStreaming = false;

        preview.format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        preview.rb.memory = V4L2_MEMORY_MMAP;
        preview.rb.count = 0;

        ret = ioctl(fd, VIDIOC_REQBUFS, &preview.rb);
        if (ret < 0) {
           DBG_LOGB("VIDIOC_REQBUFS failed: %s", strerror(errno));
           //return ret;
        }else{
           DBG_LOGA("VIDIOC_REQBUFS delete buffer success\n");
        }
        return res;
}


uintptr_t MIPIVideoInfo::get_frame_phys(void)
{
        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }
        CLEAR(preview.buf);

        preview.buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        preview.buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &preview.buf) < 0) {
                switch (errno) {
                        case EAGAIN:
                                return 0;
                        case EIO:
                        default:
                                DBG_LOGB("VIDIOC_DQBUF failed, errno=%d\n", errno);
                                exit(1);
                }
        DBG_LOGB("VIDIOC_DQBUF failed, errno=%d\n", errno);
        }

        return (uintptr_t)preview.buf.m.userptr;
}

int MIPIVideoInfo::get_frame_index(FrameV4L2Info& info) {
        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }
        CLEAR(info.buf);
        info.buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        info.buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &info.buf) < 0) {
            switch (errno) {
                case EAGAIN:
                    return -1;

                case EIO:
                default:
                    CAMHAL_LOGDB("VIDIOC_DQBUF failed, errno=%d\n", errno);
                    //here will generate crash, so delete.  when ocour error, should break while() loop
                    //exit(1);
                    if (errno == ENODEV) {
                        ALOGE("camera device is not exist!");
                        set_device_status();
                        close(fd);
                        fd = -1;
                    }
                    return -1;
            }
        }
        return info.buf.index;
}

int MIPIVideoInfo::get_frame_buffer(struct VideoInfoBuffer* b)
{
    int index = get_frame_index(preview);
    ALOGD("%s:index = %d \n",__FUNCTION__,index);
    if (index < 0)
        return -1;
    else {
        index = index % IO_PREVIEW_BUFFER;
        ALOGD("%s: index=%d,dma_fd=%d\n",__FUNCTION__,index,mem[index].dma_fd);
        b->addr = mem[index].addr;
        b->size = mem[index].size;
        b->dma_fd = mem[index].dma_fd;
    }
    return 0;
}

int MIPIVideoInfo::putback_frame()
{
        if (dev_status == -1)
            return 0;

        if (!preview.buf.length) {
            preview.buf.length = tempbuflen;
        }
        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }

        if (ioctl(fd, VIDIOC_QBUF, &preview.buf) < 0) {
            DBG_LOGB("QBUF failed :%s\n", strerror(errno));
            if (errno == ENODEV) {
                set_device_status();
            }
        }

        return 0;
}

int MIPIVideoInfo::putback_picture_frame()
{
        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }
        if (ioctl(fd, VIDIOC_QBUF, &picture.buf) < 0)
                DBG_LOGB("QBUF failed error=%d\n", errno);

        return 0;
}

int MIPIVideoInfo::start_picture(int rotate)
{
        int ret = 0;
        int i;
        enum v4l2_buf_type type;
        struct  v4l2_buffer buf;
        bool usbcamera = false;

        CLEAR(picture.rb);
        if (fd < 0) {
            ALOGE("camera not be init!");
            return -1;
        }

        //step 1 : ioctl  VIDIOC_S_FMT
        for (int i = 0; i < 3; i++) {
            ret = ioctl(fd, VIDIOC_S_FMT, &picture.format);
            if (ret < 0 ) {
             switch (errno) {
                 case  -EBUSY:
                 case  0:
                    usleep(3000); //3ms
                    continue;
                    default:
                    DBG_LOGB("Open: VIDIOC_S_FMT Failed: %s, errno=%d\n", strerror(errno), ret);
                 return ret;
             }
            }else
            break;
        }
        //step 2 : request buffer
        picture.rb.count = IO_PICTURE_BUFFER;
        picture.rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        picture.rb.memory = V4L2_MEMORY_MMAP;

        ret = ioctl(fd, VIDIOC_REQBUFS, &picture.rb);
        if (ret < 0 ) {
                DBG_LOGB("camera idx:%d does not support "
                                "memory mapping, errno=%d\n", idx, errno);
        }

        if (picture.rb.count < 1) {
                DBG_LOGB( "Insufficient buffer memory on /dev/video%d, errno=%d\n",
                                idx, errno);
                return -EINVAL;
        }

        //step 3: mmap buffer
        for (i = 0; i < (int)picture.rb.count; ++i) {
            CLEAR(picture.buf);
            picture.buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            picture.buf.memory      = V4L2_MEMORY_MMAP;
            picture.buf.index       = i;

            if (ioctl(fd, VIDIOC_QUERYBUF, &picture.buf) < 0) {
                    ALOGE("VIDIOC_QUERYBUF, errno=%d", errno);
            }
            mem_pic[i].size = picture.buf.length;
            mem_pic[i].addr = mmap(NULL, // start anywhere
                                    mem_pic[i].size,
                                    PROT_READ | PROT_WRITE, // required
                                    MAP_SHARED, //recommended
                                    fd,
                                    picture.buf.m.offset);

            if (MAP_FAILED == mem_pic[i].addr) {
                    ALOGE("mmap failed, errno=%d\n", errno);
            }
            int dma_fd = -1;
            int ret = export_dmabuf_fd(fd,i, &dma_fd);
            if (ret) {
                ALOGE("export dma fd failed,%s\n", strerror(errno));
            } else {
                mem_pic[i].dma_fd = dma_fd;
            }
        }

        //step 4 : QBUF
        for (i = 0; i < (int)picture.rb.count; ++i) {

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
                        DBG_LOGB("VIDIOC_QBUF failed, errno=%d\n", errno);
        }

        if (isPicture) {
                DBG_LOGA("already stream on\n");
        }

        if (strstr((const char *)cap.driver, "uvcvideo")) {
            usbcamera = true;
        }

        //step 5: Stream ON
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
                DBG_LOGB("VIDIOC_STREAMON, errno=%d\n", errno);
        isPicture = true;

        return 0;
}

int MIPIVideoInfo::get_picture_fd()
{
    DBG_LOGA("get picture\n");
    int index = get_frame_index(picture);
    if (index < 0)
        return -1;
    return mem_pic[picture.buf.index].dma_fd;
}

void MIPIVideoInfo::stop_picture()
{
        enum v4l2_buf_type type;
        struct  v4l2_buffer buf;
        int i;
        int ret;
        if (fd < 0) {
            ALOGE("camera not be init!");
            return ;
        }

        if (!isPicture)
                return ;

        //QBUF
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = picture.buf.index;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
            DBG_LOGB("VIDIOC_QBUF failed, errno=%d\n", errno);

        //stream off and unmap buffer
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
                DBG_LOGB("VIDIOC_STREAMOFF, errno=%d", errno);

        for (i = 0; i < (int)picture.rb.count; i++)
        {
            if (munmap(mem_pic[i].addr, mem_pic[i].size) < 0)
                DBG_LOGB("munmap failed errno=%d", errno);
        }
        picture.format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        picture.rb.memory = V4L2_MEMORY_MMAP;
        picture.rb.count = 0;

        ret = ioctl(fd, VIDIOC_REQBUFS, &picture.rb);
        if (ret < 0) {
            DBG_LOGB("VIDIOC_REQBUFS failed: %s", strerror(errno));
        } else {
            DBG_LOGA("VIDIOC_REQBUFS delete buffer success\n");
        }
        isPicture = false;
        setBuffersFormat();
        start_capturing();
}

void MIPIVideoInfo::releasebuf_and_stop_picture()
{
        enum v4l2_buf_type type;
        struct  v4l2_buffer buf;
        int i,ret;
        if (fd < 0) {
            ALOGE("camera not be init!");
            return;
        }

        if (!isPicture)
                return ;

        //QBUF
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = picture.buf.index;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
            DBG_LOGB("VIDIOC_QBUF failed, errno=%d\n", errno);

        //stream off and unmap buffer
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
                DBG_LOGB("VIDIOC_STREAMOFF, errno=%d", errno);

        for (i = 0; i < (int)picture.rb.count; i++)
        {
            if (munmap(mem_pic[i].addr, mem_pic[i].size) < 0)
                DBG_LOGB("munmap failed errno=%d", errno);
        }

        isPicture = false;

        picture.format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        picture.rb.memory = V4L2_MEMORY_MMAP;
        picture.rb.count = 0;
        ret = ioctl(fd, VIDIOC_REQBUFS, &picture.rb);
        if (ret < 0) {
          DBG_LOGB("VIDIOC_REQBUFS failed: %s", strerror(errno));
          //return ret;
        }else{
          DBG_LOGA("VIDIOC_REQBUFS delete buffer success\n");
        }
        setBuffersFormat();
        start_capturing();
}

}
#endif

