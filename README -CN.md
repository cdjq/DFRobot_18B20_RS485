# DFRobot_18B20_RS485

* [English Version](./README.md)

这是一个DS18B20单总线协议转UART/RS485协议转换板Arduino驱动库, 它具有以下功能特点：<br>
* 最大能挂载8个DS18B20传感器；
* 能读取当前挂载的DS18B20的数量；
* 能独立读取每个被挂载的18B20的唯一ROM码；
* 能独立设置每个DS18B20的温度的上下阈值及精度
* 支持8,9,11,12位精度设置；
* 温度范围：-55~125摄氏度;
* 完全兼容modbus协议；
* 能修改设备地址，实现协议转换板串口级联，地址范围1~247，最大级联248个协议转换板；
* 可修改串口配置，掉电不丢失
* 支持的波特率：2400,4800,9600,14400,19200,38400,57600,115200
* 能获取0~7对应的位置是否挂载真实18B20传感器设备

![正反面svg效果图](https://github.com/Arya11111/DFRobot_MCP23017/blob/master/resources/images/SEN0245svg1.png)


## Product Link（链接到英文商城）
    
   
## Table of Contents

* [Summary](#summary)
* [Connected](#connected)
* [Installation](#installation)
* [Calibration](#calibration)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary
这是一个DS18B20单总线协议转UART/RS485协议转换板Arduino驱动库,通过UART接口和主控连接。<br>
符合modbus协议能实现串口级联，最大支持248个，每块最大支持挂载8个DS18B20传感器<br>

## Connected
Hardware conneted table<br>
 Sensor      |               MCU                 |
------------ | :-------------------------------: |
VCC          |                5V                 |
GND          |                GND                |
RX           |connected to the UART TX pin of MCU|
TX           |connected to the UART RX pin of MCU|

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
/**
 * @brief DFRobot_18B20_UART构造函数
 * @param addr: modbus从机地址（范围1~247）或广播地址（0x00），若配置为广播地址，发送广播包，总线上所有的从机都会处理该广播包，但不会应答
 * @param s   : 指向Stream流的串口指针
 */
DFRobot_18B20_UART(uint8_t addr, Stream *s);
/**
 * @brief 传感器设备初始化。
 * @return initialization state:
 * @n      0:  sucess
 * @n      -1：failed
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
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
bool configSerial(uint32_t baud, uint16_t config);

/**
 * @brief 读取协议转换板的串口配置
 * @param baud: 获取协议转换板串口的波特率配置（不修改，默认配置位9600），可能为如下配置：
 * @n     BAUDRATE_2400      2400
 * @n     BAUDRATE_4800      4800
 * @n     BAUDRATE_9600      9600
 * @n     BAUDRATE_14400     14400
 * @n     BAUDRATE_19200     19200
 * @n     BAUDRATE_38400     38400
 * @n     BAUDRATE_57600     57600
 * @n     BAUDRATE_115200    115200
 * @param config: 获取协议转换板串口的数据位，校验位，和停止位配置，结果解析如下
 * @n ---------------------------------------------------------------------------------------------------------------------
 * @n bit15 | bit14 | bit13 | bit12 | bit11 | bit10 | bit9 | bit8 | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
 * @n ---------------------------------------------------------------------------------------------------------------------
 * @n              invaild                                 |       Data bit     |       parity       |       stopbit      |
 * @n ---------------------------------------------------------------------------------------------------------------------
 * @n  stopbit:  000-无停止位    001-1位停止位    010-2位停止位
 * @n  parity:   000-无校验      001-偶校验       010-奇校验
 * @n  Data bit: 000-7位数据位   001-8位数据位    010-9位数据位
 * @return 返回读取状态：
 * @n      true： 成功读取
 * @n      false: 读取失败
 */ 
bool readSerialConfig(uint32_t *baud, uint16_t *config);

/**
 * @brief 扫描协议转换板位置0~7是否挂载真实的18B20传感器设备
 * @return 返回8位状态位，从低到高分别代表位置0~7是否有传感器挂载，1代表对应的序号位有传感器，0代表对应序号位无传感器。
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
 * @brief 设置序号为id的18B20温度传感器的精度。
 * @param id: 范围0~7，依次对应0~7号DS18B20传感器
 * @n     DS18B20_NUM0_ID  or  0: 第0号DS18B20传感器
 * @n     DS18B20_NUM1_ID  or  1: 第1号DS18B20传感器
 * @n     DS18B20_NUM2_ID  or  2: 第2号DS18B20传感器
 * @n     DS18B20_NUM3_ID  or  3: 第3号DS18B20传感器
 * @n     DS18B20_NUM4_ID  or  4: 第4号DS18B20传感器
 * @n     DS18B20_NUM5_ID  or  5: 第5号DS18B20传感器
 * @n     DS18B20_NUM6_ID  or  6: 第6号DS18B20传感器
 * @n     DS18B20_NUM7_ID  or  7: 第7号DS18B20传感器
 * @param accuracy：精度设置
 * @n      DS18B20_ACCURACY_9_BIT  or 0: 将序号为id的DS8B20的精度设置为9位
 * @n      DS18B20_ACCURACY_10_BIT or 1: 将序号为id的DS8B20的精度设置为10位
 * @n      DS18B20_ACCURACY_11_BIT or 2: 将序号为id的DS8B20的精度设置为11位
 * @n      DS18B20_ACCURACY_12_BIT or 3: 将序号为id的DS8B20的精度设置为12位
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
bool set18B20Accuracy(uint8_t id, uint8_t accuracy);

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
uint8_t get18B20Accuracy(uint8_t id);

/**
 * @brief 设置序号为id的温度传感器的上下温度阈值。
 * @param id: 范围0~7，依次对应0~7号DS18B20传感器
 * @n     DS18B20_NUM0_ID  or  0: 第0号DS18B20传感器
 * @n     DS18B20_NUM1_ID  or  1: 第1号DS18B20传感器
 * @n     DS18B20_NUM2_ID  or  2: 第2号DS18B20传感器
 * @n     DS18B20_NUM3_ID  or  3: 第3号DS18B20传感器
 * @n     DS18B20_NUM4_ID  or  4: 第4号DS18B20传感器
 * @n     DS18B20_NUM5_ID  or  5: 第5号DS18B20传感器
 * @n     DS18B20_NUM6_ID  or  6: 第6号DS18B20传感器
 * @n     DS18B20_NUM7_ID  or  7: 第7号DS18B20传感器
 * @param tH: 设置温度的上阈值，范围-55~125℃
 * @param tL: 设置温度的下阈值，范围-55~125℃
 * @n note: 必须满足设置条件tH > tL
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
bool setTemperatureThreshold(uint8_t id, int8_t tH, int8_t tL);

/**
 * @brief 获取序号为id的温度传感器的上下温度阈值。
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
bool getTemperatureThreshold(uint8_t id, int8_t *tH, int8_t *tL);

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
bool get18B20ROM(uint8_t id, uint8_t *rom, uint8_t len = 8);
  
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
```

## Compatibility

MCU                | SoftwareSerial | HardwareSerial |
------------------ | :----------: | :----------: |
Arduino Uno        |      √       |      X       |
Mega2560           |      √       |      √       |
Leonardo           |      √       |      √       |
ESP32              |      X       |      √       |
ESP8266            |      √       |      X       |
micro:bit          |      X       |      X       |
FireBeetle M0      |      X       |      √       |
raspberry          |      X       |      √       |

## History

- Data 2021-07-12
- Version V1.0

## Credits

Written by(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





