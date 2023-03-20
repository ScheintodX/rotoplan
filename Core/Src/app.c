#include "app.h"
#include <stdio.h>
#include "hall.h"
#include "main.h"
#include "motor.h"
#include "blink.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_exti.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_tim.h"

#define PWM_MAX 256

#define abs(x) ((x)>=0?(x):(-(x)))

extern uint16_t SinusoidalWaveTable[384];

extern TIM_HandleTypeDef htim1;

static int _PWR_ON = 50;
static const int _PWR_OFF = 0;
static int _FAC = 20;

int _uvwi=0;
//const motor_t _uvw[] = { _U2W, _V2W, _V2U, _W2U, _W2V, _U2V };
const motor_t _uvw[] = { _UW2V, _W2UV, _VW2U, _V2UW, _UV2W, _U2VW };

static inline int rabs( int val ){
	while( val < 0 ) val+=384;
	while( val > 383 ) val -= 384;
	return val;
}

int tv[3];


typedef struct {
	GPIO_PinState pin;
	bool pwm;
} _pwm_t;
const _pwm_t _pinsx[] = {
	[NC] = { GPIO_PIN_RESET, false },
	[POS] = { GPIO_PIN_SET, true },
	[NEG] = { GPIO_PIN_SET, false }
};

static inline void _setMod( int idx, volatile uint32_t *reg, GPIO_TypeDef *gpio_en, int16_t pin_en, mot_t mot ){

	const _pwm_t *p = &_pinsx[ mot ];

	HAL_GPIO_WritePin( gpio_en, pin_en, p->pin );

	int val = p->pwm ? _PWR_ON : _PWR_OFF;
	tv[idx] = val;
	*reg = val;
}

static inline void _setVal( int idx, volatile uint32_t *reg, GPIO_TypeDef *gpio_en, int16_t pin_en, int val ){

	HAL_GPIO_WritePin( gpio_en, pin_en, GPIO_PIN_SET );

	tv[idx] = val;
	*reg = val;
}


void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *tim ){

	GRNon();

	//int rot = hallRot();
	int rot = hallRotGuess( false );

	int OFF = 0;

	int v1 = SinusoidalWaveTable[ rabs( rot     + OFF ) ] / _FAC;
	int v2 = SinusoidalWaveTable[ rabs( rot+128 + OFF ) ] / _FAC;
	int v3 = SinusoidalWaveTable[ rabs( rot+256 + OFF ) ] / _FAC;

	_setVal( 0, &(htim1.Instance->CCR1), EN1_GPIO_Port, EN1_Pin, v1 );
	_setVal( 1, &(htim1.Instance->CCR2), EN2_GPIO_Port, EN2_Pin, v2 );
	_setVal( 2, &(htim1.Instance->CCR3), EN3_GPIO_Port, EN3_Pin, v3 );


	/*
	hall_t pos = hallPos();
	motor_t mot = HALL2MOT[ pos.val ];

	_setMod( 0, &(htim1.Instance->CCR1), EN1_GPIO_Port, EN1_Pin, mot.U );
	_setMod( 1, &(htim1.Instance->CCR2), EN2_GPIO_Port, EN2_Pin, mot.V );
	_setMod( 2, &(htim1.Instance->CCR3), EN3_GPIO_Port, EN3_Pin, mot.W );
	*/

	/*
	//motor_t mot = _uvw[ _uvwi ];
	_setMod( 0, &(htim1.Instance->CCR1), EN1_GPIO_Port, EN1_Pin, POS );
	_setMod( 1, &(htim1.Instance->CCR2), EN2_GPIO_Port, EN2_Pin, NC  );
	_setMod( 2, &(htim1.Instance->CCR3), EN3_GPIO_Port, EN3_Pin, NEG );
	*/

	GRNoff();
}

extern UART_HandleTypeDef huart2;
int readch(){

	uint8_t ch;
	if( HAL_UART_Receive( &huart2, &ch, 1, 0 ) == HAL_OK ){
		return (int)ch;
	} else {
		return -1;
	}

}

void Setup(){

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	hallInit();
	motorInit();

	HAL_TIM_PWM_Start_IT( &htim1, TIM_CHANNEL_1 );
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_2 );
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_3 );

	printf( "Hello World\n" );
	motorStart();
}

const char POS2C[] = "X.";
const char MOT2C[] = ".HL ";

void Loop(){

	static bool output = true;

	hall_t pos = hallPos();
	int rot = hallRot();

	int speed = hallSpeed();

	//__disable_irq();
	motor_t mot = HALL2MOT[ pos.val ];
	//__enable_irq();
	//
	//int si = SinusoidalWaveTable[ rabs( rot ) ];
	

	int guess = hallRotGuess( output );
	if( false && output ){
		printf( "%c%c%c %c%c%c %03d %04d %03d",
				POS2C[pos.A], POS2C[pos.B], POS2C[pos.C], 
				MOT2C[mot.U], MOT2C[mot.V], MOT2C[mot.W],
				rot, guess, speed );

		for( int i = 0; i<48; i++ ){
			putchar( "= "[i*10<rot?0:1] );
		}

		for( int i = 0; i<48; i++ ){
			putchar( "= "[i*10<guess?0:1] );
		}

		putchar( '\n' );
	}

	int c = readch();
	if( c >= 0 ) switch( (char)c ){
		case '^':
			NVIC_SystemReset();
			break;
		case ' ':
			if( output ){
				printf( "\n-PAUSE-\n" );
			} 
			output = !output;
			break;
		case '0':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			_PWR_ON = (c-'0')*10;;
			goto ppow;
		case '1':
			_PWR_ON -= 10;
			goto ppow;
		case '9':
			_PWR_ON += 10;
			goto ppow;
		case '!':
			_PWR_ON = 257;
			goto ppow;
		case '+':
			_PWR_ON++;
			goto ppow;
		case '-':
			_PWR_ON--;
ppow:		printf( "Pwr: %d\n", _PWR_ON );
			break;
		case '#':
			_uvwi = (_uvwi+1)%6;
			printf( "(%c%c%c)\n", MOT2C[_uvw[_uvwi].U], MOT2C[_uvw[_uvwi].V], MOT2C[_uvw[_uvwi].W] );
			break;
		case 'x':
			printf( "%d/%d/%d\n", tv[0], tv[1], tv[2] );
			break;
		case ')':
			_FAC--;
			goto pfac;
		case '(':
			_FAC++;
pfac:		printf( "Fac: %d\n", _FAC );
			break;
		default:
			putchar( c );
	}

	hallLoop();
	
	HAL_Delay( 10 );   /* Insert delay 100 ms */
}
