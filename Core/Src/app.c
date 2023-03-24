#include "app.h"
#include <stdio.h>
#include "hall.h"
#include "main.h"
#include "motor.h"
#include "blink.h"
#include "microtime.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_exti.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_tim.h"

extern uint16_t SinusoidalWaveTable[384];

extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc;

static volatile int _FAC = 128;
static bool _REV = false;

static inline int rabs( int val ){
	while( val < 0 ) val+=384;
	while( val > 383 ) val -= 384;
	return val;
}

#define abs(x) ((x)<0?-(x):(x))

typedef struct {
	GPIO_PinState pin;
	bool pwm;
} _pwm_t;
const _pwm_t _pinsx[] = {
	[NC] = { GPIO_PIN_RESET, false },
	[POS] = { GPIO_PIN_SET, true },
	[NEG] = { GPIO_PIN_SET, false }
};

void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *tim ){

	GRNon();

	//int rot = hallRotPos();
	int rot = hallRotGuess( false );

	int v1 = SinusoidalWaveTable[ rabs( rot     ) ] * _FAC / 1024;
	int v2 = SinusoidalWaveTable[ rabs( rot+128 ) ] * _FAC / 1024;
	int v3 = SinusoidalWaveTable[ rabs( rot+256 ) ] * _FAC / 1024;

	if( _REV ){
		htim1.Instance->CCR1 = v1;
		htim1.Instance->CCR2 = v2;
		htim1.Instance->CCR3 = v3;
	} else {
		htim1.Instance->CCR1 = v1;
		htim1.Instance->CCR2 = v2;
		htim1.Instance->CCR3 = v3;

	}

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

	utInit();
	hallInit();
	motorInit();

	HAL_TIM_PWM_Start_IT( &htim1, TIM_CHANNEL_1 );
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_2 );
	HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_3 );

	printf( "Hello World\n" );
	HAL_ADC_Start(&hadc);
	motorStart();
}

const char POS2C[] = "X.";
const char MOT2C[] = ".HL ";

#define ADC2RPM(x) (x)

#define SPD_RND 8
#define RPM_RND 4
#define FAC_RND 4
int spd_ring[SPD_RND], spd_i=0;
int rpm_ring[RPM_RND], rpm_i=0;
int fac_ring[FAC_RND], fac_i=0;


static void rput( int *ring, int size, int *i, int val ){
	ring[ *i ] = val;
	*i = (*i+1) % size;
}
static int ravg(  int *ring, int size ){
	int avg=0;
	for( int i=0; i<size; i++ ){
		avg += ring[i];
	}
	return avg / size;
}

static void printbar( int steps, int max, int val ){

	putchar( ' ' );

	if( val < 0 ) val = 0;
	if( val > max ) val = max;

	for( int i=0; i<steps; i++ ){
		putchar( "=."[ (float)val/max*steps < i ] );
	}
}

volatile static int rpm_avg, rpm_want, rpm_err, rpm_update;

void on_hall( int speed ){

	blinkRED();

	// todo inc rpm_update according to speed

	int rpm = speed > 0 ? 4*10000.0/speed : 0;
	int fac;

	rput( rpm_ring, RPM_RND, &rpm_i, rpm );
	rpm_avg = ravg( rpm_ring, RPM_RND );

	rpm_err = rpm_want - rpm_avg;

	// tune ---
	fac = _FAC;

	rpm_update = rpm_err/8 + 8;
	fac += rpm_update;
	if( fac > 1024 ) fac=1024;
	if( fac < 0 ) fac=0;

	_FAC = fac;
	// /tune ---
}

void Loop(){

	static bool output = true;

	int spd_avg = 0;

	int speed = hallSpeed();
	int rpm = speed > 0 ? 4*10000.0/speed : 0;

	//HAL_ADC_PollForConversion(&hadc, 1000);
	int spd = HAL_ADC_GetValue( &hadc );
	HAL_ADC_Start(&hadc);

	rput( spd_ring, SPD_RND, &spd_i, spd );
	spd_avg = ravg( spd_ring, SPD_RND );

	rpm_want = ADC2RPM( spd_avg );
	hallRotGuess( true );
	if( output ){
		printf( "\n% 5d % 5d % 5d : % 5d % 5d : % 5d % 5d % 5d ", 
				speed, rpm, rpm_avg, spd, spd_avg, rpm_err, rpm_update, _FAC );

		printbar( 100, 4000, rpm_avg );
		printbar( 20, 200, abs( rpm_err ) );
		printbar( 16, 1024, _FAC );
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
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			_FAC = (c-'0')*128;;
			goto ppow;
		case '!':
			_FAC = 1024;
			goto ppow;
		case '+':
			_FAC+=16;
			goto ppow;
		case '-':
			_FAC-=16;
ppow:		printf( "Fac: %d\n", _FAC );
			break;
		default:
			putchar( c );
	}

	putchar( '.' );
	hallLoop();
	
	// make sure it gets called at least every 6.5535 seconds
	utUpdate();

	HAL_Delay( 10 );
}
