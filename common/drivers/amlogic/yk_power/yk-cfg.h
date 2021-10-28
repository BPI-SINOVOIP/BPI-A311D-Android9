#ifndef __LINUX_YK_CFG_H_
#define __LINUX_YK_CFG_H_
#include "yk-mfd.h"

/*�豸��ַ*/
/*
	һ�㲻�ı䣺
	YK618:0x34
*/
#define	YK_DEVICES_ADDR	(0x68 >> 1)
/*i2c���������豸��:���忴��ʹ��ƽ̨Ӳ��������*/
#define	YK_I2CBUS			1
/*��ԴоƬ��Ӧ���жϺţ����忴��ʹ�õ�ƽ̨Ӳ�������ӣ�
�ж���nmi����cpu����·irq����gpio*/
//#define YK_IRQNO			IRQ_EINT(20)
#define YK_IRQNO			6//192//192//64

/*��ʼ����·�������λmV��0��Ϊ�ر�*/
/*
	ldo1��
		��Ӳ�����������ѹ������Ĳ��ˣ�ֻ���������ʾ��ѹ��
*/
#define YK_LDO1_VALUE			3000
/*
	aldo1��
		YK:700~3300,100/step
*/
#define YK_ALDO1_VALUE		1800
/*
	aldo2��
		YK:700~3300,100/step
*/
#define YK_ALDO2_VALUE		1000
/*
	aldo3��
		YK:700~3300,100/step
*/
#define YK_ALDO3_VALUE		3300

/*
	eldo1��
		YK:700~3300,100/step
*/
#define YK_ELDO1_VALUE		3300
/*
	eldo2��
		YK:700~3300,100/step
*/
#define YK_ELDO2_VALUE		3300


/*
	DCDC1:
		YK:1600~3400,100/setp
*/
#define YK_DCDC1_VALUE		3200
/*
	DCDC2��
		YK:600~1540��20/step
*/
#define YK_DCDC2_VALUE		1100
/*
	DCDC3��
		YK:600~1860��20/step
*/
#define YK_DCDC3_VALUE		1100
/*
	DCDC4��
		YK:600~1540��20/step
*/
#define YK_DCDC4_VALUE		1000
/*
	DCDC5��
		YK:1000~2550��50/step
*/
#define YK_DCDC5_VALUE		1100

/*���������mAh������ʵ�ʵ�����������壬�Կ��ؼƷ�����˵
�����������Ҫ����������*/
#define BATCAP				5000

/*��ʼ��������裬m����һ����100~200֮�䣬������ø���ʵ��
���Գ�����ȷ���������Ǵ򿪴�ӡ��Ϣ�����ӵ���պù̼���
�ϵ�أ����ӳ����������������1���Ӻ󣬽��ϳ��������
1~2���ӣ�����ӡ��Ϣ�е�rdcֵ����������*/
#define BATRDC				103
/*��·��ѹ�����еĵ�ص�ѹ�Ļ���*/
#define YK_VOL_MAX			1
/*
	��繦��ʹ�ܣ�
        YK:0-�رգ�1-��
*/
#define CHGEN       1

/*
	���������ã�uA��0Ϊ�رգ�
		YK:300~2550,100/step
*/
/*������������uA*/
#define STACHGCUR			900*1000
/*������������uA*/
#define EARCHGCUR			900*1000
/*���߳�������uA*/
#define SUSCHGCUR			1500*1000
/*�ػ���������uA*/
#define CLSCHGCUR			1500*1000

/*Ŀ�����ѹ��mV*/
/*
	YK:4100000/4200000/4240000/4350000
*/
#define CHGVOL				4200000
/*������С�����õ�����ENDCHGRATE%ʱ��ֹͣ��磬%*/
/*
	YK:10\15
*/
#define ENDCHGRATE			10
/*�ػ���ѹ��mV*/
/*
	ϵͳ��ƵĹػ�����ĵ�ض˵�ѹ����Ҫ��ػ��ٷֱȡ�
	��·��ѹ��Ӧ�ٷֱȱ��͵羯���ѹ�໥��ϲŻ�������
*/
#define SHUTDOWNVOL			3300

