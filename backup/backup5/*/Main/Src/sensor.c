/*
 * sensor.c
 */

#include "header_init.h"




#define SWAP(a, b)			({ a ^= b; b ^= a; a ^= b; })


volatile uint8_t	sensorRawVals[8] = { 0, };

volatile uint8_t	sensorNormVals[8] = { 0, };
volatile uint8_t	normalizeCoef[8] = { 1, };
volatile uint8_t	whiteMaxs[8] = { 255, };
volatile uint8_t	blackMaxs[8] = { 0, };

volatile uint8_t	state = 0x00;
volatile uint8_t	threshold = THRESHOLD_RESET_VAL;






void Sensor_Start() {
	LL_ADC_Enable(ADC1);

	//sMux를 사용하여 0번 IR LED 및 수광 센서 선및 선택한 IR LED 켜기
	GPIOC->ODR = (GPIOC->ODR & ~0x07) | 0x08;   // 0000 {1}(000) == 0000 {LED}(0번)


	Custom_Delay_ms(10); // ADC를 켜고 난 후, ADC 변환을 하기 전 내부 아날로그 안정화 작업을 위해 딜레이를 준다.

	LL_TIM_EnableCounter(TIM5); // TIM5의 타이머 카운터가 증가하도록 설정한다.
	LL_TIM_EnableIT_UPDATE(TIM5); // TIM5의 인터럽트가 동작하도록 설정한다. 인터럽트가 발생하면 Core/Src/stm32f4xx_it.c 파일 내부에 있는 인터럽트 핸들러 함수가 호출된다.
}




void Sensor_Stop() {
	LL_ADC_Disable(ADC1);
	LL_TIM_DisableCounter(TIM5);
	LL_TIM_DisableIT_UPDATE(TIM5);
}






__STATIC_INLINE uint16_t Sensor_ADC_Read() {
	__disable_irq();
	LL_ADC_ClearFlag_EOCS(ADC1);
	LL_ADC_REG_StartConversionSWStart(ADC1);
	while (!LL_ADC_IsActiveFlag_EOCS(ADC1));
	uint16_t adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
	LL_ADC_ClearFlag_EOCS(ADC1);
	__enable_irq();
	return adcValue;
}





__STATIC_INLINE void Make_Sensor_Norm_Vals(uint8_t idx) {
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






__STATIC_INLINE void Make_Sensor_State(uint8_t idx) {
	// sensor state 계산
	state = ( state & ~(0x01 << idx) ) | ( (sensorNormVals[idx] > threshold) << idx );
}





void Sensor_TIM5_IRQ() {
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



/*
// 0 123 012 3
void Sensor_TIM5_IRQ() {
	static uint8_t	midian[3] = { 0, };

	//sMux를 사용하여 IR LED 및 수광 센서 선택 및 선택한 IR LED 켜고 읽기
	// 0000 {1}(XXX) == 0000 {LED}(i)
	GPIOC->ODR = (GPIOC->ODR & ~0x07) | sensorReadIdx | 0x08;

	midian[0] = Sensor_ADC_Read() >> 4;
	midian[1] = Sensor_ADC_Read() >> 4;
	midian[2] = Sensor_ADC_Read() >> 4;

	// 선택한 IR LED 끄기
	GPIOC->ODR &= ~0x08;

	Make_Sensor_Norm_Vals(sensorReadIdx);
	Make_Sensor_State(sensorReadIdx);



	// 중앙값을 sensorRawVals[i]에 저장
	if (midian[0] > midian[1]) {
		SWAP(midian[0], midian[1]);
	}
	if (midian[1] > midian[2]) {
		SWAP(midian[1], midian[2]);
	}
	sensorRawVals[sensorReadIdx] = midian[1];


	//sMux를 사용하여 IR LED 및 수광 센서 선택 및 선택한 IR LED 켜고 읽기
	// 0000 {1}(XXX) == 0000 {LED}(i)
	GPIOC->ODR = (GPIOC->ODR & ~0x07) | (7 - sensorReadIdx) | 0x08;

	midian[0] = Sensor_ADC_Read() >> 4;
	midian[1] = Sensor_ADC_Read() >> 4;
	midian[2] = Sensor_ADC_Read() >> 4;

	// 선택한 IR LED 끄기
	GPIOC->ODR &= ~0x08;

	// 중앙값을 sensorRawVals[i]에 저장
	if (midian[0] > midian[1]) {
		SWAP(midian[0], midian[1]);
	}
	if (midian[1] > midian[2]) {
		SWAP(midian[1], midian[2]);
	}
	sensorRawVals[7 - sensorReadIdx] = midian[1];


	Make_Sensor_Norm_Vals(7 - sensorReadIdx);
	Make_Sensor_State(7 - sensorReadIdx);

	sensorReadIdx = (sensorReadIdx + 1) & 0x03;
}
*/







void Sensor_Calibration() {
	uint8_t	tmp = 0;

	for (int i = 0; i < 8; i++) {
		whiteMaxs[i] = 0;
		blackMaxs[i] = 0;
	}

	Sensor_Start();
	Custom_OLED_Clear();

	// Get whiteMax
	Custom_OLED_Printf("next White Max");
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		for (uint8_t i = 0; i < 8; i++) {
			if (whiteMaxs[i] < (tmp = sensorRawVals[i])) {
				whiteMaxs[i] = tmp;
			}
		}
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			whiteMaxs[0], whiteMaxs[1], whiteMaxs[2], whiteMaxs[3], \
			whiteMaxs[4], whiteMaxs[5], whiteMaxs[6], whiteMaxs[7]);
	}

	// Get blackMax
	Custom_OLED_Clear();
	Custom_OLED_Printf("next Black Max");
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		for (uint8_t i = 0; i < 8; i++) {
			if (blackMaxs[i] < (tmp = sensorRawVals[i])) {
				blackMaxs[i] = tmp;
			}
		}
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			blackMaxs[0], blackMaxs[1], blackMaxs[2], blackMaxs[3], \
			blackMaxs[4], blackMaxs[5], blackMaxs[6], blackMaxs[7]);
	}

	// Calculate ADC coefficients
	for (uint8_t i = 0; i < 8; i++) {
		normalizeCoef[i] = whiteMaxs[i] - blackMaxs[i];
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}
