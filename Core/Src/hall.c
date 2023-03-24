#include "hall.h"
#include "main.h"
#include "blink.h"
#include <stdio.h>
#include "microtime.h"

#define ZERO_TIMEOUT 5000
#define _SIXTANT 384/6 //==64
static int _POS[8] = {
	[0] = -1,
	[_100] = _SIXTANT*0,
	[_110] = _SIXTANT*1,
	[_010] = _SIXTANT*2,
	[_011] = _SIXTANT*3,
	[_001] = _SIXTANT*4,
	[_101] = _SIXTANT*5,
};

static hall_t _pos={0};

static int _thespeed=0;
static uint64_t _thelast=0;

static inline hall_t _readPos(){
	hall_t res={0};
	res.A = HAL_GPIO_ReadPin( H1_GPIO_Port, H1_Pin );
	res.B = HAL_GPIO_ReadPin( H2_GPIO_Port, H2_Pin );
	res.C = HAL_GPIO_ReadPin( H3_GPIO_Port, H3_Pin );
	return res;
}

__weak void on_hall( int speed );

void HAL_GPIO_EXTI_Callback( uint16_t pin ){

	_pos = _readPos();

	uint64_t now = utTime();

	if( pin & (H1_Pin|H2_Pin|H3_Pin) ){

		__disable_irq();
		if( _thelast > 0 ){
			_thespeed = now - _thelast;
		}
		_thelast = now;
		__enable_irq();

		on_hall( _thespeed );
	}

}

hall_t hallPos(){
	return _pos;
}

int hallRotPos(){
	return _POS[ hallPos().val ];
}

int hallSpeed(){ 
	return _thespeed;
}

#define ISERR 64
int hallRotGuess( bool output ){

	uint64_t now = utTime();
	int coarse = _POS[ hallPos().val ];

	__disable_irq();
	utime_t last = _thelast;
	int speed = _thespeed;
	__enable_irq();

	if( speed > 0 && speed < ZERO_TIMEOUT ){

		int diff = now-last;
		//float x = (float)diff/speed;
		int x = diff*64/speed;
		int fine = coarse + x;

		int err = coarse+32 - fine;

		if( err < ISERR && err > -ISERR ) return fine;
		else if( output ){
			printf( "\n%c[%08d] % 4d/% 4d/% 4d", " !"[err<64?0:1], diff, coarse, fine, err );

			for( int i = 0; i<24; i++ ){
				putchar( "=-"[i<<4<coarse?0:1] );
			}
			printf( "  " );
			for( int i = 0; i<30; i++ ){
				putchar( "=-"[i<<4<fine?0:1] );
			}
			printf( "  " );
			for( int i = 0; i<8; i++ ){
				putchar( "=-"[i<<4<err?0:1] );
			}
		}
	}
	return coarse;
}

void hallInit(){
	//noop. already done by main init
	_pos = _readPos();
}

void hallLoop(){

	utime_t now = utTime();

	if( now-_thelast > ZERO_TIMEOUT ){

		_thespeed = 0;
		_thelast = 0;
	}
}
