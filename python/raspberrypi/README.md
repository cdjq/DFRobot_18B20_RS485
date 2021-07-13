# DFRobot_18B20_RS485
这是一个DS18B20单总线协议转UART/RS485协议转换板python驱动库, 它具有以下功能特点：<br>
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
VCC          |                5V                 |
GND          |                GND                |
RX           |connected to the UART TX pin of MCU|
TX           |connected to the UART RX pin of MCU|

## Installation
To use this library, first download the library file, then open the examples folder and run the demo in the folder Proceed as follows:
* sudo git clone https://github.com/DFRobot/DFRobot_SCW8916B
* cd python
* cd raspberrypi
* cd examples
* python demo_*
* python3 demo_*


## Methods

```C++
'''
@brief 传感器设备初始化。
@return initialization state:
@n      0:  sucess
@n      -1：failed,未接入设备，协议转化板至少要挂载一个18B20设备
@n      -2: failed,接入设备过多，协议转化板最多只能挂载8个18B20设备
'''
def begin(self,addr):

'''
@brief 配置串口。
@param baud: 波特率，支持以下配置:
@n     BAUDRATE_2400      2400
@n     BAUDRATE_4800      4800
@n     BAUDRATE_9600      9600
@n     BAUDRATE_14400     14400
@n     BAUDRATE_19200     19200
@n     BAUDRATE_38400     38400
@n     BAUDRATE_57600     57600
@n     BAUDRATE_115200    115200
@param config: 数据位，校验位，停止位配置
@n     RS485_SERIAL_7E1    :7位数据位，偶校验，1位停止位
@n     RS485_SERIAL_7E1_5  :7位数据位，偶校验，1.5位停止位
@n     RS485_SERIAL_7E2    :7位数据位，偶校验，2位停止位
@n     RS485_SERIAL_7O1    :7位数据位，奇校验，1位停止位
@n     RS485_SERIAL_7O1_5  :7位数据位，奇校验，1.5位停止位
@n     RS485_SERIAL_7O2    :7位数据位，奇校验，2位停止位
@n     RS485_SERIAL_8N1    :8位数据位，无校验，1位停止位
@n     RS485_SERIAL_8N1_5  :8位数据位，无校验，1.5位停止位
@n     RS485_SERIAL_8N2    :8位数据位，无校验，2位停止位
@n     RS485_SERIAL_8E1    :8位数据位，偶校验，1位停止位
@n     RS485_SERIAL_8E1_5  :8位数据位，偶校验，1.5位停止位
@n     RS485_SERIAL_8E2    :8位数据位，偶校验，2位停止位
@n     RS485_SERIAL_8O1    :8位数据位，奇校验，1位停止位
@n     RS485_SERIAL_8O1_5  :8位数据位，奇校验，1.5位停止位
@n     RS485_SERIAL_8O2    :8位数据位，奇校验，2位停止位
@n     RS485_SERIAL_9N1    :9位数据位，无校验，1位停止位
@n     RS485_SERIAL_9N1_5  :9位数据位，无校验，1.5位停止位
@n     RS485_SERIAL_9N2    :9位数据位，无校验，2位停止位
@return 设置状态:
@n      0:  设置成功
@n      others: 设置失败
'''
def config_serial(self, baud, config):

'''
@brief 设置设备地址。
@param newAddr: 设备地址，范围1~247。
@return 设置状态:
@n      0:  设置成功
@n      others: 设置失败
'''
def set_device_address(self, newAddr):
'''
@brief 获取设备地址。
@return 返回设备地址，范围1~247。
'''
def get_device_address(self);
'''
@brief 设置18B20精度。
@return 设置状态:
@n      0:  设置成功
@n      others: 设置失败
'''
def set18B20Accuracy(self,  id,  accuracy):
'''
@brief 设置18B20精度。
@return 精度:
@n      0:  设置成功
@n      others: 设置失败
'''
def get_18B20_accuracy(self, id):
'''
@brief 设置温度的上下阈值。
@param id: 第几个温度传感器，范围0~7。
@param tH: 设置温度的上阈值，范围-55~125℃
@param tL: 设置温度的下阈值，范围-55~125℃
@n note: 必须满足设置条件tH > tL
@return 设置状态:
@n      0:  设置成功
@n      others: 设置失败
'''
def set_temperature_threshold(self,  id,  tH,  tL):
'''
@brief 获取温度的上下阈值。
@param id: 第几个温度传感器，范围0~7。
@param tH: 存储温度的上阈值
@param tL: 存储温度的下阈值
@return 设置状态:
@n      0:  设置成功
@n      others: 获取失败
'''
def get_temperature_threshold(self, id, tH, tL);
'''
@brief 获取18B20的ROM码。
@param id: 第几个温度传感器，范围0~7。
@param rom: 存放ROM码的指针。
@param len: 固定长度，必须为8字节
@return 读取状态:
@n      0:  获取成功
@n      others: 获取失败
'''
def get18B20ROM(self, id, rom, len):
  
'''
@brief 获取设备id的温度。
@param id：18B20挂载在总线上的id号，范围0~7
@return 温度:
'''
def get_temperatureC(self,  id):
'''
@brief 获取协议转换板上挂载的18B20的个数。
@return 18B20设备数量，范围0~8：
@n      0：协议转换板上未挂载18B20传感器
@n      1: 协议转换板上挂载了1个18B20传感器
@n      ......
@n      8:  协议转换板上挂载了8个18B20传感器
'''
def get_18B20_number(self):

```

## Compatibility

MCU                | SoftwareSerial | HardwareSerial |  IO   |
------------------ | :----------: | :----------: | :---------: |
Arduino Uno        |      √       |      X       |      √       |
Mega2560           |      √       |      √       |      √       |
Leonardo           |      √       |      √       |      √       |
ESP32              |      X       |      √       |      √       |
ESP8266            |      √       |      X       |      √       |
micro:bit          |      X       |      X       |      √       |
FireBeetle M0      |      X       |      √       |      √       |
raspberry          |      X       |      √       |      √       |

## History

- Data 2021-07-13
- Version V1.0

## Credits

Written by(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





