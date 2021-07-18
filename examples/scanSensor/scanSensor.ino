/*!
 * @file scanSensor.ino
 * @brief 扫描协议转换板上挂载的18B20传感器，并读出相应的配置值。
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
  DFRobot_18B20_UART board(/*addr =*/DEFAULT_DEVICE_ADDRESS, /*s =*/&mySerial);
#else
  DFRobot_18B20_UART board(/*addr =*/DEFAULT_DEVICE_ADDRESS, /*s =*/&Serial1);
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
  
}

void loop() {
  uint8_t state = board.scan();
  uint8_t rom[8];
  int8_t thresholdH, thresholdL;
  uint8_t accuracy;
  float temp;
  for(int id = 0; id < 8; id++){
      if(state & (1 << id)){
          Serial.print("18B20 id: ");
          Serial.println(id);
          Serial.print("rom: ");
          if(board.get18B20ROM(id, rom, sizeof(rom))){
              for(int i = 0; i < 8; i++){
                  Serial.print("0x");
                  if(rom[i] < 16) Serial.print(0);
                  Serial.print(rom[i], HEX);
                  Serial.print(" ");
              }
          }
          Serial.println();
          Serial.print("Thread(H,L): (");
          if(board.getTemperatureThreshold(id, &thresholdH, &thresholdL)){
              Serial.print(thresholdH);
              Serial.print(", ");
              Serial.print(thresholdL);
          }
          Serial.println(")");
          Serial.print("Accuracy(0[9],1[10],2[11],3[12]): ");
          accuracy = board.get18B20Accuracy(id);
          Serial.println(accuracy);
          Serial.print("Temperature: ");
          temp = board.getTemperatureC(id);
          Serial.println(temp);
          Serial.println();
      }
  }
  Serial.println();
  delay(1000);
}


