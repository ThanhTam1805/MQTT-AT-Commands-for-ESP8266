#include "AT_ESP8266.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
uint32_t last = 0;
extern char rxBuffer_AT_Reponse[500];
extern char Data_AT_Reponse[500];
extern char safeBuffer[500];

extern volatile uint16_t rxDataSize;
extern volatile bool flag_ready_for_copy;

extern bool flag_MQTT_Send_AT_Sub_Commands;
extern bool flag_times_repones;


void Send_AT_Commands_Setting(char *AT_Commands, char *DataResponse, uint32_t timeSend) {
    char DataSendAT[200];
    const uint8_t maxRetries = 3;
    uint8_t retryCount = 0;
    
    while (retryCount < maxRetries) {
        // Clear buffers
        memset(DataSendAT, 0, sizeof(DataSendAT));
        memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));			
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)rxBuffer_AT_Reponse, sizeof(rxBuffer_AT_Reponse));
			  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
			  //printf("Receive:  %s\r\n", (char*)rxBuffer_AT_Reponse);		
        // Format command
        //snprintf(DataSendAT, sizeof(DataSendAT), "%s\r\n", AT_Commands);
        sprintf(DataSendAT, "%s", AT_Commands);
        // Send command
        HAL_UART_Transmit(&huart1, (uint8_t *)DataSendAT, strlen(DataSendAT), 1000);
        //printf("Send: %s\r\n", DataSendAT);  // Added \r\n for console formatting
        

        // Response monitoring
        uint32_t startTime = HAL_GetTick();
        bool responseReceived = false;
        while ((HAL_GetTick() - startTime) < timeSend) {
				 if (flag_MQTT_Send_AT_Sub_Commands == false){
            if (strstr((char*)rxBuffer_AT_Reponse, DataResponse)) {
               //printf("Success: %s\r\n", DataResponse);
							  flag_times_repones = false;
                responseReceived = true;
                break;
            }
					}
					else {
						if (strstr((char*)rxBuffer_AT_Reponse, DataResponse)) {
                //printf("Success: %s\r\n", DataResponse);
							  //flag_MQTT_Send_AT_Sub_Commands = true;
							  //flag_times_repones = false;
                responseReceived = true;
                break;
            }
					}
        }

        if (responseReceived) {
            return;  // Success exit
        }

        printf("Timeout. Retries left: %d\r\n", (maxRetries - retryCount - 1));
        retryCount++;
    }
    
    printf("Failed after %d attempts\r\n", maxRetries);
}

void ESP_AT_SETTING(void){
	//=====================RESET ESP========================================
	Send_AT_Commands_Setting("AT+RST\r\n", "OK\r\n", 1000);
	HAL_Delay(2000);
	//=====================ENABLE ESP AT MODE===============================
	Send_AT_Commands_Setting("AT\r\n", "OK\r\n", 1000);
	HAL_Delay(2000);
	//=====================Configure AT Commands Echoing====================
	Send_AT_Commands_Setting("ATE0\r\n", "OK\r\n" , 2000);
	HAL_Delay(2000);
	//=====================AT+CWMODE_CUR�Sets the Current Wi-Fi mode; Configuration Not Saved in the Flash
	Send_AT_Commands_Setting("AT+CWMODE=1\r\n", "OK\r\n", 2000);
	HAL_Delay(2000);
	//=====================AT+CWJAP_CUR�Connects to an AP; Configuration Not Saved in the Flash
	Send_AT_Commands_Setting("AT+CWJAP=\".......\",\".......\"\r\n", "OK\r\n", 10000);
  HAL_Delay(2000);
	//====================AT+CIPSERVER�Deletes/==============================
	Send_AT_Commands_Setting("AT+CIPMUX=0\r\n", "OK\r\n", 2000);
	HAL_Delay(2000);

}

void MQTT_User_Config_AT_Commands(char* server,char* port, char* username, char* password){
	
	char client_id[100] = "ESP8266";
	char MQTT_User_Config[200];
	char MQTT_Connect[200];
	// Xoa data truoc khi cau hinh 
	memset(MQTT_User_Config, 0, sizeof(MQTT_User_Config)); 
	memset(MQTT_Connect, 0, sizeof(MQTT_Connect));

  //=======================================================================================================================//
	// Noi chuoi theo dung dinh dang sau: 
	// AT+MQTTUSERCFG=0,1,"ESP8266","username","password",0,0,""$0D$0A => OK
	// Su dung ham strcat de noi chuoi=
  //=======================================================================================================================//
	
	
	//========================================================================================================================//
	// Thuc hien noi ham MQTTUSERCFG
	// AT+MQTTUSERCFG=0,1,\"ESP8266\",\"username\",\"password\",0,0,\"\""$0D$0A => OK
	strcat(MQTT_User_Config, "AT+MQTTUSERCFG=0,1,\""); // Tuong duong AT+MQTTUSERCFG=0,1,"
	strcat(MQTT_User_Config, client_id);               // Tuong duong ESP8266
	strcat(MQTT_User_Config,"\",\"");                  // Tuong duong ","
	strcat(MQTT_User_Config, username);                // Tuong duong username
	strcat(MQTT_User_Config,"\",\"");                  // Tuong duong ","
	strcat(MQTT_User_Config, password);                // Tuong duong password
	strcat(MQTT_User_Config,"\",0,0,");                // Tuong duong ,0,0,
	strcat(MQTT_User_Config,"\"\r\n\"");               // Tuong duong ""
	
 //printf("MQTTUSERCFG: %s\n", MQTT_User_Config);
	//===========================================================================================================================//
	
	
	// Thuc hien ham MQTT_Connect
	//AT+MQTTCONN=0,"ngoinhaiot.com",1111,1$0D$0A
	strcat(MQTT_Connect, "AT+MQTTCONN=0,\"");
	strcat(MQTT_Connect, server);
	strcat(MQTT_Connect, "\",");
	strcat(MQTT_Connect, port);  
	strcat(MQTT_Connect, ",1\r\n");
	//printf("MQTTCONN: %s\n",MQTT_Connect);
	//============================================================================================================================//
	
	//char* MQTTCNN="AT+MQTTCONN=0,\"8b21d658efc94018b9d31b21602078c3.s1.eu.hivemq.cloud\",8883,1\r\n";

	//============================================================================================================================//
	// Gui lenh config
	Send_AT_Commands_Setting(MQTT_User_Config, "OK\r\n", 2000);
	//memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));	 // Xoa du lieu cu truoc khi cap nhat du lieu moi	
	HAL_Delay(2000);
	// Gui lenh ket noi broker
	Send_AT_Commands_Setting(MQTT_Connect, "OK\r\n", 2000);
	//memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));	 // Xoa du lieu cu truoc khi cap nhat du lieu moi	
	HAL_Delay(2000);
	//============================================================================================================================//
}

