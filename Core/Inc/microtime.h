#ifndef __MICROTIME_H__
#define __MICROTIME_H__

#include <stdint.h>

typedef uint64_t utime_t;

void utInit();
utime_t utTime();
void utUpdate();

#endif
