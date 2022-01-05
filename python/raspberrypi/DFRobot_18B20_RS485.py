# -*- coding:utf-8 -*-
'''
  @file DFRobot_18B20_RS485.py
  @brief 这是一款18b20单总线协议转RS485及cdc协议的协议转接板驱动库。通过它，你可以通过RS485接口配置和获取18B20传感器的相关配置和数据。
  @details TEL0144协议转换板特点：
  @n 1. 这是一个RS485从机设备，默认设备地址为0x20；
  @n 3. 板载4个18B20传感器采集IO口，分别为D1、D2、D3、D4，每个IO口最大连接4个18B20传感器；
  @n 2. 板载RS485接口，用户可以通过RS485接口配置或获取连接的18B20的传感器的配置和数据；
  @n 3. 板载USB CDC接口，用户可以通过USB CDC串口来配置和读取TEL0144的相关配置和数据； 
  @n 4. 默认串口通信配置：9600波特率, 8位数据位，无校验位，1位停止位，其中波特率可配置
  @n
  @n Hardware conneted table in UART detection mode
  @n --------------------------------------------------
  @n  board       |           raspberry pi            |
  @n --------------------------------------------------
  @n VCC          |                5V                 |
  @n GND          |                GND                |
  @n RX           |connected to the UART TX pin of MCU|
  @n TX           |connected to the UART RX pin of MCU|
  @n --------------------------------------------------
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license     The MIT License (MIT)
  @author      [Arya](xue.peng@dfrobot.com)
  @version  V1.0
  @date  2021-07-05
  @get from https://www.dfrobot.com
  @url https://github.com/DFRobot/DFRobot_18B20_RS485
'''

import sys
import serial
import time
from DFRobot_RTU import *

