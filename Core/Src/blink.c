#include "blink.h"

#include "main.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_exti.h"

void blinkRED(){
	HAL_GPIO_TogglePin( LD3_GPIO_Port, LD3_Pin );
}
void blinkGRN(){
	HAL_GPIO_TogglePin( LD2_GPIO_Port, LD2_Pin );
}
void GRNon(){
	HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET );
}
void GRNoff(){
	HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET );
}
