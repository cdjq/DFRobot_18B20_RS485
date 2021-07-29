/*!
 * @file scanSensor.ino
 * @brief 扫描协议转换板上真实连接的18B20传感器，并读出相应的配置值。
 * @n 1: ROM,挂载在协议转换板上的每一个DS18B20都有唯一64位的ROM值。
 * @n 2：阈值(Threshold),可以单独设置和读取挂载在协议转换版上每一个DS28B20的高温度阈值和低温度阈值，范围为-55~125℃
 * @n 3：精度(accuracy),可以单独设置和读取挂载在协议转换版上每一个DS28B20的精度，范围如下所示：
 * @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
 * @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
 * @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
 * @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
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
 * @brief 设置序号为id的18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT，掉电保存，配置不丢失。
 * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
 * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
 * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
 * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
 * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
 * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
 * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
 * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
 * @n     e18B20_ID7 or 7:  协议板上id为7的18B20传感器
 * @param accuracy：精度设置,范围0~3或e18B20Accuracy_t枚举变量
 * @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
 * @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
 * @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
 * @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
  board.set18B20Accuracy(board.e18B20_ID0, board.e18B20_ACCURACY_12_BIT);//将协议转换板上e18B20_ID0对应的18B20传感器的精度设置为12位采样
  board.set18B20Accuracy(board.e18B20_ID1, board.e18B20_ACCURACY_9_BIT);//将协议转换板上e18B20_ID1对应的18B20传感器的精度设置为12位采样

/**
 * @brief 设置序号为id的温度传感器的上下温度阈值，如果不修改，默认配置为高阈值125℃，低阈值-55℃，掉电保存，配置不丢失。
 * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
 * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
 * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
 * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
 * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
 * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
 * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
 * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
 * @n     e18B20_ID7 or 7:  协议板上id为7的18B20传感器
 * @param tH: 设置温度的上阈值，范围-55~125℃
 * @param tL: 设置温度的下阈值，范围-55~125℃
 * @n note: 必须满足设置条件tH > tL
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
  board.setTemperatureThreshold(board.e18B20_ID0, 100, 26); //将协议转换板上e18B20_ID0对应的18B20传感器的温度阈值设置为25~100℃
  board.setTemperatureThreshold(board.e18B20_ID1, 33, 28);//将协议转换板上e18B20_ID1对应的18B20传感器的温度阈值设置为28~33℃
  
}

void loop() {
/**
 * @brief 扫描协议转换板位置0~7是否挂载真实的18B20传感器设备
 * @return 返回8位状态位，从低到高分别代表位置0~7是否有传感器挂载，1代表对应的序号位有传感器，0代表对应序号位无传感器。
 */
  uint8_t connetedState; //存储设备上0~7位置是否连接18B20传感器，bit0~bit1依次代表id0~id7 18B20传感器的连接状态，1代表连接，0代表未连接
  uint8_t connectedNum;  //存储设备上实际连接的18B20的数量，范围0~8，最大可连接8个18B20传感器
  uint16_t alarmFlag;    //存储协议转换板上所有18B20的温度报警状态标志
  uint8_t rom[DS18B20_ROM_BYTES]; //存储单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
  int8_t tempThresholdH, tempThresholdL;
  uint8_t accuracy;      //存储某个设备上某id位置连接的单个18B20的精度配置
  float temp;            //存储某个设备上某id位置连接的单个18B20的测量的温度
 
  connetedState = board.scan();
  connectedNum  = board.get18B20Number();
/**
 * @brief 获取18B20传感器温度阈值报警标志。
 * @return 8个18B20当前温度阈值报警的标志位:
 * @n -------------------------------------------------------------------------------------------------------------------------
 * @n |     bit15    |     bit14    |     bit13    |     bit12    |     bit11    |     bit10    |     bit9     |     bit8     |
 * @n -------------------------------------------------------------------------------------------------------------------------
 * @n | 18b20_HL_id7 | 18b20_HL_id6 | 18b20_HL_id5 | 18b20_HL_id4 | 18b20_HL_id3 | 18b20_HL_id2 | 18b20_HL_id1 | 18b20_HL_id0 |
 * @n |                                0:低于最低阈值报警， 1高于最高温度阈值报警                                                |
 * @n -------------------------------------------------------------------------------------------------------------------------
 * @n |     bit7     |     bit6     |     bit5     |     bit4     |     bit3     |     bit2     |     bit1     |     bit0     |
 * @n -------------------------------------------------------------------------------------------------------------------------
 * @n |  18b20_F_id7 |  18b20_F_id6 |  18b20_F_id5 |  18b20_F_id4 |  18b20_F_id3 |  18b20_F_id2 |  18b20_F_id1 |  18b20_F_id0 |
 * @n |                                    18B20是否发生阈值报警标志位，0：没有发生阈值报警，1发生了阈值报警                       |
 * @n -------------------------------------------------------------------------------------------------------------------------
 * @n  低8位0~7：代表id0~id7位置的18B20传感器是否发生阈值报警标志位，0：温度在阈值范围内，未发生阈值报警，1：温度在阈值范围外，发生了阈值报警
 * @n  高8位8~15：代表如果发生了阈值报警，是高于最高温度阈值，还是低于最低温度阈值，高8位只有在低8位对印位为1才会生效，0低于最低阈值，1高于最高阈值。
 * @n      false: 获取失败
 */
  alarmFlag     = board.getTemperatureThresholdAlarmFlag();

  Serial.print("\t18B20 connected numbers(range 0~8):");
  Serial.println(connectedNum);

  for(int id = 0; id < 8; id++){
      if(connetedState & (1 << id)){
          board.get18B20ROM(/*id =*/id, /*(&rom)[8]*/rom);
          board.getTemperatureThreshold(/*id =*/id, /*tH =*/&tempThresholdH, /*tL =*/&tempThresholdL);
          accuracy = board.get18B20Accuracy(/*id =*/id);
          temp = board.getTemperatureC(/*id =*/id);
          
          Serial.print("ID: ");
          Serial.print(id);
          Serial.print("\tROM: ");
          Serial.print(board.getROMHexString(/*rom[8] =*/rom));
          Serial.print("\tAccuarcy(0-9bits, 1-10bits, 2-11bits, 3-12bits): ");
          Serial.print(accuracy);
          Serial.print("\tALARM(0-IN, 1-BELOW, 2-ABOVE): ");
          /**
           * @brief 解析id对应的18B20温度传感器的温度相对于温度阈值范围的状态。
           * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
           * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
           * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
           * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
           * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
           * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
           * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
           * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
           * @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
           * @param alarmFlag: 传入getTemperatureThresholdAlarmFlag函数的返回值
           * @return id所对应的18B20的温度相对于温度阈值的状态:
           * @n      IN_THE_TEMPERATURE_THRESHOLD            or 0:   in 在温度阈值范围内
           * @n      BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD  or 1:   below 低于最低温度阈值
           * @n      ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD or 2:   above 高于最高温度阈值
           */
          Serial.print(board.parseThresholdAlarmFlag(/*id =*/id, /*alarmFlag =*/alarmFlag));
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


