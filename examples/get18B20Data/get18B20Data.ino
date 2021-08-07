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
 * @brief 批量设置18B20温度传感器的精度。
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
 * @param accuracy：精度设置
 * @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
 * @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
 * @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
 * @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
  board.batchSet18B20Accuracy(/*batchId =*/board.eBatch_ID_ALL, /*accuracy =*/board.e18B20_ACCURACY_12_BIT);
/**
 * @brief 批量设置18B20温度传感器的温度阈值。
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
  board.batchSet18B20TemperatureThreshold(/*batchId =*/board.eBatch_ID_ALL, /*tH =*/125, /*tL =*/-55);
}

void loop() {
/**
 * @brief 获取协议转换板上实际连接的18B20的数量。
 * @return 18B20设备数量，范围0~8：
 * @n      0：协议转换板上未连接18B20传感器
 * @n      1: 协议转换板上连接了1个18B20传感器
 * @n      ......
 * @n      8:  协议转换板上连接了8个18B20传感器
 */
  uint8_t connectedNum =  board.get18B20Number();
  uint8_t rom[DS18B20_ROM_BYTES] = {}; //DS18B20_ROM_BYTES = 8字节
  int8_t tempThresholdH;
  int8_t tempThresholdL;
  uint8_t accuarcy;
  float temp;
  Serial.print("18B20 connected numbers(range 0~8): ");
  Serial.println(connectedNum);
  
  for(int id = 0; id < SENSOR_CONNECTED_MAX_NUM; id++){
      /**
       * @brief 获取序号为id的温度传感器的ROM码。
       * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
       * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
       * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
       * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
       * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
       * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
       * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
       * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
       * @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
       * @param rom: 存放18B20 ROM数据的数组指针，只接受固定长度为8的数组指针参数。18B20的 ROM数据为8字节。
       * @return 实际读取的字节数:
       * @n      DS18B20_ROM_BYTES 或 8:  获取成功
       * @n      0: 获取失败
       */
      board.get18B20ROM(/*id =*/id, /*(&rom)[8]*/rom);
      /**
       * @brief 获取序号为id的温度传感器的上下温度阈值。
       * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
       * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
       * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
       * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
       * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
       * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
       * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
       * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
       * @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
       * @param tH: 存储温度的上阈值，范围-55~125℃
       * @param tL: 存储温度的下阈值，范围-55~125℃
       * @return 获取状态:
       * @n      true:  获取成功
       * @n      false: 获取失败
       */
      board.getTemperatureThreshold(/*id =*/id, /*tH =*/&tempThresholdH, /*tL =*/&tempThresholdL);
      accuarcy = board.get18B20Accuracy(id);
      /**
       * @brief 获取序号为id的18B20的温度数据，单位：摄氏度(℃)。
       * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
       * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
       * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
       * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
       * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
       * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
       * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
       * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
       * @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
       * @return 温度:单位摄氏度，可以测量-55~125摄氏度范围内的温度
       */
      temp = board.getTemperatureC(/*id =*/id);
      
      Serial.print("id(0~7): ");
      Serial.print(id);
      Serial.print("\tROM: ");
      /**
       * @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
       * @n 字符串为28AAAD385414016A。
       * @param rom[8]: 8字节ROM码，只接受8位字节数组参数。
       * @return ROM码的16进制字符串:
       */
      Serial.print(board.getROMHexString(/*rom[8] =*/rom));
      Serial.print("\tThreshold High(-55~125): ");
      Serial.print(tempThresholdH);
      Serial.print("\tThreshold Low(-55~125): ");
      Serial.print(tempThresholdL);
      Serial.print("\taccuarcy(0-9bits, 1-10bits, 2-11bits, 3-12bits): ");
      Serial.print(accuarcy);
      Serial.print("\tTemperature: ");
      Serial.println(temp);
      delay(1000);
  }
  Serial.println();
}


