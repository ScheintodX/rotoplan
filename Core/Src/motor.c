#include "motor.h"

#include "stm32f0xx_hal.h"

#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "hall.h"
#include "stm32f0xx_hal_gpio.h"

static GPIO_TypeDef * const PORT_EN[] = {
		EN1_GPIO_Port, EN2_GPIO_Port, EN3_GPIO_Port };
static const uint16_t PIN_EN[] = {
		EN1_Pin, EN2_Pin, EN3_Pin };
static const uint32_t TIMS[] = {
	TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3 };


extern TIM_HandleTypeDef htim1;

// s. https://www.nxp.com/docs/en/application-note/AN4776.pdf Fig. 12
const motor_t HALL2MOT[8] = {
	[_011] = _C2B,
	[_001] = _A2B,
	[_101] = _A2C,
	[_100] = _B2C,
	[_110] = _B2A,
	[_010] = _C2A,
};

void motorTim(){
}

void motorInit(){
	int i;
	for( i=0; i<3; i++ ){
		HAL_GPIO_WritePin( PORT_EN[i], PIN_EN[i], GPIO_PIN_RESET );
	}
}

void motorStart(){
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_1 );
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_2 );
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_3 );
	__HAL_TIM_SET_COMPARE( &htim1, TIM_CHANNEL_1, 0 );
	__HAL_TIM_SET_COMPARE( &htim1, TIM_CHANNEL_2, 0 );
	__HAL_TIM_SET_COMPARE( &htim1, TIM_CHANNEL_3, 0 );
}

int speed = 500; // 0..n..1000

static inline void _set( uint32_t ch, mot_t mo ){
	switch( mo ){
		case NC: 
			HAL_GPIO_WritePin( PORT_EN[ch], PIN_EN[ch], GPIO_PIN_RESET );
			break;
		case POS:
			HAL_GPIO_WritePin( PORT_EN[ch], PIN_EN[ch], GPIO_PIN_SET );
			__HAL_TIM_SET_COMPARE( &htim1, TIMS[ch], speed );
			break;
		case NEG:
			HAL_GPIO_WritePin( PORT_EN[ch], PIN_EN[ch], GPIO_PIN_SET );
			__HAL_TIM_SET_COMPARE( &htim1, TIMS[ch], 0 );
			break;
	}
}

void motorMot( motor_t m ){

	_set( 0, m.A );
	_set( 1, m.B );
	_set( 2, m.C );

}
