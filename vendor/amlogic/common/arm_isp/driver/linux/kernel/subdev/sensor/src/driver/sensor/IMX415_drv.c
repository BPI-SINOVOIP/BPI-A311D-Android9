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

#include <linux/delay.h>
#include "acamera_types.h"
#include "system_spi.h"
#include "system_sensor.h"
#include "sensor_bus_config.h"
#include "acamera_command_api.h"
#include "acamera_sbus_api.h"
#include "acamera_sensor_api.h"
#include "system_timer.h"
#include "sensor_init.h"
#include "IMX415_seq.h"
#include "IMX415_config.h"
#include "acamera_math.h"
#include "system_am_mipi.h"
#include "system_am_adap.h"
#include "sensor_bsp_common.h"

#define NEED_CONFIG_BSP 1

// As per IMX334 datasheet
// Max gain is 72 DB
// Analog gain is 0 - 30 dB (step pitch 0.3 dB)
// Digital gain is 30.3 dB to 72 dB
// Gain set in register as (10/3 * gain)
#define AGAIN_MAX_DB (0x64 + 0x8C)   // (30 * 10/3), (42 * 10/3)
#define DGAIN_MAX_DB 0


// Basic read out lines from the specs
#define WDR_2_DOL_4K_BRL 2200
#define WDR_2_DOL_1080P_BRL  1100

static void start_streaming( void *ctx );
static void stop_streaming( void *ctx );

static imx415_private_t imx415_ctx;
static sensor_context_t sensor_ctx;

static sensor_mode_t supported_modes[] = {
    {
        .wdr_mode = WDR_MODE_LINEAR,
        .fps = 30 * 256,
        .resolution.width = 3840,
        .resolution.height = 2160,
        .bits = 10,
        .exposures = 1,
        .lanes = 4,
        .bps = 720,
        .bayer = BAYER_GBRG,
        .dol_type = DOL_NON,
        .num = 0,
    },
    {
        .wdr_mode = WDR_MODE_FS_LIN,
        .fps = 30 * 256,
        .resolution.width = 3840,
        .resolution.height = 2160,
        .bits = 10,
        .exposures = 2,
        .lanes = 4,
        .bps = 1440,
        .bayer = BAYER_GBRG,
        .dol_type = DOL_VC,
        .num = 7,
    },
    {
        .wdr_mode = WDR_MODE_LINEAR,
        .fps = 30 * 256,
        .resolution.width = 1920,
        .resolution.height = 1080,
        .bits = 10,
        .exposures = 1,
        .lanes = 4,
        .bps = 720,
        .bayer = BAYER_GBRG,
        .dol_type = DOL_NON,
        .num = 2,
    },
    {
        .wdr_mode = WDR_MODE_FS_LIN,
        .fps = 30 * 256,
        .resolution.width = 1920,
        .resolution.height = 1080,
        .bits = 10,
        .exposures = 2,
        .lanes = 4,
        .bps = 1440,
        .bayer = BAYER_GBRG,
        .dol_type = DOL_VC,
        .num = 3,
    },
};

//-------------------------------------------------------------------------------------

static const acam_reg_t **p_sensor_data = imx415_seq_table;
static const acam_reg_t **p_isp_data = isp_seq_table;

//--------------------RESET------------------------------------------------------------
static void sensor_hw_reset_enable( void )
{
    system_reset_sensor( 0 );
}

static void sensor_hw_reset_disable( void )
{
    system_reset_sensor( 3 );
}

//-------------------------------------------------------------------------------------
static int32_t sensor_alloc_analog_gain( void *ctx, int32_t gain )
{
    sensor_context_t *p_ctx = ctx;

    // Input gain is in Log2 format of linear gain with LOG2_GAIN_SHIFT
    // Gain(dB) = (20 * Log10(linear_gain)) = 20 * Log2(linear_gain) * Log10(2)
    // As per specs, analog gain register setting is calculated as
    // gain(register) = gain(dB) * 10 / 3
    uint16_t again = ( 20 * gain ) >> LOG2_GAIN_SHIFT;

    if ( again > p_ctx->again_limit ) again = p_ctx->again_limit;

    if ( p_ctx->again[0] != again ) {
        p_ctx->gain_cnt = p_ctx->again_delay + 1;
        p_ctx->again[0] = again;
    }

    return ( ( (int32_t)again ) << LOG2_GAIN_SHIFT ) / 20;
}

