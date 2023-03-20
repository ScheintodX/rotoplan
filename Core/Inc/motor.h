#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "threep.h"

typedef enum {
	NC = 0, POS, NEG
} mot_t;

typedef union {
	uint32_t val;
	struct {
		mot_t U : 2;
		mot_t V : 2;
		mot_t W : 2;
	};
} motor_t;

#define _U2V { .U=POS, .V=NEG }
#define _U2W { .U=POS, .W=NEG }
#define _V2U { .V=POS, .U=NEG }
#define _V2W { .V=POS, .W=NEG }
#define _W2U { .W=POS, .U=NEG }
#define _W2V { .W=POS, .V=NEG }

#define _U2VW { .U=POS, .V=NEG, .W=NEG }
#define _V2UW { .V=POS, .U=NEG, .W=NEG }
#define _W2UV { .W=POS, .U=NEG, .V=NEG }

#define _UV2W { .U=POS, .V=POS, .W=NEG }
#define _UW2V { .U=POS, .W=POS, .V=NEG }
#define _VW2U { .V=POS, .W=POS, .U=NEG }

#define mot2c(v) (".+-"[(v)])

extern const motor_t HALL2MOT[8];

void motorInit();
void motorSpeed( int rpm );
void motorStart();
void motorStop();
void motorTim();

void motorMot( motor_t m );

#endif
