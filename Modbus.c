	#include "Modbus.h"
   
	//extern uint8_t modbus_rx_buffer[];
	extern uint16_t holdingRegisters[HOLDING_REG_SIZE];
  extern UART_HandleTypeDef huart2;
  //	extern UART_HandleTypeDef huart4;
	
 void ModbusRTU_Process(uint8_t *frame, uint16_t length)
	{
		if (length < 4) return;
		// Ki?m tra dúng d?a ch? thi?t b?
		if (frame[0] != SLAVE_ID) return;

		

		switch (frame[1])
		{
		case 0x03:
				HandleFunction03_ReadHoldingRegisters(frame, length);
				break;

		case 0x06:
				HandleFunction06_WriteSingleRegister(frame, length);
				break;

		case 0x10:
				HandleFunction16_WriteMultipleRegisters(frame, length);
				break;

		default:
				// Unsupported function
				break;
		}
	}
	void HandleFunction03_ReadHoldingRegisters(uint8_t *frame, uint16_t length)
	{
		if (length < 8) return;

		uint16_t startAddr = (frame[2] << 8) | frame[3];
		uint16_t numRegs   = (frame[4] << 8) | frame[5];

		if ((startAddr + numRegs) > HOLDING_REG_SIZE) return;

		uint8_t txFrame[256];
		txFrame[0] = SLAVE_ID;
		txFrame[1] = 0x03;
		txFrame[2] = numRegs * 2;  // Byte count

		// Ghi d? li?u t? thanh ghi vào frame
		for (int i = 0; i < numRegs; i++) {
				txFrame[3 + i * 2]     = holdingRegisters[startAddr + i] >> 8;      // High byte
				txFrame[4 + i * 2]     = holdingRegisters[startAddr + i] & 0xFF;    // Low byte
		}

		// Tính CRC d?a trên txFrame
		uint16_t txCrc = ModRTU_CRC(txFrame, 3 + numRegs * 2);
		txFrame[3 + numRegs * 2] = txCrc & 0xFF;
		txFrame[4 + numRegs * 2] = txCrc >> 8;

		// G?i frame ph?n h?i
		HAL_UART_Transmit(&huart2, txFrame, 3 + numRegs * 2 + 2, HAL_MAX_DELAY);
//		printf("Du lieu modbus: ");
//		for (int i = 0; i < 3 + numRegs * 2 + 2; i++) {
//				printf("%02X ", txFrame[i]);
//		}
//		printf("\r\n");
//		printf("Gui func 03 \r\n");
	}

	void HandleFunction06_WriteSingleRegister(uint8_t *frame, uint16_t length)
	{
		if (length < 8) return;

		uint16_t regAddr  = (frame[2] << 8) | frame[3];
		uint16_t regValue = (frame[4] << 8) | frame[5];

		

		if (regAddr >= HOLDING_REG_SIZE) return;
		holdingRegisters[regAddr] = regValue;

		
		uint8_t txFrame[8];
		txFrame[0] = SLAVE_ID;
		txFrame[1] = 0x06;
		txFrame[2] = frame[2]; // StartAddr Hi
		txFrame[3] = frame[3]; // StartAddr Lo
		txFrame[4] = frame[4]; // Quantity Hi
		txFrame[5] = frame[5]; // Quantity Lo
		
		uint16_t crc = ModRTU_CRC(txFrame, 6);
		txFrame[6] = crc & 0xFF;
		txFrame[7] = crc >> 8;

		// Echo l?i toàn b? khung d? li?u nhu chu?n Modbus yêu c?u
		HAL_UART_Transmit(&huart2, txFrame, 8, HAL_MAX_DELAY);
		printf("Du lieu modbus: %s \r\n", txFrame);
		printf("Gui func 06 \r\n");
	}
	void HandleFunction16_WriteMultipleRegisters(uint8_t *frame, uint16_t length)
	{
		//if (length < 9) return; // T?i thi?u frame c?n d?: Addr + Func + Addr + Qty + ByteCnt + CRC

		uint16_t startAddr = (frame[2] << 8) | frame[3];
		uint16_t quantity  = (frame[4] << 8) | frame[5];
		uint8_t  byteCount = frame[6];

		if (quantity == 0 || quantity > HOLDING_REG_SIZE) return;
		if ((startAddr + quantity) > HOLDING_REG_SIZE) return;
		if (byteCount != quantity * 2) return;

		// Ki?m tra toàn b? d? dài frame h?p l? (7 byte dau + data + 2 CRC)
		if (length < (7 + byteCount + 2)) return;

		// Ghi d? li?u vào thanh ghi
		for (uint16_t i = 0; i < quantity; i++) {
				uint16_t value = (frame[7 + i * 2] << 8) | frame[8 + i * 2];
				holdingRegisters[startAddr + i] = value;
		}

		// Ph?n h?i: SlaveID + Function + StartAddr + Quantity + CRC
		uint8_t txFrame[8];
		txFrame[0] = SLAVE_ID;
		txFrame[1] = 0x10;
		txFrame[2] = frame[2]; // StartAddr Hi
		txFrame[3] = frame[3]; // StartAddr Lo
		txFrame[4] = frame[4]; // Quantity Hi
		txFrame[5] = frame[5]; // Quantity Lo

		// ? Tính CRC dúng cho ph?n ph?n h?i
		uint16_t crc = ModRTU_CRC(txFrame, 6);
		txFrame[6] = crc & 0xFF;
		txFrame[7] = crc >> 8;

		HAL_UART_Transmit(&huart2, txFrame, 8, HAL_MAX_DELAY);
		printf("Du lieu modbus: %s \r\n", txFrame);
		printf("Gui func 10 \r\n");
	}

	uint16_t ModRTU_CRC(uint8_t *buf, int len) {
		uint16_t crc = 0xFFFF;
		for (int pos = 0; pos < len; pos++) {
				crc ^= (uint16_t)buf[pos];
				for (int i = 8; i != 0; i--) {
						if ((crc & 0x0001) != 0) {
								crc >>= 1;
								crc ^= 0xA001;
						} else
								crc >>= 1;
				}
		}
		return crc;
	}

