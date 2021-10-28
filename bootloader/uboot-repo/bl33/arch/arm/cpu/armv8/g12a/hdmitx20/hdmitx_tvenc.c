
/*
 * arch/arm/cpu/armv8/txl/hdmitx20/hdmitx_tvenc.c
 *
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "mach_reg.h"
#include "hdmitx_tvenc.h"

#define MREG_END_MARKER 0xFFFF

static const struct reg_t tvregs_720p[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},

	{P_ENCP_VIDEO_MODE, 0x4040,},
	{P_ENCP_VIDEO_MODE_ADV, 0x18,},
	{P_ENCP_VIDEO_MAX_PXCNT, 1649,},
	{P_ENCP_VIDEO_MAX_LNCNT, 0x2ED,},
	{P_ENCP_VIDEO_HAVON_BEGIN, 0x104,},
	{P_ENCP_VIDEO_HAVON_END, 0x603,},
	{P_ENCP_VIDEO_VAVON_BLINE, 0x19,},
	{P_ENCP_VIDEO_VAVON_ELINE, 0x2E8,},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x0,},
	{P_ENCP_VIDEO_HSO_END, 0x28,},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x1E,},
	{P_ENCP_VIDEO_VSO_END, 0x32,},
	{P_ENCP_VIDEO_VSO_BLINE, 0x0,},
	{P_ENCP_VIDEO_VSO_ELINE, 0x5,},
	{P_ENCP_DVI_HSO_BEGIN, 0x2,},
	{P_ENCP_DVI_HSO_END, 0x2A,},
	{P_ENCP_DVI_VSO_BLINE_EVN, 0x0,},
	{P_ENCP_DVI_VSO_BLINE_ODD, 0x0,},
	{P_ENCP_DVI_VSO_ELINE_EVN, 0x5,},
	{P_ENCP_DVI_VSO_ELINE_ODD, 0x0,},
	{P_ENCP_DVI_VSO_BEGIN_EVN, 0x2,},
	{P_ENCP_DVI_VSO_BEGIN_ODD, 0x0,},
	{P_ENCP_DVI_VSO_END_EVN, 0x2,},
	{P_ENCP_DVI_VSO_END_ODD, 0x0,},
	{P_ENCP_DE_H_BEGIN, 0x106,},
	{P_ENCP_DE_H_END, 0x606,},
	{P_ENCP_DE_V_BEGIN_EVEN, 0x19,},
	{P_ENCP_DE_V_END_EVEN, 0x2E9,},
	{P_ENCP_DE_V_BEGIN_ODD, 0x0,},
	{P_ENCP_DE_V_END_ODD, 0x0,},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_720p_50hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},

	{P_ENCP_VIDEO_MODE, 0x4040,},
	{P_ENCP_VIDEO_MODE_ADV, 0x18,},
	{P_ENCP_VIDEO_MAX_PXCNT, 1979,},
	{P_ENCP_VIDEO_MAX_LNCNT, 0x2ED,},
	{P_ENCP_VIDEO_HAVON_BEGIN, 0x104,},
	{P_ENCP_VIDEO_HAVON_END, 0x603,},
	{P_ENCP_VIDEO_VAVON_BLINE, 0x19,},
	{P_ENCP_VIDEO_VAVON_ELINE, 0x2E8,},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x0,},
	{P_ENCP_VIDEO_HSO_END, 0x28,},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x1E,},
	{P_ENCP_VIDEO_VSO_END, 0x32,},
	{P_ENCP_VIDEO_VSO_BLINE, 0x0,},
	{P_ENCP_VIDEO_VSO_ELINE, 0x5,},
	{P_ENCP_DVI_HSO_BEGIN, 0x2,},
	{P_ENCP_DVI_HSO_END, 0x2A,},
	{P_ENCP_DVI_VSO_BLINE_EVN, 0x0,},
	{P_ENCP_DVI_VSO_BLINE_ODD, 0x0,},
	{P_ENCP_DVI_VSO_ELINE_EVN, 0x5,},
	{P_ENCP_DVI_VSO_ELINE_ODD, 0x0,},
	{P_ENCP_DVI_VSO_BEGIN_EVN, 0x2,},
	{P_ENCP_DVI_VSO_BEGIN_ODD, 0x0,},
	{P_ENCP_DVI_VSO_END_EVN, 0x2,},
	{P_ENCP_DVI_VSO_END_ODD, 0x0,},
	{P_ENCP_DE_H_BEGIN, 0x106,},
	{P_ENCP_DE_H_END, 0x606,},
	{P_ENCP_DE_V_BEGIN_EVEN, 0x19,},
	{P_ENCP_DE_V_END_EVEN, 0x2E9,},
	{P_ENCP_DE_V_BEGIN_ODD, 0x0,},
	{P_ENCP_DE_V_END_ODD, 0x0,},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_480i[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCI_CFILT_CTRL, 0x12},
	{P_ENCI_CFILT_CTRL2, 0x12},
	{P_VENC_DVI_SETTING, 0},
	{P_ENCI_VIDEO_MODE, 0},
	{P_ENCI_VIDEO_MODE_ADV, 0},
	{P_ENCI_SYNC_HSO_BEGIN, 5},
	{P_ENCI_SYNC_HSO_END, 129},
	{P_ENCI_SYNC_VSO_EVNLN, 0x0003},
	{P_ENCI_SYNC_VSO_ODDLN, 0x0104},
	{P_ENCI_MACV_MAX_AMP, 0x810b},
	{P_VENC_VIDEO_PROG_MODE, 0xf0},
	{P_ENCI_VIDEO_MODE, 0x08},
	{P_ENCI_VIDEO_MODE_ADV, 0x26},
	{P_ENCI_VIDEO_SCH, 0x20},
	{P_ENCI_SYNC_MODE, 0x07},
	{P_ENCI_DBG_PX_RST, 0},
	{P_ENCI_VFIFO2VD_CTL, 0x4e01},
	{P_ENCI_VFIFO2VD_PIXEL_START, 0xf3,},
	{P_ENCI_VFIFO2VD_PIXEL_END, 0x0693,},
	{P_ENCI_VFIFO2VD_LINE_TOP_START, 0x12,},
	{P_ENCI_VFIFO2VD_LINE_TOP_END, 0x102,},
	{P_ENCI_VFIFO2VD_LINE_BOT_START, 0x13,},
	{P_ENCI_VFIFO2VD_LINE_BOT_END, 0x103,},
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 1},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_480p[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_FILT_CTRL, 0x2052},
	{P_VENC_DVI_SETTING, 0x21},
	{P_ENCP_VIDEO_MODE, 0x4000},
	{P_ENCP_VIDEO_MODE_ADV, 9},
	{P_ENCP_VIDEO_YFP1_HTIME, 244},
	{P_ENCP_VIDEO_YFP2_HTIME, 1630},
	{P_ENCP_VIDEO_MAX_PXCNT, 1715},
	{P_ENCP_VIDEO_MAX_LNCNT, 524},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 0x22},
	{P_ENCP_VIDEO_HSPULS_END, 0xa0},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 88},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 0},
	{P_ENCP_VIDEO_VSPULS_END, 1589},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 5},
	{P_ENCP_VIDEO_HAVON_BEGIN, 249},
	{P_ENCP_VIDEO_HAVON_END, 1689},
	{P_ENCP_VIDEO_VAVON_BLINE, 42},
	{P_ENCP_VIDEO_VAVON_ELINE, 521},
	{P_ENCP_VIDEO_SYNC_MODE, 0x07},
	{P_VENC_VIDEO_PROG_MODE, 0x0},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x3},
	{P_ENCP_VIDEO_HSO_END, 0x5},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x3},
	{P_ENCP_VIDEO_VSO_END, 0x5},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_SY_VAL, 8},
	{P_ENCP_VIDEO_SY2_VAL, 0x1d8},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_576i[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_VENC_DVI_SETTING, 0},
	{P_ENCI_VIDEO_MODE, 0},
	{P_ENCI_VIDEO_MODE_ADV, 0},
	{P_ENCI_SYNC_HSO_BEGIN, 3},
	{P_ENCI_SYNC_HSO_END, 129},
	{P_ENCI_SYNC_VSO_EVNLN, 0x0003},
	{P_ENCI_SYNC_VSO_ODDLN, 0x0104},
	{P_ENCI_MACV_MAX_AMP, 0x8107},
	{P_VENC_VIDEO_PROG_MODE, 0xff},
	{P_ENCI_VIDEO_MODE, 0x13},
	{P_ENCI_VIDEO_MODE_ADV, 0x26},
	{P_ENCI_VIDEO_SCH, 0x28},
	{P_ENCI_SYNC_MODE, 0x07},
	{P_ENCI_YC_DELAY, 0x333},
	{P_ENCI_VFIFO2VD_PIXEL_START, 0x010b},
	{P_ENCI_VFIFO2VD_PIXEL_END, 0x06ab},
	{P_ENCI_VFIFO2VD_LINE_TOP_START, 0x0016},
	{P_ENCI_VFIFO2VD_LINE_TOP_END, 0x0136},
	{P_ENCI_VFIFO2VD_LINE_BOT_START, 0x0017},
	{P_ENCI_VFIFO2VD_LINE_BOT_END, 0x0137},
	{P_ENCI_DBG_PX_RST, 0},
	{P_ENCI_VFIFO2VD_CTL, 0x4e01},
	{P_ENCI_VFIFO2VD_PIXEL_START, 0x010b},
	{P_ENCI_VFIFO2VD_PIXEL_END, 0x06ab},
	{P_ENCI_VFIFO2VD_LINE_TOP_START, 0x0016},
	{P_ENCI_VFIFO2VD_LINE_TOP_END, 0x0136},
	{P_ENCI_VFIFO2VD_LINE_BOT_START, 0x0017},
	{P_ENCI_VFIFO2VD_LINE_BOT_END, 0x0137},
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 1},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_576p[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_FILT_CTRL, 0x52},
	{P_VENC_DVI_SETTING, 0x21},
	{P_ENCP_VIDEO_MODE, 0x4000},
	{P_ENCP_VIDEO_MODE_ADV, 9},
	{P_ENCP_VIDEO_YFP1_HTIME, 235},
	{P_ENCP_VIDEO_YFP2_HTIME, 1674},
	{P_ENCP_VIDEO_MAX_PXCNT, 1727},
	{P_ENCP_VIDEO_MAX_LNCNT, 624},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 0},
	{P_ENCP_VIDEO_HSPULS_END, 0x80},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 88},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 0},
	{P_ENCP_VIDEO_VSPULS_END, 1599},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_HAVON_BEGIN, 235},
	{P_ENCP_VIDEO_HAVON_END, 1674},
	{P_ENCP_VIDEO_VAVON_BLINE, 44},
	{P_ENCP_VIDEO_VAVON_ELINE, 619},
	{P_ENCP_VIDEO_SYNC_MODE, 0x07},
	{P_VENC_VIDEO_PROG_MODE, 0x0},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x80},
	{P_ENCP_VIDEO_HSO_END, 0x0},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x0},
	{P_ENCP_VIDEO_VSO_END, 0x5},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_SY_VAL, 8},
	{P_ENCP_VIDEO_SY2_VAL, 0x1d8},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_1080i[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_VENC_DVI_SETTING, 0x2029},
	{P_ENCP_VIDEO_MAX_PXCNT, 2199},
	{P_ENCP_VIDEO_MAX_LNCNT, 1124},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 88},
	{P_ENCP_VIDEO_HSPULS_END, 264},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 88},
	{P_ENCP_VIDEO_HAVON_BEGIN, 192},
	{P_ENCP_VIDEO_HAVON_END, 2111},
	{P_ENCP_VIDEO_HSO_BEGIN, 0},
	{P_ENCP_VIDEO_HSO_END, 44},
	{P_ENCP_VIDEO_EQPULS_BEGIN, 2288},
	{P_ENCP_VIDEO_EQPULS_END, 2464},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 440},
	{P_ENCP_VIDEO_VSPULS_END, 2200},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_EQPULS_BLINE, 0},
	{P_ENCP_VIDEO_EQPULS_ELINE, 4},
	{P_ENCP_VIDEO_VAVON_BLINE, 20},
	{P_ENCP_VIDEO_VAVON_ELINE, 559},
	{P_ENCP_VIDEO_VSO_BEGIN, 30},
	{P_ENCP_VIDEO_VSO_END, 50},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_VSO_ELINE, 5},
	{P_ENCP_VIDEO_YFP1_HTIME, 516},
	{P_ENCP_VIDEO_YFP2_HTIME, 4355},
	{P_VENC_VIDEO_PROG_MODE, 0x100},
	{P_ENCP_VIDEO_OFLD_VOAV_OFST, 0x11},
	{P_ENCP_VIDEO_MODE, 0x5ffc},
	{P_ENCP_VIDEO_MODE_ADV, 0x0018},
	{P_ENCP_VIDEO_SYNC_MODE, 0x207},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_1080i_50hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_VENC_DVI_SETTING, 0x202d},
	{P_ENCP_VIDEO_MAX_PXCNT, 2639},
	{P_ENCP_VIDEO_MAX_LNCNT, 1124},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 88},
	{P_ENCP_VIDEO_HSPULS_END, 264},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 88},
	{P_ENCP_VIDEO_HAVON_BEGIN, 192},
	{P_ENCP_VIDEO_HAVON_END, 2111},
	{P_ENCP_VIDEO_HSO_BEGIN, 0},
	{P_ENCP_VIDEO_HSO_END, 44},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 440},
	{P_ENCP_VIDEO_VSPULS_END, 2200},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_VAVON_BLINE, 20},
	{P_ENCP_VIDEO_VAVON_ELINE, 559},
	{P_ENCP_VIDEO_VSO_BEGIN, 30},
	{P_ENCP_VIDEO_VSO_END, 50},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_VSO_ELINE, 5},
	{P_ENCP_VIDEO_YFP1_HTIME, 526},
	{P_ENCP_VIDEO_YFP2_HTIME, 4365},
	{P_VENC_VIDEO_PROG_MODE, 0x100},
	{P_ENCP_VIDEO_OFLD_VOAV_OFST, 0x11},
	{P_ENCP_VIDEO_MODE, 0x5ffc},
	{P_ENCP_VIDEO_MODE_ADV, 0x0018},
	{P_ENCP_VIDEO_SYNC_MODE, 0x7},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_1080p[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_FILT_CTRL, 0x1052},
	{P_VENC_DVI_SETTING, 0x0001},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0018},
	{P_ENCP_VIDEO_YFP1_HTIME, 140},
	{P_ENCP_VIDEO_YFP2_HTIME, 2060},
	{P_ENCP_VIDEO_MAX_PXCNT, 2199},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 2156},
	{P_ENCP_VIDEO_HSPULS_END, 44},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 140},
	{P_ENCP_VIDEO_VSPULS_END, 2059},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_HAVON_BEGIN, 148},
	{P_ENCP_VIDEO_HAVON_END, 2067},
	{P_ENCP_VIDEO_VAVON_BLINE, 42},
	{P_ENCP_VIDEO_VAVON_ELINE, 1121},
	{P_ENCP_VIDEO_HSO_BEGIN, 44},
	{P_ENCP_VIDEO_HSO_END, 2156},
	{P_ENCP_VIDEO_VSO_BEGIN, 2100},
	{P_ENCP_VIDEO_VSO_END, 2164},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_VSO_ELINE, 5},
	{P_ENCP_VIDEO_MAX_LNCNT, 1124},
	{P_VENC_VIDEO_PROG_MODE, 0x100},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_1080p_50hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_VENC_DVI_SETTING, 0x000d},
	{P_ENCP_VIDEO_MAX_PXCNT, 2639},
	{P_ENCP_VIDEO_MAX_LNCNT, 1124},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 44},
	{P_ENCP_VIDEO_HSPULS_END, 132},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_HAVON_BEGIN, 271},
	{P_ENCP_VIDEO_HAVON_END, 2190},
	{P_ENCP_VIDEO_HSO_BEGIN, 79},
	{P_ENCP_VIDEO_HSO_END, 123},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 220},
	{P_ENCP_VIDEO_VSPULS_END, 2140},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_EQPULS_BLINE, 0},
	{P_ENCP_VIDEO_EQPULS_ELINE, 4},
	{P_ENCP_VIDEO_VAVON_BLINE, 41},
	{P_ENCP_VIDEO_VAVON_ELINE, 1120},
	{P_ENCP_VIDEO_VSO_BEGIN, 79},
	{P_ENCP_VIDEO_VSO_END, 79},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_VSO_ELINE, 5},
	{P_ENCP_VIDEO_YFP1_HTIME, 271},
	{P_ENCP_VIDEO_YFP2_HTIME, 2190},
	{P_VENC_VIDEO_PROG_MODE, 0x100},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0018},
	{P_ENCP_VIDEO_SYNC_MODE, 0x7},
	{P_ENCP_VIDEO_YC_DLY, 0},
	{P_ENCP_VIDEO_RGB_CTRL, 2},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_1080p_24hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_FILT_CTRL, 0x1052},
	{P_VENC_DVI_SETTING, 0x000d},
	{P_ENCP_VIDEO_MAX_PXCNT, 2749},
	{P_ENCP_VIDEO_MAX_LNCNT, 1124},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 44},
	{P_ENCP_VIDEO_HSPULS_END, 132},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_HAVON_BEGIN, 271},
	{P_ENCP_VIDEO_HAVON_END, 2190},
	{P_ENCP_VIDEO_HSO_BEGIN, 79},
	{P_ENCP_VIDEO_HSO_END, 123},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 220},
	{P_ENCP_VIDEO_VSPULS_END, 2140},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_EQPULS_BLINE, 0},
	{P_ENCP_VIDEO_EQPULS_ELINE, 4},
	{P_ENCP_VIDEO_VAVON_BLINE, 41},
	{P_ENCP_VIDEO_VAVON_ELINE, 1120},
	{P_ENCP_VIDEO_VSO_BEGIN, 79},
	{P_ENCP_VIDEO_VSO_END, 79},
	{P_ENCP_VIDEO_VSO_BLINE, 0},
	{P_ENCP_VIDEO_VSO_ELINE, 5},
	{P_ENCP_VIDEO_YFP1_HTIME, 271},
	{P_ENCP_VIDEO_YFP2_HTIME, 2190},
	{P_VENC_VIDEO_PROG_MODE, 0x100},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0018},
	{P_ENCP_VIDEO_SYNC_MODE, 0x7},
	{P_ENCP_VIDEO_YC_DLY, 0},
	{P_ENCP_VIDEO_RGB_CTRL, 2},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_4k2k_30hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0008},
	{P_ENCP_VIDEO_YFP1_HTIME, 140},
	{P_ENCP_VIDEO_YFP2_HTIME, 140+3840},
	{P_ENCP_VIDEO_MAX_PXCNT, 3840+560-1},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 2156+1920},
	{P_ENCP_VIDEO_HSPULS_END, 44},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 140},
	{P_ENCP_VIDEO_VSPULS_END, 2059+1920},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_HAVON_BEGIN, 148},
	{P_ENCP_VIDEO_HAVON_END, 3987},
	{P_ENCP_VIDEO_VAVON_BLINE, 89},
	{P_ENCP_VIDEO_VAVON_ELINE, 2248},
	{P_ENCP_VIDEO_HSO_BEGIN, 44},
	{P_ENCP_VIDEO_HSO_END, 2156+1920},
	{P_ENCP_VIDEO_VSO_BEGIN, 2100+1920},
	{P_ENCP_VIDEO_VSO_END, 2164+1920},
	{P_ENCP_VIDEO_VSO_BLINE, 51},
	{P_ENCP_VIDEO_VSO_ELINE, 53},
	{P_ENCP_VIDEO_MAX_LNCNT, 2249},
	{P_ENCP_VIDEO_FILT_CTRL, 0x1000},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_4k2k_25hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0008},
	{P_ENCP_VIDEO_YFP1_HTIME, 140},
	{P_ENCP_VIDEO_YFP2_HTIME, 140+3840},
	{P_ENCP_VIDEO_MAX_PXCNT, 3840+1440-1},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 2156+1920},
	{P_ENCP_VIDEO_HSPULS_END, 44},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 140},
	{P_ENCP_VIDEO_VSPULS_END, 2059+1920},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_HAVON_BEGIN, 148},
	{P_ENCP_VIDEO_HAVON_END, 3987},
	{P_ENCP_VIDEO_VAVON_BLINE, 89},
	{P_ENCP_VIDEO_VAVON_ELINE, 2248},
	{P_ENCP_VIDEO_HSO_BEGIN, 44},
	{P_ENCP_VIDEO_HSO_END, 2156+1920},
	{P_ENCP_VIDEO_VSO_BEGIN, 2100+1920},
	{P_ENCP_VIDEO_VSO_END, 2164+1920},
	{P_ENCP_VIDEO_VSO_BLINE, 51},
	{P_ENCP_VIDEO_VSO_ELINE, 53},
	{P_ENCP_VIDEO_MAX_LNCNT, 2249},
	{P_ENCP_VIDEO_FILT_CTRL, 0x1000},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_4k2k_24hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0008},
	{P_ENCP_VIDEO_YFP1_HTIME, 140},
	{P_ENCP_VIDEO_YFP2_HTIME, 140+3840},
	{P_ENCP_VIDEO_MAX_PXCNT, 3840+1660-1},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 2156+1920},
	{P_ENCP_VIDEO_HSPULS_END, 44},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 140},
	{P_ENCP_VIDEO_VSPULS_END, 2059+1920},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_HAVON_BEGIN, 148},
	{P_ENCP_VIDEO_HAVON_END, 3987},
	{P_ENCP_VIDEO_VAVON_BLINE, 89},
	{P_ENCP_VIDEO_VAVON_ELINE, 2248},
	{P_ENCP_VIDEO_HSO_BEGIN, 44},
	{P_ENCP_VIDEO_HSO_END, 2156+1920},
	{P_ENCP_VIDEO_VSO_BEGIN, 2100+1920},
	{P_ENCP_VIDEO_VSO_END, 2164+1920},
	{P_ENCP_VIDEO_VSO_BLINE, 51},
	{P_ENCP_VIDEO_VSO_ELINE, 53},
	{P_ENCP_VIDEO_MAX_LNCNT, 2249},
	{P_ENCP_VIDEO_FILT_CTRL, 0x1000},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_4k2k_smpte[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_MODE, 0x4040},
	{P_ENCP_VIDEO_MODE_ADV, 0x0008},
	{P_ENCP_VIDEO_YFP1_HTIME, 140},
	{P_ENCP_VIDEO_YFP2_HTIME, 140+3840+256},
	{P_ENCP_VIDEO_MAX_PXCNT, 4096+1404-1},
	{P_ENCP_VIDEO_HSPULS_BEGIN, 2156+1920},
	{P_ENCP_VIDEO_HSPULS_END, 44},
	{P_ENCP_VIDEO_HSPULS_SWITCH, 44},
	{P_ENCP_VIDEO_VSPULS_BEGIN, 140},
	{P_ENCP_VIDEO_VSPULS_END, 2059+1920},
	{P_ENCP_VIDEO_VSPULS_BLINE, 0},
	{P_ENCP_VIDEO_VSPULS_ELINE, 4},
	{P_ENCP_VIDEO_HAVON_BEGIN, 148},
	{P_ENCP_VIDEO_HAVON_END, 3987+256},
	{P_ENCP_VIDEO_VAVON_BLINE, 89},
	{P_ENCP_VIDEO_VAVON_ELINE, 2248},
	{P_ENCP_VIDEO_HSO_BEGIN, 44},
	{P_ENCP_VIDEO_HSO_END, 2156+1920+256},
	{P_ENCP_VIDEO_VSO_BEGIN, 2100+1920+256},
	{P_ENCP_VIDEO_VSO_END, 2164+1920+256},
	{P_ENCP_VIDEO_VSO_BLINE, 51},
	{P_ENCP_VIDEO_VSO_ELINE, 53},
	{P_ENCP_VIDEO_MAX_LNCNT, 2249},
	{P_ENCP_VIDEO_FILT_CTRL, 0x1000},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_4k2k_smpte_25hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_MODE, 0x4040,},
	{P_ENCP_VIDEO_MODE_ADV, 0x18,},
	{P_ENCP_VIDEO_MAX_PXCNT, 0x149F,},
	{P_ENCP_VIDEO_MAX_LNCNT, 0x8C9,},
	{P_ENCP_VIDEO_HAVON_BEGIN, 0xD8,},
	{P_ENCP_VIDEO_HAVON_END, 0x10D7,},
	{P_ENCP_VIDEO_VAVON_BLINE, 0x52,},
	{P_ENCP_VIDEO_VAVON_ELINE, 0x8C1,},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x0,},
	{P_ENCP_VIDEO_HSO_END, 0x58,},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x1E,},
	{P_ENCP_VIDEO_VSO_END, 0x32,},
	{P_ENCP_VIDEO_VSO_BLINE, 0x0,},
	{P_ENCP_VIDEO_VSO_ELINE, 0xA,},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_4k2k_smpte_30hz[] = {
	{P_ENCP_VIDEO_EN, 0},
	{P_ENCI_VIDEO_EN, 0},
	{P_ENCP_VIDEO_MODE, 0x4040,},
	{P_ENCP_VIDEO_MODE_ADV, 0x18,},
	{P_ENCP_VIDEO_MAX_PXCNT, 0x112F,},
	{P_ENCP_VIDEO_MAX_LNCNT, 0x8C9,},
	{P_ENCP_VIDEO_HAVON_BEGIN, 0xD8,},
	{P_ENCP_VIDEO_HAVON_END, 0x10D7,},
	{P_ENCP_VIDEO_VAVON_BLINE, 0x52,},
	{P_ENCP_VIDEO_VAVON_ELINE, 0x8C1,},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x0,},
	{P_ENCP_VIDEO_HSO_END, 0x58,},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x1E,},
	{P_ENCP_VIDEO_VSO_END, 0x32,},
	{P_ENCP_VIDEO_VSO_BLINE, 0x0,},
	{P_ENCP_VIDEO_VSO_ELINE, 0xA,},
	{P_ENCI_VIDEO_EN, 0},
	{MREG_END_MARKER, 0},
};

static const struct reg_t tvregs_vesa_1440x2560p60hz[] = {
	{P_ENCP_VIDEO_EN, 0,},
	{P_ENCI_VIDEO_EN, 0,},

	{P_ENCP_VIDEO_MODE, 0x4040,},
	{P_ENCP_VIDEO_MODE_ADV, 0x18,},
	{P_ENCP_VIDEO_MAX_PXCNT, 0x623,},
	{P_ENCP_VIDEO_MAX_LNCNT, 0xA23,},
	{P_ENCP_VIDEO_HAVON_BEGIN, 0x44,},
	{P_ENCP_VIDEO_HAVON_END, 0x5E3,},
	{P_ENCP_VIDEO_VAVON_BLINE, 0x14,},
	{P_ENCP_VIDEO_VAVON_ELINE, 0xA13,},
	{P_ENCP_VIDEO_HSO_BEGIN, 0x0,},
	{P_ENCP_VIDEO_HSO_END, 0x4,},
	{P_ENCP_VIDEO_VSO_BEGIN, 0x1E,},
	{P_ENCP_VIDEO_VSO_END, 0x32,},
	{P_ENCP_VIDEO_VSO_BLINE, 0x0,},
	{P_ENCP_VIDEO_VSO_ELINE, 0x4,},

	{P_ENCI_VIDEO_EN, 0,},
	{MREG_END_MARKER, 0}
};

struct vic_tvregs_set {
	enum hdmi_vic vic;
	const struct reg_t *reg_setting;
};

/* Using HDMI vic as index */
static struct vic_tvregs_set tvregsTab[] = {
	{HDMI_720x480i60_16x9, tvregs_480i},
	{HDMI_2880x480i60_16x9, tvregs_480i},
	{HDMI_720x480p60_16x9, tvregs_480p},
	{HDMI_2880x240p60_16x9, tvregs_480p},
	{HDMI_720x576i50_16x9, tvregs_576i},
	{HDMI_2880x576i50_16x9, tvregs_576i},
	{HDMI_720x576p50_16x9, tvregs_576p},
	{HDMI_2880x576p50_16x9, tvregs_576p},
	{HDMI_1280x720p60_16x9, tvregs_720p},
	{HDMI_1920x1080i60_16x9, tvregs_1080i},
	{HDMI_1920x1080p60_16x9, tvregs_1080p},
	{HDMI_1920x1080p30_16x9, tvregs_1080p},
	{HDMI_1280x720p50_16x9, tvregs_720p_50hz},
	{HDMI_1920x1080i50_16x9, tvregs_1080i_50hz},
	{HDMI_1920x1080p50_16x9, tvregs_1080p_50hz},
	{HDMI_1920x1080p25_16x9, tvregs_1080p_50hz},
	{HDMI_1920x1080p24_16x9, tvregs_1080p_24hz},
	{HDMI_3840x2160p30_16x9, tvregs_4k2k_30hz},
	{HDMI_3840x2160p25_16x9, tvregs_4k2k_25hz},
	{HDMI_3840x2160p24_16x9, tvregs_4k2k_24hz},
	{HDMI_4096x2160p24_256x135, tvregs_4k2k_smpte},
	{HDMI_4096x2160p25_256x135, tvregs_4k2k_smpte_25hz},
	{HDMI_4096x2160p30_256x135, tvregs_4k2k_smpte_30hz},
	{HDMI_4096x2160p50_256x135, tvregs_4k2k_smpte_25hz},
	{HDMI_4096x2160p60_256x135, tvregs_4k2k_smpte_30hz},
	{HDMI_4096x2160p50_256x135_Y420, tvregs_4k2k_smpte_25hz},
	{HDMI_4096x2160p60_256x135_Y420, tvregs_4k2k_smpte_30hz},
	{HDMI_3840x2160p60_16x9, tvregs_4k2k_30hz},
	{HDMI_3840x2160p50_16x9, tvregs_4k2k_25hz},
	{HDMI_3840x2160p60_16x9_Y420, tvregs_4k2k_30hz},
	{HDMI_3840x2160p50_16x9_Y420, tvregs_4k2k_25hz},
	{HDMIV_1440x2560p60hz, tvregs_vesa_1440x2560p60hz},
};

static inline void setreg(const struct reg_t *r)
{
	hd_write_reg(r->reg, r->val);
	/* printk("[0x%x] = 0x%x\n", r->reg, r->val); */
}

static const struct reg_t *tvregs_setting_mode(enum hdmi_vic vic)
{
	int i = 0;
	for (i = 0; i < ARRAY_SIZE(tvregsTab); i++) {
		if (vic == tvregsTab[i].vic)
			return tvregsTab[i].reg_setting;
	}
	return NULL;
}

void set_vmode_enc_hw(enum hdmi_vic vic)
{
	const struct reg_t *s = tvregs_setting_mode(vic);

	if (s) {
		pr_info("hdmitx: set enc for VIC: %d\n", vic);
		while (MREG_END_MARKER != s->reg)
			setreg(s++);
	} else
		pr_info("hdmitx: not find VIC: %d\n", vic);
}
