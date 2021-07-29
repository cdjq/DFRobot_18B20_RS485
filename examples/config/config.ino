/*!
 * @file config.ino
 * @brief 配置TEL0144协议转换板的设备地址和串口波特率。TEL0144协议转换板遵循modbus RTU协议，用户可以自行将TEL0144设备的串口波特率配置为下表所示波特率，
 * @n 波特率配置后，需掉电重启才会生效，在知道配置的波特率后，用户可以自行将地址更改为1~247(0x00~0xF7)范围内的地址，下面介绍2种配置TEL0144设备地址的方法,
 * @n 地址修改，如果不是广播包，将会在从机回应后，主机的下一个包开始生效：
 * @n 1.通过modbus广播地址（0x00）修改，这种适用于用户不知道设备的地址。所有modbus从机都会处理广播包，但不会响应，用户可以通过广播包批量设置从机的参数或
 * @n 修改从机设备地址。
 * @n 2.通过设备的地址修改，这种只适用于用户知道设备地址的情况。
 * @n 注意：上述两种修改设备地址的方法成功的前提是必须知道设备的串口参数配置，如果不修改，默认的串口参数配置为：9600波特率，8位数据位，无校验，1位停止位，
 * @n 其中用户只可以修改串口的波特率配置。
 * @n ---------------------------------------------
 * @n |     TEL0144协议转换板支持的串口波特率配置表  | 
 * @n --------------------------------------------- 
 * @n |  eBaudrateConfig_t      |     uint32_t    |
 * @n |   eBAUDRATE_2400        |     2400        |
 * @n |   eBAUDRATE_4800        |     4800        |
 * @n |   eBAUDRATE_9600        |     9600        |
 * @n |   eBAUDRATE_14400       |     14400       |
 * @n |   eBAUDRATE_19200       |     19200       |
 * @n |   eBAUDRATE_38400       |     38400       |
 * @n |   eBAUDRATE_57600       |     57600       |
 * @n |   eBAUDRATE_115200      |     115200      |
 * @n ---------------------------------------------
 * @n note: TEL0144支持上述波特率配置，用户可以选一种主控也支持的波特率，9600是它的默认波特率配置。
 * 
 * @n demo实验：接下来先发一个广播包把TEL0144的串口波特率配置为9600，并将其设备地址修改为0x10，在设备地址修改为16（十六进制0x10）后，在用第2种方法，将地址修到其默认的
 * @n 设备地址32(十六进制0x20)。
 * 
 * @n connected table
 * ---------------------------------------------------------------------------------------------------------------
 *    board   |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
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
 * @n     TEL0144_DEFAULT_DEVICE_ADDRESS: TEL0144协议转换板默认地址0x20（十进制32）
 * @n     RTU_BROADCAST_ADDRESS         : modbus RTU广播地址0x00（十进制0）,设置为改地址后，将会发送广播包，所有modbus从机都会处理该数据包，但不会应答
 * @n  用户可以通过该地址批量配置TEL0144协议转换板
 * @param s   : 指向Stream流的串口指针
 */
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
  /**
   * @brief 传感器设备初始化。
   * @return initialization state:
   * @n      0:  sucess
   * @n      -1：failed
   */
  Serial.print("Initialization board...");
  while(board.begin() != 0){
      Serial.println("failed.");
      delay(1000);
      Serial.print("Initialization board...");
  }
  Serial.println("done.");
  Serial.print("DEVICE ADDR(range (1~247)): ");
  Serial.println(board.getDeviceAddress());

/**
 * @brief 配置串口,掉电重启后生效，如果从未设置，串口波特率默认设置为9600波特率。
 * @param baud: 波特率，支持以下配置:
 * @n     eBAUDRATE_2400   or 2400
 * @n     eBAUDRATE_4800   or 4800
 * @n     eBAUDRATE_9600   or 9600
 * @n     eBAUDRATE_14400  or 14400
 * @n     eBAUDRATE_19200  or 19200
 * @n     eBAUDRATE_38400  or 38400
 * @n     eBAUDRATE_57600  or 57600
 * @n     eBAUDRATE_115200 or 115200
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
  board.setBaudrate(/*baud =*/board.eBAUDRATE_9600); //设置波特率为9600，该配置掉电重启生效，广播包无应答
/**
 * @brief 设置设备地址。
 * @param newAddr: 设备地址，范围1~247(0x01~0xF7)。
 * @return 设置状态:
 * @n      0:  设置成功
 * @n      others: 设置失败
 */
  board.setDeviceAddress(/*newAddr =*/16);//通过广播地址0将TEL0144的设备地址修改为16（十六进制0x10）后，延时1s
  if(board.getDeviceAddress() == 16){ //判断设备地址是为16，如果是，则将它修改为32
    Serial.print("Curent Device addr: ");
    Serial.println(board.getDeviceAddress());
    bool flag = board.setDeviceAddress(/*newAddr =*/32); //这不是广播包，从机会应答，如果为True，则标识新地址设置成功
    if(flag){
      Serial.print("New device addr :");
      Serial.println(board.getDeviceAddress());
      Serial.print("baud: ");
      Serial.println(board.getBaudrate());
    }else{
      Serial.println("Set new address 32 failed.");
    }
  }else{
     Serial.println("Set new address 16 failed.");
  }
}

void loop() {
  
}


