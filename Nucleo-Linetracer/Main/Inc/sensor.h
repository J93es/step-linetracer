/*
 * sensor.h
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_


#include "main.h"
#include "drive_def_var.h"


#define SWAP(a, b)				({ a ^= b; b ^= a; a ^= b; })


#define	THRESHOLD_MAX 			125
#define	THRESHOLD_MIN			25
#define	THRESHOLD_CHANGE_VAL	5
#define	THRESHOLD_INIT			70




extern volatile uint8_t		sensorRawVals[8];

extern volatile uint8_t		sensorNormVals[8];
extern volatile uint8_t		normalizeCoef[8];
extern volatile uint8_t		whiteMaxs[8];
extern volatile uint8_t		blackMaxs[8];

extern volatile uint8_t		state;
extern volatile uint32_t	threshold;

extern volatile uint8_t		sensorReadIdx;
extern volatile uint8_t		sensorReadIdxTable[8];




void	Sensor_Start();
void	Sensor_Stop();
void	Sensor_Calibration();





__STATIC_INLINE uint16_t	Sensor_ADC_Read() {
	uint16_t	adcValue = 0;
	__disable_irq();
	LL_ADC_ClearFlag_EOCS(ADC1);
	LL_ADC_REG_StartConversionSWStart(ADC1);
	while (!LL_ADC_IsActiveFlag_EOCS(ADC1));
	adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
	LL_ADC_ClearFlag_EOCS(ADC1);
	__enable_irq();
	return adcValue;
}


// rawValue 계산
__STATIC_INLINE void	Make_Sensor_Raw_Vals(uint8_t idx) {
	uint8_t	midian[3];

	GPIOC->ODR = (GPIOC->ODR & ~0x07) | (idx) | 0x08;
	// ADC 읽기
	midian[0] = Sensor_ADC_Read() >> 4;
	midian[1] = Sensor_ADC_Read() >> 4;
	midian[2] = Sensor_ADC_Read() >> 4;

	// 선택한 IR LED 끄기
	GPIOC->ODR &= ~0x08;


	//sMux를 사용하여 다음 IR LED 및 수광 센서 선택 및 선택한 IR LED 켜기
	// 0000 {1}(XXX) == 0000 {LED}(다음 번 i)
//	GPIOC->ODR = (GPIOC->ODR & ~0x07) | ( (sensorReadIdx + 1) & 0x07 ) | 0x08;


	// 중앙값을 sensorRawVals[i]에 저장
	if (midian[0] > midian[1]) {
		SWAP(midian[0], midian[1]);
	}
	if (midian[1] > midian[2]) {
		SWAP(midian[1], midian[2]);
	}
	sensorRawVals[idx] =  midian[1];
}



// normalized value 계산
__STATIC_INLINE void	Make_Sensor_Norm_Vals(uint8_t idx) {


	if (sensorRawVals[idx] < blackMaxs[idx])
		sensorNormVals[idx] = 0;
	else if (sensorRawVals[idx] > whiteMaxs[idx])
		sensorNormVals[idx] = 255;
	else
		sensorNormVals[idx] = (255 * (sensorRawVals[idx] - blackMaxs[idx]) / normalizeCoef[idx]);


/*
	sensorNormVals[idx] = ( (255 * (sensorRawVals[idx] - blackMaxs[idx]) / normalizeCoef[idx]) \
		& ( (sensorRawVals[idx] < blackMaxs[idx]) - 0x01 )  ) \
		| ( (sensorRawVals[idx] < whiteMaxs[idx]) - 0x01 );
*/


}





// sensor state 계산
__STATIC_INLINE void	Make_Sensor_State(uint8_t idx) {

	//state = ( state & ~(0x01 << idx) ) | ( (sensorNormVals[idx] > threshold) << idx );
	if (sensorNormVals[idx] > threshold) {
		state |= 0x01 << (7 - idx);
	}
	else {
		state &= ~(0x01 << (7 - idx));
	}
}






__STATIC_INLINE void	Sensor_TIM5_IRQ() {

	Make_Sensor_Raw_Vals(sensorReadIdxTable[sensorReadIdx]);

	Make_Sensor_Norm_Vals(sensorReadIdxTable[sensorReadIdx]);

	Make_Sensor_State(sensorReadIdxTable[sensorReadIdx]);

	sensorReadIdx = (sensorReadIdx + 1) & 0x07;
}






#endif /* INC_SENSOR_H_ */