static int32_t sensor_alloc_digital_gain( void *ctx, int32_t gain )
{
    return 0;
}

static void sensor_alloc_integration_time( void *ctx, uint16_t *int_time_S, uint16_t *int_time_M, uint16_t *int_time_L )
{
    sensor_context_t *p_ctx = ctx;
    imx415_private_t *p_imx415 = (imx415_private_t *)p_ctx->sdrv;

    switch ( p_ctx->wdr_mode ) {
    case WDR_MODE_LINEAR: // Normal mode
        if ( *int_time_S > (p_ctx->vmax_adjust - 4 )) *int_time_S = p_ctx->vmax_adjust - 4;
        if ( *int_time_S < 8 ) *int_time_S = 8;
        // Integration time is calculated using following formula
        // Integration time = vmax - (SHS1 + 1) lines
        uint16_t shr0 = p_ctx->vmax_adjust - *int_time_S;
        if ( p_imx415->shr0 != shr0 ) {
            p_ctx->int_cnt = 2;
            p_imx415->shr0 = shr0;
            p_ctx->int_time_S = *int_time_S;
        }
        break;
    case WDR_MODE_FS_LIN:
        if ( *int_time_S < 8 ) *int_time_S = 8;
        if ( *int_time_S > p_ctx->max_S ) *int_time_S = p_ctx->max_S;
        if ( *int_time_L < 8 ) *int_time_L =8;

        if ( *int_time_L > p_ctx->max_L ) *int_time_L = p_ctx->max_L;

        if ( p_ctx->int_time_S != *int_time_S || p_ctx->int_time_L != *int_time_L ) {
            p_ctx->int_cnt = 2;

            p_ctx->int_time_S = *int_time_S;
            p_ctx->int_time_L = *int_time_L;

            p_imx415->shr0 = p_ctx->frame - *int_time_L;
            p_imx415->shr1 = p_imx415->rhs1 - *int_time_S;
        }

        break;
    }
}

static int32_t sensor_ir_cut_set( void *ctx, int32_t ir_cut_state )
{
    sensor_context_t *t_ctx = ctx;
    int ret;
    sensor_bringup_t* sensor_bp = t_ctx->sbp;

    LOG( LOG_ERR, "ir_cut_state = %d", ir_cut_state);
    LOG( LOG_INFO, "entry ir cut" );

    //ir_cut_GPIOZ_7 =1 && ir_cut_GPIOZ_11=0, open ir cut
    //ir_cut_GPIOZ_7 =0 && ir_cut_GPIOZ_11=1, close ir cut
    //ir_cut_srate, 2: no operation

    if (sensor_bp->ir_gname[0] <= 0 && sensor_bp->ir_gname[1] <= 0) {
        pr_err("get gpio id fail\n");
        return 0;
    }

    if (ir_cut_state == 1) {
        ret = pwr_ir_cut_enable(sensor_bp, sensor_bp->ir_gname[1], 0);
        if (ret < 0 )
            pr_err("set power fail\n");

        ret = pwr_ir_cut_enable(sensor_bp, sensor_bp->ir_gname[0], 1);
        if (ret < 0 )
            pr_err("set power fail\n");

        mdelay(500);
        ret = pwr_ir_cut_enable(sensor_bp, sensor_bp->ir_gname[1], 1);
        if (ret < 0 )
            pr_err("set power fail\n");
    } else if(ir_cut_state == 0) {
        ret = pwr_ir_cut_enable(sensor_bp, sensor_bp->ir_gname[1], 1);
        if (ret < 0 )
            pr_err("set power fail\n");

        ret = pwr_ir_cut_enable(sensor_bp, sensor_bp->ir_gname[0], 0);
        if (ret < 0 )
            pr_err("set power fail\n");

        mdelay(500);
        ret = pwr_ir_cut_enable(sensor_bp, sensor_bp->ir_gname[0], 1);
        if (ret < 0 )
            pr_err("set power fail\n");
    }

    LOG( LOG_INFO, "exit ir cut" );

    return 0;
}

