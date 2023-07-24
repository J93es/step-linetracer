/*
 * sensor.h
 */

void Sensor_TIM5_IRQ();

void Sensor_Start();
void Sensor_Stop();

void Sensor_Calibration();

void Sensor_Test_Raw();
void Sensor_Test_Normalized();
void Sensor_Test_State();
