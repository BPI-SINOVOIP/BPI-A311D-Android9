#ifndef __MIPI_CAMERA_IO_H__
#define __MIPI_CAMERA_IO_H__

#include "CameraIO.h"

namespace android {
class MIPIVideoInfo:public CVideoInfo {
public:
    MIPIVideoInfo();
    ~MIPIVideoInfo();
public:
    int start_capturing(void) override;
    int stop_capturing(void) override;
    int releasebuf_and_stop_capturing(void) override;
    uintptr_t get_frame_phys(void) override;
    int putback_frame() override;
    int putback_picture_frame() override;
    int start_picture(int rotate) override;
    void stop_picture() override;
    void releasebuf_and_stop_picture() override;
    int get_picture_fd();
    int get_frame_buffer(struct VideoInfoBuffer* b);
private:
    int get_frame_index(FrameV4L2Info& info);
    int export_dmabuf_fd(int v4lfd, int index, int* dmafd);

};
}
#endif