static uint32_t sensor_vmax_fps( void *ctx, uint32_t framerate )
{
    sensor_context_t *p_ctx = ctx;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;

    if ( framerate == 0 )
        return p_ctx->vmax_fps;

    if (framerate > p_ctx->s_fps )
        return 0;

    sensor_param_t *param = &p_ctx->param;
    uint32_t vmax = ( (( p_ctx->s_fps * 1000) / framerate ) * p_ctx->vmax ) / 1000;
    acamera_sbus_write_u8( p_sbus, 0x3024, vmax & 0xFF );
    acamera_sbus_write_u8( p_sbus, 0x3025, vmax >> 8 );

    if ( p_ctx->wdr_mode == WDR_MODE_LINEAR ) {
        p_ctx->max_L = 2 * vmax - 4;
        param->integration_time_limit = vmax - 10;
        param->integration_time_max = vmax - 10;
    } else {
        p_ctx->max_L = 2 * vmax - 4;
        param->integration_time_limit = p_ctx->max_S;
        param->integration_time_max = p_ctx->max_S;
        param->integration_time_long_max = ( vmax << 1 ) - 256;
        param->lines_per_second = param->lines_per_second >> 1;
        p_ctx->frame = vmax << 1;
    }

    p_ctx->vmax_adjust = vmax;
    p_ctx->vmax_fps = framerate;

    LOG(LOG_INFO,"framerate:%d, vmax:%d, p_ctx->max_L:%d, param->integration_time_long_max:%d",
        framerate, vmax, p_ctx->max_L, param->integration_time_long_max);

    return 0;
}

static void sensor_update( void *ctx )
{
    sensor_context_t *p_ctx = ctx;
    imx415_private_t *p_imx415 = (imx415_private_t *)p_ctx->sdrv;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    if ( p_ctx->int_cnt || p_ctx->gain_cnt || p_ctx->dgain_change ) {
        // ---------- Start Changes -------------
        acamera_sbus_write_u8( p_sbus, 0x3001, 1 );

        // ---------- Analog & Digital Gain -------------
        // IMX307 uses same register to update analog and digital gain
        // gain = analog gain (0 - 30dB) + digital gain (0 - 42dB)
        if ( p_ctx->gain_cnt || p_ctx->dgain_change ) {
            uint16_t reg_gain = 0;
            if ( p_ctx->gain_cnt ) {
                reg_gain = p_ctx->again[p_ctx->again_delay];
                p_ctx->gain_cnt--;
            }
            if ( p_ctx->dgain_change ) {
                reg_gain = p_ctx->again[p_ctx->again_delay] + p_ctx->dgain[0];
                p_ctx->dgain_change = 0;
            }

            acamera_sbus_write_u8( p_sbus, 0x3091, (reg_gain >> 8 ) & 0x07 );
            acamera_sbus_write_u8( p_sbus, 0x3090, (reg_gain >> 0 ) & 0xFF );
        }

        // -------- Integration Time ----------
        if ( p_ctx->int_cnt ) {
            p_ctx->int_cnt--;
            switch ( p_ctx->wdr_mode ) {
            case WDR_MODE_LINEAR:
                acamera_sbus_write_u8( p_sbus, 0x3051, ( p_imx415->shr0 >> 8 ) & 0xFF );
                acamera_sbus_write_u8( p_sbus, 0x3050, ( p_imx415->shr0 >> 0 ) & 0xFF );
                break;
            case WDR_MODE_FS_LIN:
                p_imx415->shr1_old = p_imx415->shr1;
                p_imx415->shr0_old = p_imx415->shr0;

                p_imx415->shr0_old = (p_imx415->shr0_old >> 2) << 2;
                if ( p_imx415->shr0 < 26 ) p_imx415->shr0 = 26;
                p_imx415->shr1_old = ((p_imx415->shr1_old >> 2) << 2) + 1;
                if ( p_imx415->shr1 < 9 ) p_imx415->shr1 = 9;

                // SHR0
                acamera_sbus_write_u8( p_sbus, 0x3051, ( p_imx415->shr0_old >> 8 ) & 0xFF );
                acamera_sbus_write_u8( p_sbus, 0x3050, ( p_imx415->shr0_old >> 0 ) & 0xFF );

                // SHR1
                acamera_sbus_write_u8( p_sbus, 0x3055, ( p_imx415->shr1_old >> 8 ) & 0xFF );
                acamera_sbus_write_u8( p_sbus, 0x3054, ( p_imx415->shr1_old >> 0 ) & 0xFF );
                break;
            }
        }

        // ---------- End Changes -------------
        acamera_sbus_write_u8( p_sbus, 0x3001, 0 );
    }
    p_imx415->shr0_old = p_imx415->shr0;
    p_imx415->shr1_old = p_imx415->shr1;
    p_ctx->again[3] = p_ctx->again[2];
    p_ctx->again[2] = p_ctx->again[1];
    p_ctx->again[1] = p_ctx->again[0];
}

