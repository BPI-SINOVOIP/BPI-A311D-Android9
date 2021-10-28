#ifndef _VAD_TINYMIX_H
#define _VAD_TINYMIX_H

int vadTinymix(char *id, char *val);
int vadTinycapStart(int device, int rate, int channel);
void vadTinycapStop(void);

#endif
