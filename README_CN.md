DFRobot_18B20_RS485
===========================

* [English Version](./README.md)

TEL0144 18B20单总线转UART/RS485协议转换板板载一个18B20单总线接口，1个排针类型UART接口和1个gravity 4pin UART串口，和2个RS485接口，其中单总线接口最多能连接8个18B20传感器，PC或MCU能通过UART接口或RS485接口配置连接的18B20传感器或读取连接的18B20的数据。DFRobot_18B20_RS485库是DFRobot专门为该协议板写的一个Arduino驱动库，用户可以用此驱动库来驱动Ardunio主控控制协议转换板上的18B20传感器。

![正反面svg效果图](https://github.com/Arya11111/DFRobot_MCP23017/blob/master/resources/images/SEN0245svg1.png)


## Product Link（链接到英文商城）
    在运行此库的demo之前，你需要下载关联库: https://github.com/DFRobot/DFRobot_RTU
   
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
TEL0144 18B20单总线转UART/RS485协议转换板具备以下特点:
- 使用modbus RTU通信协议
- 出厂默认串口配置： 9600波特率，8位数据位，无校验位，1位停止位，支持以下波特率配置：2400,4800,9600,14400,19200,38400,57600,115200
- 出厂默认设备地址：32（16进制0x20），可被设置为1-247（16进制0x01-0xF7）范围内的地址
- 能接收并处理广播包
- 板载排针和Gravity 4pin类型的UART接口，用户可以用该接口调式和配置协议转换板
- 板载2个RS485接口，支持RS485设备级联，最大级联248个
- 板载18B20单总线接口，最多连接8个18B20传感器，并为连接的每一个传感器分配0-7序号范围内的id

传感器ID分配原理：每个18B20传感器都有一个唯一标识的64位ROM码，每连接一个传感器，就会给这个ROM码按序匹配0-7序号范围内的ID，直到0-7中的每个号码都被分配完毕，ROM码和ID号一经匹配，则永远使用，掉电重启，ROM号对应的ID号不会被重新更改，除非拔掉改ID对应的传感器，重新插入第9个传感器，此传感器的ROM码会替换掉以前的ROM码。 


18B20传感器相关特点：
- 具有64位唯一标识ROM码
- 支持9，10，11，12位温度精度配置和采集
- 温度测量范围：-55-125℃
- 支持设置温度的上下报警阈值，阈值范围（-55-125），设置条件**上温度报警阈值 > 下温度阈值**

DFRobot_18B20_RS485 驱动库特点：
- 支持修改**TEL0144 18B20单总线转UART/RS485协议转换板**的串口波特率
- 支持修改**TEL0144 18B20单总线转UART/RS485协议转换板**的设备地址，配置范围：1-247(16进制0x01-0xF7)
- 能读取转换板上18B20的连接数量
- 能扫描0-7序号范围中那些id对应的18B20传感器在线
- 能设置单独或批量设置连接的每个18B20传感器的温度报警阈值，采样精度
- 能单独读取每个18B20传感器的温度阈值范围，精度，温度，ROM码等参数
- 支持发送广播包
- 支持驱动级联的协议转换板
- 能扫描那些id对应的温度传感器发生了温度报警

相关名词说明：  
**TEL0144设备地址**  
地址范围1-247，主机需通过设备地址才能和TEL0144设备通信。  
**广播地址0**  
0为广播地址，地址为0的数据包称为广播包，总线上不同设备地址的设备收到广播包都会处理改包，但不会应答，一般被用来主机批量设置从机的相关配置。  
**18B20 ID**  
ID范围0-7，TEL0144设备能同时连接8个18B20传感器，每连接一个传感器都会从序号0开始按序为它分配一个id号，用户可以通过这个id号来设置或读取对应的18B20传感器的相关数据。  
**ROM码**  
每个18B20都有一个专属的64位的标识号，通过这个可以区分不同的18B20传感器，通过查询ROM码为18B20绑定唯一的ID号，ROM码一旦绑定除非用另一个传感器清楚，否则永远保存。  

## Connected
Hardware conneted table

 Sensor      |               MCU                 |
------------ | :-------------------------------: |
VCC          |                5V                 |
GND          |                GND                |
RX           |connected to the UART TX pin of MCU|
TX           |connected to the UART RX pin of MCU|

RS485级联：<br>
总线上所有TEL0144设备设置成不同的设备地址后，将板子上RS485接口的A连A，B连B，即可实现RS485级联。

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
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
 * @brief 每块TEL0144设备最多能连接8个18B20温度传感器，并为每个传感器分配序号为0~7的id,以此来保存对应18B20传感器的相关配置，比如id为0的18B20的ROM码，精度，上下阈值和测量的温度
 * @n 会保存在0号id对应的寄存器中，id为1的保存在1号id对应的寄存器中。这个函数的作用就是检查0~7序号的id哪些连接了18B20传感器，在采集中，从而可以知道id号来获取相应18B20传感器的配置。
 * @n  
 * @n 18B20的id分配原理：1.序号从0~7，即第一个连接的18B20传感器，分配的id序号为0，接下来的依次分配，直到0~7号id全部被分配；
 * @n                   2.新接入的传感器，先分配没有分配过的id，如果0~7都被分配完了，再从头查找是否有id号被分配，但是分配的那个传感器已掉线，没有使用。例：0号id已被分配，此时把0
 * @n                     号id对应的传感器拔掉，新插入传感器，新传感器将从1开始分配，直到7被分配完了，再占用已经被分配的id0。
 * @n                   3.传感器掉线后，再次连接，如果它的id没有被占用，将继续使用以前给他分配的id号。
 * @n
 * @return 返回8位状态位，从bit0~bit7分别代表0~7序号的id对应的18B20传感器是否存在，置1代表对应的序号已被分配且传感器在线，清0代表该序号未被分配或已被分配但该传感器离线，无法检测。
 */
uint8_t scan();

/**
 * @brief 设置TEL0144设备的设备地址。设置地址有2种方式：1.创建一个地址为广播地址0的类对象进行设置；2.知道TEL0144的设备地址，将其修改为1~247范围内的另一个地址，区别：
 * @n     使用已知道的地址将其修改为另一个地址，修改成功后，会将对象的地址更新为修改后的地址，比如创建一个设备地址为32的对象，调用set_device_address函数将其修改为16后
 * @n     修改成功后，会将对象的设备地址同步更新为16，并返回True，此时调用get_device_address，将会打印16而不是32.
 * @n     而使用广播地址修改地址，只负责发送修改地址的广播包，但不会检验是否修改成功，也不会将广播地址更新为新设置的地址， 调用get_device_address函数永远是0。
 * @param new_addr: 设备地址，范围1~247(0x01~0xF7)。
 * @n
 * @n 注意：设置地址有两种方式：
 * @n 1. 已知地址是1~247范围内的一个地址，现在要将它修改为1~247范围内的另一个地址：  修改成功后，会将对象的地址更新为修改后的地址，比如创建一个设备地址为32的对象，
 * @n    调用set_device_address函数将其修改为16，修改成功后，会将对象的设备地址同步更新为16，并返回True，此时调用get_device_address，将会返回16而不是32.
 * @n 2. 初始化一个地址为广播地址的类对象，将总线上所有设备的地址修改为1~247范围内的一个地址：  使用广播地址修改地址，只负责发送修改地址的广播包，但不会检验是否修改成功，
 * @n    也不会将广播地址更新为新设置的地址，调用get_device_address函数永远返回地址0。（通过广播包设置地址时，需谨慎使用，会将总线上所有设备设置成相同的地址）
 * @n
 * @return 设置状态:
 * @n      true:  设置成功
 * @n      false: 设置失败
 */
bool setDeviceAddress(uint8_t newAddr);

/**
 * @brief 获取设备地址。
 * @return 设备地址（1~247范围内的一个地址）或广播地址（0）。
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