static uint16_t sensor_get_id( void *ctx )
{
    /* return that sensor id register does not exist */

    sensor_context_t *p_ctx = ctx;
    uint16_t sensor_id = 0;

    sensor_id |= acamera_sbus_read_u8(&p_ctx->sbus, 0x30d9) << 8;
    sensor_id |= acamera_sbus_read_u8(&p_ctx->sbus, 0x30da);

    if (sensor_id != SENSOR_CHIP_ID) {
        LOG(LOG_CRIT, "%s: Failed to read sensor id\n", __func__);
        return 0xFFFF;
    }

    LOG(LOG_INFO, "%s: success to read sensor %x\n", __func__, sensor_id);
    return sensor_id;
}

static void sensor_set_mode( void *ctx, uint8_t mode )
{
    sensor_context_t *p_ctx = ctx;
    imx415_private_t *p_imx415 = (imx415_private_t *)p_ctx->sdrv;
    sensor_param_t *param = &p_ctx->param;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    uint8_t setting_num = param->modes_table[mode].num;

    reset_am_enable(p_ctx->sbp,"reset", 0);
    sensor_hw_reset_enable();
    system_timer_usleep( 10000 );
    sensor_hw_reset_disable();
    system_timer_usleep( 10000 );
    reset_am_enable(p_ctx->sbp,"reset", 1);

    p_ctx->again[0] = 0;
    p_ctx->int_time_S = 0;
    p_ctx->int_time_L = 0;

    if (sensor_get_id(ctx) != SENSOR_CHIP_ID) {
        LOG(LOG_INFO, "%s: check sensor failed\n", __func__);
        return;
    }

    switch ( param->modes_table[mode].wdr_mode ) {
    case WDR_MODE_LINEAR:
        sensor_load_sequence( p_sbus, p_ctx->seq_width, p_sensor_data, setting_num );
        p_ctx->again_delay = 0;
        param->integration_time_apply_delay = 2;
        param->isp_exposure_channel_delay = 0;
        break;
    case WDR_MODE_FS_LIN:
        p_ctx->again_delay = 0;
        param->integration_time_apply_delay = 2;
        param->isp_exposure_channel_delay = 0;
        sensor_load_sequence( p_sbus, p_ctx->seq_width, p_sensor_data, setting_num);
        break;
    default:
        return;
        break;
    }

    uint8_t r = acamera_sbus_read_u8( p_sbus, 0x301c );
    switch ( r ) {
    case 0: // full size
       // RHS1 should be 4n+1 and (SHR1+8) <= RHS1 <= (SHR0 - 9)
        // RHS1 < (BRL * 2)
        // FSC = 2 * Vmax = 5400
        p_imx415->rhs1 = 264;

        // Long integration time = FSC - SHR0
        // SHR0 should be 4n and (SHS1 + 18) <= SHR0 <= (FSC-4)
        p_ctx->max_L = 2 * p_ctx->vmax - 8; //4494

        // Short integration time = RHS1 - SHR1
        // Shutter time SHR1 should be 2n+1 and 9 <= SHR1 <= (RHS1 - 18)
        p_ctx->max_S = p_imx415->rhs1 - 8;
        break;
    case 4:
        p_imx415->rhs1 = 159;
        p_ctx->max_L = 2 * p_ctx->vmax - 8;
        p_ctx->max_S = p_imx415->rhs1 - 8;

    break;
    default:
        // 4- Window cropping from 1080p, Other- Prohibited
        //LOG(LOG_CRIT,"WRONG IMAGE SIZE CONFIG");
        break;
    }

    param->active.width = param->modes_table[mode].resolution.width;
    param->active.height = param->modes_table[mode].resolution.height;

    param->total.width = ( (uint16_t)acamera_sbus_read_u8( p_sbus, 0x3029 ) << 8 ) | acamera_sbus_read_u8( p_sbus, 0x3028 );
    param->total.height = ((uint32_t)acamera_sbus_read_u8( p_sbus, 0x3025 ) << 8 ) | acamera_sbus_read_u8( p_sbus, 0x3024 );

    p_ctx->s_fps = param->modes_table[mode].fps >> 8;
    p_ctx->pixel_clock = param->total.width * param->active.height * p_ctx->s_fps;
    p_ctx->vmax = param->total.height;

    param->lines_per_second = p_ctx->pixel_clock / param->total.width;
    param->pixels_per_line = param->total.width;
    param->integration_time_min = SENSOR_MIN_INTEGRATION_TIME;
    if ( param->modes_table[mode].wdr_mode == WDR_MODE_LINEAR ) {
        param->integration_time_limit = p_ctx->vmax - 8;
        param->integration_time_max = p_ctx->vmax - 8;
    } else {
        param->integration_time_limit = p_ctx->max_S;
        param->integration_time_max = p_ctx->max_S;
        if ( param->modes_table[mode].exposures == 2 ) {
            param->integration_time_long_max = ( p_ctx->vmax << 1 ) - 256;
            param->lines_per_second = param->lines_per_second >> 1;
            p_ctx->frame = p_ctx->vmax << 1;
        } else {
            param->integration_time_long_max = ( p_ctx->vmax << 2 ) - 256;
            param->lines_per_second = param->lines_per_second >> 2;
            p_ctx->frame = p_ctx->vmax << 2;
        }
    }
    param->sensor_exp_number = param->modes_table[mode].exposures;
    param->mode = mode;
    param->bayer = param->modes_table[mode].bayer;
    p_ctx->wdr_mode = param->modes_table[mode].wdr_mode;
    p_ctx->vmax_adjust = p_ctx->vmax;
    p_ctx->vmax_fps = p_ctx->s_fps;

    sensor_set_iface(&param->modes_table[mode], p_ctx->win_offset);

    LOG( LOG_CRIT, "Mode %d, Setting num: %d, RES:%dx%d\n", mode, setting_num,
                (int)param->active.width, (int)param->active.height );
}

