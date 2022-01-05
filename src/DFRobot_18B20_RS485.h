/*!
 * @file DFRobot_18B20_RS485.h
 * @brief 这是一款18b20单总线协议转RS485及cdc协议的协议转接板驱动库。通过它，你可以通过RS485接口配置和获取18B20传感器的相关配置和数据。
 * @details TEL0144协议转换板特点：
 * @n 1. 这是一个RS485从机设备，默认设备地址为0x20；
 * @n 3. 板载4个18B20传感器采集IO口，分别为D1、D2、D3、D4，每个IO口最大连接4个18B20传感器；
 * @n 2. 板载RS485接口，用户可以通过RS485接口配置或获取连接的18B20的传感器的配置和数据；
 * @n 3. 板载USB CDC接口，用户可以通过USB CDC串口来配置和读取TEL0144的相关配置和数据； 
 * @n 4. 默认串口通信配置：9600波特率, 8位数据位，无校验位，1位停止位，其中波特率可配置
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
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
#define RTU_BROADCAST_ADDRESS                      0x00 ///< modbus协议的广播地址为0x00
#endif
#define TEL0144_DEFAULT_DEVICE_ADDRESS             0x20 ///< TEL0144传感器的默认设备地址为0x20

class DFRobot_18B20_RS485: public DFRobot_RTU {

public:
  #define DEVICE_PID                           0x8090 ///< TEL0144设备产品标识符为0x8090
  #define DEVICE_VID                           0x3343 ///< TEL0144设备厂商标识符为0x3343
  #define DS18B20_ROM_BYTES                    8      ///< 18B20传感器的唯一标识ROM号的长度为8个字节
  #define NONE_DEVICE_CONNECTED                0      ///< 协议转换板上没有18B20设备连接

  #define DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM 4      ///< 每个IO口可连接的18B20传感器的最大数量为4
  #define DS18B20_CONFIGURATION_NUM            16     ///< 协议转换板上可配置8个18B20传感器
  #define DS18B20_CONNECT_IO_NUM               4      ///< 协议转换板上可配置的18B20采集IO口数量：4个
  
  #define IN_THE_TEMPERATURE_THRESHOLD              0  ///< in 在温度阈值范围内
  #define BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD    1  ///< below 低于最低温度阈值
  #define ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD   2  ///< above 高于最高温度阈值
  #define OCCUR_ERROR_TEMPERATURE_THRESHOLD         3  ///< 发生了错误

  /**
   * @enum eGPIONum_t
   * @brief 传感器采集IO引脚参数列表， TEL0144协议转换板上18B20传感器采集IO口列表，一般用于独立配置某一个18B20传感器
   * @details 每个TEL0144协议转换板上都有4个IO引脚，用来驱动18B20传感器，每个IO引脚最多能检测4个18B20传感器
   */
  typedef enum{
    eD1 = 1,  /**< D1 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器*/
    eD2,      /**< D2 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器*/
    eD3,      /**< D3 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器*/
    eD4,      /**< D4 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器*/
  }eGPIONum_t;
  
  /**
   * @enum eGPIOBatchNum_t
   * @brief 传感器采集IO口批量配置参数列表
   * @details 使用批量配置接口函数时，可以选择配置任意组合的IO口,例：
   * @n (eBatch_D1 | eBatch_D2)表示要配置D1和D2口
   * @n (eBatch_D1 | eBatch_D2 | eBatch_D3)表示要配置D1、D2和D3口等
   */
  typedef enum{
    eBatch_D1 = 1 << 0,/**<批量配置中， D1 IO引脚被选择，需要参与配置*/
    eBatch_D2 = 1 << 1,/**<批量配置中， D2 IO引脚被选择，需要参与配置*/
    eBatch_D3 = 1 << 2,/**<批量配置中， D3 IO引脚被选择，需要参与配置*/
    eBatch_D4 = 1 << 3,/**<批量配置中， D4 IO引脚被选择，需要参与配置*/
    eBatch_ALL = 0x0F  /**<批量配置中， D1~D4所有IO引脚被选择，也可以被替换为(eBatch_D1 | eBatch_D2 | eBatch_D3 | eBatch_D4)*/
  }eGPIOBatchNum_t;
  
  /**
   * @enum eBaudrateConfig_t
   * @brief 串口波特率配置列表
   */
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

  /**
   * @enum eBaudrateConfig_t
   * @brief 18B20传感器精度配置列表
   */
  typedef enum{
    e18B20_ACCURACY_9_BIT = 0,  /**< 9位精度*/
    e18B20_ACCURACY_10_BIT = 1, /**< 10位精度*/
    e18B20_ACCURACY_11_BIT = 2, /**< 11位精度*/
    e18B20_ACCURACY_12_BIT = 3, /**< 12位精度*/
  }e18B20Accuracy_t;
  
  /**
   * @enum e18B20IDNum_t
   * @brief 单个传感器采集IO口上，18B20序号集合列表，该参数一般用于单独配置或获取数据
   */
  typedef enum{
    eID0 = 0,/**< IO检测口上，序号为0的18B20传感器*/
    eID1 = 1,/**< IO检测口上，序号为1的18B20传感器*/
    eID2 = 2,/**< IO检测口上，序号为2的18B20传感器*/
    eID3 = 3,/**< IO检测口上，序号为3的18B20传感器*/
  }e18B20IDNum_t;

  /**
   * @enum e18B20BatchID_t
   * @brief 单个传感器采集IO口上，批量配置组合参数列表
   * @details 每个IO采集口，能检测4个18B20传感器，此列表的目的是可以选择任意组合的序号18B20传感器，进行批量配置，例:
   * @n (eBatch_ID0 | eBatch_ID1)表示将配置某个IO口上序号为0和1的18B20传感器
   * @n (eBatch_ID0 | eBatch_ID2)表示将配置某个IO口上序号为0和2的18B20传感器
   */
  typedef enum{
    eBatch_ID0 = 1 << 0,/**< 批量配置中，协议板上id为0的18B20传感器需要配置*/
    eBatch_ID1 = 1 << 1,/**< 批量配置中，协议板上id为1的18B20传感器需要配置*/
    eBatch_ID2 = 1 << 2,/**< 批量配置中，协议板上id为2的18B20传感器需要配置*/
    eBatch_ID3 = 1 << 3,/**< 批量配置中，协议板上id为3的18B20传感器需要配置*/
    eBatch_ID_ALL  = 0x0F   /**< 批量配置中，协议板上所有的18B20传感器需要配置*/
  }e18B20BatchID_t;

  /**
   * @brief DFRobot_18B20_RS485构造函数。
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
  DFRobot_18B20_RS485(uint8_t addr, Stream *s);
  DFRobot_18B20_RS485();
  ~DFRobot_18B20_RS485();
  /**
   * @brief TEL0144设备或广播地址类对象（地址为广播地址0的类对象）初始化。
   * @return initialization state:
   * @n      0:  sucess
   * @n      -1：failed
   */
  int begin();
  /**
   * @brief 获取设备PID(产品标识ID),注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
   * @return PID:
   * @n      0:  获取失败
   * @n      DEVICE_PID or 0x8090：设备PID
   */
  uint16_t getDevicePID();
  /**
   * @brief 获取设备VID（厂商标识ID）,注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
   * @return PID:
   * @n      0:  获取失败
   * @n      DEVICE_VID or 0x3343：设备PID
   */
  uint16_t getDeviceVID();
  /**
   * @brief 设置TEL0144设备的串口通信波特率，该配置设置后，TEL0144需掉电重启后才会生效，如果用户从未修改过串口波特率，其默认出厂波特率为9600。
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
   * @brief 读取TEL0144设备的串口波特率配置
   * @return TEL0144设备的串口波特率，返回值解释：
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
   * @brief 扫描TEL0144协议转换板每个IO口上是否连接的18B20传感器的情况
   * @return 返回TEL0144设备4个IO上序号0~3对应的18B20传感器是否连接的状态，1表示已连接18B20传感器，0表示传感器未连接
   * @n ----------------------------------------------------------------------------------------------------------------------
   * @n |  b15  |  b14  |  b13  |  b12  |  b11  |  b10  |  b9  |  b8  |  b7  |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
   * @n |              D4               |              D3             |            D2             |            D1             |
   * @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1 |  id0 |  id3 |  id2 |  id1 |  id0 |  id3 |  id2 |  id1 |  id0 |
   * @n -----------------------------------------------------------------------------------------------------------------------
   * @n 16位返回值的b0~b3,   对应D1 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
   * @n 16位返回值的b4~b7,   对应D2 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
   * @n 16位返回值的b8~b11,  对应D3 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
   * @n 16位返回值的b12~b15, 对应D4 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
   */
  uint16_t scan();

  /**
   * @brief 设置TEL0144设备的设备地址。
   * @param new_addr: 设备地址，范围1~247(0x01~0xF7)。
   * @return 设置状态:
   * @n      true:  设置成功
   * @n      false: 设置失败
   * @attention 用广播地址设置设备地址时，从机会处理该命令，但不会给任何反馈。
   */
  bool setDeviceAddress(uint8_t newAddr);

  /**
   * @brief 获取设备地址。
   * @return 设备地址（1~247范围内的一个地址）或广播地址（0）。
   */
  uint8_t getDeviceAddress();

  /**
   * @brief 配置指定的18B20传感器的精度，例： 将D1口上序号为0的18B20的传感器的精度配置为12位。
   * @param io  指定TEL0144协议转换板的IO口，即要配置的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id 指定TEL0144协议转换板上IO口上的序号，即要配置的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @param accuracy：精度设置
   * @n     e18B20_ACCURACY_9_BIT  or 0:  9位精度
   * @n     e18B20_ACCURACY_10_BIT or 1:  10位精度
   * @n     e18B20_ACCURACY_11_BIT or 2:  11位精度
   * @n     e18B20_ACCURACY_12_BIT or 3:  12位精度
   * @return 设置状态:
   * @n      true:  设置成功
   * @n      false: 设置失败
   * @attention 精度配置掉电保存，不丢失
   */ 
  bool set18B20Accuracy(uint8_t io, uint8_t id, uint16_t accuracy);
  bool set18B20Accuracy(eGPIONum_t io, e18B20IDNum_t id, e18B20Accuracy_t accuracy);
  
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
  bool batchSet18B20Accuracy(uint8_t batchIo, uint8_t batchId, uint16_t accuracy);
  bool batchSet18B20Accuracy(eGPIOBatchNum_t batchIo, e18B20BatchID_t batchId, e18B20Accuracy_t accuracy);

  /**
   * @brief 获取单个18B20传感器的精度配置。
   * @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @return accuracy：精度设置,范围0~3或e18B20Accuracy_t枚举变量
   * @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
   * @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
   * @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
   * @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
   * @n      0xFFFF                       :  获取精度失败
   * @attention 广播地址（0x00）无法获取任何数据，只能设置
   */
  uint16_t get18B20Accuracy(uint8_t io, uint8_t id);
  uint16_t get18B20Accuracy(eGPIONum_t io, e18B20IDNum_t id);

  /**
   * @brief 设置指定18B20的温度阈值
   * @param io  指定TEL0144协议转换板的IO口，即要设置的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id: 指定TEL0144协议转换板上IO口上的序号，即要配置的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @param tH: 设置温度的上阈值，范围-55~125℃
   * @param tL: 设置温度的下阈值，范围-55~125℃
   * @n note: 必须满足设置条件tH > tL
   * @return 设置状态:
   * @n      true:  设置成功
   * @n      false: 设置失败
   * @attention 温度阈值配置掉电保存，不丢失
   */
  bool setTemperatureThreshold(uint8_t io, uint8_t id, int8_t tH, int8_t tL);
  bool setTemperatureThreshold(eGPIONum_t io, e18B20IDNum_t id, int8_t tH, int8_t tL);

  /**
   * @brief 批量设置18B20传感器的温度阈值。
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
   * @param tH: 设置温度的上阈值，范围-55~125℃
   * @param tL: 设置温度的下阈值，范围-55~125℃
   * @n note: 必须满足设置条件tH > tL
   * @return 设置状态:
   * @n      true:  设置成功
   * @n      false: 设置失败
   */
  bool batchSet18B20TemperatureThreshold(uint8_t batchIo , uint8_t batchId, int8_t tH, int8_t tL);
  bool batchSet18B20TemperatureThreshold(eGPIONum_t batchIo, e18B20IDNum_t batchId, int8_t tH, int8_t tL);

  /**
   * @brief 获取单个18B20传感器的温度阈值。
   * @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @param tH: 存储温度的上阈值，范围-55~125℃
   * @param tL: 存储温度的下阈值，范围-55~125℃
   * @return 获取状态:
   * @n      true:  获取成功
   * @n      false: 获取失败
   */
  bool getTemperatureThreshold(uint8_t io, uint8_t id, int8_t *tH, int8_t *tL);
  bool getTemperatureThreshold(eGPIONum_t io, e18B20IDNum_t id, int8_t *tH, int8_t *tL);
  /**
   * @brief 获取TEL0144协议转换板上各IO口连接的18B20传感器温度阈值报警状态。
   * @return 单个TEL0144协议板上所有传感器是否发生温度阈值报警的状态:
   * @n -----------------------------------------------------------------
   * @n |  b31  |  b30  |  b29  |  b28  |  b27  |  b26  |  b25  |  b24  |
   * @n |                               D4                              |
   * @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
   * @n -----------------------------------------------------------------
   * @n |  b23  |  b22  |  b21  |  b20  |  b19  |  b18  |  b17  |  b16  |
   * @n |                               D3                              |
   * @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
   * @n -----------------------------------------------------------------
   * @n |  b15  |  b14  |  b13  |  b12  |  b11  |  b10  |  b9   |  b8   |
   * @n |                               D2                              |
   * @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
   * @n -----------------------------------------------------------------
   * @n |  b7   |  b6   |  b5   |  b4   |  b3   |  b2   |  b1   |  b0   |
   * @n |                               D1                              |
   * @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
   * @n -----------------------------------------------------------------
   * @n 32位返回值中b0~b7，   表示D1引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
   * @n 32位返回值中b8~b15，  表示D2引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
   * @n 32位返回值中b16~b23， 表示D3引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
   * @n 32位返回值中b24~b31， 表示D4引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
   * @attention 广播地址（0x00）无法获取任何数据，只能设置
   */
  uint32_t getTemperatureThresholdAlarmFlag();

  /**
   * @brief 解析指定IO引脚指定序号对应的18B20温度传感器的温度相对于温度阈值范围的状态
   * @param io  指定TEL0144协议转换板的IO口，即要解析的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id: 指定TEL0144协议转换板上IO口上的序号，即要解析的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @param alarmFlag: 传入getTemperatureThresholdAlarmFlag函数的返回值
   * @return id所对应的18B20的温度相对于温度阈值的状态:
   * @n      IN_THE_TEMPERATURE_THRESHOLD            or 0:   in 在温度阈值范围内
   * @n      BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD  or 1:   below 低于最低温度阈值
   * @n      ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD or 2:   above 高于最高温度阈值
   * @n      OCCUR_ERROR_TEMPERATURE_THRESHOLD       or 3:   分析过程中发生了错误
   */
  uint8_t parseThresholdAlarmFlag(uint8_t io, uint8_t id, uint32_t alarmFlag);
  uint8_t parseThresholdAlarmFlag(eGPIONum_t io, e18B20IDNum_t id, uint32_t alarmFlag); 

  /**
   * @brief 获取指定18B20传感器的ROM码
   * @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @param rom: 存放18B20 ROM数据的数组指针，只接受固定长度为8的数组指针参数。18B20的 ROM数据为8字节。
   * @return 实际读取的字节数:
   * @n      DS18B20_ROM_BYTES 或 8:  获取成功
   * @n      0: 获取失败
   */
  uint8_t get18B20ROM(uint8_t io, uint8_t id, uint8_t (&rom)[8]);
  uint8_t get18B20ROM(eGPIONum_t io, e18B20IDNum_t id,  uint8_t (&rom)[8]);

  /**
   * @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
   * @n 字符串为28AAAD385414016A。
   * @param rom: 8字节ROM码，只接受8位字节数组参数。
   * @return ROM码的16进制字符串:
   */
  String getROMHexString(uint8_t rom[8]);
  
  /**
   * @brief 获取指定18B20传感器的温度，单位：摄氏度(℃)。
   * @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
   * @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
   * @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
   * @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
   * @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
   * @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
   * @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
   * @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
   * @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
   * @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
   * @return 温度:单位摄氏度，可以测量-55~125摄氏度范围内的温度
   */
  float getTemperatureC(uint8_t io, uint8_t id);
  float getTemperatureC(eGPIONum_t io, e18B20IDNum_t id);

  /**
   * @brief 获取选中的IO口上实际连接的18B20的数量。
   * @param batchIo 选择要获取那些IO引脚上连接的传感器的数量和，各项之间用|表示，比如要获取D1和D2口上连接的18B20传感器的数量，则参数为(eBatch_D1|eBatch_D2)
   * @n ---------------------------------------------------------------------------------------------------------
   * @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
   * @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_D4  | eBatch_D3  | eBatch_D2  | eBatch_D1  |
   * @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
   * @n |                                                   |                      eBatch_ALL                   |
   * @n ---------------------------------------------------------------------------------------------------------
   * @n     eBatch_D1     or 1 << 0 : 批量配置中， D1 IO引脚被选择，即获取D1引脚上连接的18B20传感器的数量
   * @n     eBatch_D2     or 1 << 1 : 批量配置中， D2 IO引脚被选择，即获取D2引脚上连接的18B20传感器的数量
   * @n     eBatch_D3     or 1 << 2 : 批量配置中， D3 IO引脚被选择，即获取D3引脚上连接的18B20传感器的数量
   * @n     eBatch_D4     or 1 << 3 : 批量配置中， D4 IO引脚被选择，即获取D4引脚上连接的18B20传感器的数量
   * @n     eBatch_ALL    or  0x0F  : 批量配置中， D1~D4 所有IO引脚被选择，即获取板子上所有连接的18B20传感器的数量
   * @return 18B20设备数量，范围0~16：
   * @n      0：协议转换板上未连接18B20传感器
   * @n      1: 协议转换板上连接了1个18B20传感器
   * @n      ......
   * @n      16:  协议转换板上连接了16个18B20传感器
   */
  uint8_t get18B20Number(uint8_t batchIo = (uint8_t)eBatch_ALL);
  uint8_t get18B20Number(eGPIOBatchNum_t batchIo = eBatch_ALL);

protected:
  bool detectDeviceAddress(uint8_t addr);

private:
  uint8_t _addr;
};

class DFRobot_18B20_UART: public DFRobot_18B20_RS485{
public:
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
  DFRobot_18B20_UART(uint8_t addr, Stream *s);
  DFRobot_18B20_UART();
};

#endif
