# DFRobot_18B20_RS485

* [English Version](./README.md)

TEL0144 Convert Board 18B20 to RS485/UART产品是一个符合modbus RTU协议的单总线转串口或RS485接口的协议转换板,它的单总线最多能连接8个<br>
18B20传感器，它支持RS485级联，可以同时使用多块TEL0144转换板，同时使用时每个TEL0144板子的串口必须配置成一致，并将每块板子设置成不一样的地址。<br>
下面就来介绍这个板子里面的一些定义: <br>
* 串口通信配置：MCU（modbus主机）和TEL0144板子需要配置成相同的波特率，校验位，停止位和数据位。（TEL0144默认串口配置为：9600波特率，无校验位，1位停止位，8位数据位，其中用户可自行更改TEL0144的串口波特率，更改后需掉电重启后才会生效）
* TEL0144设备地址：用户可将TEL0144板子配置成1~247（0x00~0xF7）范围的设备地址，默认设备地址为32（0x20），如果使用级联功能，则需将总线上的板子设置成不一样的设备地址，主机只有知道设备的设备地址，才可以实现交互，用户可以通过scanModbusID.ino来扫描总线上设备的设备地址。
* 广播地址（0x00）：主机可以发送广播包去设置总线上的从机，总线上的所有从机会处理广播包，但不会回应，如果你忘了设备地址，可以通过广播包将设备的地址修改成1~247范围内的地址。
* 18B20 ID：每块TEL0144板子可连接8个18B20传感器，这里的id代表板子上顺序0~7对印的18B20的序号。
* ROM 码： 每个18B20都有一个专属的64位的标识号，通过这个可以区分不同的18B20传感器。<br>
* 总结： （1~247）地址用来区分不同TEL0144协议转换板，（0~7）ID用来区分某块TEL0144板子上0~7序号对应的18B20传感器，64位ROM码用来区分不同的18B20传感器,只要知道这些就可以找到系统上某个特定序号的18B20传感器的位置<br>

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
 * @n     TEL0144_DEFAULT_DEVICE_ADDRESS: TEL0144协议转换板默认地址0x20（十进制32）
 * @n     RTU_BROADCAST_ADDRESS         : modbus RTU广播地址0x00（十进制0）,设置为改地址后，将会发送广播包，所有modbus从机都会处理该数据包，但不会应答
 * @n  用户可以通过该地址批量配置TEL0144协议转换板
 * @param s   : 指向Stream流的串口指针
 */
DFRobot_18B20_UART(uint8_t addr, Stream *s);
DFRobot_18B20_UART();

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





