/*
 * Copyright (c) 2014 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <IONmem.h>
#include <gdc_api.h>

#define   FILE_NAME_GDC "/dev/gdc"

int gdc_create_ctx(struct gdc_usr_ctx_s *ctx)
{
	int ret = -1;

	ctx->gdc_client = open(FILE_NAME_GDC, O_RDWR | O_SYNC);

	if (ctx->gdc_client < 0) {
		E_GDC("gdc open failed error=%d, %s",
			errno, strerror(errno));
		return -1;
	}
	ret = ion_mem_init();
	if (ret < 0)
		return -1;

	return 0;
}

int gdc_destroy_ctx(struct gdc_usr_ctx_s *ctx)
{
	int i;

	if (ctx->c_buff != NULL) {
		munmap(ctx->c_buff, ctx->c_len);
		ctx->c_buff = NULL;
		ctx->c_len = 0;
	}
	if (ctx->gs_ex.config_buffer.shared_fd > 0) {
		D_GDC("close config_fd\n");
		close(ctx->gs_ex.config_buffer.shared_fd);
	}

	for (i = 0; i < ctx->plane_number; i++) {
		if (ctx->i_buff[i] != NULL) {
			munmap(ctx->i_buff[i], ctx->i_len[i]);
			ctx->i_buff[i] = NULL;
			ctx->i_len[i] = 0;
		}

		if (i == 0) {
			if (ctx->gs_ex.input_buffer.shared_fd > 0) {
				D_GDC("close in_fd=%d\n",
					ctx->gs_ex.input_buffer.shared_fd);
				close(ctx->gs_ex.input_buffer.shared_fd);
			}
		} else if (i == 1) {
			if (ctx->gs_ex.input_buffer.uv_base_fd > 0) {
				D_GDC("close in_fd=%d\n",
					ctx->gs_ex.input_buffer.uv_base_fd);
				close(ctx->gs_ex.input_buffer.uv_base_fd);
			}
		} else if (i == 2) {
			if (ctx->gs_ex.input_buffer.v_base_fd > 0) {
				D_GDC("close in_fd=%d\n",
					ctx->gs_ex.input_buffer.v_base_fd);
				close(ctx->gs_ex.input_buffer.v_base_fd);
			}
		}

		if (ctx->o_buff[i] != NULL) {
			munmap(ctx->o_buff[i], ctx->o_len[i]);
			ctx->o_buff[i] = NULL;
			ctx->o_len[i] = 0;
		}
		if (i == 0) {
			if (ctx->gs_ex.output_buffer.shared_fd > 0) {
				D_GDC("close out_fd=%d\n",
					ctx->gs_ex.output_buffer.shared_fd);
				close(ctx->gs_ex.output_buffer.shared_fd);
			}
		} else if (i == 1) {
			if (ctx->gs_ex.output_buffer.uv_base_fd > 0) {
				D_GDC("close out_fd=%d\n",
					ctx->gs_ex.output_buffer.uv_base_fd);
				close(ctx->gs_ex.output_buffer.uv_base_fd);
			}
		} else if (i == 2) {
			if (ctx->gs_ex.output_buffer.v_base_fd > 0) {
				D_GDC("close out_fd=%d\n",
					ctx->gs_ex.output_buffer.v_base_fd);
				close(ctx->gs_ex.output_buffer.v_base_fd);
			}
		}
	}
	if (ctx->gdc_client >= 0) {
		close(ctx->gdc_client);
		ctx->gdc_client = -1;
	}
	ion_mem_exit();

	return 0;
}

static int _gdc_alloc_dma_buffer(int fd, unsigned int dir,
						unsigned int len)
{
	int ret = -1;
	struct gdc_dmabuf_req_s buf_cfg;

	memset(&buf_cfg, 0, sizeof(buf_cfg));

	buf_cfg.dma_dir = dir;
	buf_cfg.len = len;

	ret = ioctl(fd, GDC_REQUEST_DMA_BUFF, &buf_cfg);
	if (ret < 0) {
		E_GDC("GDC_REQUEST_DMA_BUFF %s failed: %s, fd=%d\n", __func__,
			strerror(ret), fd);
		return ret;
	}
	return buf_cfg.index;
}

static int _gdc_get_dma_buffer_fd(int fd, int index)
{
	struct gdc_dmabuf_exp_s ex_buf;

	ex_buf.index = index;
	ex_buf.flags = O_RDWR;
	ex_buf.fd = -1;

	if (ioctl(fd, GDC_EXP_DMA_BUFF, &ex_buf)) {
		E_GDC("failed get dma buf fd\n");
		return -1;
	}
	D_GDC("dma buffer export, fd=%d\n", ex_buf.fd);

	return ex_buf.fd;
}

static int _gdc_free_dma_buffer(int fd, int index)
{
	if (ioctl(fd, GDC_FREE_DMA_BUFF, &index))  {
		E_GDC("failed free dma buf fd\n");
		return -1;
	}
	return 0;
}

static int set_buf_fd(gdc_alloc_buffer_t *buf, gdc_buffer_info_t *buf_info,
				int buf_fd, int plane_id)
{
	int ret = 0;

	D_GDC("buf_fd=%d, plane_id=%d, buf->format=%d\n", buf_fd,
				plane_id, buf->format);
	switch (buf->format) {
	case NV12:
		if (buf->plane_number == 1) {
			buf_info->shared_fd = buf_fd;
		} else {
			if (plane_id == 0)
				buf_info->y_base_fd = buf_fd;
			else if(plane_id == 1)
				buf_info->uv_base_fd = buf_fd;
			else {
				E_GDC("plane id(%d) error\n", plane_id);
				ret = -1;
			}
		}
		break;
	case YV12:
		if (buf->plane_number == 1) {
			buf_info->shared_fd = buf_fd;
		} else {
			if (plane_id == 0)
				buf_info->y_base_fd = buf_fd;
			else if (plane_id == 1)
				buf_info->u_base_fd = buf_fd;
			else if (plane_id == 2)
				buf_info->v_base_fd = buf_fd;
			else {
				E_GDC("plane id(%d) error\n", plane_id);
				ret = -1;
			}
		}
		break;
	case Y_GREY:
		if (buf->plane_number == 1) {
			buf_info->shared_fd = buf_fd;
		} else {
			E_GDC("plane id(%d) error\n", plane_id);
			ret = -1;
		}
		break;
	case YUV444_P:
	case RGB444_P:
		if (buf->plane_number == 1) {
			buf_info->shared_fd = buf_fd;
		} else {
			if (plane_id == 0)
			buf_info->y_base_fd = buf_fd;
			else if (plane_id == 1)
				buf_info->u_base_fd = buf_fd;
			else if (plane_id == 2)
				buf_info->v_base_fd = buf_fd;
			else {
				E_GDC("plane id(%d) error\n", plane_id);
				ret = -1;
			}
		}
		break;
	default:
		return -1;
		break;
	}
	D_GDC("buf_info->shared_fd=%d\n",buf_info->shared_fd);
	D_GDC("buf_info->y_base_fd=%d\n",buf_info->y_base_fd);
	D_GDC("buf_info->uv_base_fd=%d\n",buf_info->uv_base_fd);
	return ret;
}

int gdc_alloc_buffer (struct gdc_usr_ctx_s *ctx, uint32_t type,
			struct gdc_alloc_buffer_s *buf, bool cache_flag)
{
	int dir = 0, index = -1, buf_fd[MAX_PLANE] = {-1};
	struct gdc_dmabuf_req_s buf_cfg;
	struct gdc_settings_ex *gs_ex;
	unsigned int i;

	gs_ex = &(ctx->gs_ex);
	memset(&buf_cfg, 0, sizeof(buf_cfg));
	if (type == OUTPUT_BUFF_TYPE)
		dir = DMA_FROM_DEVICE;
	else
		dir = DMA_TO_DEVICE;

	for (i = 0; i < buf->plane_number; i++) {
		buf_cfg.len = buf->len[i];
		buf_cfg.dma_dir = dir;

		if (ctx->mem_type == AML_GDC_MEM_ION) {
			int ret = -1;
			IONMEM_AllocParams ion_alloc_params;

			ret = ion_mem_alloc(buf->len[i], &ion_alloc_params,
						cache_flag);
			if (ret < 0) {
				E_GDC("%s,%d,Not enough memory\n",__func__,
					__LINE__);
				return -1;
			}
			buf_fd[i] = ion_alloc_params.mImageFd;
			D_GDC("gdc_alloc_buffer: ion_fd=%d\n", buf_fd[i]);
		} else if (ctx->mem_type == AML_GDC_MEM_DMABUF) {
			index = _gdc_alloc_dma_buffer(ctx->gdc_client, dir,
							buf_cfg.len);
			if (index < 0)
				return -1;

			/* get  dma fd*/
			buf_fd[i] = _gdc_get_dma_buffer_fd(ctx->gdc_client,
								index);
			if (buf_fd[i] < 0)
				return -1;
			D_GDC("gdc_alloc_buffer: dma_fd=%d\n", buf_fd[i]);
			/* after alloc, dmabuffer free can be called, it just dec refcount */
			_gdc_free_dma_buffer(ctx->gdc_client, index);
		} else {
			E_GDC("gdc_alloc_buffer: wrong mem_type\n");
			return -1;
		}

		switch (type) {
		case INPUT_BUFF_TYPE:
			ctx->i_len[i] = buf->len[i];
			ctx->i_buff[i] = mmap(NULL, buf->len[i],
				PROT_READ | PROT_WRITE,
				MAP_SHARED, buf_fd[i], 0);
			if (ctx->i_buff[i] == MAP_FAILED) {
				ctx->i_buff[i] = NULL;
				E_GDC("Failed to alloc i_buff:%s\n",
					strerror(errno));
			}
			set_buf_fd(buf, &(gs_ex->input_buffer) ,buf_fd[i], i);
			gs_ex->input_buffer.plane_number = buf->plane_number;
			gs_ex->input_buffer.mem_alloc_type = ctx->mem_type;
			break;
		case OUTPUT_BUFF_TYPE:
			ctx->o_len[i] = buf->len[i];
			ctx->o_buff[i] = mmap(NULL, buf->len[i],
				PROT_READ | PROT_WRITE,
				MAP_SHARED, buf_fd[i], 0);
			if (ctx->o_buff[i] == MAP_FAILED) {
				ctx->o_buff[i] = NULL;
				E_GDC("Failed to alloc o_buff:%s\n",
						strerror(errno));
			}
			set_buf_fd(buf, &(gs_ex->output_buffer),buf_fd[i], i);
			gs_ex->output_buffer.plane_number = buf->plane_number;
			gs_ex->output_buffer.mem_alloc_type = ctx->mem_type;
			break;
		case CONFIG_BUFF_TYPE:
			if (i > 0)
				break;
			ctx->c_len = buf->len[i];
			ctx->c_buff = mmap(NULL, buf->len[i],
				PROT_READ | PROT_WRITE,
				MAP_SHARED, buf_fd[i], 0);
			if (ctx->c_buff == MAP_FAILED) {
				ctx->c_buff = NULL;
				E_GDC("Failed to alloc c_buff:%s\n",
						strerror(errno));
			}
			gs_ex->config_buffer.shared_fd = buf_fd[i];
			gs_ex->config_buffer.plane_number = 1;
			gs_ex->config_buffer.mem_alloc_type = ctx->mem_type;
			break;
		default:
			E_GDC("Error no such buff type\n");
			break;
		}
	}
	return 0;
}

