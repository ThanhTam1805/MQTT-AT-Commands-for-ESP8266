
#ifndef __MODBUS_H__
#define __MODBUS_H__
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
//#include "stm32f1xx_hal.h"

#define SLAVE_ID 1
#define HOLDING_REG_SIZE 100

void ModbusRTU_Process(uint8_t *frame, uint16_t length);
void HandleFunction16_WriteMultipleRegisters(uint8_t *frame, uint16_t length);
void HandleFunction03_ReadHoldingRegisters(uint8_t *frame, uint16_t length);
void HandleFunction06_WriteSingleRegister(uint8_t *frame, uint16_t length);
uint16_t ModRTU_CRC(uint8_t *buf, int len);

#endif