static const sensor_param_t *sensor_get_parameters( void *ctx )
{
    sensor_context_t *p_ctx = ctx;
    return (const sensor_param_t *)&p_ctx->param;
}

static void sensor_disable_isp( void *ctx )
{
}

static uint32_t read_register( void *ctx, uint32_t address )
{
    sensor_context_t *p_ctx = ctx;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    return acamera_sbus_read_u8( p_sbus, address );
}

static void write_register( void *ctx, uint32_t address, uint32_t data )
{
    sensor_context_t *p_ctx = ctx;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    acamera_sbus_write_u8( p_sbus, address, data );
}

static void stop_streaming( void *ctx )
{
    sensor_context_t *p_ctx = ctx;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    p_ctx->streaming_flg = 0;

    acamera_sbus_write_u8( p_sbus, 0x3000, 0x01 );

    reset_sensor_bus_counter();
    am_adap_deinit();
    am_mipi_deinit();
}

static void start_streaming( void *ctx )
{
    sensor_context_t *p_ctx = ctx;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    sensor_param_t *param = &p_ctx->param;
    sensor_set_iface(&param->modes_table[param->mode], p_ctx->win_offset);
    p_ctx->streaming_flg = 1;
    acamera_sbus_write_u8( p_sbus, 0x3000, 0x00 );
}

