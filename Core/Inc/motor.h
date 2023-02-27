#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "threep.h"

typedef enum {
	NC = 0, POS, NEG
} mot_t;

typedef union {
	uint32_t val;
	struct {
		mot_t A : 8;
		mot_t B : 8;
		mot_t C : 8;
	};
} motor_t;

#define _A2B { .A=POS, .B=NEG }
#define _A2C { .A=POS, .C=NEG }
#define _B2A { .B=POS, .A=NEG }
#define _B2C { .B=POS, .C=NEG }
#define _C2A { .C=POS, .A=NEG }
#define _C2B { .C=POS, .B=NEG }

#define mot2c(v) (".+-"[(v)])

const motor_t HALL2MOT[8];

void motorInit();
void motorSpeed( int rpm );
void motorStart();
void motorStop();
void motorTim();

void motorMot( motor_t m );

#endif
