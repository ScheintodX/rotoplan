#ifndef __HALL_H__
#define __HALL_H__

#include "threep.h"

typedef bool hal_t;

typedef union {
	uint32_t val; // for alignment
	struct {
		hal_t U: 1;
		hal_t V: 1;
		hal_t W: 1;
	};
} hall_t;

#define _U00 (hall_t){ _100 }
#define _UV0 (hall_t){ _110 }
#define _0V0 (hall_t){ _010 }
#define _0VW (hall_t){ _011 }
#define _00W (hall_t){ _001 }
#define _U0W (hall_t){ _101 }

#define hal2c(v) (".X"[(v)])

// init pheripherials
void hallInit();

// return pos using only hall
hall_t hallPos();

// return pos 
int hallPosFine();

#endif
