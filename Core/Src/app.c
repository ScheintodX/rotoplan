#include "app.h"
#include <stdio.h>

#include "stm32f0xx_hal.h"

extern TIM_HandleTypeDef htim16;

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *tim ){
  if( tim == &htim16 ){
    HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_2);
  }
}
void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *tim ){
  if( tim == &htim16 ){
    HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_2);
  }
}

void Setup(){
  HAL_TIM_Base_Start_IT(&htim16);
//  HAL_TIM_OC_Start(&htim16,0);
  setvbuf(stdin, NULL, _IONBF, 8);
  setvbuf(stdout, NULL, _IONBF, 8);
  setvbuf(stderr, NULL, _IONBF, 8);
  printf( "Hello World" );
}

void Loop(){
  HAL_GPIO_TogglePin (GPIOA, GPIO_PIN_5);

  fputc( '.', stdout );
  HAL_Delay (100);   /* Insert delay 100 ms */
}
