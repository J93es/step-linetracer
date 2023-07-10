/*
 * sensor.c
 */

#include "main.h"
#include "sensor.h"
#include "custom_delay.h"
#include "custom_gpio.h"
#include "custom_oled.h"
#include "custom_switch.h"
#include "custom_filesystem.h"
#include "custom_exception.h"





uint8_t		sensorRawVals[8] = { 0, };

uint8_t		sensorNormVals[8] = { 0, };
uint8_t		normalizeCoef[8] = { 0, };

uint8_t		whiteMaxs[8] = { 0, };
uint8_t		blackMaxs[8] = { 0, };

uint32_t	threshold = THRESHOLD_RESET_VAL;
uint32_t	state = 0x00;





void Sensor_Start() {
	LL_ADC_Enable(ADC1);
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
	uint32_t adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
	LL_ADC_ClearFlag_EOCS(ADC1);
	__enable_irq();
	return adcValue;
}



void Sensor_TIM5_IRQ() {
	static uint32_t	i = 0; // 현재 값을 읽을 센서 인덱스

	//sMux를 사용하여 IR LED 및 수광 센서 선택 및 선택한 IR LED 켜기
	GPIOC->ODR = (GPIOC->ODR & ~0x07) | i | 0x08;   // 0000 {1}(XXX) == 0000 {LED}(i)
	/*
	GPIOC->ODR &= ~0x07;
	GPIOC->ODR |= i;
	*/

	// 선택한 IR LED 켜기
	/*
	GPIOC->ODR |= 0x08;
	*/


	// ADC 읽기
	sensorRawVals[i] = Sensor_ADC_Read() >> 4;

	// 선택한 IR LED 끄기
	GPIOC->ODR &= ~0x08;

	// normalized value 계산
	/*if (sensorRawVals[i] < blackMaxs[i]) {
		sensorNormVals[i] = 0;
	}
	else if (sensorRawVals[i] > whiteMaxs[i]) {
		sensorNormVals[i] = 255;
	}
	else {
		sensorNormVals[i] = 255 * (sensorRawVals[i] - blackMaxs[i]) / normalizeCoef[i];
	}*/
	sensorNormVals[i] = ( 255 * (sensorRawVals[i] - blackMaxs[i]) / normalizeCoef[i] ) \
			& ( ((sensorRawVals[i] < blackMaxs[i]) - 0x01) | ((sensorRawVals[i] < whiteMaxs[i]) - 0x01) );

	// sensor state 계산
	state = ( state & ~(0x01 << i) ) | ( (sensorNormVals[i] > threshold) << i );
	//state &= ~(0x01 << i);
	//state |= (sensorNormVals[i] > threshold) << i;

	// 인덱스 증가
	i = (i + 1) & 0x07;
}



void Sensor_Calibration() {
	uint32_t	i = 0;
	uint32_t	tmp = 0;

	Sensor_Start();
	Custom_OLED_Clear();

	// Get whiteMax
	Custom_OLED_Printf("next White Max");
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		for (i = 0; i < 8; i++) {
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
		for (i = 0; i < 8; i++) {
			if (blackMaxs[i] < (tmp = sensorRawVals[i])) {
				blackMaxs[i] = tmp;
			}
		}
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			blackMaxs[0], blackMaxs[1], blackMaxs[2], blackMaxs[3], \
			blackMaxs[4], blackMaxs[5], blackMaxs[6], blackMaxs[7]);
	}

	// Calculate ADC coefficients
	for (i = 0; i < 8; i++) {
		normalizeCoef[i] = whiteMaxs[i] - blackMaxs[i];
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}



void Sensor_Test_Raw() {
	Sensor_Start();
	Custom_OLED_Clear();

	// 센서의 Raw 값을 디스플레이에 출력해 확인하기
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			sensorRawVals[0], sensorRawVals[1], sensorRawVals[2], sensorRawVals[3], \
			sensorRawVals[4], sensorRawVals[5], sensorRawVals[6], sensorRawVals[7]);
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}



void Sensor_Test_Normalized() {
	Sensor_Start();
	Custom_OLED_Clear();

	// 센서의 Normalized 값을 디스플레이에 출력해 확인하기
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			sensorNormVals[0], sensorNormVals[1], sensorNormVals[2], sensorNormVals[3], \
			sensorNormVals[4], sensorNormVals[5], sensorNormVals[6], sensorNormVals[7]);
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}



void Sensor_Test_State() {
	uint32_t sw = 0;

	Sensor_Start();
	Custom_OLED_Clear();

	// 센서의 State 값을 디스플레이에 출력해 확인하기
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			(state >> 0) & 1, (state >> 1) & 1, (state >> 2) & 1, (state >> 3) & 1, \
			(state >> 4) & 1, (state >> 5) & 1, (state >> 6) & 1, (state >> 7) & 1);
		Custom_OLED_Printf("/1threshold: %d", threshold);


		if (sw == CUSTOM_SW_1) {
			if (threshold > THRESHOLD_MIN) {
				threshold -= THRESHOLD_CHANGE_VAL;
			}
		}
		else if (sw == CUSTOM_SW_2) {
			if (threshold < THRESHOLD_MAX) {
				threshold += THRESHOLD_CHANGE_VAL;
			}
		}
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}