int check_plane_number(int plane_number, int format)
{
	int ret = -1;

	if (plane_number == 1) {
		ret = 0;
	} else if (plane_number == 2) {
		if (format == NV12)
		ret = 0;
	} else if (plane_number == 3) {
		if (format == YV12 ||
			(format == YUV444_P)  ||
			(format == RGB444_P))
		ret = 0;
	}
	return ret;
}

int get_file_size(const char *f_name)
{
	int f_size = -1;
	FILE *fp = NULL;

	if (f_name == NULL) {
		E_GDC("Error file name\n");
		return f_size;
	}

	fp = fopen(f_name, "rb");
	if (fp == NULL) {
		E_GDC("Error open file %s\n", f_name);
		return f_size;
	}

	fseek(fp, 0, SEEK_END);

	f_size = ftell(fp);

	fclose(fp);

	D_GDC("%s: size %d\n", f_name, f_size);

	return f_size;
}

int gdc_set_config_param(struct gdc_usr_ctx_s *ctx,
				const char *f_name, int len)
{
	FILE *fp = NULL;
	int r_size = -1;

	if (f_name == NULL || ctx == NULL || ctx->c_buff == NULL) {
		E_GDC("Error input param\n");
		return r_size;
	}

	fp = fopen(f_name, "rb");
	if (fp == NULL) {
		E_GDC("Error open file %s\n", f_name);
		return -1;
	}

	r_size = fread(ctx->c_buff, len, 1, fp);
	if (r_size <= 0)
		E_GDC("Failed to read file %s\n", f_name);

	fclose(fp);

	return r_size;
}

