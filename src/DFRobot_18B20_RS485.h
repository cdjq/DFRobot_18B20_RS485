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
#if 1
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

class DFRobot_18B20_RS485: public DFRobot_RTU {
public:
#define REG_PID                  0x0000
#define REG_VID                  0x0001
#define REG_DEVICE_ADDR          0x0002
#define REG_UART_CTRL0           0x0003
#define EG_UART_CTRL1            0x0004
#define REG_VERSION              0x0005
#define REG_ALARM                0x0008
#define REG_ROM_FLAG             0x0009
#define REG_18B20_NUM            0x000A
#define REG_18B20_NUM0_ADDR      0x000B
#define REG_18B20_NUM1_ADDR      0x000F
#define REG_18B20_NUM2_ADDR      0x0013
#define REG_18B20_NUM3_ADDR      0x0017
#define REG_18B20_NUM4_ADDR      0x001B
#define REG_18B20_NUM5_ADDR      0x001F
#define REG_18B20_NUM6_ADDR      0x0023
#define REG_18B20_NUM7_ADDR      0x0027
#define REG_18B20_NUM0_TEMP      0x002B
#define REG_18B20_NUM1_TEMP      0x002C
#define REG_18B20_NUM2_TEMP      0x002D
#define REG_18B20_NUM3_TEMP      0x002E
#define REG_18B20_NUM4_TEMP      0x002F
#define REG_18B20_NUM5_TEMP      0x0030
#define REG_18B20_NUM6_TEMP      0x0031
#define REG_18B20_NUM7_TEMP      0x0032
#define REG_18B20_NUM0_TH_TL     0x0033
#define REG_18B20_NUM1_TH_TL     0x0034
#define REG_18B20_NUM2_TH_TL     0x0035
#define REG_18B20_NUM3_TH_TL     0x0036
#define REG_18B20_NUM4_TH_TL     0x0037
#define REG_18B20_NUM5_TH_TL     0x0038
#define REG_18B20_NUM6_TH_TL     0x0039
#define REG_18B20_NUM7_TH_TL     0x003A
#define REG_18B20_NUM0_ACCURACY  0x003B
#define REG_18B20_NUM1_ACCURACY  0x003C
#define REG_18B20_NUM2_ACCURACY  0x003D
#define REG_18B20_NUM3_ACCURACY  0x003E
#define REG_18B20_NUM4_ACCURACY  0x003F
#define REG_18B20_NUM5_ACCURACY  0x0040
#define REG_18B20_NUM6_ACCURACY  0x0041
#define REG_18B20_NUM7_ACCURACY  0x0042

#define DEVICE_PID  0x8090
#define DEVICE_VID  0x3343
#define NO_DEVICE_NUM          0
#define BROADCAST_ADDRESS            0x00
#define DS18B20_ROM_SIZE       8
#define DEFAULT_DEVICE_ADDRESS         0x20
#define DS18B20_MAX_NUM        8


#define BAUDRATE_2400      2400
#define BAUDRATE_4800      4800
#define BAUDRATE_9600      9600
#define BAUDRATE_14400     14400
#define BAUDRATE_19200     19200
#define BAUDRATE_38400     38400
#define BAUDRATE_57600     57600
#define BAUDRATE_115200    115200

#define RS485_SERIAL_7E1    (0<<6) | (1<<3) | (0 << 0)
#define RS485_SERIAL_7E1_5  (0<<6) | (1<<3) | (1 << 0)
#define RS485_SERIAL_7E2    (0<<6) | (1<<3) | (2 << 0)
#define RS485_SERIAL_7O1    (0<<6) | (2<<3) | (0 << 0)
#define RS485_SERIAL_7O1_5  (0<<6) | (2<<3) | (1 << 0)
#define RS485_SERIAL_7O2    (0<<6) | (2<<3) | (2 << 0)
#define RS485_SERIAL_8N1    (1<<6) | (0<<3) | (0 << 0)
#define RS485_SERIAL_8N1_5  (1<<6) | (0<<3) | (1 << 0)
#define RS485_SERIAL_8N2    (1<<6) | (0<<3) | (2 << 0)
#define RS485_SERIAL_8E1    (1<<6) | (1<<3) | (0 << 0)
#define RS485_SERIAL_8E1_5  (1<<6) | (1<<3) | (1 << 0)
#define RS485_SERIAL_8E2    (1<<6) | (1<<3) | (2 << 0)
#define RS485_SERIAL_8O1    (1<<6) | (2<<3) | (0 << 0)
#define RS485_SERIAL_8O1_5  (1<<6) | (2<<3) | (1 << 0)
#define RS485_SERIAL_8O2    (1<<6) | (2<<3) | (2 << 0)
#define RS485_SERIAL_9N1    (2<<6) | (0<<3) | (0 << 0)
#define RS485_SERIAL_9N1_5  (2<<6) | (0<<3) | (1 << 0)
#define RS485_SERIAL_9N2    (2<<6) | (0<<3) | (2 << 0)

#define DS18B20_NUM0_ID      0
#define DS18B20_NUM1_ID      1
#define DS18B20_NUM2_ID      2
#define DS18B20_NUM3_ID      3
#define DS18B20_NUM4_ID      4
#define DS18B20_NUM5_ID      5
#define DS18B20_NUM6_ID      6
#define DS18B20_NUM7_ID      7

#define DS18B20_ACCURACY_9_BIT   0
#define DS18B20_ACCURACY_10_BIT  1
#define DS18B20_ACCURACY_11_BIT  2
#define DS18B20_ACCURACY_12_BIT  3

