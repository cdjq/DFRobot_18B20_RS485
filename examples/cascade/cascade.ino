/*!
 * @file cascade.ino
 * @brief TEL0144协议转换板支持多块板子级联，这个demo演示了如何使用主控操控2块级联的TEL0144板子。
 * @n 准备工作：在运行此demo之前，需要保证2块板子的串口配置一致，这里使用的默认串口配置（9600波特率，8位数据位，无校验，1位停止位），并
 * @n 将2块板子设置为不同的设备地址(1~247范围内)，设置设备的串口通信参数和设备地址可以参考config.ino demo.
 *  
 * @n  实验演示：将板1的设备地址设置为16（十六进制0x10），板2设置为32（十六进制0x20），循环采集2块板子上18B20的连接数量，ROM号，温度上下阈值，精度及温度。
 * @n  在这里你可以创建一个广播对象，批量修改总线上所有设备的配置（此方法需谨慎使用，最好不要用来批量修改设备地址，会导致总线上所有设备地址一致）。
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
 * @n TEL0144 RS485接口级联方式：总线上所有TEL0144板子A连A，B连B。如果主控有RS485接口，则直接把其中一块板子的A/B接口连接到主控的A，B接口上 ；
 * @n 如果主控上没有RS485接口，则需要一块TTL转RS485转换板，将转换板的RS485接口连接到级联的其中一块TEL0144板子的RS485接口上，再将主控的串口和
 * @n 转换板的TTL接口相连接。
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

#define CASCADE_DEVICE_NUM     2   //协议转换板的级联数量，这里表示总线上有2个设备级联

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
  DFRobot_18B20_UART broadcast(/*addr =*/RTU_BROADCAST_ADDRESS, /*s =*/&mySerial);//创建一个广播地址的对象，可批量配置总线上的所有设备
#else
  DFRobot_18B20_UART broadcast(/*addr =*/RTU_BROADCAST_ADDRESS, /*s =*/&Serial1); //创建一个广播地址的对象，可批量配置总线上的所有设备
#endif

DFRobot_18B20_UART board[CASCADE_DEVICE_NUM]; //定义一个对象数组

uint8_t modbusDeviceAddr[CASCADE_DEVICE_NUM] = {16, 32};//定义一个数组，存放协议转换板的设备地址，分别为16或32

void setup() {
  Serial.begin(115200);
  while(!Serial){                                                     //Waiting for USB Serial COM port to open.
  }

  //初始化类对象数组
  for(uint8_t num = 0; num < CASCADE_DEVICE_NUM; num++){
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
    board[num] = DFRobot_18B20_UART(/*addr =*/modbusDeviceAddr[num], /*s =*/&mySerial); //初始化级联设备
#else
    board[num] = DFRobot_18B20_UART(/*addr =*/modbusDeviceAddr[num], /*s =*/&Serial1); //初始化级联设备
#endif
  }

  //初始化MCU通信串口
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  mySerial.begin(9600);
#elif defined(ESP32)
  Serial1.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  Serial1.begin(9600);
#endif

  //初始化modbus从机列表
  uint8_t cascadeNum = 0;
  for(uint8_t i = 0; i < CASCADE_DEVICE_NUM; i++){
    Serial.print("Initialization Modbus device [");
    Serial.print(modbusDeviceAddr[i]);
    Serial.print("] ");
    if(board[i].begin() != 0){    //TEL0144设备初始化，成功返回0，失败返回-1
       Serial.println(" failed.");
    }else{
      cascadeNum += 1;
      Serial.println(" done.");
    }
  }
  
  if(cascadeNum != CASCADE_DEVICE_NUM){ //如果初始化成功的设备数不等于级联数，则循环等待
    Serial.println("Initialization Modbus device Lists failed.");
    while(1);
  }

  broadcast.begin();
/**
 * @brief 批量设置18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT（3），掉电保存，配置不丢失。
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
  broadcast.batchSet18B20Accuracy(broadcast.eBatch_ID_ALL, broadcast.e18B20_ACCURACY_10_BIT); //将总线上所有设备上连接的和没连接的18B20传感器的进度设置为10位，（0-9bits，1-10bits，2-11bits，3-12bits）
/**
 * @brief 批量设置18B20温度传感器的温度阈值，如果不修改，默认配置为高阈值125℃，低阈值-55℃，掉电保存，配置不丢失。
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
  board[0].batchSet18B20TemperatureThreshold(board[0].eBatch_ID_ALL, 100, -44);//将总线上设备地址为16的协议板上18B20传感器的温度阈值设置为高阈值100℃，低阈值-44℃
  board[1].batchSet18B20TemperatureThreshold(board[1].eBatch_ID0, 100, -44);//将总线上设备地址为32的协议板上id为0的18B20传感器的温度阈值设置为高阈值100℃，低阈值-44℃
}

void loop() {
  uint8_t deviceAddr;    //存储设备的设备地址
  uint8_t connectedNum;  //存储设备上实际连接的18B20的数量，范围0~8，最大可连接8个18B20传感器
  uint8_t connectedState;//存储设备上0~7位置是否连接18B20传感器，bit0~bit1依次代表id0~id7 18B20传感器的连接状态，1代表连接，0代表未连接
  uint8_t rom[DS18B20_ROM_BYTES]; //存储单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
  int8_t tempThresholdH; //存储某个设备上某id位置连接的单个18B20的高温度阈值
  int8_t tempThresholdL; //存储某个设备上某id位置连接的单个18B20的低温度阈值
  uint8_t accuarcy;      //存储某个设备上某id位置连接的单个18B20的精度配置
  float temp;            //存储某个设备上某id位置连接的单个18B20的测量的温度，如未连接，则为0

  for(int num = 0; num < CASCADE_DEVICE_NUM; num++){
    deviceAddr = board[num].getDeviceAddress();
    connectedNum = board[num].get18B20Number();
    connectedState = board[num].scan();
    
    Serial.print("--------------borad [");
    Serial.print(num);
    Serial.println("] info--------------");
    Serial.print("Device Address(range 001~247 or 0): ");
    Serial.print(printFormatNum(deviceAddr));
    Serial.print("\t18B20 connected numbers(range 0~8):");
    Serial.println(connectedNum);

    for(uint8_t id = 0; id < SENSOR_CONNECTED_MAX_NUM; id++){
       board[num].get18B20ROM(/*id =*/id, /*(&rom)[8]*/rom);
       board[num].getTemperatureThreshold(/*id =*/id, /*tH =*/&tempThresholdH, /*tL =*/&tempThresholdL);
       accuarcy = board[num].get18B20Accuracy(id);
       temp = board[num].getTemperatureC(/*id =*/id);

       Serial.print("id(0~7): ");
       Serial.print(id);
       Serial.print("\tROM: ");
       Serial.print(board[num].getROMHexString(/*rom[8] =*/rom));
       Serial.print("\tconnected(0-disconnected, 1-connected):");
       Serial.print((connectedState >> id)&0x01);
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
  Serial.println();
}

String printFormatNum(uint8_t num){
  String str = "";
  
  if(num < 100){
    str += '0';
  }
  if(num < 10){
    str += '0';
  }
  str += num;
  return str;
}


