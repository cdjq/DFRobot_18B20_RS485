/*!
 * @file getTemperature.ino
 * @brief 获取18B20转RS485协议板上所有DS18B20的温度信息,未更改前，默认串口配置为：9600波特率，8位数据位，无校验位，1位停止位。
 * @n connected table
 * ---------------------------------------------------------------------------------------------------------------
 * sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |
 *     RX     |              TX                |     Serial1 RX1      |     5     |   5/D6  |  D2   |     X      |
 *     TX     |              RX                |     Serial1 TX1      |     4     |   4/D7  |  D3   |     X      |
 * ---------------------------------------------------------------------------------------------------------------
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-07-05
 * @https://github.com/DFRobot/DFRobot_18B20_RS485
 */
#include "DFRobot_18B20_RS485.h"
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
#include <SoftwareSerial.h>
#endif


#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  SoftwareSerial mySerial(/*rx =*/4, /*tx =*/5);
  DFRobot_18B20_UART board(/*s =*/&mySerial);
#else
  DFRobot_18B20_UART board(/*s =*/&Serial1);
#endif

static uint8_t ds18b20Num = 0;


void setup() {
  Serial.begin(115200);
  while(!Serial){                                                     //Waiting for USB Serial COM port to open.
  }
  
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  mySerial.begin(9600);
#elif defined(ESP32)
  Serial1.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  Serial1.begin(9600);
#endif

  Serial.print("Initialization board...");
  while(board.begin(/*addr =*/DEFAULT_DEVICE_ADDRESS) != 0){
      Serial.println("failed.");
      delay(1000);
      Serial.print("Initialization board...");
  }
  Serial.println("done.");
  
}

void loop() {
  ds18b20Num = board.get18B20Number();
  Serial.print("18B20 NUM: ");
  Serial.println(ds18b20Num);
  
  for(int id = 0; id < DS18B20_MAX_NUM; id++){
      Serial.print("id: ");
      Serial.println(id);
      Serial.print("\tTemperature: ");
      Serial.println(board.getTemperatureC(/*id= */id));
	  delay(1000)
  }
  Serial.println();
}


