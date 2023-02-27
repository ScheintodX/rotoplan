#include "hall.h"
#include "main.h"

void hallInit(){
	//noop. already done by main init
}

hall_t hallPos(){
	hall_t res={0};
	res.U = HAL_GPIO_ReadPin( H1_GPIO_Port, H1_Pin );
	res.V = HAL_GPIO_ReadPin( H2_GPIO_Port, H2_Pin );
	res.W = HAL_GPIO_ReadPin( H3_GPIO_Port, H3_Pin );
	return res;
}
