#include "microtime.h"

#include "main.h"

extern TIM_HandleTypeDef htim14;

static uint64_t _myTime;
static int last;

utime_t utTime(){
	int now = htim14.Instance->CNT;
	utime_t newt = _myTime + now-last;
	if( now < last ) newt += 0x10000;
	return newt;
}

void utUpdate(){
	int now = htim14.Instance->CNT;
	utime_t newt = _myTime + now-last;
	if( now < last ) newt += 0x10000;
	__disable_irq();
	_myTime = newt;
	last = now;
	__enable_irq();
}

void utInit(){
	HAL_TIM_OC_Start( &htim14, TIM_CHANNEL_1 );
}
