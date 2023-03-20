#ifndef __HALL_H__
#define __HALL_H__

#include "threep.h"

/*
* Rotation in "rot" unit. As "deg" is from 0..360 or "rad" 0..2pi "rot" is 0..384.
* 384 is 3*128.
*
* Hall sensors are named A, B, C and are at
* A:   0rot
* B: 128rot
* C: 256rot
*
* Windings are 192 rot turned (180deg). so
* U: 192rot
* V: 320rot
* W:  64rot
*/

/*
*               0°
*           ---------
*         /           \
*       /      (A)      \
*     /  ---         ---  \
*    |  [[V]]       [[W]]  |
*    |   ---    x    ---   |
*    |                     |
*     \  (B)   ---   (C)  /
*   256°\     [[U]]     / 128°
*         \    ---    /
*           ---------
*/

typedef bool hal_t;

typedef union {
	uint32_t val; // for alignment
	struct {
		hal_t A: 1;
		hal_t B: 1;
		hal_t C: 1;
	};
} hall_t;

/*
#define _A00 (hall_t){ _100 }
#define _AB0 (hall_t){ _110 }
#define _0B0 (hall_t){ _010 }
#define _0BC (hall_t){ _011 }
#define _00C (hall_t){ _001 }
#define _A0C (hall_t){ _101 }
*/
#define _A00 _100
#define _AB0 _110
#define _0B0 _010
#define _0BC _011
#define _00C _001
#define _A0C _101

#define hal2c(v) (".X"[(v)])

// init pheripherials
void hallInit();

// return pos using only hall
hall_t hallPos();

// return pos as 384 rad
int hallRot(); //0..383rad
int hallRotGuess(bool); //0..383rad

// return pos
int hallPosFine();

// return speed
int hallSpeed();

uint64_t hallTime();

void hallLoop();

#endif