void MQTT_Publish_Message(char* topic, char* message, char* lenght) {
	
	// Su dung cau truc AT+MQTTPUBRAW=<LinkID>,<"topic">,<length>,<qos>,<retain> 
	// de publish du lieu len broker
    char MQTT_Publish[200];

    // Xóa dữ liệu trước đó
    memset(MQTT_Publish, 0, sizeof(MQTT_Publish));

    // Format lệnh AT: AT+MQTTPUB=0,"topic","message",0,0\r\n
    strcat(MQTT_Publish, "AT+MQTTPUBRAW=0,\"");
    strcat(MQTT_Publish, topic);
    strcat(MQTT_Publish, "\",");
    strcat(MQTT_Publish, lenght);
    strcat(MQTT_Publish, ",0,1\r\n"); // Cau hinh de tu dong ket noi lai

    // Gửi lệnh
		if (flag_MQTT_Send_AT_Sub_Commands == true){
			Send_AT_Commands_Setting(MQTT_Publish, "OK", 2000);
			//memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));	 // Xoa du lieu cu truoc khi cap nhat du lieu moi		
			sendData_to_MQTT_Broker(message);
		}
			
			//memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));	 // Xoa du lieu cu truoc khi cap nhat du lieu moi	
	
//		char test[256] = "AT+MQTTPUB=0,\"thanhtam/guidulieu\",\"ND=120.00;DA=12.90;N1=1;N2=1;N3=1\",0,0\r\n";
//		Send_AT_Commands_Setting(test, "OK", 2000);



}

void MQTT_Subcribe_Message(char* topic){
	char MQTT_Subcribe[100];
	// Xóa dữ liệu trước đó
  memset(MQTT_Subcribe, 0, sizeof(MQTT_Subcribe));
	// Format lệnh AT: AT+MQTTSUB=0,"topic",0\r\n
	strcat(MQTT_Subcribe, "AT+MQTTSUB=0,\"");
	strcat(MQTT_Subcribe, topic);
	strcat(MQTT_Subcribe, "\",0\r\n");
	
	// Gửi lệnh
  Send_AT_Commands_Setting(MQTT_Subcribe, "OK", 2000);
	//memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));	 // Xoa du lieu cu truoc khi cap nhat du lieu moi	
	//printf("Data receive from MQTT boker: %s \n", Data_AT_Reponse);
	//flag_MQTT_Send_AT_Sub_Commands = true;
}
//bool flag_new_command_received = false;


void receiverData_From_MQTT(void) {
    flag_times_repones = true;
//    memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));			
//    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)rxBuffer_AT_Reponse, sizeof(rxBuffer_AT_Reponse));
   
    // So sánh nguyên chuỗi để kiểm tra đúng gói điều khiển từ người dùng
			if (strstr((char*)safeBuffer, "+MQTTSUBRECV:0,\"thanhtam/nhandulieu\"") && flag_ready_for_copy){
				memset(Data_AT_Reponse, 0, sizeof(Data_AT_Reponse));
        memcpy(Data_AT_Reponse, safeBuffer, rxDataSize);
        Data_AT_Reponse[rxDataSize] = '\0';

	}
}
void sendData_to_MQTT_Broker(char* message){
	char dataPub_to_MQTT[200];
	 // Clear buffers
   memset(dataPub_to_MQTT, 0, sizeof(dataPub_to_MQTT));
	 // Chuyen chuoi tin nhan vao dataPub_to_MQTT[200]
   sprintf(dataPub_to_MQTT, "%s", message);	
	 // Gui len MQTT Broker
	 HAL_UART_Transmit(&huart1, (uint8_t *)dataPub_to_MQTT, strlen(dataPub_to_MQTT), 1000);
	 //memset(rxBuffer_AT_Reponse, 0, sizeof(rxBuffer_AT_Reponse));	 // Xoa du lieu cu truoc khi cap nhat du lieu moi	
	 //HAL_Delay(50);
   //printf("Data Publish To MQTT: %s\r\n", dataPub_to_MQTT);  // Added \r\n for console formatting
}















