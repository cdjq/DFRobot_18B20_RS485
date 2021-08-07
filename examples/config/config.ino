/*!
 * @file config.ino
 * @brief 配置TEL0144协议转换板的设备地址和串口波特率。TEL0144协议转换板遵循modbus RTU协议，用户可以自行将TEL0144设备的串口波特率配置为下表所示波特率，
 * @n 波特率配置后，需掉电重启才会生效，在知道配置的波特率后，用户可以自行将地址更改为1~247(0x00~0xF7)范围内的地址，下面介绍2种配置TEL0144设备地址的方法,
 * @n 地址修改，如果不是广播包，将会在从机回应后，主机的下一个包开始生效：
 * @n 1.通过modbus广播地址（0x00）修改，这种适用于用户不知道设备的地址。所有modbus从机都会处理广播包，但不会响应，用户可以通过广播包批量设置从机的参数或
 * @n 修改从机设备地址。（注意：用户不能通过广播包得知地址是否设置成功，如果想知道需要单独询问，如果用户通过广播包配置地址，总线上所有设备的地址都会被配置
 * @n 成一样的地址，批量配置地址需谨慎使用）
 * @n 2.通过设备的地址修改，这种只适用于用户知道设备地址的情况。
 * @n 注意：上述两种修改设备地址的方法成功的前提是必须知道设备的串口参数配置，如果不修改，默认的串口参数配置为：9600波特率，8位数据位，无校验，1位停止位，
 * @n 其中用户只可以修改串口的波特率配置。
 * @n 注意：如果创建了一个广播地址设备对象，它永远只能发广播包，getDeviceAddress函数只能返回0地址，非广播地址设备对象，getDeviceAddress函数可以返回修改后的设备地址。
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
 *     RX     |              TX                |     Serial1 TX1      |     5     |   5/D6  |  D2   |     X      |
 *     TX     |              RX                |     Serial1 RX1      |     4     |   4/D7  |  D3   |     X      |
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
 * @brief DFRobot_18B20_UART构造函数。
 * @param addr: TEL0144设备的设备地址(1~247)或广播地址(0)。主机要和TEL0144从机设备通信，需要知道从机设备的串口通信配置和设备地址，主机使用广播地址将发送广播包，
 * @n     总线上所有从机设备都会处理该广播包，但不会响应。地址介绍：
 * @n     RTU_BROADCAST_ADDRESS or 0(0x00)            : 广播地址，使用改地址将初始化一个地址为广播地址的类对象，该类对象只能用来设置总线上所有TEL0144的参数，
 * @n                                                  比如设备地址、串口通信波特率、18B20传感器精度和温度的上下阈值等，无法用来获取总线上相关设备的具体配置。
 * @n     TEL0144_DEFAULT_DEVICE_ADDRESS or 32（0x20）: TEL0144设备出厂默认设备地址，如果用户没有修改设备的地址，那么TEL0144的设备地址为32。
 * @n     1~247 or 0x01~0xF7                          : TEL0144设备支持的设备地址范围，可以被设置成1~147范围内的任意设备地址。
 * @param s   : 指向Stream流的串口指针，此种传递方式需要在demo中调用begin初始化Arduino主控的通信串口配置，需和TEL0144设备从机的串口配置一致，如果不修改，
 * @n TEL0144设备的出厂默认串口配置为：9600波特率，8位数据位，无校验位，1位停止位，用户只能修改串口的波特率，其他参数无法修改。
 * @n 注意：主机和TEL0144成功通信的前提是知道TEL0144设备的串口配置和设备地址，其中串口配置是十分重要的，不能遗忘的，请谨慎修改，如果知道串口配置，但忘记了设备地址，
 * @n 可以通过以下2种方式，重新得到设备地址，从而实现主机和TEL0144之间的通信：
 * @n 1: 主机上连接一个TEL0144设备，修改scanModbusID.ino里的串口配置后，下载烧录，通过地址扫描程序扫描改设备的地址。
 * @n 2: 直接初始化一个广播地址类对象，将地址修改位1~247范围内的任意地址。
 * @n TEL0144支持以下几种波特率配置，用户可以调用setBaudrate函数将其配置为以下波特率：
 * @n     eBAUDRATE_2400    or 2400  :  TEL0144设备串口波特率2400
 * @n     eBAUDRATE_4800    or 4800  :  TEL0144设备串口波特率4800 
 * @n     eBAUDRATE_9600    or 9600  :  TEL0144设备串口波特率9600 (出厂默认波特率配置)
 * @n     eBAUDRATE_14400   or 14400 :  TEL0144设备串口波特率14400 
 * @n     eBAUDRATE_19200   or 19200 :  TEL0144设备串口波特率19200 
 * @n     eBAUDRATE_38400   or 38400 :  TEL0144设备串口波特率38400 
 * @n     eBAUDRATE_57600   or 57600 :  TEL0144设备串口波特率57600 
 * @n     eBAUDRATE_115200  or 115200:  TEL0144设备串口波特率115200 
 */
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  SoftwareSerial mySerial(/*rx =*/4, /*tx =*/5);
  DFRobot_18B20_UART board(/*addr =*/RTU_BROADCAST_ADDRESS, /*s =*/&mySerial);//创建一个广播地址对象board，先将TEL0144板子的设备地址更改为16（16进制0x10）
  DFRobot_18B20_UART board1(/*addr =*/16, /*s =*/&mySerial);//已知一个板子的设备地址为16，将改设备地址更改为32（16进制0x20）
