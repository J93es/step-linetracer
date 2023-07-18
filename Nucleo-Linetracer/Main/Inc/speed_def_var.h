/*
 * speed_def_var.h
 */


#ifndef INC_SPEED_DEF_VAR_H_
#define INC_SPEED_DEF_VAR_H_


#include <stdint.h>
#include <stdbool.h>



#define DESISION_END_MARK			0
#define DESISION_CROSS				1
#define DESISION_STRAIGHT			2
#define DESISION_CURVE_R			3
#define DESISION_CURVE_L			4
#define DESISION_MEASURE_STRAIGHT	5
#define DESISION_MEASURE_CURVE_R	6
#define DESISION_MEASURE_CURVE_L	7



#define ACCELE_INIT					4.f
#define TARGET_SPEED_INIT			2.f
#define MAX_SPEED_INIT				2.4f
#define MIN_SPEED_INIT				0.1f
#define CURVE_DECELE_INIT			-0.5f


#define TIRE_RADIUS	0.025f					// m
#define	SPEED_COEF	15707.f * TIRE_RADIUS
#define POSITION_COEF_INIT			0.00004f
/*
 * (2 * l(m) * 3.141592) / (t(s) * 400) = v(m/s) * (arr+1)
 *
 * t =  1 / 1Mhz = 1 / 1,000,000 = 타이머 주
 * 1 / (t * 400) = 2,500
 *
 * l(m) = 타이어 반지름
 * 2 * l * 3.14
 *
 * v * (arr + 1) = SPEED_COEF
 */




extern volatile int32_t		positionVal;
extern volatile float		positionCoef;

extern volatile float		targetSpeed;
extern volatile float		currentSpeed;
extern volatile float		minSpeed;
extern volatile float		maxSpeed;
extern volatile float		accele;
extern volatile float		curveDecele;

extern volatile float		targetSpeed_init;
extern volatile float		currentSpeed_init;
extern volatile float		minSpeed_init;
extern volatile float		maxSpeed_init;
extern volatile float		accele_init;
extern volatile float		curveDecele_init;



#endif //INC_SPEED_DEF_VAR_H_
