#include "hall.h"
#include "main.h"
#include "blink.h"
#include <stdio.h>

static uint64_t _myTime;

extern TIM_HandleTypeDef htim14;

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

static uint64_t _last[6];
static int _speed[6];

static int _thespeed;
static uint64_t _thelastZero;

static inline hall_t _readPos(){
	hall_t res={0};
	res.A = HAL_GPIO_ReadPin( H1_GPIO_Port, H1_Pin );
	res.B = HAL_GPIO_ReadPin( H2_GPIO_Port, H2_Pin );
	res.C = HAL_GPIO_ReadPin( H3_GPIO_Port, H3_Pin );
	return res;
}

void HAL_GPIO_EXTI_Callback( uint16_t pin ){ 

	uint64_t now = hallTime();

	int i, j, s, sum=0;
	GPIO_TypeDef *pin_port = NULL;
	uint16_t pin_no;
		
	blinkRED();

	_pos = _readPos();

	if( pin & H1_Pin ){
		pin_port = H1_GPIO_Port;
		pin_no = H1_Pin,
		i = 0;
	}
	if( pin & H2_Pin ){
		pin_port = H2_GPIO_Port;
		pin_no = H2_Pin,
		i = 2;
	}
	if( pin & H3_Pin ){
		pin_port = H3_GPIO_Port;
		pin_no = H3_Pin,
		i = 4;
	}

	if( pin_port ){

		bool val;
		bool ok;

		ok = true;

		val = HAL_GPIO_ReadPin( pin_port, pin_no );
		i += val ? 1 : 0;

		if( _last[ i ] != 0 ){
			_speed[ i ] = now - _last[ i ];
		} 
		_last[ i ] = now;

		for( j=0; j<6; j++ ){

			s = _speed[j];

			if( s == 0 ) ok = false;

			sum += s;
		}

		if( ok ){
			_thespeed = sum / 6;
		} else {
			_thespeed = 0;
		}

		if( i==0 ){
			// for speed based on time
			if( ok ){
				_thelastZero = now;
			} else {
				_thelastZero = 0;
			}
		}
	}
}

int hallSpeed(){ 
	return _thespeed;
}

void hallInit(){
	//noop. already done by main init
	_pos = _readPos();

	HAL_TIM_OC_Start( &htim14, TIM_CHANNEL_1 );
}

int hallRot(){

	int rot;

	/*
	if( _thelastZero > 0 ){

		int now = hallTime(), diff = now-_thelastZero;

		float x = (float)diff/_thespeed;
		rot = 384*x;
	} else {
		rot = _POS[ hallPos().val ];
	}
	*/
		rot = _POS[ hallPos().val ];
	return rot;
}


int hallRotGuess( bool output ){

	int coarse = _POS[ hallPos().val ];

	int diff = hallTime()-_thelastZero;

	float x = (float)diff/_thespeed;

	int fine = 384*x;

	int err = coarse+32 - fine;
	if( err < 0 ) err=-err;

	if( output ){
		printf( "\n[%05d] % 4d/% 4d/% 4d", diff, coarse, fine, err );

		for( int i = 0; i<40; i++ ){
			putchar( "=-"[i*10<coarse?0:1] );
		}
		printf( "  " );
		for( int i = 0; i<48; i++ ){
			putchar( "=-"[i*10<fine?0:1] );
		}
		printf( "  " );
		for( int i = 0; i<10; i++ ){
			putchar( "=-"[i*10<err?0:1] );
		}
	}

	if( err < 64 ) return fine;
	else return coarse;
}

hall_t hallPos(){
	return _pos;
}
uint64_t hallTime(){
	static int last=0;
	int now = htim14.Instance->CNT;
	uint64_t newt = _myTime + now-last;
	if( now < last ) newt += 0x10000;
	__disable_irq();
	_myTime = newt;
	__enable_irq();
	last = now;
	return _myTime;
}

void hallLoop(){

	// make sure it gets called at least every 6.5535 seconds
	UNUSED( hallTime() );
}