int gdc_init_cfg(struct gdc_usr_ctx_s *ctx, struct gdc_param *tparm,
				const char *f_name)
{
	struct gdc_settings_ex *gdc_gs = NULL;
	int ret = -1;
	uint32_t format = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t o_width = 0;
	uint32_t o_height = 0;
	uint32_t i_y_stride = 0;
	uint32_t i_c_stride = 0;
	uint32_t o_y_stride = 0;
	uint32_t o_c_stride = 0;
	uint32_t i_len = 0;
	uint32_t o_len = 0;
	uint32_t c_len = 0;
	int plane_number = 1;
	gdc_alloc_buffer_t buf;

	if (ctx == NULL || tparm == NULL || f_name == NULL) {
		E_GDC("Error invalid input param\n");
		return ret;
	}

	plane_number = ctx->plane_number;
	i_width = tparm->i_width;
	i_height = tparm->i_height;
	o_width = tparm->o_width;
	o_height = tparm->o_height;

	format = tparm->format;

	i_y_stride = AXI_WORD_ALIGN(i_width);
	o_y_stride = AXI_WORD_ALIGN(o_width);

	if (format == NV12 || format == YUV444_P || format == RGB444_P) {
		i_c_stride = AXI_WORD_ALIGN(i_width);
		o_c_stride = AXI_WORD_ALIGN(o_width);
	} else if (format == YV12) {
		i_c_stride = AXI_WORD_ALIGN(i_width) / 2;
		o_c_stride = AXI_WORD_ALIGN(o_width) / 2;
	} else if (format == Y_GREY) {
		i_c_stride = 0;
		o_c_stride = 0;
	} else {
		E_GDC("Error unknow format\n");
		return ret;
	}

	gdc_gs = &ctx->gs_ex;

	gdc_gs->gdc_config.input_width = i_width;
	gdc_gs->gdc_config.input_height = i_height;
	gdc_gs->gdc_config.input_y_stride = i_y_stride;
	gdc_gs->gdc_config.input_c_stride = i_c_stride;
	gdc_gs->gdc_config.output_width = o_width;
	gdc_gs->gdc_config.output_height = o_height;
	gdc_gs->gdc_config.output_y_stride = o_y_stride;
	gdc_gs->gdc_config.output_c_stride = o_c_stride;
	gdc_gs->gdc_config.format = format;
	gdc_gs->magic = sizeof(*gdc_gs);

	buf.format = format;

	ret = gdc_create_ctx(ctx);
	if (ret < 0)
		return -1;

	if (!ctx->custom_fw) {
		c_len = get_file_size(f_name);
		if (c_len <= 0) {
			gdc_destroy_ctx(ctx);
			E_GDC("Error gdc config file size\n");
			return ret;
		}

		buf.plane_number = 1;
		buf.len[0] = c_len;
		ret = gdc_alloc_buffer(ctx, CONFIG_BUFF_TYPE, &buf, false);
		if (ret < 0) {
			gdc_destroy_ctx(ctx);
			E_GDC("Error alloc gdc cfg buff\n");
			return ret;
		}

		ret = gdc_set_config_param(ctx, f_name, c_len);
		if (ret < 0) {
			gdc_destroy_ctx(ctx);
			E_GDC("Error cfg gdc param buff\n");
			return ret;
		}

		gdc_gs->gdc_config.config_size = c_len / 4;
	}
	buf.plane_number = plane_number;
	if ((plane_number == 1) || (format == Y_GREY)) {
		if (format == RGB444_P || format == YUV444_P)
			i_len = i_y_stride * i_height * 3;
		else if (format == NV12 || format == YV12)
			i_len = i_y_stride * i_height * 3 / 2;
		else if (format == Y_GREY)
			i_len = i_y_stride * i_height;
		buf.plane_number = 1;
		buf.len[0] = i_len;
	} else if ((plane_number == 2) && (format == NV12)) {
		buf.len[0] = i_y_stride * i_height;
		buf.len[1] = i_y_stride * i_height / 2;
	} else if ((plane_number == 3) &&
		(format == YV12 ||
		(format == YUV444_P) ||
		(format == RGB444_P))) {
		buf.len[0] = i_y_stride * i_height;
		if (format == YV12) {
			buf.len[1] = i_y_stride * i_height / 4;
			buf.len[2] = i_y_stride * i_height / 4;
		} else if ((format == YUV444_P) ||
			(format == RGB444_P)) {
			buf.len[1] = i_y_stride * i_height;
			buf.len[2] = i_y_stride * i_height;
		}
	}
	ret = gdc_alloc_buffer(ctx, INPUT_BUFF_TYPE, &buf, false);
	if (ret < 0) {
		gdc_destroy_ctx(ctx);
		E_GDC("Error alloc gdc input buff\n");
		return ret;
	}

	buf.plane_number = plane_number;
	if ((plane_number == 1) || (format == Y_GREY)) {
		if (format == RGB444_P || format == YUV444_P)
			o_len = o_y_stride * o_height * 3;
		else if (format == NV12 || format == YV12)
			o_len = o_y_stride * o_height * 3 / 2;
		else if (format == Y_GREY)
			o_len = o_y_stride * o_height;
		buf.plane_number = 1;
		buf.len[0] = o_len;
	} else if ((plane_number == 2) && (format == NV12)) {
		buf.len[0] = o_y_stride * o_height;
		buf.len[1] = o_y_stride * o_height / 2;
	} else if ((plane_number == 3) &&
		(format == YV12 ||
		(format == YUV444_P) ||
		(format == RGB444_P))) {
		buf.len[0] = o_y_stride * o_height;
		if (format == YV12) {
			buf.len[1] = o_y_stride * o_height / 4;
			buf.len[2] = o_y_stride * o_height / 4;
		} else if ((format == YUV444_P) ||
			(format == RGB444_P)) {
			buf.len[1] = o_y_stride * o_height;
			buf.len[2] = o_y_stride * o_height;
		}
	}

	ret = gdc_alloc_buffer(ctx, OUTPUT_BUFF_TYPE, &buf, true);
	if (ret < 0) {
		gdc_destroy_ctx(ctx);
		E_GDC("Error alloc gdc input buff\n");
		return ret;
	}
	return ret;
}

