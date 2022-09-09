#define LOG_TAG "Virtual_Bass_Effect_Arithmetic"
#define LOG_NDEBUG 0

#include <cutils/log.h>
#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include <hardware/audio_effect.h>
#include <cutils/properties.h>

#include "Virtual_Bass_Arithmetic.h"

int VirtualBassInit(void)
{
	ALOGE("%s: test \n", __FUNCTION__);
	return 0;
}

int VirtualBassSetParameter(void) {
	ALOGE("%s: test \n", __FUNCTION__);
	return 0;
}

int VirtualBassProcess(void *date)
{
	ALOGE("%s: test date = %s\n", __FUNCTION__,date);
	return 0;
}


