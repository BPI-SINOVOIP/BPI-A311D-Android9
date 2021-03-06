/*
 * Copyright (c) 2015 South Silicon Valley Microelectronics Inc.
 * Copyright (c) 2015 iComm Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/irq.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/gpio.h>
#include <linux/mmc/host.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/semaphore.h>
#include <asm/io.h>
#include <linux/cpufreq.h>
#include <linux/notifier.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
#include <linux/printk.h>
#include <linux/err.h>
#else
#include <config/printk.h>
#endif

#include <linux/amlogic/aml_thermal_hw.h>
extern void sdio_reinit(void);
extern void extern_wifi_set_enable(int is_on);
extern int ssv6xxx_get_dev_status(void);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
extern int wifi_setup_dt(void);
#endif
#define GPIO_REG_WRITEL(val, reg)                                              \
	do {                                                                   \
		__raw_writel(val, CTL_PIN_BASE + (reg));                       \
	} while (0)
struct semaphore icomm_chipup_sem;
static int g_wifidev_registered = 0;
char ssvcabrio_fw_name[50] = "ssv6051-sw.bin";
extern int ssvdevice_init(void);
extern void ssvdevice_exit(void);
#ifdef CONFIG_SSV_SUPPORT_AES_ASM
extern int aes_init(void);
extern void aes_fini(void);
extern int sha1_mod_init(void);
extern void sha1_mod_fini(void);
#endif
extern int max_aggr_num;

static int aml_cpufreq_notifier(struct notifier_block *nb,
                                         unsigned long event, void *data)
{
        //unsigned int cur_freq = 0;
        struct cpufreq_freqs *frq = data;
	int cpu_temp;

	if((int)frq->cpu != 0 || (int)event == CPUFREQ_TRANSITION_NOTIFIER)
		return 0;
		
	cpu_temp = get_cpu_temp();
	if(cpu_temp >= 85 && max_aggr_num != 3 )
		max_aggr_num = 3;
	else if(cpu_temp <= 83 && max_aggr_num != 24 )
		max_aggr_num = 24;

	printk(KERN_INFO "SSV set max_aggr_num : %d\n",max_aggr_num);
        //cur_freq = cpufreq_generic_get(0);
        //printk("event=%d, cur_freq = %d\n", (int)event, (int)cur_freq);
	printk(KERN_INFO "SSV get temp : cur_temp = %d\n",cpu_temp);
	printk(KERN_INFO "event=%d, cpu=%d, old =%d, new = %d\n", (int)event, (int)frq->cpu, (int)frq->old, (int)frq->new);

        return 0;
}

static struct notifier_block aml_cpufreq_notifier_block = {
        .notifier_call = aml_cpufreq_notifier,
};

void ssv_wifi_power(void)
{
	extern_wifi_set_enable(0);
	mdelay(150);
	extern_wifi_set_enable(1);
	mdelay(150);
	sdio_reinit();
	mdelay(150);
}

int initWlan(void)
{
	int ret = 0;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
	if (wifi_setup_dt()) {
		printk("wifi_dt : fail to setup dt\n");
		goto fail;
	}
#endif
	ssv_wifi_power();
	g_wifidev_registered = 1;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
fail:
#endif
	up(&icomm_chipup_sem);
	return ret;
}
void exitWlan(void)
{
	if (g_wifidev_registered) {
		ssvdevice_exit();
		extern_wifi_set_enable(0);
		g_wifidev_registered = 0;
		mdelay(150);
	}
	return;
}
static __init int generic_wifi_init_module(void)
{
	int ret, time = 5;
	printk("%s\n", __func__);
	sema_init(&icomm_chipup_sem, 0);
#ifdef CONFIG_SSV_SUPPORT_AES_ASM
	sha1_mod_init();
	aes_init();
#endif
	ret = initWlan();
	if (down_timeout(&icomm_chipup_sem, msecs_to_jiffies(1000)) != 0) {
		ret = -EINVAL;
		printk(KERN_ALERT "%s: platform_driver_register timeout\n",
		       __FUNCTION__);
		goto out;
	}
	ret = ssvdevice_init();
	while(time-- > 0){
		mdelay(500);
		if(ssv6xxx_get_dev_status() == 1)
			break;
		printk("%s : Retry to carddetect\n",__func__);
		ssv_wifi_power();	
	}
    cpufreq_register_notifier(&aml_cpufreq_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
out:
	printk("generic_wifi_init finished\n");
	return ret;
}
static __exit void generic_wifi_exit_module(void)
{
	printk("%s\n", __func__);
#ifdef CONFIG_SSV_SUPPORT_AES_ASM
	aes_fini();
	sha1_mod_fini();
#endif
	exitWlan();
    cpufreq_unregister_notifier(&aml_cpufreq_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
}
EXPORT_SYMBOL(generic_wifi_init_module);
EXPORT_SYMBOL(generic_wifi_exit_module);
module_init(generic_wifi_init_module);
module_exit(generic_wifi_exit_module);
MODULE_LICENSE("Dual BSD/GPL");