  DFRobot_18B20_RS485(uint8_t addr, Stream *s);//eUARTDetecteMode
  ~DFRobot_18B20_RS485();
/**
 * @brief 传感器设备初始化。
 * @return initialization state:
 * @n      0:  sucess
 * @n      -1：failed,未接入设备，协议转化板至少要挂载一个18B20设备
 * @n      -2: failed,接入设备过多，协议转化板最多只能挂载8个18B20设备
 */
  int begin();
/**
 * @brief 配置串口,掉电重启后生效。
 * @param baud: 波特率，支持以下配置:
 * @n     BAUDRATE_2400      2400
 * @n     BAUDRATE_4800      4800
 * @n     BAUDRATE_9600      9600
 * @n     BAUDRATE_14400     14400
 * @n     BAUDRATE_19200     19200
 * @n     BAUDRATE_38400     38400
 * @n     BAUDRATE_57600     57600
 * @n     BAUDRATE_115200    115200
 * @param config: 数据位，校验位，停止位配置
 * @n     RS485_SERIAL_7E1    :7位数据位，偶校验，1位停止位
 * @n     RS485_SERIAL_7E1_5  :7位数据位，偶校验，1.5位停止位
 * @n     RS485_SERIAL_7E2    :7位数据位，偶校验，2位停止位
 * @n     RS485_SERIAL_7O1    :7位数据位，奇校验，1位停止位
 * @n     RS485_SERIAL_7O1_5  :7位数据位，奇校验，1.5位停止位
 * @n     RS485_SERIAL_7O2    :7位数据位，奇校验，2位停止位
 * @n     RS485_SERIAL_8N1    :8位数据位，无校验，1位停止位
 * @n     RS485_SERIAL_8N1_5  :8位数据位，无校验，1.5位停止位
 * @n     RS485_SERIAL_8N2    :8位数据位，无校验，2位停止位
 * @n     RS485_SERIAL_8E1    :8位数据位，偶校验，1位停止位
 * @n     RS485_SERIAL_8E1_5  :8位数据位，偶校验，1.5位停止位
 * @n     RS485_SERIAL_8E2    :8位数据位，偶校验，2位停止位
 * @n     RS485_SERIAL_8O1    :8位数据位，奇校验，1位停止位
 * @n     RS485_SERIAL_8O1_5  :8位数据位，奇校验，1.5位停止位
 * @n     RS485_SERIAL_8O2    :8位数据位，奇校验，2位停止位
 * @n     RS485_SERIAL_9N1    :9位数据位，无校验，1位停止位
 * @n     RS485_SERIAL_9N1_5  :9位数据位，无校验，1.5位停止位
 * @n     RS485_SERIAL_9N2    :9位数据位，无校验，2位停止位
 * @return 设置状态:
 * @n      0:  设置成功
 * @n      others: 设置失败
 */
  uint8_t configSerial(uint32_t baud, uint16_t config);
  
