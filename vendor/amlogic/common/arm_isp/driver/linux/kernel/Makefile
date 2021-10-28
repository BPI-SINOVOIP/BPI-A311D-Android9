
LOCAL_ARCH := arm
LOCAL_KDIR := /mnt/fileroot02/keke.li/android-p/out/target/product/w400/obj/KERNEL_OBJ/
LOCAL_CROSS_COMPILE := /opt/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
LOCAL_PATH := $(shell pwd)

define module_func
	make -C $(LOCAL_KDIR) M=$(LOCAL_PATH)/$(1) \
	ARCH=$(LOCAL_ARCH) CROSS_COMPILE=$(LOCAL_CROSS_COMPILE) modules
endef

define module_clean
	make -C $(LOCAL_KDIR) M=$(LOCAL_PATH)/$(1) \
        ARCH=$(LOCAL_ARCH) CROSS_COMPILE=$(LOCAL_CROSS_COMPILE) clean
endef

.PHONY: clean v4l2_clean sensor_clean lens_clean iq_clean

all:
	$(call module_func,v4l2_dev)
	$(call module_func,subdev/sensor)
	$(call module_func,subdev/lens)
	$(call module_func,subdev/iq)

clean:
	$(call module_clean,v4l2_dev)
	$(call module_clean,subdev/sensor)
	$(call module_clean,subdev/lens)
	$(call module_clean,subdev/iq)
	rm $(LOCAL_PATH)/*.ko

v4l2:
	$(call module_func,v4l2_dev)

sensor:
	$(call module_func,subdev/sensor)

lens:
	$(call module_func,subdev/lens)

iq:
	$(call module_func,subdev/iq)

v4l2_clean:
	$(call module_clean,v4l2_dev)

sensor_clean:
	$(call module_clean,subdev/sensor)

lens_clean:
	$(call module_clean,subdev/lens)

iq_clean:
	$(call module_clean,subdev/iq)

install:
	find $(LOCAL_PATH) -name "*.ko" | xargs -i cp {} $(LOCAL_PATH)