static void sensor_test_pattern( void *ctx, uint8_t mode )
{
    sensor_context_t *p_ctx = ctx;
    acamera_sbus_ptr_t p_sbus = &p_ctx->sbus;
    if (mode == 0xff)
        return;

    sensor_load_sequence( p_sbus, p_ctx->seq_width, p_sensor_data, SENSOR_IMX415_SEQUENCE_DEFAULT_TEST_PATTERN );
}

#if PLATFORM_C308X
static uint32_t write1_reg(unsigned long addr, uint32_t val)
{
    void __iomem *io_addr;
    io_addr = ioremap_nocache(addr, 8);
    if (io_addr == NULL) {
        LOG(LOG_ERR, "%s: Failed to ioremap addr\n", __func__);
        return -1;
    }
    __raw_writel(val, io_addr);
    iounmap(io_addr);
    return 0;
}
#endif

void sensor_deinit_imx415( void *ctx )
{
    sensor_context_t *t_ctx = ctx;
    reset_sensor_bus_counter();
    acamera_sbus_deinit(&t_ctx->sbus,  sbus_i2c);
    if (t_ctx != NULL && t_ctx->sbp != NULL)
        clk_am_disable(t_ctx->sbp);
}

static sensor_context_t *sensor_global_parameter(void* sbp)
{
    // Local sensor data structure
    int ret;
    sensor_bringup_t* sensor_bp = (sensor_bringup_t*) sbp;

    sensor_ctx.sbp = sbp;
    sensor_ctx.sdrv = &imx415_ctx;

#if PLATFORM_G12B
    ret = clk_am_enable(sensor_bp, "g12a_24m");
    if (ret < 0 )
        pr_err("set mclk fail\n");
#elif PLATFORM_C308X
    ret = pwr_am_enable(sensor_bp, "power-enable", 0);
    if (ret < 0 )
        pr_err("set power fail\n");
    mdelay(50);
    ret = clk_am_enable(sensor_bp, "g12a_24m");
    if (ret < 0 )
        pr_err("set mclk fail\n");
    write1_reg(0xfe000428, 0x11400400);
#elif PLATFORM_C305X
    ret = gp_pl_am_enable(sensor_bp, "mclk_0", 24000000);
    if (ret < 0 )
        pr_info("set mclk fail\n");
#endif

    udelay(30);

#if NEED_CONFIG_BSP
    ret = reset_am_enable(sensor_bp,"reset", 1);
    if (ret < 0 )
       pr_info("set reset fail\n");
#endif

    sensor_ctx.sbus.mask = SBUS_MASK_SAMPLE_8BITS | SBUS_MASK_ADDR_16BITS | SBUS_MASK_ADDR_SWAP_BYTES;
    sensor_ctx.sbus.control = 0;
    sensor_ctx.sbus.bus = 1;
    sensor_ctx.sbus.device = SENSOR_DEV_ADDRESS;
    acamera_sbus_init( &sensor_ctx.sbus, sbus_i2c );

    // Initial local parameters
    sensor_ctx.address = SENSOR_DEV_ADDRESS;
    sensor_ctx.seq_width = 1;
    sensor_ctx.streaming_flg = 0;
    sensor_ctx.again[0] = 0;
    sensor_ctx.again[1] = 0;
    sensor_ctx.again[2] = 0;
    sensor_ctx.again[3] = 0;
    sensor_ctx.again_limit = AGAIN_MAX_DB + DGAIN_MAX_DB;
    sensor_ctx.pixel_clock = 71986980;
    sensor_ctx.s_fps = 30;
    sensor_ctx.vmax = 2250;
    sensor_ctx.vmax_fps = sensor_ctx.s_fps;
    sensor_ctx.vmax_adjust = sensor_ctx.vmax;

    sensor_ctx.param.again_accuracy = 1 << LOG2_GAIN_SHIFT;
    sensor_ctx.param.sensor_exp_number = 1;
    sensor_ctx.param.again_log2_max = ( ( AGAIN_MAX_DB + DGAIN_MAX_DB ) << LOG2_GAIN_SHIFT ) / 20;
    sensor_ctx.param.dgain_log2_max = 0;
    sensor_ctx.param.integration_time_apply_delay = 2;
    sensor_ctx.param.isp_exposure_channel_delay = 0;
    sensor_ctx.param.modes_table = supported_modes;
    sensor_ctx.param.modes_num = array_size_s( supported_modes );
    sensor_ctx.param.sensor_ctx = &sensor_ctx;
    sensor_ctx.param.isp_context_seq.sequence = p_isp_data;
    sensor_ctx.param.isp_context_seq.seq_num= SENSOR_IMX415_CONTEXT_SEQ;
    sensor_ctx.param.isp_context_seq.seq_table_max = array_size_s( isp_seq_table );

    sensor_ctx.win_offset.offset_x = 0;
    sensor_ctx.win_offset.offset_y = 0;
#if PLATFORM_G12B
    sensor_ctx.win_offset.long_offset = 0xa;
    sensor_ctx.win_offset.short_offset = 0x1d;
#else
    sensor_ctx.win_offset.long_offset = 0x8;
    sensor_ctx.win_offset.short_offset = 0x8;
#endif

    return &sensor_ctx;
}