/*adc���������ã�Hz*/
/*
	YK:100\200\400\800
*/
#define ADCFREQ				100
/*Ԥ��糬ʱʱ�䣬min*/
/*
	YK:40\50\60\70
*/
#define CHGPRETIME			50
/*������糬ʱʱ�䣬min*/
/*
	YK:360\480\600\720
*/
#define CHGCSTTIME			480


/*pek����ʱ�䣬ms*/
/*
	��power��Ӳ������ʱ�䣺
		YK:128/1000/2000/3000
*/
#define PEKOPEN				1000
/*pek����ʱ�䣬ms*/
/*
	��power���������жϵ�ʱ�䣬���ڴ�ʱ���Ƕ̰������̰���irq��
	���ڴ�ʱ���ǳ�������������irq��
		YK:1000/1500/2000/2500
*/
#define PEKLONG				1500
/*pek�����ػ�ʹ��*/
/*
	��power�������ػ�ʱ��Ӳ���ػ�����ʹ�ܣ�
		YK:0-���أ�1-�ػ�
*/
#define PEKOFFEN			1
/*pek�����ػ�ʹ�ܺ󿪻�ѡ��*/
/*
	��power�������ػ�ʱ��Ӳ���ػ���������ѡ��:
		YK:0-ֻ�ػ���������1-�ػ�������
*/
#define PEKOFFRESTART			0
/*pekpwr�ӳ�ʱ�䣬ms*/
/*
	������powerok�źŵ��ӳ�ʱ�䣺
		YK20:8/16/32/64
*/
#define PEKDELAY			32
/*pek�����ػ�ʱ�䣬ms*/
/*
	��power���Ĺػ�ʱ����
		YK:4000/6000/8000/10000
*/
#define PEKOFF				6000
/*���¹ػ�ʹ��*/
/*
	YK�ڲ��¶ȹ���Ӳ���ػ�����ʹ�ܣ�
		YK:0-���أ�1-�ػ�
*/
#define OTPOFFEN			0
/* ����ѹ����ʹ��*/
/*
	YK:0-�رգ�1-��
*/
#define USBVOLLIMEN		1
/*  �����ѹ��mV��0Ϊ������*/
/*
	YK:4000~4700��100/step
*/
#define USBVOLLIM			4500
/*  USB�����ѹ��mV��0Ϊ������*/
/*
	YK:4000~4700��100/step
*/
#define USBVOLLIMPC			4700

/* ����������ʹ��*/
/*
	YK:0-�رգ�1-��
*/
#define USBCURLIMEN		1
/* ���������mA��0Ϊ������*/
/*
	YK:500/900
*/
#define USBCURLIM			0
/* usb ���������mA��0Ϊ������*/
/*
	YK:500/900
*/
#define USBCURLIMPC			500
/* PMU �жϴ�������ʹ��*/
/*
	YK:0-�����ѣ�1-����
*/
#define IRQWAKEUP			0
/* N_VBUSEN PIN ���ܿ���*/
/*
	YK:0-���������OTG��ѹģ�飬1-���룬����VBUSͨ·
*/
#define VBUSEN			1
/* ACIN/VBUS In-short ��������*/
/*
	YK:0-AC VBUS�ֿ���1-ʹ��VBUS��AC,�޵���AC
*/
#define VBUSACINSHORT			0
/* CHGLED �ܽſ�������*/
/*
	YK:0-������1-�ɳ�繦�ܿ���
*/
#define CHGLEDFUN			1
/* CHGLED LED ��������*/
/*
	YK:0-���ʱled������1-���ʱled��˸
*/
#define CHGLEDTYPE			0
/* ���������У��ʹ��*/
/*
	YK:0-�رգ�1-��
*/
#define BATCAPCORRENT			0
/* �����ɺ󣬳�����ʹ��*/
/*
	YK:0-�رգ�1-��
*/
#define BATREGUEN			0
/* ��ؼ�⹦��ʹ��*/
/*
	YK:0-�رգ�1-��
*/
#define BATDET		1
/* PMU����ʹ��*/
/*
	YK:0-�رգ�1-�򿪰���Դ��16������PMU����
*/
#define PMURESET		0
/*�͵羯���ѹ1��%*/
/*
	����ϵͳ���������
	YK:5%~20%
*/
#define BATLOWLV1    15
/*�͵羯���ѹ2��%*/
/*
	����ϵͳ���������
	YK:0%~15%
*/
#define BATLOWLV2    0

