#include "ring.h"

#include <stdbool.h>

static char _thering[ RING_SIZE ];
static char *_start = _thering,
            *_end = _thering;


static inline char* inc( char *cp ){
	cp++;
	if( cp >= _thering+sizeof(_thering) ){
		cp = _thering;
	}
	return cp;
}
static inline char* dec( char *cp ){
	cp--;
	if( cp < 0 ){
		cp = _thering + RING_SIZE -1;
	}
	return cp;
}

static inline bool _ring_full(){
	return inc( _end )==_start;
}
static inline bool _ring_empty(){
	return _start==_end;
}

void ring_push( char c ) {

	if( _ring_full() ) return;

	*_end = c;
	_end = inc( _end );
}

int ring_pop(){

	if( _ring_empty() ) return -1;

	char res = *_start;
	inc( _start );
	return res;
}
