/*
 * sensor.h
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_


#include "main.h"


#define SWAP(a, b)				({ a ^= b; b ^= a; a ^= b; })


#define	THRESHOLD_MAX 			125
#define	THRESHOLD_MIN			25
#define	THRESHOLD_CHANGE_VAL	5
#define	THRESHOLD_INIT			50




extern volatile uint8_t	sensorRawVals[8];
extern volatile uint8_t	sensorNormVals[8];
extern volatile uint8_t	state;

extern volatile uint8_t	threshold_init;
extern volatile uint8_t	threshold;

extern volatile uint8_t	normalizeCoef[8];
extern volatile uint8_t	whiteMaxs[8];
extern volatile uint8_t	blackMaxs[8];





void	Sensor_Start();
void	Sensor_Stop();
void	Sensor_Calibration();





__STATIC_INLINE uint16_t	Sensor_ADC_Read() {
	__disable_irq();
	LL_ADC_ClearFlag_EOCS(ADC1);
	LL_ADC_REG_StartConversionSWStart(ADC1);
	while (!LL_ADC_IsActiveFlag_EOCS(ADC1));
	uint16_t adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
	LL_ADC_ClearFlag_EOCS(ADC1);
	__enable_irq();
	return adcValue;
}





__STATIC_INLINE void		Make_Sensor_Norm_Vals(uint8_t idx) {
	// normalized value 계산
	/*
		if (sensorRawVals[i] < blackMaxs[i])
			sensorNormVals[i] = 0;
		else if (sensorRawVals[i] > whiteMaxs[i])
			sensorNormVals[i] = 255;
		else
			sensorNormVals[i] = (255 * (sensorRawVals[i] - blackMaxs[i]) / (whiteMaxs[i] - blackMax[i]));
	 */
	sensorNormVals[idx] = ( (255 * (sensorRawVals[idx] - blackMaxs[idx]) / normalizeCoef[idx]) \
		& ((sensorRawVals[idx] < blackMaxs[idx]) - 0x01) ) \
		| ((sensorRawVals[idx] < whiteMaxs[idx]) - 0x01);
}






__STATIC_INLINE void		Make_Sensor_State(uint8_t idx) {
	// sensor state 계산
	state = ( state & ~(0x01 << idx) ) | ( (sensorNormVals[idx] > threshold) << idx );
}





__STATIC_INLINE void		Sensor_TIM5_IRQ() {
	static uint8_t	sensorReadIdx = 0;
	static uint8_t	midian[3] = { 0, };


	// ADC 읽기
	midian[0] = Sensor_ADC_Read() >> 4;
	midian[1] = Sensor_ADC_Read() >> 4;
	midian[2] = Sensor_ADC_Read() >> 4;

	// 선택한 IR LED 끄기
	GPIOC->ODR &= ~0x08;


	//sMux를 사용하여 다음 IR LED 및 수광 센서 선택 및 선택한 IR LED 켜기
	// 0000 {1}(XXX) == 0000 {LED}(다음 번 i)
	GPIOC->ODR = (GPIOC->ODR & ~0x07) | ( (sensorReadIdx + 1) & 0x07 ) | 0x08;


	// 중앙값을 sensorRawVals[i]에 저장
	if (midian[0] > midian[1]) {
		SWAP(midian[0], midian[1]);
	}
	if (midian[1] > midian[2]) {
		SWAP(midian[1], midian[2]);
	}
	sensorRawVals[sensorReadIdx] =  midian[1];


	Make_Sensor_Norm_Vals(sensorReadIdx);

	Make_Sensor_State(sensorReadIdx);


	sensorReadIdx = (sensorReadIdx + 1) & 0x07;
}






#endif /* INC_SENSOR_H_ */