class DFRobot_18B20_RS485(DFRobot_RTU):
  '''!
    @brief DFRobot_18B20_RS485类
    @details 18B20转RS485协议接口python库，用于树莓派上
    @n xxx
  '''
  ## TEL0144 board register map
  REG_PID                    = 0x0000
  REG_VID                    = 0x0001
  REG_DEVICE_ADDR            = 0x0002
  REG_UART_CTRL0             = 0x0003
  REG_UART_CTRL1             = 0x0004
  REG_VERSION                = 0x0005
  REG_D1_CONFIG              = 0x0008
  REG_18B20_D1_ALARM         = 0x000C
  REG_D1_CONNECTED_FLAG      = 0x0081

  REG_18B20_NUM              = 0x0010
  REG_18B20_D1_NUM0_ROM      = 0x0011
  REG_18B20_D1_NUM0_TEMP     = 0x0051
  REG_18B20_D1_NUM0_TH_TL    = 0x0061
  REG_18B20_D1_NUM0_ACCURACY = 0x0071
  ## TEL0144 device PID
  DEVICE_PID                = 0x8090
  ## TEL0144 device VID 
  DEVICE_VID                = 0x3343 
  ## 18B20传感器的唯一标识ROM号的长度为8个字节
  DS18B20_ROM_BYTES         = 8     
  ## 协议转换板上没有18B20设备连接
  NONE_DEVICE_CONNECTED     = 0      
  ## 协议转换板上可配置的18B20采集IO口数量：4个
  DS18B20_CONNECT_IO_NUM    = 4      
  ## 每个IO口可连接的18B20传感器的最大数量为4
  DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM = 4 
  ## 协议转换板上可配置8个18B20传感器
  DS18B20_CONFIGURATION_NUM = 16 
  ## in 在温度阈值范围内
  IN_THE_TEMPERATURE_THRESHOLD            = 0 
  ## below 低于最低温度阈值
  BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD  = 1
  ## above 高于最高温度阈值
  ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD = 2
  ## 发生了错误
  OCCUR_ERROR_TEMPERATURE_THRESHOLD       = 3
  
  ## D1 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器
  eD1 = 1
  ## D2 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器
  eD2 = 2
  ## D3 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器
  eD3 = 3
  ## D4 IO引脚， 用于硬件连接18B20传感器，该IO口线上最多同时连接4个18B20传感器
  eD4 = 4

  ## 批量配置中， D1 IO引脚被选择，需要参与配置
  eBatch_D1  = 1 << 0
  ## 批量配置中， D2 IO引脚被选择，需要参与配置
  eBatch_D2  = 1 << 1
  ## 批量配置中， D3 IO引脚被选择，需要参与配置
  eBatch_D3  = 1 << 2
  ## 批量配置中， D4 IO引脚被选择，需要参与配置
  eBatch_D4  = 1 << 3
  ## 批量配置中， D1~D4所有IO引脚被选择，也可以被替换为(eBatch_D1 | eBatch_D2 | eBatch_D3 | eBatch_D4)
  eBatch_ALL = 0x0F

  ## 串口波特率2400
  eBAUDRATE_2400    = 2400
  ## 串口波特率4800
  eBAUDRATE_4800    = 4800
  ## 串口波特率9600
  eBAUDRATE_9600    = 9600
  ## 串口波特率14400
  eBAUDRATE_14400   = 14400
  ## 串口波特率19200
  eBAUDRATE_19200   = 19200
  ## 串口波特率38400
  eBAUDRATE_38400   = 38400
  ## 串口波特率57600
  eBAUDRATE_57600   = 57600
  ## 串口波特率115200
  eBAUDRATE_115200  = 115200
  
  ## 9位精度 
  e18B20_ACCURACY_9_BIT  = 0
  ## 10位精度 
  e18B20_ACCURACY_10_BIT = 1
  ## 11位精度 
  e18B20_ACCURACY_11_BIT = 2
  ## 12位精度 
  e18B20_ACCURACY_12_BIT = 3

  ## IO检测口上，序号为0的18B20传感器
  eID0 = 0
  ## IO检测口上，序号为1的18B20传感器
  eID1 = 1
  ## IO检测口上，序号为2的18B20传感器
  eID2 = 2
  ## IO检测口上，序号为3的18B20传感器
  eID3 = 3

  ## 批量配置中，协议板上id为0的18B20传感器需要配置
  eBatch_ID0 = 1 << 0
  ## 批量配置中，协议板上id为1的18B20传感器需要配置
  eBatch_ID1 = 1 << 1
  ## 批量配置中，协议板上id为2的18B20传感器需要配置
  eBatch_ID2 = 1 << 2
  ## 批量配置中，协议板上id为3的18B20传感器需要配置
  eBatch_ID3 = 1 << 3
  ## 批量配置中，协议板上所有的18B20传感器需要配置
  eBatch_ID_ALL = 0x0F  

  def __init__(self, addr, baud = 9600):
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
    self._addr = addr
    DFRobot_RTU.__init__(self, baud, 8, 'N', 1)

  def begin(self):
    '''!
      @brief TEL0144设备或广播地址类对象（地址为广播地址0的类对象）初始化。
      @return initialization state:
      @n       0: sucess
      @n      -1: failed
    '''
    time.sleep(1)
    self.set_timout_time_s(0.5)
    if self._addr > 0xF7:
      print("Invaild Device addr.")
    if self._addr != 0:
      if self._detect_device_id(self._addr) != True:
        print("Device addr Error.")
        return -1
      if self._get_pid() != self.DEVICE_PID:
        print("PID Error")
        return -1
      if self._get_vid() != self.DEVICE_VID:
        print("VID Error")
        return -1
    return 0
  
  def get_device_pid(self):
    '''!
      @brief 获取设备PID(产品标识ID),注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
      @return PID:
      @n      0:  获取失败
      @n      DEVICE_PID or 0x8090：设备PID
    '''
    return self._get_pid()

  def get_device_vid(self):
      '''!
        @brief 获取设备VID（厂商标识ID）,注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
        @return PID:
        @n      0:  获取失败
        @n      DEVICE_PID or 0x3343：设备VID
      '''
      return self._get_vid()

  def set_baudrate(self, baud):
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
    l = [0x00, 0x00]
    if baud == self.eBAUDRATE_2400:
      l[1] = 1
    elif baud == self.eBAUDRATE_4800:
      l[1] = 2
    elif baud == self.eBAUDRATE_9600:
      l[1] = 3
    elif baud == self.eBAUDRATE_14400:
      l[1] = 4
    elif baud == self.eBAUDRATE_19200:
      l[1] = 5
    elif baud == self.eBAUDRATE_38400:
      l[1] = 6
    elif baud == self.eBAUDRATE_57600:
      l[1] = 7
    elif baud == self.eBAUDRATE_115200:
      l[1] = 8
    else:
      l[1] = 3
    ret = self.write_holding_registers(self._addr, self.REG_UART_CTRL0, l)
    if ret == 0:
      return True
    return False

  def get_baudrate(self):
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
    val = self.read_holding_register(self._addr, self.REG_UART_CTRL0)
    baud = 0
    if val == 1:
      baud = 2400
    elif val == 2:
      baud = 4800
    elif val == 3:
      baud = 9600
    elif val == 4:
      baud = 14400
    elif val == 5:
      baud = 19200
    elif val == 6:
      baud = 38400
    elif val == 7:
      baud = 57600
    elif val == 8:
      baud = 115200
    else:
      baud = 9600
    return baud

  def scan(self):
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
    l = self.read_holding_registers(self._addr, self.REG_D1_CONNECTED_FLAG, 4)
    state = 0
    if (l[0] == 0) and (len(l) == 9):
      state = (l[1] & l[2]) & 0x0F         
      state |= ((l[3] & l[4]) & 0x0F) << 4
      state |= ((l[5] & l[6]) & 0x0F) << 8
      state |= ((l[7] & l[8]) & 0x0F) << 12
    return state

  def set_device_address(self, new_addr):
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
    if new_addr < 1 or new_addr > 0xF7:
      print("Error id")
    ret = self.write_holding_register(self._addr, self.REG_DEVICE_ADDR, new_addr)
    if(self._addr == 0):
      return True
    if ret == 0:
      self._addr = new_addr
      return True
    return False

  def get_device_address(self):
    '''!
      @brief 获取设备地址。
      @return 设备地址（1~247范围内的一个地址）或广播地址（0）。
    '''
    return self._addr

  def batch_set_18b20_accuracy(self, batch_io, batch_id, accuracy):
    '''
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
    batch_io &= 0x0F
    batch_id &= 0x0F
    if batch_io == 0:
      print("batchIo param is zero.")
      return False
    if batch_id == 0:
      print("batch_id param error.")
      return False
    if accuracy > self.e18B20_ACCURACY_12_BIT:
      print("accuracy out of range(0~3): %d"%accuracy)
      return False

    temp = [0]*8
    id = 0
    size = 0
    ret = 0
    batch_id &= 0xFF
    i = 0
    while i < self.DS18B20_CONNECT_IO_NUM:
      if(batch_io & (1 << i)):
        j = 0
        while j <= self.DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM:
          if (j < self.DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM) and (batch_id & (1 << i)):
            if size == 0:
              id = i
            temp[size] = 0x00
            temp[size + 1] = accuracy & 0xFF
            size += 2
          else:
            if size != 0:
              ret = self.write_holding_registers(self._addr, self.REG_18B20_D1_NUM0_ACCURACY + 4*i + id, temp[:size])
              size = 0
              if ret != 0:
                print("batch_id = ",bin(batch_id),"i - 1 = %d"%(i-1))
                print("Set Accuracy Error.")
                return False
          j += 1
      i += 1
    return True

  def set_18B20_accuracy(self, io, id, accuracy):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return False
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return False
    ret = self.write_holding_register(self._addr, self.REG_18B20_D1_NUM0_ACCURACY + (io - 1)*4 + id, accuracy)
    if ret != 0:
      print("set accuracy error.")
      return False
    return True
  
  def get_18B20_accuracy(self, io, id):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return 0xFFFF
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return 0xFFFF
    accuracy = self.read_holding_register(self._addr, self.REG_18B20_D1_NUM0_ACCURACY + (io - 1)*4 +id)
    return accuracy

  def batch_set_18b20_temperature_threshold(self, batch_io, batch_id, th, tl):
    '''
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
    batch_io &= 0x0F
    batch_id &= 0x0F
    if batch_io == 0:
      print("batchIo param is zero.")
      return False
    if batch_id == 0:
      print("batch_id param error.")
      return False
    if (th > 125) or (tl < -55) or (th < tl) or (th == tl):
      print("th, tl params error: th(-55~125)=%d, tl(-55~125)=%d"%(th,tl))
      return False
    if th < 0:
      th = 256 + th
    if tl < 0:
      tl = 256 + tl
    temp = [0]*8
    id = 0
    size = 0
    ret = 0
    batch_id &= 0xFF
    i = 0
    while i < self.DS18B20_CONNECT_IO_NUM:
      if(batch_io & (1 << i)):
        j = 0
        while j <= self.DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM:
          if (j < self.DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM) and (batch_id & (1 << j)):
            if size == 0:
              id = j
            temp[size] = th
            temp[size + 1] = tl
            size += 2
          else:
            if size != 0:
              ret = self.write_holding_registers(self._addr, self.REG_18B20_D1_NUM0_TH_TL + 4*i + id, temp[:size])
              size = 0
              if ret != 0:
                print("batch_id = ",bin(batch_id),"i = %d, j=%d ret=%d"%(i,j, ret))
                print("Set batch threshold Error.")
                return False
          j += 1
      i += 1
    return True

  def set_temperature_threshold(self, io, id, th, tl):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return False
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return False
    if (th > 125) or (tl < -55) or (th < tl) or (th == tl):
      print("th, tl params error: th(-55~125)=%d, tl(-55~125)=%d"%(th,tl))
      return False
    
    if th < 0:
      th = 256 - th
    if tl < 0:
      tl = 256 - tl
    val = (th << 8) | tl
    ret = self.write_holding_register(self._addr, self.REG_18B20_D1_NUM0_TH_TL + (io - 1)*4 + id, val)
    if ret != 0:
      print("Set threshold failed,ret=%d"%ret)
      return False
    return True

  def get_temperature_threshold(self, io, id):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return False
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return False
    val = self.read_holding_register(self._addr, self.REG_18B20_D1_NUM0_TH_TL + (io - 1)*4 + id)
    
    th = (val >> 8) & 0xFF
    tl = val  & 0xFF
    if th & 0x80:
      th = -(((~(th & 0x7F)) + 1) & 0x7F)
    if tl & 0x80:
      tl = -(((~(tl & 0x7F)) + 1) & 0x7F)
    return [th, tl]
  
  def get_temperature_threshold_alarm_flag(self):
    '''
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
    state = 0
    temp = self.read_holding_registers(self._addr, self.REG_18B20_D1_ALARM, 4)
    if temp[0] == 0 and len(temp) == 9:
      state =  (temp[1] & 0x0F) | (temp[2] << 4)
      state |= (((temp[3] & 0x0F) | (temp[4] << 4)) << 8)
      state |= (((temp[5] & 0x0F) | (temp[6] << 4)) << 16)
      state |= (((temp[7] & 0x0F) | (temp[8] << 4)) << 24)

    return state

  def parse_threshold_alarm_flag(self, io, id, alarm_flag):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return OCCUR_ERROR_TEMPERATURE_THRESHOLD
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return OCCUR_ERROR_TEMPERATURE_THRESHOLD
    temp =  (alarm_flag >> (8*(io - 1))) & 0xFF
    if (temp & (1 << id)):
      if(temp & (1 << (4+id))):
        return self.ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD
      return self.BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD
    return self.IN_THE_TEMPERATURE_THRESHOLD

  def get_18B20_number(self, batch_io = eBatch_ALL):
    '''
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
    num = 0
    batch_io &= 0x0F
    if batch_io == 0:
      print("batchIo param is zero.")
      return 0
    if(batch_io == self.eBatch_ALL):
      num = self.read_holding_register(self._addr, self.REG_18B20_NUM)
      return num
    temp = self.read_holding_registers(self._addr, self.REG_D1_CONFIG, 4)
    if temp[0] == 0 and len(temp) == 9:
      i = 0
      while i < self.DS18B20_CONNECT_IO_NUM:
        if (batch_io & (1 << i)):
          num += (temp[1 + 2*i + 1] >> 4) & 0x0F
        i += 1
      return num
    return num

  def get_temperature_c(self, io, id):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return 0
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return 0
    val = self.read_holding_register(self._addr, self.REG_18B20_D1_NUM0_TEMP + 4*(io - 1) + id)
    return val/16.0
  
  def get_18B20_rom(self, io, id):
    '''
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
    if io > self.eD4 and io < self.eD1:
      print("io is out of range(1~4)")
      return [0]*8
    if id < self.eID0 and id > self.eID3:
      print("id is out of range(0~3):id=%d"%id)
      return [0]*8
    ret = self.read_holding_registers(self._addr, self.REG_18B20_D1_NUM0_ROM + (io-1)*16 + id*4, 4)
    if ret[0] == 0:
      return ret[1:]
    return [0]*8
  
  def get_rom_hex_string(self,rom):
    '''
      @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
      @n 字符串为28AAAD385414016A。
      @param rom: 长度为8的ROM列表。
      @return ROM码的16进制字符串或空字符串:
    '''
    rslt = ""
    code = ""
    hexarr = ['0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F']
    n = 0 
    i = 0
    if len(rom) != 8:
      return rslt
    while i < 8:
      n = rom[i]
      i += 1
      if n == 0:
        rslt += "00"
        continue
      code = ""
      while n:
        j = n % 16
        code = hexarr[j] + code
        n = n // 16
      if (len(code) < 2):
        code = '0' + code
      rslt += code
      
    return rslt

  def _get_pid(self):
    val = self.read_holding_register(self._addr, self.REG_PID)
    return val

  def _get_vid(self):
    val = self.read_holding_register(self._addr, self.REG_VID)
    return val

  def _detect_device_id(self, addr):
    val = self.read_holding_register(self._addr, self.REG_DEVICE_ADDR)
    if (val & 0xFF) == addr:
      return True
    return False


