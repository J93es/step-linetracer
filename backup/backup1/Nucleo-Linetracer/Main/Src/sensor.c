/*
 * sensor.c
 */

#include "header_init.h"




volatile uint8_t	sensorRawVals[8] = { 0, };

volatile uint8_t	sensorNormVals[8] = { 0, };
volatile uint8_t	normalizeCoef[8] = { 1, };
volatile uint8_t	whiteMaxs[8] = { 255, };
volatile uint8_t	blackMaxs[8] = { 0, };

volatile uint8_t	state = 0x00;
volatile uint32_t	threshold_init = THRESHOLD_INIT;
volatile uint8_t	threshold;






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
