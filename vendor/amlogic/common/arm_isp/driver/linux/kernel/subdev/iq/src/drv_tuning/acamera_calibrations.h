/*
*
* SPDX-License-Identifier: GPL-2.0
*
* Copyright (C) 2011-2018 ARM or its affiliates
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2.
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#ifndef __ACAMERA_CALIBRATIONS_H__
#define __ACAMERA_CALIBRATIONS_H__

#include "acamera_types.h"
#include "acamera_math.h"


#define CALIBRATION_BASE_SET 0
#define CALIBRATION_PREVIEW_SET 1


LookupTable *_GET_LOOKUP_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

const void *_GET_LUT_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

uint8_t *_GET_UCHAR_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

uint16_t *_GET_USHORT_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t *_GET_UINT_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

modulation_entry_t *_GET_MOD_ENTRY16_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

modulation_entry_32_t *_GET_MOD_ENTRY32_PTR( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t _GET_ROWS( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t _GET_COLS( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t _GET_LEN( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t _GET_WIDTH( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t _GET_SIZE( ACameraCalibrations *p_ctx, uint32_t idx );

uint32_t get_calibratin_external_tuning( char *sensor_name, void *sensor_arg, ACameraCalibrations *c );

#endif // __ACAMERA_CALIBRATIONS_H__
