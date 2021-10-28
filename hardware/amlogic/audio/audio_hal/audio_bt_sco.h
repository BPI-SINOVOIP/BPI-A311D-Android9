/*
 * Copyright (C) 2017 Amlogic Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _AUDIO_BT_SCO_H_
#define _AUDIO_BT_SCO_H_

#include "audio_hw.h"
#include "aml_malloc_debug.h"
#include "audio_hw_utils.h"

int open_btSCO_device(struct aml_audio_device *adev, size_t frames);
void close_btSCO_device(struct aml_audio_device *adev);
ssize_t write_to_sco(struct audio_stream_out *stream,
        const void *buffer, size_t bytes);
bool is_sco_port(enum OUT_PORT outport);

#endif /* _AUDIO_BT_SCO_H_ */

