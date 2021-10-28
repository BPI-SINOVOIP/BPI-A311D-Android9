/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2014, The Linux Foundation. All rights reserved.
 * Portions Copyright 2014 Broadcom Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of The Linux Foundation nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * NOTE:
 *   Although it is very similar, this license text is not identical
 *   to the "BSD-3-Clause", therefore, DO NOT MODIFY THIS LICENSE TEXT!
 */

#include <config.h>
#include <common.h>
#include <aboot.h>
#include <malloc.h>
#include <part.h>
#include <fb_fastboot.h>
#include <sparse_format.h>

#ifndef CONFIG_FASTBOOT_FLASH_FILLBUF_SIZE
#define CONFIG_FASTBOOT_FLASH_FILLBUF_SIZE (1024 * 512)
#endif
int write_sparse_image(
		struct sparse_storage *info, const char *part_name,
		void *data, unsigned sz)
{
	lbaint_t blk;
	lbaint_t blkcnt;
	lbaint_t blks;
	uint64_t bytes_written = 0;
	unsigned int chunk;
	uint64_t chunk_data_sz;
	uint32_t *fill_buf = NULL;
	uint32_t fill_val;
	sparse_header_t *sparse_header;
	chunk_header_t *chunk_header;
	uint32_t total_blocks = 0;
	int fill_buf_num_blks;
	int i;
	int j;

	fill_buf_num_blks = CONFIG_FASTBOOT_FLASH_FILLBUF_SIZE / info->blksz;
	/* Read and skip over sparse image header */
	sparse_header = (sparse_header_t *) data;

	data += sparse_header->file_hdr_sz;
	if (sparse_header->file_hdr_sz > sizeof(sparse_header_t))
	{
		/*
		 * Skip the remaining bytes in a header that is longer than
		 * we expected.
		 */
		data += (sparse_header->file_hdr_sz - sizeof(sparse_header_t));
	}

	debug("=== Sparse Image Header ===\n");
	debug("magic: 0x%x\n", sparse_header->magic);
	debug("major_version: 0x%x\n", sparse_header->major_version);
	debug("minor_version: 0x%x\n", sparse_header->minor_version);
	debug("file_hdr_sz: %d\n", sparse_header->file_hdr_sz);
	debug("chunk_hdr_sz: %d\n", sparse_header->chunk_hdr_sz);
	debug("blk_sz: %d\n", sparse_header->blk_sz);
	debug("total_blks: %d\n", sparse_header->total_blks);
	debug("total_chunks: %d\n", sparse_header->total_chunks);

	/* verify sparse_header->blk_sz is an exact multiple of info->blksz */
	if (sparse_header->blk_sz !=
	    (sparse_header->blk_sz & ~(info->blksz - 1))) {
		printf("%s: Sparse image block size issue [%u]\n",
		       __func__, sparse_header->blk_sz);
		fastboot_fail("sparse image block size issue");
		return -1;
	}

	puts("Flashing Sparse Image\n");

	/* Start processing chunks */
	blk = info->start;
	for (chunk=0; chunk<sparse_header->total_chunks; chunk++)
	{
		/* Read and skip over chunk header */
		chunk_header = (chunk_header_t *) data;
		data += sizeof(chunk_header_t);

		if (chunk_header->chunk_type != CHUNK_TYPE_RAW) {
			debug("=== Chunk Header ===\n");
			debug("chunk_type: 0x%x\n", chunk_header->chunk_type);
			debug("chunk_data_sz: 0x%x\n", chunk_header->chunk_sz);
			debug("total_size: 0x%x\n", chunk_header->total_sz);
		}

		if (sparse_header->chunk_hdr_sz > sizeof(chunk_header_t))
		{
			/*
			 * Skip the remaining bytes in a header that is longer
			 * than we expected.
			 */
			data += (sparse_header->chunk_hdr_sz -
				 sizeof(chunk_header_t));
		}

		chunk_data_sz = (uint64_t)sparse_header->blk_sz * (uint64_t)chunk_header->chunk_sz;
		blkcnt = chunk_data_sz / info->blksz;
		switch (chunk_header->chunk_type)
		{
			case CHUNK_TYPE_RAW:
			if (chunk_header->total_sz !=
			    (sparse_header->chunk_hdr_sz + chunk_data_sz))
			{
				fastboot_fail(
					"Bogus chunk size for chunk type Raw");
				return -1;
			}

			if (blk + blkcnt > info->start + info->size) {
				printf(
				    "%s: Request would exceed partition size!\n",
				    __func__);
				fastboot_fail(
				    "Request would exceed partition size!");
				return -1;
			}

			blks = info->write(info, blk, blkcnt,
						     data);
			if (blks < blkcnt) {
				printf("%s: Write failed " LBAFU "\n",
				       __func__, blks);
				fastboot_fail("flash write failure");
				return -1;
			}
			blk += blks;
			bytes_written += blkcnt * info->blksz;
			total_blocks += chunk_header->chunk_sz;
			data += chunk_data_sz;
			break;

			case CHUNK_TYPE_FILL:
			if (chunk_header->total_sz !=
			    (sparse_header->chunk_hdr_sz + sizeof(uint32_t)))
			{
				fastboot_fail(
					"Bogus chunk size for chunk type FILL");
				return -1;
			}

			fill_buf = (uint32_t *)
				   memalign(ARCH_DMA_MINALIGN,
					    ROUNDUP(
						info->blksz * fill_buf_num_blks,
						    ARCH_DMA_MINALIGN));
			if (!fill_buf)
			{
				fastboot_fail(
					"Malloc failed for: CHUNK_TYPE_FILL");
				return -1;
			}

			fill_val = *(uint32_t *)data;
			data = (char *) data + sizeof(uint32_t);

			for (i = 0;
			     i < (info->blksz * fill_buf_num_blks /
				  sizeof(fill_val));
			     i++)
				fill_buf[i] = fill_val;

			if (blk + blkcnt > info->start + info->size) {
				printf(
				    "%s: Request would exceed partition size!\n",
				    __func__);
				fastboot_fail(
				    "Request would exceed partition size!");
				free(fill_buf);
				return -1;
			}

			for (i = 0; i < blkcnt;) {
				j = blkcnt - i;
				if (j > fill_buf_num_blks)
					j = fill_buf_num_blks;
				blks = info->write(info, blk, j, fill_buf);
				/* blks might be > j (eg. NAND bad-blocks) */
				if (blks < j) {
					printf("%s: %s " LBAFU " [%d]\n",
					       __func__,
					       "Write failed, block #",
					       blk, j);
					fastboot_fail(
						      "flash write failure");
					free(fill_buf);
					return -1;
				}
				blk += blks;
				i += j;
			}
			bytes_written += blkcnt * info->blksz;
			total_blocks += chunk_data_sz / sparse_header->blk_sz;

			free(fill_buf);
			break;

			case CHUNK_TYPE_DONT_CARE:
			blk += info->reserve(info, blk, blkcnt);
			total_blocks += chunk_header->chunk_sz;
			break;

			case CHUNK_TYPE_CRC32:
			if (chunk_header->total_sz !=
			    sparse_header->chunk_hdr_sz)
			{
				fastboot_fail(
					"Bogus chunk size for chunk type Dont Care");
				return -1;
			}
			total_blocks += chunk_header->chunk_sz;
			data += chunk_data_sz;
			break;

			default:
			printf("%s: Unknown chunk type: %x\n", __func__,
			       chunk_header->chunk_type);
			fastboot_fail("Unknown chunk type");
			return -1;
		}
	}

	debug("Wrote %d blocks, expected to write %d blocks\n",
	      total_blocks, sparse_header->total_blks);
	printf("........ wrote %u bytes to '%s'\n", (int)bytes_written, part_name);

	if (total_blocks != sparse_header->total_blks) {
		fastboot_fail("sparse image write failure");
		return -1;
	} else {
		fastboot_okay("");
		return 0;
	}
}