//--------------------Initialization------------------------------------------------------------
void sensor_init_imx415( void **ctx, sensor_control_t *ctrl, void* sbp)
{
    *ctx = sensor_global_parameter(sbp);

    ctrl->alloc_analog_gain = sensor_alloc_analog_gain;
    ctrl->alloc_digital_gain = sensor_alloc_digital_gain;
    ctrl->alloc_integration_time = sensor_alloc_integration_time;
    ctrl->ir_cut_set= sensor_ir_cut_set;
    ctrl->sensor_update = sensor_update;
    ctrl->set_mode = sensor_set_mode;
    ctrl->get_id = sensor_get_id;
    ctrl->get_parameters = sensor_get_parameters;
    ctrl->disable_sensor_isp = sensor_disable_isp;
    ctrl->read_sensor_register = read_register;
    ctrl->write_sensor_register = write_register;
    ctrl->start_streaming = start_streaming;
    ctrl->stop_streaming = stop_streaming;
    ctrl->sensor_test_pattern = sensor_test_pattern;
    ctrl->vmax_fps = sensor_vmax_fps;

    // Reset sensor during initialization
    sensor_hw_reset_enable();
    system_timer_usleep( 1000 ); // reset at least 1 ms
    sensor_hw_reset_disable();
    system_timer_usleep( 1000 );
}

int sensor_detect_imx415( void* sbp)
{
    int ret = 0;
    sensor_ctx.sbp = sbp;
    sensor_bringup_t* sensor_bp = (sensor_bringup_t*) sbp;
#if PLATFORM_G12B
    ret = clk_am_enable(sensor_bp, "g12a_24m");
    if (ret < 0 )
        pr_err("set mclk fail\n");
#elif PLATFORM_C308X
    write1_reg(0xfe000428, 0x11400400);
#elif PLATFORM_C305X
    ret = gp_pl_am_enable(sensor_bp, "mclk_0", 24000000);
    if (ret < 0 )
        pr_info("set mclk fail\n");
#endif
    udelay(30);

#if NEED_CONFIG_BSP
    ret = reset_am_enable(sensor_bp,"reset", 1);
    if (ret < 0 )
        pr_err("set reset fail\n");
#endif

    sensor_ctx.sbus.mask = SBUS_MASK_SAMPLE_8BITS | SBUS_MASK_ADDR_16BITS | SBUS_MASK_ADDR_SWAP_BYTES;
    sensor_ctx.sbus.control = 0;
    sensor_ctx.sbus.bus = 0;
    sensor_ctx.sbus.device = SENSOR_DEV_ADDRESS;
    acamera_sbus_init( &sensor_ctx.sbus, sbus_i2c );

    ret = 0;
    if (sensor_get_id(&sensor_ctx) == 0xFFFF)
        ret = -1;
    else
        pr_info("sensor_detect_imx415 id:%d\n", sensor_get_id(&sensor_ctx));

    acamera_sbus_deinit(&sensor_ctx.sbus,  sbus_i2c);
    reset_am_disable(sensor_bp);
    return ret;
}

//********************CONSTANT SECTION END*********************************************
//*************************************************************************************
