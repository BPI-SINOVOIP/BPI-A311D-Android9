#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/regulator/machine.h>
#include <linux/i2c.h>
//#include <mach/irqs.h>
#include <linux/power_supply.h>
#include <linux/module.h>

#include "yk-cfg.h"
#include "yk-mfd.h"

static struct platform_device virt[]={
	{
			.name = "reg-618-cs-rtc",
			.id = -1,
			.dev		= {
				.platform_data = "rtcldo",
			}
 	},{
			.name = "reg-618-cs-aldo1",
			.id = -1,
			.dev		= {
				.platform_data = "aldo1",
			}
 	},{
			.name = "reg-618-cs-aldo2",
			.id = -1,
			.dev		= {
				.platform_data = "aldo2",
			}
 	},{
			.name = "reg-618-cs-aldo3",
			.id = -1,
			.dev		= {
				.platform_data = "aldo3",
			}
	},{
			.name = "reg-618-cs-eldo1",
			.id = -1,
			.dev		= {
				.platform_data = "eldo1",
			}
	},{
			.name = "reg-618-cs-eldo2",
			.id = -1,
			.dev		= {
				.platform_data = "eldo2",
			}
	},{
			.name = "reg-618-cs-dcdc1",
			.id = -1,
			.dev		= {
				.platform_data = "dcdc1",
			}
	},{
			.name = "reg-618-cs-dcdc2",
			.id = -1,
			.dev		= {
				.platform_data = "dcdc2",
			}
	},{
			.name = "reg-618-cs-dcdc3",
			.id = -1,
			.dev		= {
				.platform_data = "dcdc3",
			}
	},{
			.name = "reg-618-cs-dcdc4",
			.id = -1,
			.dev		= {
				.platform_data = "dcdc4",
			}
 	},{
			.name = "reg-618-cs-dcdc5",
			.id = -1,
			.dev		= {
				.platform_data = "dcdc5",
			}
	},{
			.name = "reg-618-cs-gpio1ldo",
			.id = -1,
			.dev		= {
				.platform_data = "gpio_ldo1",
			}
	},
};



 static int __init virtual_init(void)
{
	int j,ret;
	for (j = 0; j < ARRAY_SIZE(virt); j++){
 		ret =  platform_device_register(&virt[j]);
  		if (ret)
				goto creat_devices_failed;
	}
	return ret;

creat_devices_failed:
	while (j--)
		platform_device_register(&virt[j]);
	return ret;

}

module_init(virtual_init);

static void __exit virtual_exit(void)
{
	int j;
	for (j = ARRAY_SIZE(virt) - 1; j >= 0; j--){
		platform_device_unregister(&virt[j]);
	}
}
module_exit(virtual_exit);

MODULE_DESCRIPTION("YEKER regulator test");
MODULE_AUTHOR("YEKER");
MODULE_LICENSE("GPL");
