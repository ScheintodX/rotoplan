#include "app.h"
#include <stdio.h>
#include "hall.h"
#include "motor.h"

#include "stm32f0xx_hal.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim16;

void blink( TIM_HandleTypeDef *tim ){
	HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_2);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim){ blink( htim ); }
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim){ blink( htim ); }

/*
void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *tim ){
	if( tim == &htim1 ){
		motorTim();
	} else if( tim == &htim16 ){
		HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_2);
	}
}
*/

void Setup(){
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_3);
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	hallInit();
	motorInit();

	printf( "Hello World" );

	motorStart();

}

void Loop(){

	HAL_GPIO_TogglePin (GPIOA, GPIO_PIN_5);

	hall_t pos = hallPos();
	motor_t mot = HALL2MOT[ pos.val ];
	printf( "%1x:%c%c%c -> %06x:%c%c%c\n", 
			(int)pos.val, hal2c( pos.U ), hal2c( pos.V ), hal2c( pos.W  ),
			(int)mot.val, mot2c( mot.A ), mot2c( mot.B ), mot2c( mot.C  )
	);
	//motorMot( mot );
	HAL_Delay (100);   /* Insert delay 100 ms */
}