#define ABS(x)				((x) >0 ? (x) : -(x) )

#ifdef	CONFIG_AMLOGIC_YK618
/*YK GPIO start NUM,����ƽ̨ʵ���������*/
#define YK_NR_BASE 100

/*YK GPIO NUM,�������������LCD power�Լ�VBUS driver pin*/
#define YK_NR 5

/*��ʼ����·��ѹ��Ӧ�ٷֱȱ�*/
/*
	����ʹ��Ĭ��ֵ��������ø���ʵ�ʲ��Եĵ����ȷ��ÿ��
	��Ӧ��ʣ��ٷֱȣ��ر���Ҫע�⣬�ػ���ѹSHUTDOWNVOL�͵��
	������ʼУ׼ʣ�������ٷֱ�BATCAPCORRATE��������׼ȷ��
	YK����
*/
#define OCVREG0				0		 //3.13V
#define OCVREG1				0		 //3.27V
#define OCVREG2				0		 //3.34V
#define OCVREG3				0		 //3.41V
#define OCVREG4				0		 //3.48V
#define OCVREG5				1		 //3.52V
#define OCVREG6				2		 //3.55V
#define OCVREG7				3		 //3.57V
#define OCVREG8				4		 //3.59V
#define OCVREG9				5		 //3.61V
#define OCVREGA				5		 //3.63V
#define OCVREGB				6		 //3.64V
#define OCVREGC				8		 //3.66V
#define OCVREGD				9		 //3.7V
#define OCVREGE				13		 //3.73V
#define OCVREGF				17		 //3.77V
#define OCVREG10		 	20                //3.78V
#define OCVREG11		 	23                //3.8V
#define OCVREG12		 	27                //3.82V
#define OCVREG13		 	32                //3.84V
#define OCVREG14		 	38                //3.85V
#define OCVREG15		 	43                //3.87V
#define OCVREG16		 	52                //3.91V
#define OCVREG17		 	57                //3.94V
#define OCVREG18		 	65                //3.98V
#define OCVREG19		 	75                //4.01V
#define OCVREG1A		 	89                //4.05V
#define OCVREG1B		 	95                //4.08V
#define OCVREG1C		 	97                //4.1V
#define OCVREG1D		 	98                //4.12V
#define OCVREG1E		 	99                //4.14V
#define OCVREG1F		 	100                //4.15V

#endif

/*ѡ����Ҫ�򿪵��ж�ʹ��*/
static const uint64_t YK_NOTIFIER_ON = (YK_IRQ_USBIN |YK_IRQ_USBRE |
				       		                            YK_IRQ_ACIN |YK_IRQ_ACRE |
				       		                            YK_IRQ_BATIN |YK_IRQ_BATRE |
				       		                            YK_IRQ_CHAST |YK_IRQ_CHAOV |
						                            (uint64_t)YK_IRQ_PEKFE |(uint64_t)YK_IRQ_PEKRE);

/* ��Ҫ�������ţ��usb�ػ�������bootʱpower_start����Ϊ1������Ϊ0*/
#define POWER_START 0


#endif
