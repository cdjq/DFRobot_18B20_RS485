/*!
 * @file scanSensor.ino
 * @brief 扫描协议转换板上挂载的18B20传感器，并读出相应的配置值。
 * @n 1: ROM,挂载在协议转换板上的每一个DS18B20都有唯一64位的ROM值。
 * @n 2：阈值(Threshold),可以单独设置和读取挂载在协议转换版上每一个DS28B20的高温度阈值和低温度阈值，范围为-55~125℃
 * @n 3：精度(accuracy),可以单独设置和读取挂载在协议转换版上每一个DS28B20的精度，范围如下所示：
 * @n     DS18B20_ACCURACY_9_BIT  or  0: 9位精度
 * @n     DS18B20_ACCURACY_10_BIT or  1: 10位精度
 * @n     DS18B20_ACCURACY_11_BIT or  2: 11位精度
 * @n     DS18B20_ACCURACY_12_BIT or  3: 12位精度
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

/**
 * @brief DFRobot_18B20_UART构造函数
 * @param addr: modbus从机地址（范围1~247）或广播地址（0x00），若配置为广播地址，发送广播包，总线上所有的从机都会处理该广播包，但不会应答
 * @param s   : 指向Stream流的串口指针
 */
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
/**
 * @brief 扫描协议转换板位置0~7是否挂载真实的18B20传感器设备
 * @return 返回8位状态位，从低到高分别代表位置0~7是否有传感器挂载，1代表对应的序号位有传感器，0代表对应序号位无传感器。
 */
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
          /**
           * @brief 获取序号为id的温度传感器的ROM码。
           * @param id: 范围0~7，依次对应0~7号DS18B20传感器
           * @n     DS18B20_NUM0_ID  or  0: 第0号DS18B20传感器
           * @n     DS18B20_NUM1_ID  or  1: 第1号DS18B20传感器
           * @n     DS18B20_NUM2_ID  or  2: 第2号DS18B20传感器
           * @n     DS18B20_NUM3_ID  or  3: 第3号DS18B20传感器
           * @n     DS18B20_NUM4_ID  or  4: 第4号DS18B20传感器
           * @n     DS18B20_NUM5_ID  or  5: 第5号DS18B20传感器
           * @n     DS18B20_NUM6_ID  or  6: 第6号DS18B20传感器
           * @n     DS18B20_NUM7_ID  or  7: 第7号DS18B20传感器
           * @param rom: 存放ROM码的指针, 全为0代表无效ROM。
           * @param len: 固定长度，必须为8字节
           * @return 读取状态:
           * @n      true:  获取成功
           * @n      false: 获取失败
           */
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
          /**
           * @brief 获取温度的上下阈值。
           * @param id: 范围0~7，依次对应0~7号DS18B20传感器
           * @n     DS18B20_NUM0_ID  or  0: 第0号DS18B20传感器
           * @n     DS18B20_NUM1_ID  or  1: 第1号DS18B20传感器
           * @n     DS18B20_NUM2_ID  or  2: 第2号DS18B20传感器
           * @n     DS18B20_NUM3_ID  or  3: 第3号DS18B20传感器
           * @n     DS18B20_NUM4_ID  or  4: 第4号DS18B20传感器
           * @n     DS18B20_NUM5_ID  or  5: 第5号DS18B20传感器
           * @n     DS18B20_NUM6_ID  or  6: 第6号DS18B20传感器
           * @n     DS18B20_NUM7_ID  or  7: 第7号DS18B20传感器
           * @param tH: 存储温度的上阈值，范围-55~125℃
           * @param tL: 存储温度的下阈值，范围-55~125℃
           * @return 设置状态:
           * @n      true:  获取成功
           * @n      false: 获取失败
           */
          if(board.getTemperatureThreshold(id, &thresholdH, &thresholdL)){
              Serial.print(thresholdH);
              Serial.print(", ");
              Serial.print(thresholdL);
          }
          Serial.println(")");
          Serial.print("Accuracy(0[9],1[10],2[11],3[12]): ");
          /**
          * @brief 获取序号为id的18B20温度传感器的精度。
          * @param id: 范围0~7，依次对应0~7号DS18B20传感器
          * @n     DS18B20_NUM0_ID  or  0: 第0号DS18B20传感器
          * @n     DS18B20_NUM1_ID  or  1: 第1号DS18B20传感器
          * @n     DS18B20_NUM2_ID  or  2: 第2号DS18B20传感器
          * @n     DS18B20_NUM3_ID  or  3: 第3号DS18B20传感器
          * @n     DS18B20_NUM4_ID  or  4: 第4号DS18B20传感器
          * @n     DS18B20_NUM5_ID  or  5: 第5号DS18B20传感器
          * @n     DS18B20_NUM6_ID  or  6: 第6号DS18B20传感器
          * @n     DS18B20_NUM7_ID  or  7: 第7号DS18B20传感器
          * @return 精度:
          * @n      DS18B20_ACCURACY_9_BIT  or 0: 序号为id的DS8B20对应的精度设置为9位
          * @n      DS18B20_ACCURACY_10_BIT or 1: 序号为id的DS8B20对应的精度设置为10位
          * @n      DS18B20_ACCURACY_11_BIT or 2: 序号为id的DS8B20对应的精度设置为11位
          * @n      DS18B20_ACCURACY_12_BIT or 3: 序号为id的DS8B20对应的精度设置为12位
          */
          accuracy = board.get18B20Accuracy(id);
          Serial.println(accuracy);
          Serial.print("Temperature: ");
          /**
           * @brief 获取序号为id的18B20的温度数据，单位：摄氏度(℃)。
           * @param id: 范围0~7，依次对应0~7号DS18B20传感器
           * @n     DS18B20_NUM0_ID  or  0: 第0号DS18B20传感器
           * @n     DS18B20_NUM1_ID  or  1: 第1号DS18B20传感器
           * @n     DS18B20_NUM2_ID  or  2: 第2号DS18B20传感器
           * @n     DS18B20_NUM3_ID  or  3: 第3号DS18B20传感器
           * @n     DS18B20_NUM4_ID  or  4: 第4号DS18B20传感器
           * @n     DS18B20_NUM5_ID  or  5: 第5号DS18B20传感器
           * @n     DS18B20_NUM6_ID  or  6: 第6号DS18B20传感器
           * @n     DS18B20_NUM7_ID  or  7: 第7号DS18B20传感器
           * @return 温度:
           */
          temp = board.getTemperatureC(id);
          Serial.println(temp);
          Serial.println();
      }
  }
  Serial.println();
  delay(1000);
}


