# DFRobot_18B20_RS485

* [中文版](./README_CN.md)

TEL0144设备板载USB CDC接口和RS485接口，集成了4个IO口用来驱动18B20传感器，每个IO口上最多可连接4个18B20传感器。用户可以通过USB CDC接口和RS485接口实现对18B20传感器温度、温度高低阈值的设置，以及温度的采集。它符合RS485协议，支持RS485从机设备级联，从机地址范围1~247.<br>

![正反面svg效果图](https://github.com/Arya11111/DFRobot_MCP23017/blob/master/resources/images/SEN0245svg1.png)


## Product Link（链接到英文商城）
    在运行此库的demo之前，你需要下载关联库: https://github.com/DFRobot/DFRobot_RTU
   
## Table of Contents

* [Summary](#summary)
* [Connected](#connected)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary
此python库具有以下功能: 
- 设置TEL0144设备的设备地址，范围1~247；
- 设置通信串口的波特率，默认为9600，波特率设置掉电生效；
- 单独配置任意18B20传感器
- 批量配置总线上的传感器
- 支持广播地址(0x00)设置
- 扫描各IO口上18B20传感器的连接情况
- 支持设备级联  

## Connected
 Sensor      |         raspberry pi              |
------------ | :-------------------------------: |
VCC          |                5V                 |
GND          |                GND                |
RX           |connected to the UART TX pin of MCU|
TX           |connected to the UART RX pin of MCU|


## Installation
To use this library, first download the library file, then open the examples folder and run the demo in the folder Proceed as follows:
* sudo git clone https://github.com/DFRobot/DFRobot_18B20_RS485
* cd python
* cd raspberrypi
* cd examples
* python demo_*
* python3 demo_*


## Methods

```python
  '''!
    @brief DFRobot_18B20_RS485类参数初始化列表。
    @param addr: TEL0144设备的设备地址(1~247)或广播地址(0)。主机要和TEL0144从机设备通信，需要知道从机设备的串口通信配置和设备地址，主机使用广播地址将发送广播包，
    @n     总线上所有从机设备都会处理该广播包，但不会响应。地址介绍：
    @n     0  or 0x00(Hexadecimal) :   广播地址，使用改地址将初始化一个地址为广播地址的类对象，该类对象只能用来设置总线上所有TEL0144
    @n                            的参数，比如设备地址、串口通信波特率、18B20传感器精度和温度的上下阈值等，无法用来获取总线上相关设备的具体配置。
    @n     32 or 0x20(Hexadecimal) :  TEL0144设备出厂默认设备地址，如果用户没有修改设备的地址，那么TEL0144的设备地址为32。
    @n     1~247 or 0x01~0xF7      :  TEL0144设备支持的设备地址范围，可以被设置成1~147范围内的任意设备地址。
    @param baud: TEL0144设备的串口波特率配置，用来设置树莓派主机的串口通信波特率，这个波特率应该和TEL0144设备的串口波特率一致，如果用户不修改，TEL0144设备的默认波特率为9600. TEL0144支持以下波特率配置：
    @n     eBAUDRATE_2400    or 2400  :  TEL0144设备串口波特率2400
    @n     eBAUDRATE_4800    or 4800  :  TEL0144设备串口波特率4800 
    @n     eBAUDRATE_9600    or 9600  :  TEL0144设备串口波特率9600 (如果不修改，默认波特率为9600)
    @n     eBAUDRATE_14400   or 14400 :  TEL0144设备串口波特率14400 
    @n     eBAUDRATE_19200   or 19200 :  TEL0144设备串口波特率19200 
    @n     eBAUDRATE_38400   or 38400 :  TEL0144设备串口波特率38400 
    @n     eBAUDRATE_57600   or 57600 :  TEL0144设备串口波特率57600 
    @n     eBAUDRATE_115200  or 115200:  TEL0144设备串口波特率115200 
    @n note: 树莓派主机要和TEL0144设备从机通信，则它们的串口通信配置必须一致，TEL0144设备的出厂串口通信配置为：9600波特率，8位数据位，无校验位，1位停止位，
    @n 用户必须知道TEL0144设备的串口通信配置，如果忘记了TEL0144设备的地址，可以通过初始化一个广播地址类对象将地址设置为1~247范围内的地址，或者通过scan_modbus_id.py
    @n 例程来扫描TEL0144的地址，这些配置成功的前提是用户必须知道TEL0144设备的串口通信配置。
  '''
  def __init__(self, addr, baud = 9600):

  '''!
    @brief TEL0144设备或广播地址类对象（地址为广播地址0的类对象）初始化。
    @return initialization state:
    @n       0: sucess
    @n      -1: failed
  '''
  def begin(self):

  '''!
    @brief 获取设备PID(产品标识ID),注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
    @return PID:
    @n      0:  获取失败
    @n      DEVICE_PID or 0x8090：设备PID
  '''
  def get_device_pid(self):

  '''!
    @brief 获取设备VID（厂商标识ID）,注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
    @return PID:
    @n      0:  获取失败
    @n      DEVICE_PID or 0x3343：设备VID
  '''
  def get_device_vid(self):

  '''!
    @brief 设置TEL0144设备的串口通信波特率，该配置设置后，TEL0144需掉电重启后才会生效，如果用户从未修改过串口波特率，其默认出厂波特率为9600。
    @param baud: 波特率，支持以下配置:
    @n     eBAUDRATE_2400    or   2400
    @n     eBAUDRATE_4800    or   4800
    @n     eBAUDRATE_9600    or   9600
    @n     eBAUDRATE_14400   or   14400
    @n     eBAUDRATE_19200   or   19200
    @n     eBAUDRATE_38400   or   38400
    @n     eBAUDRATE_57600   or   57600
    @n     eBAUDRATE_115200  or   115200
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_baudrate(self, baud):

  '''!
    @brief 读取TEL0144设备的串口波特率配置
    @return TEL0144设备的串口波特率，返回值解释：
    @n     eBAUDRATE_2400    or   2400
    @n     eBAUDRATE_4800    or   4800
    @n     eBAUDRATE_9600    or   9600
    @n     eBAUDRATE_14400   or   14400
    @n     eBAUDRATE_19200   or   19200
    @n     eBAUDRATE_38400   or   38400
    @n     eBAUDRATE_57600   or   57600
    @n     eBAUDRATE_115200  or   115200
    @n     0： 获取失败
  '''
  def get_baudrate(self):

  '''!
    @brief 扫描TEL0144协议转换板每个IO口上是否连接的18B20传感器的情况
    @return 返回TEL0144设备4个IO上序号0~3对应的18B20传感器是否连接的状态，1表示已连接18B20传感器，0表示传感器未连接
    @n ----------------------------------------------------------------------------------------------------------------------
    @n |  b15  |  b14  |  b13  |  b12  |  b11  |  b10  |  b9  |  b8  |  b7  |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
    @n |              D4               |              D3             |            D2             |            D1             |
    @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1 |  id0 |  id3 |  id2 |  id1 |  id0 |  id3 |  id2 |  id1 |  id0 |
    @n -----------------------------------------------------------------------------------------------------------------------
    @n 16位返回值的b0~b3,   对应D1 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
    @n 16位返回值的b4~b7,   对应D2 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
    @n 16位返回值的b8~b11,  对应D3 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
    @n 16位返回值的b12~b15, 对应D4 IO口上序号id0~id3位置上连接的18B20传感器状态，0->未连接传感器， 1->有传感器连接
  '''
  def scan(self):
  
  '''!
    @brief 设置TEL0144设备的设备地址。设置地址有2种方式：1.创建一个地址为广播地址0的类对象进行设置；2.知道TEL0144的设备地址，将其修改为1~247范围内的另一个地址，区别：
    @n     使用已知道的地址将其修改为另一个地址，修改成功后，会将对象的地址更新为修改后的地址，比如创建一个设备地址为32的对象，调用set_device_address函数将其修改为16后
    @n     修改成功后，会将对象的设备地址同步更新为16，并返回True，此时调用get_device_address，将会打印16而不是32.
    @n     而使用广播地址修改地址，只负责发送修改地址的广播包，但不会检验是否修改成功，也不会将广播地址更新为新设置的地址， 调用get_device_address函数永远是0。
    @param new_addr: 设备地址，范围1~247(0x01~0xF7)。
    @n
    @n 注意：设置地址有两种方式：
    @n 1. 已知地址是1~247范围内的一个地址，现在要将它修改为1~247范围内的另一个地址：  修改成功后，会将对象的地址更新为修改后的地址，比如创建一个设备地址为32的对象，
    @n    调用set_device_address函数将其修改为16，修改成功后，会将对象的设备地址同步更新为16，并返回True，此时调用get_device_address，将会返回16而不是32.
    @n 2. 初始化一个地址为广播地址的类对象，将总线上所有设备的地址修改为1~247范围内的一个地址：  使用广播地址修改地址，只负责发送修改地址的广播包，但不会检验是否修改成功，
    @n    也不会将广播地址更新为新设置的地址，调用get_device_address函数永远返回地址0。（通过广播包设置地址时，需谨慎使用，会将总线上所有设备设置成相同的地址）
    @n
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_device_address(self, new_addr):

  '''!
    @brief 获取设备地址。
    @return 设备地址（1~247范围内的一个地址）或广播地址（0）。
  '''
  def get_device_address(self):

  '''!
    @brief 批量设置18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT，掉电保存，配置不丢失。
    @param batch_io 选择要配置那些IO引脚上连接的传感器，各项之间用|表示，比如要配置D1和D2口上的传感器，则参数为(eBatch_D1|eBatch_D2)
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_D4  | eBatch_D3  | eBatch_D2  | eBatch_D1  |
    @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                                   |                      eBatch_ALL                   |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_D1     or 1 << 0 : 批量配置中， D1 IO引脚被选择
    @n     eBatch_D2     or 1 << 1 : 批量配置中， D2 IO引脚被选择
    @n     eBatch_D3     or 1 << 2 : 批量配置中， D3 IO引脚被选择
    @n     eBatch_D4     or 1 << 3 : 批量配置中， D4 IO引脚被选择
    @n     eBatch_ALL    or  0x0F  : 批量配置中， D1~D4 所有IO引脚被选择
    @param batch_id: 选择要配置IO引脚上连接的那些18B20传感器进度
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_ID3 | eBatch_ID2 | eBatch_ID1 | eBatch_ID0 |
    @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                                   |                      eBatch_ID_ALL                |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_ID0     or 1 << 0 : 批量配置中，协议板上id为0的18B20传感器需要配置
    @n     eBatch_ID1     or 1 << 1 : 批量配置中，协议板上id为1的18B20传感器需要配置
    @n     eBatch_ID2     or 1 << 2 : 批量配置中，协议板上id为2的18B20传感器需要配置
    @n     eBatch_ID3     or 1 << 3 : 批量配置中，协议板上id为3的18B20传感器需要配置
    @n     eBatch_ID_ALL  or  0x0F  : 批量配置中，协议板上所有的18B20传感器需要配置
    @param accuracy：精度设置,范围0~3
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败      
  '''
  def batch_set_18b20_accuracy(self, batch_io, batch_id, accuracy):

  '''！
    @brief 获取单个18B20传感器的精度配置。
    @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @param accuracy：精度设置,范围0~3
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_18B20_accuracy(self, io, id, accuracy):
    
  '''！
    @brief 获取单个18B20传感器的精度配置。
    @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @return accuracy：精度设置,范围0~3
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @n      0xFFFF                       :  获取精度失败
    @attention 广播地址（0x00）无法获取任何数据，只能设置
  '''
  def get_18B20_accuracy(self, io, id):

  '''！
    @brief 批量设置18B20传感器的温度阈值。
    @param batch_io 选择要配置那些IO引脚上连接的传感器，各项之间用|表示，比如要配置D1和D2口上的传感器，则参数为(eBatch_D1|eBatch_D2)
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_D4  | eBatch_D3  | eBatch_D2  | eBatch_D1  |
    @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                                   |                      eBatch_ALL                   |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_D1     or 1 << 0 : 批量配置中， D1 IO引脚被选择
    @n     eBatch_D2     or 1 << 1 : 批量配置中， D2 IO引脚被选择
    @n     eBatch_D3     or 1 << 2 : 批量配置中， D3 IO引脚被选择
    @n     eBatch_D4     or 1 << 3 : 批量配置中， D4 IO引脚被选择
    @n     eBatch_ALL    or  0x0F  : 批量配置中， D1~D4 所有IO引脚被选择
    @param batch_id: 选择要配置IO引脚上连接的那些18B20传感器进度
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_ID3 | eBatch_ID2 | eBatch_ID1 | eBatch_ID0 |
    @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                                   |                      eBatch_ID_ALL                |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_ID0     or 1 << 0 : 批量配置中，协议板上id为0的18B20传感器需要配置
    @n     eBatch_ID1     or 1 << 1 : 批量配置中，协议板上id为1的18B20传感器需要配置
    @n     eBatch_ID2     or 1 << 2 : 批量配置中，协议板上id为2的18B20传感器需要配置
    @n     eBatch_ID3     or 1 << 3 : 批量配置中，协议板上id为3的18B20传感器需要配置
    @n     eBatch_ID_ALL  or  0x0F  : 批量配置中，协议板上所有的18B20传感器需要配置
    @param th: 设置温度的上阈值，范围-55~125℃
    @param tl: 设置温度的下阈值，范围-55~125℃
    @n note: 必须满足设置条件th > tl
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def batch_set_18b20_temperature_threshold(self, batch_io, batch_id, th, tl):

  '''！
    @brief 设置指定18B20的温度阈值
    @param io  指定TEL0144协议转换板的IO口，即要设置的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要配置的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @param th: 设置温度的上阈值，范围-55~125℃
    @param tl: 设置温度的下阈值，范围-55~125℃
    @n note: 必须满足设置条件th > tl
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_temperature_threshold(self, io, id, th, tl):
  
  '''！
    @brief 获取单个18B20传感器的温度阈值,范围-55~125℃。
    @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @return 长度为2的整型列表:
    @n      列表索引0:  存储温度的上阈值
    @n      列表索引1:  存储温度的下阈值
  '''
  def get_temperature_threshold(self, io, id):
    
  '''！
    @brief 获取TEL0144协议转换板上各IO口连接的18B20传感器温度阈值报警状态。
    @return 单个TEL0144协议板上所有传感器是否发生温度阈值报警的状态:
    @n -----------------------------------------------------------------
    @n |  b31  |  b30  |  b29  |  b28  |  b27  |  b26  |  b25  |  b24  |
    @n |                               D4                              |
    @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
    @n -----------------------------------------------------------------
    @n |  b23  |  b22  |  b21  |  b20  |  b19  |  b18  |  b17  |  b16  |
    @n |                               D3                              |
    @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
    @n -----------------------------------------------------------------
    @n |  b15  |  b14  |  b13  |  b12  |  b11  |  b10  |  b9   |  b8   |
    @n |                               D2                              |
    @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
    @n -----------------------------------------------------------------
    @n |  b7   |  b6   |  b5   |  b4   |  b3   |  b2   |  b1   |  b0   |
    @n |                               D1                              |
    @n |  id3  |  id2  |  id1  |  id0  |  id3  |  id2  |  id1  |  id0  |
    @n -----------------------------------------------------------------
    @n 32位返回值中b0~b7，   表示D1引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
    @n 32位返回值中b8~b15，  表示D2引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
    @n 32位返回值中b16~b23， 表示D3引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
    @n 32位返回值中b24~b31， 表示D4引脚上连接的18B20传感器发生阈值报警的状态，其中b0~b3分别代表IO引脚上序号id0~id3是否发生温度阈值报警，0->未发生，1->发生，b4~b5分别代表该IO引脚上的传感器如果发生了阈值报警，发生的是什么情况的报警，0->低于最低阈值报警,1->1高于最高温度阈值报警;
    @attention 广播地址（0x00）无法获取任何数据，只能设置
  '''
  def get_temperature_threshold_alarm_flag(self):
    
  '''！
    @brief 解析指定IO引脚指定序号对应的18B20温度传感器的温度相对于温度阈值范围的状态
    @param io  指定TEL0144协议转换板的IO口，即要解析的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要解析的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @param alarm_flag: 传入get_temperature_threshold_alarm_flag函数的返回值
    @return id所对应的18B20的温度相对于温度阈值的状态:
    @n      IN_THE_TEMPERATURE_THRESHOLD            or 0:   in 在温度阈值范围内
    @n      BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD  or 1:   below 低于最低温度阈值
    @n      ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD or 2:   above 高于最高温度阈值
    @n      OCCUR_ERROR_TEMPERATURE_THRESHOLD       or 3:   发生了错误
  '''
  def parse_threshold_alarm_flag(self, io, id, alarm_flag):
    
  '''！
    @brief 获取选中的IO口上实际连接的18B20的数量。
    @param batch_io 选择要获取那些IO引脚上连接的传感器的数量和，各项之间用|表示，比如要获取D1和D2口上连接的18B20传感器的数量，则参数为(eBatch_D1|eBatch_D2)
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n |    rsv     |    rsv     |    rsv     |    rsv     | eBatch_D4  | eBatch_D3  | eBatch_D2  | eBatch_D1  |
    @n |     x      |     x      |     x      |     x      |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                                   |                      eBatch_ALL                   |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_D1     or 1 << 0 : 批量配置中， D1 IO引脚被选择，即获取D1引脚上连接的18B20传感器的数量
    @n     eBatch_D2     or 1 << 1 : 批量配置中， D2 IO引脚被选择，即获取D2引脚上连接的18B20传感器的数量
    @n     eBatch_D3     or 1 << 2 : 批量配置中， D3 IO引脚被选择，即获取D3引脚上连接的18B20传感器的数量
    @n     eBatch_D4     or 1 << 3 : 批量配置中， D4 IO引脚被选择，即获取D4引脚上连接的18B20传感器的数量
    @n     eBatch_ALL    or  0x0F  : 批量配置中， D1~D4 所有IO引脚被选择，即获取板子上所有连接的18B20传感器的数量
    @return 18B20设备数量，范围0~16：
    @n      0：协议转换板上未连接18B20传感器
    @n      1: 协议转换板上连接了1个18B20传感器
    @n      ......
    @n      16:  协议转换板上连接了16个18B20传感器
  '''
  def get_18B20_number(self, batch_io = eBatch_ALL):

  '''！
    @brief 获取指定18B20传感器的温度，单位：摄氏度(℃)。
    @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @return 温度:单位摄氏度，可以测量-55~125摄氏度范围内的温度
  '''
  def get_temperature_c(self, io, id):
  
  '''！
    @brief 获取指定18B20传感器的ROM码
    @param io  指定TEL0144协议转换板的IO口，即要获取的18B20传感器隶属于那个IO引脚，IO引脚参数如下:
    @n     eD1  or 1: TEL0144板子上丝印为D1的IO引脚
    @n     eD2  or 2: TEL0144板子上丝印为D2的IO引脚
    @n     eD3  or 3: TEL0144板子上丝印为D3的IO引脚
    @n     eD4  or 4: TEL0144板子上丝印为D4的IO引脚
    @param id: 指定TEL0144协议转换板上IO口上的序号，即要获取的18B20隶属于IO引脚上的序号，序号参数如下:
    @n     eID0    or 0: TEL0144板子上IO口上编号为0的18B20传感器
    @n     eID1    or 1: TEL0144板子上IO口上编号为1的18B20传感器
    @n     eID3    or 2: TEL0144板子上IO口上编号为2的18B20传感器
    @n     eID3    or 3: TEL0144板子上IO口上编号为3的18B20传感器
    @return 长度为8的ROM码列表，全为0代表获取失败或该id未被分配给18B20传感器
  '''
  def get_18B20_rom(self, io, id):
  
  '''！
    @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
    @n 字符串为28AAAD385414016A。
    @param rom: 长度为8的ROM列表。
    @return ROM码的16进制字符串或空字符串:
  '''
  def get_rom_hex_string(self,rom):
```

## Compatibility

| 主板         | 通过 | 未通过 | 未测试 | 备注 |
| ------------ | :--: | :----: | :----: | :--: |
| RaspberryPi2 |      |        |   √    |      |
| RaspberryPi3 |      |        |   √    |      |
| RaspberryPi4 |  √   |        |        |      |

* Python 版本

| Python  | 通过 | 未通过 | 未测试 | 备注 |
| ------- | :--: | :----: | :----: | ---- |
| Python2 |  √   |        |        |      |
| Python3 |  √   |        |        |      |

## History

- 2021/07/12 - Version 1.0.0 released.
- 2021/12/20 - Version 1.0.1 released.

## Credits

Written by Arya(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