  bool readSerialConfig(uint32_t *baud, uint16_t *config);
/**
 * @brief 扫描位置0~7是否挂载18B20传感器。协议转换板最多能挂载8个18B20传感器，分别对应0~7的18B20配置
 * @return 返回8位状态位，从低到高分别代表0~7个传感器是否被挂载，某位置1代表对应的序号有传感器，置0代表无传感器。
 */
  uint8_t scan();
/**
 * @brief 设置设备地址。
 * @param newAddr: 设备地址，范围1~247。
 * @return 设置状态:
 * @n      0:  设置成功
 * @n      others: 设置失败
 */
  bool setDeviceAddress(uint8_t newAddr);
/**
 * @brief 获取设备地址。
 * @return 返回设备地址，范围1~247。
 */
  uint8_t getDeviceAddress();
/**
 * @brief 设置18B20精度。
 * @param id: ds18b20序号，范围0~7
 * @param accuracy：精度设置
 * @return 设置状态:
 * @n      0:  设置成功
 * @n      others: 设置失败
 */
  bool set18B20Accuracy(uint8_t id, uint8_t accuracy);
/**
 * @brief 设置18B20精度。
 * @return 精度:
 */
  uint8_t get18B20Accuracy(uint8_t id);
/**
 * @brief 设置温度的上下阈值。
 * @param id: 第几个温度传感器，范围0~7。
 * @param tH: 设置温度的上阈值，范围-55~125℃
 * @param tL: 设置温度的下阈值，范围-55~125℃
 * @n note: 必须满足设置条件tH > tL
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
  bool setTemperatureThreshold(uint8_t id, int8_t tH, int8_t tL);
/**
 * @brief 获取温度的上下阈值。
 * @param id: 第几个温度传感器，范围0~7。
 * @param tH: 存储温度的上阈值
 * @param tL: 存储温度的下阈值
 * @return 设置状态:
 * @n      true:  获取成功
 * @n      false: 获取失败
 */
  bool getTemperatureThreshold(uint8_t id, int8_t *tH, int8_t *tL);
/**
 * @brief 获取18B20的ROM码。
 * @param id: 第几个温度传感器，范围0~7。
 * @param rom: 存放ROM码的指针。
 * @param len: 固定长度，必须为8字节
 * @return 读取状态:
 * @n      true:  获取成功
 * @n      false: 获取失败
 */
  bool get18B20ROM(uint8_t id, uint8_t *rom, uint8_t len = 8);
  
/**
 * @brief 获取设备id的温度。
 * @param id：18B20挂载在总线上的id号，范围0~7
 * @return 温度:
 */
  float getTemperatureC(uint8_t id);
/**
 * @brief 获取协议转换板上挂载的18B20的个数。
 * @return 18B20设备数量，范围0~8：
 * @n      0：协议转换板上未挂载18B20传感器
 * @n      1: 协议转换板上挂载了1个18B20传感器
 * @n      ......
 * @n      8:  协议转换板上挂载了8个18B20传感器
 */
  uint8_t get18B20Number();
/**
 * @brief 判断设备是否存在？如果存在将存储该设备的64位ROM码。
 * @param id：18B20挂载在总线上的id号，范围0~7
 * @param rom: 64位（8字节）18B20的ROM码，用来区分同一总线上的设备。
 * @return 设备存在状态
 * @n      true:  该设备存在
 * @n      false：该设备不存在
 */
  bool exist(uint8_t id, uint8_t rom[8]);

protected:
  uint16_t getPID();
  uint16_t getVID();
  bool detectDeviceAddress(uint8_t addr);

private:
  uint8_t _addr;
};

class DFRobot_18B20_UART: public DFRobot_18B20_RS485{
public:
/**
 * @brief DFRobot_18B20_UART abstract class constructor. 配置从机的设备地址，以及串口
 * @param addr:  
 * @param s:  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object
 * @param en: The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.when you call selfCheck,
 * @n setSensitivityLevel, LowerWaterLevelCalibration,or UpperWaterLevelCalibration function, you must use EN pin.
 */
  DFRobot_18B20_UART(uint8_t addr, Stream *s);
};

#endif
