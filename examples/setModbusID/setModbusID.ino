/*!
 * @file config.ino
 * @brief 通过广播地址(RTU_BROADCAST_ADDRESS(0x00))配置协议转换板的modbus地址和串口。将modbus地址配置为0x20，并将串口配置为
 * @n 9600波特率，8位数据位，无校验位，1位停止位。（串口配置必须掉电重启后才能生效）设备地址和串口配置掉电不会丢失。
 * @n 注意：所有从机都会处理广播包，但不会响应。
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
  DFRobot_18B20_UART board(/*addr =*/RTU_BROADCAST_ADDRESS, /*s =*/&mySerial);
#else
  DFRobot_18B20_UART board(/*addr =*/RTU_BROADCAST_ADDRESS, /*s =*/&Serial1);
#endif

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
  while(board.begin() != 0){
      Serial.println("failed.");
      delay(1000);
      Serial.print("Initialization board...");
  }
  Serial.println("done.");
  
  board.configSerial(/*baud =*/BAUDRATE_9600, /*config =*/RS485_SERIAL_8N1);
  
  board.setDeviceAddress(/*newAddr*/0x20);
  Serial.print("New device addr : 0x");
  Serial.println(board.getDeviceAddress(),HEX);
}

void loop() {
  
}


