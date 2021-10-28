/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *  @author   Tellen Yu
 *  @version  2.0
 *  @date     2019/09/12
 *  @par function description:
 *  - 1 open vad related devices first before enter freeze mode
 */

#define LOG_TAG "vadservice"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <cutils/properties.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/reboot.h>
#include <android-base/logging.h>

#include "log.h"

#define ANDROID_RB_PROPERTY "sys.powerctl"

#define PATH_CMD_LINE "/proc/cmdline"
#define PATH_KERN_PRINTK "/proc/sys/kernel/printk"
#define PATH_REBOOT_REASON "/sys/devices/platform/reboot/reboot_reason"
#define PATH_FREEZE "/sys/power/state"
#define PATH_CEC_CONFIG "/sys/class/cec/fun_cfg"
#define PATH_HDMIRX_CEC "/sys/class/hdmirx/hdmirx0/cec"
#define PATH_HDCP_ESM "/sys/module/tvin_hdmirx/parameters/hdcp22_kill_esm"
#define PATH_SND_CONTROL "/dev/snd/controlC0"

#define BUF_LEN_MAX    (4096)
#define BUF_LEN_NORMAL    (32)

typedef struct VadCaptureConfig {
    char source;
    char device;
    char channel;
    int rate;
} VadConfig;

extern "C" int vadTinymix(char *id, char *val);
extern "C" int vadTinycapStart(int device, int rate, int channel);
extern "C" void vadTinycapStop(void);

void writeSys(const char *path, const char *val) {
    int fd;

    if ((fd = open(path, O_RDWR)) < 0) {
		LOG(android::base::ERROR) << "writeSysFs, open fail:" << path;
        goto exit;
    }
    write(fd, val, strlen(val));

exit:
    close(fd);
}

int readSys(const char *path, char *buf, int count) {
    int fd, len = -1;

    if (NULL == buf) {
		LOG(android::base::ERROR) << "readSys, buf is null";
        return len;
    }

    if ((fd = open(path, O_RDONLY)) < 0) {
		LOG(android::base::ERROR) << "readSys, open fail: " << path << ", error=" << strerror(errno);
        return len;
    }

    len = read(fd, buf, count);
    if (len < 0)
		LOG(android::base::ERROR) << "readSys, read error: " << path << ", error=" << strerror(errno);
    close(fd);
    return len;
}

int processBuffer(char *t_buf, char split_ch, char *t_name, char *t_value) {
    char *tmp_ptr = NULL, *tmp_name = NULL, *tmp_value = NULL;

    tmp_ptr = t_buf;
    while (tmp_ptr && *tmp_ptr) {
        char *x = strchr(tmp_ptr, split_ch);
        if (x != NULL) {
            *x++ = '\0';
        }
        tmp_name = tmp_ptr;
        if (tmp_name[0] != '\0') {
            tmp_value = strchr(tmp_ptr, '=');
            if (tmp_value != NULL) {
                *tmp_value++ = '\0';
                if (strncmp(tmp_name, t_name, strlen(t_name)) == 0) {
                    strncpy(t_value, tmp_value, BUF_LEN_NORMAL);
                    return 0;
                }
            }
        }
        tmp_ptr = x;
    }
    return -1;
}

int processReadFile(char *file_path, int offset, char *pBuf, int len) {
    int tmp_cnt = 0;
    int dev_fd = -1;

    dev_fd = open(file_path, O_RDONLY);
    if (dev_fd >= 0) {
        lseek(dev_fd, offset, SEEK_SET);
        tmp_cnt = read(dev_fd, pBuf, len);
        if (tmp_cnt < 0)
            tmp_cnt = 0;
        /* get rid of trailing newline, it happens */
        if (tmp_cnt > 0 && pBuf[tmp_cnt - 1] == '\n')
            tmp_cnt--;
        pBuf[tmp_cnt] = 0;
        close(dev_fd);
    } else
        pBuf[0] = 0;

    return tmp_cnt;
}

int processFile(char *t_fname, char split, char *t_name, char *t_value) {
    char line_buf[BUF_LEN_MAX];

    processReadFile(t_fname, 0, line_buf, BUF_LEN_MAX);
    return processBuffer(line_buf, split, t_name, t_value);
}

void vadReboot(void) {
    LOG(android::base::WARNING) << "ffv normal reboot";
    if (true) {
        writeSys(PATH_HDCP_ESM, "1");
        usleep(50 * 1000);
        syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                LINUX_REBOOT_CMD_RESTART2, "normal");
    } else {
        property_set(ANDROID_RB_PROPERTY, "normal");
    }
}

bool isCoolboot() {
    char buf[BUF_LEN_NORMAL] = { 0 };

    readSys(PATH_REBOOT_REASON, buf, BUF_LEN_NORMAL);
    LOG(android::base::WARNING) << "reboot reason is " << buf;
    if (strncmp(buf, "0", 1) == 0) {    // cold boot
        return true;
    } else if (strncmp(buf, "15", 2) == 0) { // ffv reboot
        return true;
    }
    return false;
}

bool isFFVFreezeMode() {
    char buf[BUF_LEN_NORMAL] = { 0 };

    memset(buf, 0, BUF_LEN_NORMAL);
    if (processFile((char*)PATH_CMD_LINE, ' ', (char*)"ffv_freeze", buf) == 0) {
        if (strncmp(buf, "on", 2) == 0)
            return true;
    }
    return false;
}

static void getVadConfig(VadConfig* vadconfig) {
    vadconfig->source = property_get_int32("persist.vendor.sys.vadwake.source", 4);
    vadconfig->device = property_get_int32("persist.vendor.sys.vadwake.device", 3);
    vadconfig->channel = property_get_int32("persist.vendor.sys.vadwake.channel", 1);
    vadconfig->rate = property_get_int32("persist.vendor.sys.vadwake.rate", 16000);
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    VadConfig tmpConfig;
    char tmp[BUF_LEN_NORMAL] = {0};

    android::vad::VadKernelLogging(argv);

    if (/*isCoolboot() && */isFFVFreezeMode()) {
        int tmp_cnt = 0;

        LOG(android::base::WARNING) << "ffv freeze mode";
        getVadConfig(&tmpConfig);
        while (1) {
            if (access(PATH_SND_CONTROL, R_OK | W_OK) == 0) {
                break;
            }
            if (tmp_cnt >= 50) {
                break;
            }
            usleep(100 * 1000);
            tmp_cnt += 1;
        }

        tmp_cnt = 0;
        memset(tmp, 0, BUF_LEN_NORMAL);
        sprintf(tmp, "/dev/snd/pcmC0D%dc", tmpConfig.device);
        while (1) {
            if (access(tmp, R_OK | W_OK) == 0) {
                break;
            }
            if (tmp_cnt >= 50) {
                break;
            }
            usleep(100 * 1000);
            tmp_cnt += 1;
        }

        vadTinymix((char*)"VAD enable", (char*)"1");
        memset(tmp, 0, BUF_LEN_NORMAL);
        sprintf(tmp, "%d", tmpConfig.source);
        vadTinymix((char*)"VAD Source sel", tmp);
        vadTinycapStart(tmpConfig.device, tmpConfig.rate, tmpConfig.channel);
        vadTinymix((char*)"VAD Switch", (char*)"1"); //enable the vad engine to get data from ddr

        writeSys(PATH_FREEZE, "freeze"); // block here
        vadReboot();
    }

    return 0;
}
