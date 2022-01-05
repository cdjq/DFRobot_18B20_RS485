/*!
 * @file get18B20Data.ino
 * @brief 获取单个协议转换板上连接的18B20温度传感器的数量，以及所有18B20的ROM号，精度，温度上下阈值的配置，以及当前采集的温度。
 * @n 注意：协议转换板的默认串口配置为：9600波特率，8位数据维，无校验，以及一位停止位，其中波特率可以通过setBaudrate函数更改。
 * @n  协议转换板的默认设备地址为0x20（十进制32，TEL0144_DEFAULT_DEVICE_ADDRESS），可以通过setDeviceAddr函数将地址更改为1~247(0x01~0xF7)
 * @n  范围内的任意数值,0x00是广播地址，任何modbus从机都会处理广播包，但不会响应。
 * @n 注意：如果你想更改协议转换板的设备地址和串口波特率，可参考demo：config.ino
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
  DFRobot_18B20_UART board(/*addr =*/TEL0144_DEFAULT_DEVICE_ADDRESS, /*s =*/&mySerial);
#else
  DFRobot_18B20_UART board(/*addr =*/TEL0144_DEFAULT_DEVICE_ADDRESS, /*s =*/&Serial1);
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
  Serial.print("DEVICE PID: 0x");
  Serial.println(board.getDevicePID(),HEX);
  Serial.print("DEVICE VID: 0x");
  Serial.println(board.getDeviceVID(),HEX);
  Serial.print("BAUDRATE: ");
  Serial.println(board.getBaudrate());
  
  /**
   * @brief 批量设置18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT，掉电保存，配置不丢失。
   * @param batchIo 选择要配置那些IO引脚上连接的传感器，各项之间用|表示，比如要配置D1和D2口上的传感器，则参数为(eBatch_D1|eBatch_D2)
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
   * @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_D4  | eBatch_D3  | eBatch_D2  | eBatch_D1  |
   * @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
   * @n |                                                   |                      eBatch_ALL                   |
   * @n ---------------------------------------------------------------------------------------------------------
   * @n     eBatch_D1     or 1 << 0 : 批量配置中， D1 IO引脚被选择
   * @n     eBatch_D2     or 1 << 1 : 批量配置中， D2 IO引脚被选择
   * @n     eBatch_D3     or 1 << 2 : 批量配置中， D3 IO引脚被选择
   * @n     eBatch_D4     or 1 << 3 : 批量配置中， D4 IO引脚被选择
   * @n     eBatch_ALL    or  0x0F  : 批量配置中， D1~D4 所有IO引脚被选择
   * @param batchId: 选择要配置IO引脚上连接的那些18B20传感器进度
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
   * @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_ID3 | eBatch_ID2 | eBatch_ID1 | eBatch_ID0 |
   * @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
   * @n |                                                   |                      eBatch_ID_ALL                |
   * @n ---------------------------------------------------------------------------------------------------------
   * @n     eBatch_ID0     or 1 << 0 : 批量配置中，协议板上id为0的18B20传感器需要配置
   * @n     eBatch_ID1     or 1 << 1 : 批量配置中，协议板上id为1的18B20传感器需要配置
   * @n     eBatch_ID2     or 1 << 2 : 批量配置中，协议板上id为2的18B20传感器需要配置
   * @n     eBatch_ID3     or 1 << 3 : 批量配置中，协议板上id为3的18B20传感器需要配置
   * @n     eBatch_ID_ALL  or  0x0F  : 批量配置中，协议板上所有的18B20传感器需要配置
   * @param accuracy：精度设置
   * @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
   * @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
   * @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
   * @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
   * @return 设置状态:
   * @n      true:  设置成功
   * @n      false: 设置失败
   * @attention 精度配置掉电保存，不丢失
   */
  board.batchSet18B20Accuracy(/*batchIo =*/board.eBatch_ALL, /*batchId =*/board.eBatch_ID_ALL, /*accuracy =*/board.e18B20_ACCURACY_12_BIT);//批量设置，将TEL0144设备上所有IO口连接的18B20传感器的精度设置为12位
  /**
   * @brief 批量设置18B20温度传感器的温度阈值。
   * @param batchIo 选择要配置那些IO引脚上连接的传感器，各项之间用|表示，比如要配置D1和D2口上的传感器，则参数为(eBatch_D1|eBatch_D2)
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
   * @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_D4  | eBatch_D3  | eBatch_D2  | eBatch_D1  |
   * @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
   * @n |                                                   |                      eBatch_ALL                   |
   * @n ---------------------------------------------------------------------------------------------------------
   * @n     eBatch_D1     or 1 << 0 : 批量配置中， D1 IO引脚被选择
   * @n     eBatch_D2     or 1 << 1 : 批量配置中， D2 IO引脚被选择
   * @n     eBatch_D3     or 1 << 2 : 批量配置中， D3 IO引脚被选择
   * @n     eBatch_D4     or 1 << 3 : 批量配置中， D4 IO引脚被选择
   * @n     eBatch_ALL    or  0x0F  : 批量配置中， D1~D4 所有IO引脚被选择
   * @param batchId: 18B20传感器ID集合
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
   * @n | eBatch_ID7 | eBatch_ID6 | eBatch_ID5 | eBatch_ID4 | eBatch_ID3 | eBatch_ID2 | eBatch_ID1 | eBatch_ID0 |
   * @n |   1 << 7   |   1 << 6   |   1 << 5   |   1 << 4   |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
   * @n |                                         eBatch_ID_ALL                                                 |
   * @n ---------------------------------------------------------------------------------------------------------
   * @n     eBatch_ID0     or 1 << 0 : 批量配置中，协议板上id为0的18B20传感器需要配置
   * @n     eBatch_ID1     or 1 << 1 : 批量配置中，协议板上id为1的18B20传感器需要配置
   * @n     eBatch_ID2     or 1 << 2 : 批量配置中，协议板上id为2的18B20传感器需要配置
   * @n     eBatch_ID3     or 1 << 3 : 批量配置中，协议板上id为3的18B20传感器需要配置
   * @n     eBatch_ID4     or 1 << 4 : 批量配置中，协议板上id为4的18B20传感器需要配置
   * @n     eBatch_ID5     or 1 << 5 : 批量配置中，协议板上id为5的18B20传感器需要配置
   * @n     eBatch_ID6     or 1 << 6 : 批量配置中，协议板上id为6的18B20传感器需要配置
   * @n     eBatch_ID7     or 1 << 7 : 批量配置中，协议板上id为7的18B20传感器需要配置
   * @n     eBatch_ID_ALL  or  0xFF  : 批量配置中，协议板上所有的18B20传感器需要配置
   * @param tH: 设置温度的上阈值，范围-55~125℃
   * @param tL: 设置温度的下阈值，范围-55~125℃
   * @n note: 必须满足设置条件tH > tL
   * @return 设置状态:
   * @n      true:  设置成功
   * @n      false: 设置失败
   */
  board.batchSet18B20TemperatureThreshold(/*batchIo =*/board.eBatch_ALL, /*batchId =*/board.eBatch_ID_ALL, /*tH =*/125, /*tL =*/-55);//批量设置，将TEL0144设备上所有IO口连接的18B20传感器的阈值设置为-55~125
}