#else
  DFRobot_18B20_UART board(/*addr =*/RTU_BROADCAST_ADDRESS, /*s =*/&Serial1); //创建一个广播地址对象board，先将TEL0144板子的设备地址更改为16（16进制0x10）
  DFRobot_18B20_UART board1(/*addr =*/16, /*s =*/&Serial1);//已知一个板子的设备地址为16，将改设备地址更改为32（16进制0x20）
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
/////////////方法1：通过广播地址0（16进制0x00）将设备地址更改为16(注意用广播地址修改设备地址时，总线上所有设备都会被设置成同一地址)/////////////
  /**
   * @brief 传感器设备初始化。
   * @return initialization state:
   * @n      0:  sucess
   * @n      -1：failed
   */
  Serial.print("Initialization board [0]...");
  while(board.begin() != 0){
      Serial.println("failed.");
      delay(1000);
      Serial.print("Initialization board...");
  }
  Serial.println("done.");
  Serial.print("Current DEVICE ADDR(range (1~247 or 0)): ");
  Serial.println(board.getDeviceAddress());

  Serial.print("Set new device address(16) and baudrate by BROADCAST ADDRESS(0): ");
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
  board.setBaudrate(/*baud =*/board.eBAUDRATE_9600); //通过地址为广播地址的类对象将TEL0144设备的波特率设置为9600，该配置需掉电重启后生效
/**
 * @brief 设置设备地址。
 * @param newAddr: 设备地址，范围1~247(0x01~0xF7)。
 * @return 设置状态:
 * @n      0:  设置成功
 * @n      others: 设置失败
 */
  board.setDeviceAddress(/*newAddr =*/16);//通过广播地址0将TEL0144的设备地址修改为16（十六进制0x10），注意：广播包不能查询是否设置成功
  Serial.println("done.");

  Serial.print("Current DEVICE ADDR(range (1~247 or 0)): ");
  Serial.println(board.getDeviceAddress()); //广播地址对象修改地址，只能返回广播地址0，无法返回修改后的地址，也无法通过广播地址查询设备地址是否修改成功

  ///////////board1是一个设备地址为16的设备对象，查询总线上是否有设备地址为16的设备，如果有则会初始化成功///////////
  Serial.print("Check if the device with address 16 exists: ");
  while(board1.begin() != 0){
      Serial.println("failed.");
      delay(1000);
      Serial.print("Check if the device with address 16 exists: ");
  }
  Serial.println("exist.");
  Serial.print("Current DEVICE ADDR(range (1~247 or 0)): ");
  Serial.println(board1.getDeviceAddress());

  /////////////方法2：已知总线上有一个设备地址为16（16进制0x10）的设备，现将其设备地址修改为32（16进制0x20）（注意：非广播地址操作时，总线上不能连接设备地址相同的设备）/////////////
  Serial.print("Set new device address(32) by device address(16): ");
  bool flag = board1.setDeviceAddress(/*newAddr =*/32); //通过设备地址16将TEL0144的设备地址修改为32（十六进制0x20）
  if(flag){
    Serial.println("sucess.");
    Serial.print("New device addr :");
    Serial.println(board1.getDeviceAddress());
    Serial.print("baud: ");
    Serial.println(board1.getBaudrate());
  }else{
    Serial.println("failed.");
    Serial.println("Set new address 32 failed, current address is: ");
    Serial.println(board1.getDeviceAddress());
  }
}

void loop() {
  
}


