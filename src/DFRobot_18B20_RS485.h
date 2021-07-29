/*!
 * @file DFRobot_18B20_RS485.h
 * @brief 这是一款18b20单总线协议转RS485及UART协议的协议转接板驱动库。使用它，你可以通过Arduino 主控的RS485和UART接口驱动18B20
 * @传感器。这块转换板有以下特点：
 * @n 1. 最大能同时连接8个18B20传感器;
 * @n 2. Arduino MCU通过UART接口或RS485接口同传感器通信。
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-07-05
 * @https://github.com/DFRobot/DFRobot_18B20_RS485
 */
#ifndef __DFRobot_18B20_RS485_H
#define __DFRobot_18B20_RS485_H


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include<Stream.h>
#include "DFRobot_RTU.h"

//Define DBG, change 0 to 1 open the DBG, 1 to 0 to close.  
#if 0
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

#ifndef RTU_BROADCAST_ADDRESS
#define RTU_BROADCAST_ADDRESS                      0x00 /**<modbus协议的广播地址为0x00*/
#endif
#define TEL0144_DEFAULT_DEVICE_ADDRESS             0x20 /**<TEL0144传感器的默认设备地址为0x20*/

class DFRobot_18B20_RS485: public DFRobot_RTU {

public:
#define DEVICE_PID  0x8090
#define DEVICE_VID  0x3343
#define DS18B20_ROM_BYTES                    8  /**<18B20传感器的唯一标识ROM号的长度为8个字节*/
#define NONE_DEVICE_CONNECTED                0  /**< 协议转换板上没有18B20设备连接*/
#define DEVICE_CONNECTED_MAX_NUM             8  /**< 一个协议转换板上最多可连接8个18B20传感器*/
#define DS18B20_CONFIGURATION_NUM            8  /**< 协议转换板上可配置8个18B20传感器*/

#define IN_THE_TEMPERATURE_THRESHOLD              0  /**< in 在温度阈值范围内*/
#define BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD    1  /**< below 低于最低温度阈值*/
#define ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD   2  /**< above 高于最高温度阈值*/

typedef enum{
  eBAUDRATE_2400    =  2400,  /**< 串口波特率2400*/
  eBAUDRATE_4800    =  4800,  /**< 串口波特率4800*/
  eBAUDRATE_9600    =  9600,  /**< 串口波特率9600*/
  eBAUDRATE_14400   =  14400, /**< 串口波特率14400*/
  eBAUDRATE_19200   =  19200, /**< 串口波特率19200*/
  eBAUDRATE_38400   =  38400, /**< 串口波特率38400*/
  eBAUDRATE_57600   =  57600, /**< 串口波特率57600*/
  eBAUDRATE_115200  =  115200,/**< 串口波特率115200*/
}eBaudrateConfig_t;

typedef enum{
  e18B20_ACCURACY_9_BIT = 0,  /**< 9位精度*/
  e18B20_ACCURACY_10_BIT = 1, /**< 10位精度*/
  e18B20_ACCURACY_11_BIT = 2, /**< 11位精度*/
  e18B20_ACCURACY_12_BIT = 3, /**< 12位精度*/
}e18B20Accuracy_t;

typedef enum{
  e18B20_ID0 = 0,/**< 协议板上id为0的18B20传感器*/
  e18B20_ID1 = 1,/**< 协议板上id为1的18B20传感器*/
  e18B20_ID2 = 2,/**< 协议板上id为2的18B20传感器*/
  e18B20_ID3 = 3,/**< 协议板上id为3的18B20传感器*/
  e18B20_ID4 = 4,/**< 协议板上id为4的18B20传感器*/
  e18B20_ID5 = 5,/**< 协议板上id为5的18B20传感器*/
  e18B20_ID6 = 6,/**< 协议板上id为6的18B20传感器*/
  e18B20_ID7 = 7,/**< 协议板上id为7的18B20传感器*/
  e18B20_ID_MAX      /**< 协议板上能配置的18B20传感器的最大数量*/
}e18B20IDNum_t;

typedef enum{
  eBatch_ID0 = 1 << 0,/**< 批量配置中，协议板上id为0的18B20传感器需要配置*/
  eBatch_ID1 = 1 << 1,/**< 批量配置中，协议板上id为1的18B20传感器需要配置*/
  eBatch_ID2 = 1 << 2,/**< 批量配置中，协议板上id为2的18B20传感器需要配置*/
  eBatch_ID3 = 1 << 3,/**< 批量配置中，协议板上id为3的18B20传感器需要配置*/
  eBatch_ID4 = 1 << 4,/**< 批量配置中，协议板上id为4的18B20传感器需要配置*/
  eBatch_ID5 = 1 << 5,/**< 批量配置中，协议板上id为5的18B20传感器需要配置*/
  eBatch_ID6 = 1 << 6,/**< 批量配置中，协议板上id为6的18B20传感器需要配置*/
  eBatch_ID7 = 1 << 7,/**< 批量配置中，协议板上id为7的18B20传感器需要配置*/
  eBatch_ID_ALL  = 0xFF   /**< 批量配置中，协议板上所有的18B20传感器需要配置*/
}e18B20BatchID_t;

/**
 * @brief  DFRobot_18B20_RS485构造函数
 * @param addr: modbus从机地址（范围1~247）或广播地址（0x00），若配置为广播地址，发送广播包，总线上所有的从机都会处理该广播包，但不会应答
 * @param s   : 指向Stream流的串口指针
 */
  DFRobot_18B20_RS485(uint8_t addr, Stream *s);
  DFRobot_18B20_RS485();
  ~DFRobot_18B20_RS485();
/**
 * @brief 传感器设备初始化。
 * @return initialization state:
 * @n      0:  sucess
 * @n      -1：failed
 */
  int begin();
/**
 * @brief 获取设备PID(产品标识ID)。
 * @return PID:
 * @n      0:  获取失败
 * @n      DEVICE_PID or 0x8090：设备PID
 */
uint16_t getDevicePID();
/**
 * @brief 获取设备VID（厂商标识ID）。
 * @return PID:
 * @n      0:  获取失败
 * @n      DEVICE_VID or 0x3343：设备PID
 */
uint16_t getDeviceVID();
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
  bool setBaudrate(uint32_t baud);
  bool setBaudrate(eBaudrateConfig_t baud);

/**
 * @brief 读取协议转换板的串口通信波特率配置
 * @return baud: 返回协议转换板串口波特率配置（不修改，默认配置位9600），可能为如下配置：
 * @n      eBAUDRATE_2400      2400
 * @n      eBAUDRATE_4800      4800
 * @n      eBAUDRATE_9600      9600
 * @n      eBAUDRATE_14400     14400
 * @n      eBAUDRATE_19200     19200
 * @n      eBAUDRATE_38400     38400
 * @n      eBAUDRATE_57600     57600
 * @n      eBAUDRATE_115200    115200
 * @n      0   获取失败
 */ 
  uint32_t getBaudrate();

/**
 * @brief 扫描协议转换板位置0~7是否挂载真实的18B20传感器设备
 * @return 返回8位状态位，从低到高分别代表位置0~7是否有传感器挂载，1代表对应的序号位有传感器，0代表对应序号位无传感器。
 */
  uint8_t scan();
/**
 * @brief 设置设备地址。
 * @param newAddr: 设备地址，范围1~247(0x01~0xF7)。
 * @return 设置状态:
 * @n      0:  设置成功
 * @n      others: 设置失败
 */
  bool setDeviceAddress(uint8_t newAddr);
/**
 * @brief 获取设备地址。
 * @return 返回设备地址，范围1~247（0x01~0xF7）。
 */
  uint8_t getDeviceAddress();
  
/**
 * @brief 批量设置18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT，掉电保存，配置不丢失。
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
  bool batchSet18B20Accuracy(uint8_t batchId, uint8_t accuracy);
  bool batchSet18B20Accuracy(e18B20BatchID_t batchId, e18B20Accuracy_t accuracy);

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
  bool set18B20Accuracy(uint8_t id, uint8_t accuracy);
  bool set18B20Accuracy(e18B20IDNum_t id, e18B20Accuracy_t accuracy);

/**
 * @brief 获取序号为id的18B20温度传感器的精度。
 * @param id: 范围0~7或e18B20IDNum_t枚举变量，依次对应0~7号DS18B20传感器
 * @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
 * @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
 * @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
 * @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
 * @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
 * @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
 * @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
 * @n     e18B20_ID7 or 7:  协议板上id为7的18B20传感器
 * @return accuracy：精度设置,范围0~3或e18B20Accuracy_t枚举变量
 * @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
 * @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
 * @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
 * @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
 * @n      0xFF                       :  获取精度失败
 */
  uint8_t get18B20Accuracy(uint8_t id);
  uint8_t get18B20Accuracy(e18B20IDNum_t id);

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
  bool batchSet18B20TemperatureThreshold(uint8_t batchId, int8_t tH, int8_t tL);
  bool batchSet18B20TemperatureThreshold(e18B20IDNum_t batchId, int8_t tH, int8_t tL);

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
  bool setTemperatureThreshold(uint8_t id, int8_t tH, int8_t tL);
  bool setTemperatureThreshold(e18B20IDNum_t id, int8_t tH, int8_t tL);

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
  bool getTemperatureThreshold(uint8_t id, int8_t *tH, int8_t *tL);
  bool getTemperatureThreshold(e18B20IDNum_t id, int8_t *tH, int8_t *tL);
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
  uint16_t getTemperatureThresholdAlarmFlag();

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
  uint8_t parseThresholdAlarmFlag(uint8_t id, uint16_t alarmFlag);
  uint8_t parseThresholdAlarmFlag(e18B20IDNum_t id, uint16_t alarmFlag); 

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
  uint8_t get18B20ROM(uint8_t id, uint8_t (&rom)[8]);
  uint8_t get18B20ROM(e18B20IDNum_t id,  uint8_t (&rom)[8]);

/**
 * @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
 * @n 字符串为28AAAD385414016A。
 * @param rom: 8字节ROM码，只接受8位字节数组参数。
 * @return ROM码的16进制字符串:
 */
  String getROMHexString(uint8_t rom[8]);
  
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
  float getTemperatureC(uint8_t id);
  float getTemperatureC(e18B20IDNum_t id);

/**
 * @brief 获取协议转换板上实际连接的18B20的数量。
 * @return 18B20设备数量，范围0~8：
 * @n      0：协议转换板上未连接18B20传感器
 * @n      1: 协议转换板上连接了1个18B20传感器
 * @n      ......
 * @n      8:  协议转换板上连接了8个18B20传感器
 */
  uint8_t get18B20Number();

protected:
  bool detectDeviceAddress(uint8_t addr);

private:
  uint8_t _addr;
};

class DFRobot_18B20_UART: public DFRobot_18B20_RS485{
public:
/**
 * @brief DFRobot_18B20_UART构造函数
 * @param addr: modbus从机地址（范围1~247）或广播地址（0x00），若配置为广播地址，发送广播包，总线上所有的从机都会处理该广播包，但不会应答
 * @n     TEL0144_DEFAULT_DEVICE_ADDRESS: TEL0144协议转换板默认地址0x20（十进制32）
 * @n     RTU_BROADCAST_ADDRESS         : modbus RTU广播地址0x00（十进制0）,设置为改地址后，将会发送广播包，所有modbus从机都会处理该数据包，但不会应答
 * @n  用户可以通过该地址批量配置TEL0144协议转换板
 * @param s   : 指向Stream流的串口指针
 */
  DFRobot_18B20_UART(uint8_t addr, Stream *s);
  DFRobot_18B20_UART();
};

#endif