void loop() {
  uint8_t totalConnected; //记录TEL0144设备上所有IO口连接的18B20传感器数量之和
  uint8_t ioConnected;    //记录TEL0144设备上单独IO口连接的18B20传感器数量
  uint16_t connetedState; //记录TEL0144设备各IO口的每个位置是否连接18B20传感器的情况，TEL0144有4个IO口，每个IO口最多可连接4个传感器，用16位数据分别代表各IO口上传感器的连接情况，0->未连接，1->连接
  uint8_t rom[DS18B20_ROM_BYTES]; //存储单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
  int8_t tempThresholdH, tempThresholdL; //记录18B20的温度阈值
  uint8_t accuracy;      //记录18B20的精度配置
  float temp;            //记录18B20传感器采集到的温度数据
  uint32_t alarmFlag;    //记录TEL0144设备所有18B20的温度报警状态标志

  connetedState  = board.scan();
  totalConnected = board.get18B20Number(board.eBatch_ALL);
  alarmFlag      = board.getTemperatureThresholdAlarmFlag();
  Serial.print("18B20 connected numbers(range 0~16): ");
  Serial.println(totalConnected);
  Serial.println("|-----------------------------------------------------------------------------------------------|");
  Serial.println("|              18B20 connecte table of TEL0144(0->not connected, 1->connected)                  |");
  Serial.println("|-----------------------------------------------------------------------------------------------|");
  Serial.println("|           D4          |           D3          |           D2          |           D1          |");
  Serial.println("| b15 | b14 | b13 | b12 | b11 | b10 | b09 | b08 | b07 | b06 | b05 | b04 | b03 | b02 | b01 | b00 |");
  Serial.println("| id3 | id2 | id1 | id0 | id3 | id2 | id1 | id0 | id3 | id2 | id1 | id0 | id3 | id2 | id1 | id0 |");
  for(int i = DS18B20_CONNECT_IO_NUM*DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM; i > 0; i--){
    if(connetedState & (1 << (i - 1))){
      Serial.print("|  1  ");
    }else{
      Serial.print("|  0  ");
    }
  }
  Serial.println("|\n|-----------------------------------------------------------------------------------------------|");
  
  for(uint8_t io = 1; io <= DS18B20_CONNECT_IO_NUM; io++){
    Serial.print("IO: D");
    Serial.print(io);
    ioConnected = board.get18B20Number(1 << (io - 1));
    Serial.print("\tconnected(range 0~4): ");
    Serial.println(ioConnected);

    for(uint8_t id = 0; id < DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM; id++){
      board.get18B20ROM(/*io =*/io, /*id =*/id, /*(&rom)[8]*/rom);
      board.getTemperatureThreshold(/*io =*/io, /*id =*/id, /*tH =*/&tempThresholdH, /*tL =*/&tempThresholdL);
      accuracy = board.get18B20Accuracy(/*io =*/io, /*id =*/id);
      temp = board.getTemperatureC(/*io =*/io, /*id =*/id);
      Serial.print("\tID: id");
      Serial.print(id);
      Serial.print("\tROM: ");
      Serial.print(board.getROMHexString(/*rom[8] =*/rom));
      Serial.print("\tAccuarcy(0-9bits, 1-10bits, 2-11bits, 3-12bits): ");
      Serial.print(accuracy);
      Serial.print("\tALARM(0-IN, 1-BELOW, 2-ABOVE): ");
      Serial.print(board.parseThresholdAlarmFlag(/*io =*/io, /*id =*/id, /*alarmFlag =*/alarmFlag));
      Serial.print("\tThreshold High(-55~125): ");
      Serial.print(tempThresholdH);
      Serial.print("\tThreshold Low(-55~125): ");
      Serial.print(tempThresholdL);
      Serial.print("\tTemperature: ");
      Serial.println(temp);
      delay(1000);

    }
  }
  Serial.println();
  delay(1000);
}

