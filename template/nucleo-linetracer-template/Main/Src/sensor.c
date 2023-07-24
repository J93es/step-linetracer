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
	uint16_t adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
	LL_ADC_ClearFlag_EOCS(ADC1);
	__enable_irq();
	return adcValue;
}

void Sensor_TIM5_IRQ() {
	static uint32_t i = 0; // 현재 값을 읽을 센서 인덱스

	// Mux를 사용하여 IR LED 및 수광 센서 선택
	// --- 코드 작성 ---

	// 선택한 IR LED 켜기
	// --- 코드 작성 ---

	// ADC 읽기
	// --- 코드 작성 ---

	// 선택한 IR LED 끄기
	// --- 코드 작성 ---

	// normalized value 계산
	// --- 코드 작성 ---

	// sensor state 계산
	// --- 코드 작성 ---

	// 다른 곳에서 쓸 수 있게 전역 배열에 할당
	// --- 코드 작성 ---

	// 인덱스 증가
	i = (i + 1) & 0x07;
}

void Sensor_Test_Raw() {
	Sensor_Start();
	// 센서의 Raw 값을 디스플레이에 출력해 확인하기
	// --- 코드 작성 ---
	Sensor_Stop();
}

void Sensor_Calibration() {
	Sensor_Start();

	// Get whiteMax
	// --- 코드 작성 ---

	// Get blackMax
	// --- 코드 작성 ---

	// Calculate ADC coefficients
	// --- 코드 작성 ---

	Sensor_Stop();
}

void Sensor_Test_Normalized() {
	Sensor_Start();
	// 센서의 Normalized 값을 디스플레이에 출력해 확인하기
	// --- 코드 작성 ---
	Sensor_Stop();
}

void Sensor_Test_State() {
	Sensor_Start();
	// 센서의 State 값을 디스플레이에 출력해 확인하기
	// --- 코드 작성 ---
	Sensor_Stop();
}

