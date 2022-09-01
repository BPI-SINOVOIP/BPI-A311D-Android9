#ifndef __BOARD_BANANAPI_H
#define __BOARD_BANANAPI_H

#include <linux/types.h>

extern bool bpi_amlogic_usb3(void);
#define bpi_amlogic_usb3(void)       false

#endif