int gdc_process(struct gdc_usr_ctx_s *ctx)
{
	int ret = -1;
	struct gdc_settings_ex *gs_ex = &ctx->gs_ex;

	ret = gdc_sync_for_device(ctx);
	if (ret < 0)
		return ret;

	ret = ioctl(ctx->gdc_client, GDC_PROCESS_EX, gs_ex);
	if (ret < 0) {
		E_GDC("GDC_RUN ioctl failed\n");
		return ret;
	}

	ret = gdc_sync_for_cpu(ctx);
	if (ret < 0)
		return ret;

	return 0;
}

int gdc_process_with_builtin_fw(struct gdc_usr_ctx_s *ctx)
{
	int ret = -1;
	struct gdc_settings_with_fw *gs_with_fw = &ctx->gs_with_fw;

	ret = gdc_sync_for_device(ctx);
	if (ret < 0)
		return ret;

	ret = ioctl(ctx->gdc_client, GDC_PROCESS_WITH_FW, gs_with_fw);
	if (ret < 0) {
		E_GDC("GDC_PROCESS_WITH_FW ioctl failed\n");
		return ret;
	}
	ret = gdc_sync_for_cpu(ctx);
	if (ret < 0)
		return ret;

	return 0;
}

int gdc_sync_for_device(struct gdc_usr_ctx_s *ctx)
{
	int ret = -1, i;
	int shared_fd[MAX_PLANE];
	int plane_number = 0;
	struct gdc_settings_ex *gs_ex = &ctx->gs_ex;

	if (gs_ex->input_buffer.mem_alloc_type == AML_GDC_MEM_DMABUF) {
		plane_number = gs_ex->input_buffer.plane_number;
		for (i = 0; i < plane_number; i++) {
			if (i == 0)
				shared_fd[i] = gs_ex->input_buffer.shared_fd;
			else if (i == 1)
				shared_fd[i] = gs_ex->input_buffer.uv_base_fd;
			else if (i == 2)
				shared_fd[i] = gs_ex->input_buffer.v_base_fd;
			ret = ioctl(ctx->gdc_client, GDC_SYNC_DEVICE,
					&shared_fd[i]);
			if (ret < 0) {
				E_GDC("gdc_sync_for_device ioctl failed\n");
				return ret;
			}
		}
	}
	if (!ctx->custom_fw &&
		gs_ex->config_buffer.mem_alloc_type == AML_GDC_MEM_DMABUF) {
		shared_fd[0] = gs_ex->config_buffer.shared_fd;
		ret = ioctl(ctx->gdc_client, GDC_SYNC_DEVICE,
				&shared_fd[0]);
		if (ret < 0) {
			E_GDC("gdc_sync_for_device ioctl failed\n");
			return ret;
		}
	}

	return 0;
}

int gdc_sync_for_cpu(struct gdc_usr_ctx_s *ctx)
{
	int ret = -1, i;
	int shared_fd[MAX_PLANE];
	int plane_number = 0;
	struct gdc_settings_ex *gs_ex = &ctx->gs_ex;

	if (gs_ex->output_buffer.mem_alloc_type == AML_GDC_MEM_DMABUF) {
		plane_number = gs_ex->output_buffer.plane_number;
		for (i = 0; i < plane_number; i++) {
			if (i == 0)
				shared_fd[i] = gs_ex->output_buffer.shared_fd;
			else if (i == 1)
				shared_fd[i] = gs_ex->output_buffer.uv_base_fd;
			else if (i == 2)
				shared_fd[i] = gs_ex->output_buffer.v_base_fd;
			ret = ioctl(ctx->gdc_client, GDC_SYNC_CPU,
					&shared_fd[i]);
			if (ret < 0) {
				E_GDC("gdc_sync_for_cpu ioctl failed\n");
				return ret;
			}
		}
	}
	return 0;
}
