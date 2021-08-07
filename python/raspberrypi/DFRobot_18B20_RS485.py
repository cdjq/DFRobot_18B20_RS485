# -*- coding:utf-8 -*-
'''
  @file DFRobot_18B20_RS485.py
  @brief 这是一款18b20单总线协议转RS485及UART协议的协议转接板驱动库。使用它，你可以通过Arduino 主控的RS485和UART接口驱动18B20
  @n 传感器。这块转换板有以下特点：
  @n 1. 最大能同时连接8个18B20传感器;
  @n 2. 外部主机可通过UART接口或RS485接口同传感器通信;
  @n 3. 协议转换板之间支持RS485级联 
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
  @licence     The MIT License (MIT)
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
  '''TEL0144 board register map'''
  REG_PID                  = 0x0000
  REG_VID                  = 0x0001
  REG_DEVICE_ADDR          = 0x0002
  REG_UART_CTRL0           = 0x0003
  REG_UART_CTRL1           = 0x0004
  REG_VERSION              = 0x0005
  REG_ALARM                = 0x0008
  REG_ROM_FLAG             = 0x0009
  REG_18B20_NUM            = 0x000A
  REG_18B20_NUM0_ROM       = 0x000B
  REG_18B20_NUM0_TEMP      = 0x002B
  REG_18B20_NUM0_TH_TL     = 0x0033
  REG_18B20_NUM0_ACCURACY  = 0x003B
  
  DEVICE_PID                = 0x8090 #TEL0144 device PID
  DEVICE_VID                = 0x3343 #TEL0144 device VID
  DS18B20_ROM_BYTES         = 8      #18B20传感器的唯一标识ROM号的长度为8个字节
  NONE_DEVICE_CONNECTED     = 0      #协议转换板上没有18B20设备连接
  SENSOR_CONNECTED_MAX_NUM  = 8      #一个协议转换板上最多可连接8个18B20传感器
  DS18B20_CONFIGURATION_NUM = 8      #协议转换板上可配置8个18B20传感器

  IN_THE_TEMPERATURE_THRESHOLD            = 0  #in 在温度阈值范围内
  BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD  = 1  #below 低于最低温度阈值
  ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD = 2  #above 高于最高温度阈值
  
  '''enum baudrate'''
  eBAUDRATE_2400    = 2400   #串口波特率2400
  eBAUDRATE_4800    = 4800   #串口波特率4800 
  eBAUDRATE_9600    = 9600   #串口波特率9600 
  eBAUDRATE_14400   = 14400  #串口波特率14400 
  eBAUDRATE_19200   = 19200  #串口波特率19200 
  eBAUDRATE_38400   = 38400  #串口波特率38400 
  eBAUDRATE_57600   = 57600  #串口波特率57600 
  eBAUDRATE_115200  = 115200 #串口波特率115200 
  
  '''enum accuracy'''
  e18B20_ACCURACY_9_BIT  = 0 # 9位精度 
  e18B20_ACCURACY_10_BIT = 1 # 10位精度 
  e18B20_ACCURACY_11_BIT = 2 # 11位精度 
  e18B20_ACCURACY_12_BIT = 3 # 12位精度 

  '''enum 18b20 ID '''
  e18B20_ID0    = 0 #协议板上id为0的18B20传感器
  e18B20_ID1    = 1 #协议板上id为1的18B20传感器
  e18B20_ID2    = 2 #协议板上id为2的18B20传感器
  e18B20_ID3    = 3 #协议板上id为3的18B20传感器
  e18B20_ID4    = 4 #协议板上id为4的18B20传感器
  e18B20_ID5    = 5 #协议板上id为5的18B20传感器
  e18B20_ID6    = 6 #协议板上id为6的18B20传感器
  e18B20_ID7    = 7 #协议板上id为7的18B20传感器
  e18B20_ID_MAX = 8 #协议板上能配置的18B20传感器的最大数量

  '''enum 18b20 batch ID '''
  eBatch_ID0     = 1 << 0 #批量配置中，协议板上id为0的18B20传感器需要配置
  eBatch_ID1     = 1 << 1 #批量配置中，协议板上id为1的18B20传感器需要配置
  eBatch_ID2     = 1 << 2 #批量配置中，协议板上id为2的18B20传感器需要配置
  eBatch_ID3     = 1 << 3 #批量配置中，协议板上id为3的18B20传感器需要配置
  eBatch_ID4     = 1 << 4 #批量配置中，协议板上id为4的18B20传感器需要配置
  eBatch_ID5     = 1 << 5 #批量配置中，协议板上id为5的18B20传感器需要配置
  eBatch_ID6     = 1 << 6 #批量配置中，协议板上id为6的18B20传感器需要配置
  eBatch_ID7     = 1 << 7 #批量配置中，协议板上id为7的18B20传感器需要配置
  eBatch_ID_ALL  = 0xFF   #批量配置中，协议板上所有的18B20传感器需要配置

  '''
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
    self._addr = addr
    DFRobot_RTU.__init__(self, baud, 8, 'N', 1)

  '''
    @brief TEL0144设备或广播地址类对象（地址为广播地址0的类对象）初始化。
    @return initialization state:
    @n       0: sucess
    @n      -1: failed
  '''
  def begin(self):
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
  
  '''
    @brief 获取设备PID(产品标识ID),注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
    @return PID:
    @n      0:  获取失败
    @n      DEVICE_PID or 0x8090：设备PID
  '''
  def get_device_pid(self):
      return self._get_pid()

  '''
    @brief 获取设备VID（厂商标识ID）,注意此功能只有地址不为广播地址的类对象可以使用，广播地址类对象调用该方法无效。
    @return PID:
    @n      0:  获取失败
    @n      DEVICE_PID or 0x3343：设备VID
  '''
  def get_device_vid(self):
      return self._get_vid()

  '''
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

  '''
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

  '''
    @brief 每块TEL0144设备最多能连接8个18B20温度传感器，并为每个传感器分配序号为0~7的id,以此来保存对应18B20传感器的相关配置，比如id为0的18B20的ROM码，精度，上下阈值和测量的温度
    @n 会保存在0号id对应的寄存器中，id为1的保存在1号id对应的寄存器中。这个函数的作用就是检查0~7序号的id哪些连接了18B20传感器，在采集中，从而可以知道id号来获取相应18B20传感器的配置。
    @n
    @n 18B20的id分配原理：1.序号从0~7，即第一个连接的18B20传感器，分配的id序号为0，接下来的依次分配，直到0~7号id全部被分配；
    @n                   2.新接入的传感器，先分配没有分配过的id，如果0~7都被分配完了，再从头查找是否有id号被分配，但是分配的那个传感器已掉线，没有使用。例：0号id已被分配，此时把0
    @n                     号id对应的传感器拔掉，新插入传感器，新传感器将从1开始分配，直到7被分配完了，再占用已经被分配的id0。
    @n                   3.传感器掉线后，再次连接，如果它的id没有被占用，将继续使用以前给他分配的id号。
    @n
    @return 返回8位状态位，从bit0~bit7分别代表0~7序号的id对应的18B20传感器是否存在，置1代表对应的序号已被分配且传感器在线，清0代表该序号未被分配或已被分配但该传感器离线，无法检测。
    @n note: 此功能不支持广播地址类对象
  '''
  def scan(self):
    l = self.read_holding_registers(self._addr, self.REG_ROM_FLAG, 1)
    state = 0
    if (l[0] == 0) and (len(l) == 3):
      state = l[1] & l[2]
    return state

  '''
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
    if new_addr < 1 or new_addr > 0xF7:
      print("Error id")
    ret = self.write_holding_register(self._addr, self.REG_DEVICE_ADDR, new_addr)
    if(self._addr == 0):
      return True
    if ret == 0:
      self._addr = new_addr
      return True
    return False

  '''
    @brief 获取设备地址。
    @return 设备地址（1~247范围内的一个地址）或广播地址（0）。
  '''
  def get_device_address(self):
     return self._addr

  '''
    @brief 批量设置18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT，掉电保存，配置不丢失。
    @param batchId: 18B20传感器ID集合
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n | eBatch_ID7 | eBatch_ID6 | eBatch_ID5 | eBatch_ID4 | eBatch_ID3 | eBatch_ID2 | eBatch_ID1 | eBatch_ID0 |
    @n |   1 << 7   |   1 << 6   |   1 << 5   |   1 << 4   |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                         eBatch_ID_ALL                                                 |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_ID0     or 1 << 0 : 批量配置中，协议板上id为0的18B20传感器需要配置
    @n     eBatch_ID1     or 1 << 1 : 批量配置中，协议板上id为1的18B20传感器需要配置
    @n     eBatch_ID2     or 1 << 2 : 批量配置中，协议板上id为2的18B20传感器需要配置
    @n     eBatch_ID3     or 1 << 3 : 批量配置中，协议板上id为3的18B20传感器需要配置
    @n     eBatch_ID4     or 1 << 4 : 批量配置中，协议板上id为4的18B20传感器需要配置
    @n     eBatch_ID5     or 1 << 5 : 批量配置中，协议板上id为5的18B20传感器需要配置
    @n     eBatch_ID6     or 1 << 6 : 批量配置中，协议板上id为6的18B20传感器需要配置
    @n     eBatch_ID7     or 1 << 7 : 批量配置中，协议板上id为7的18B20传感器需要配置
    @n     eBatch_ID_ALL  or  0xFF  : 批量配置中，协议板上所有的18B20传感器需要配置
    @param accuracy：精度设置
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @return 设置状态
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def batch_set_18b20_accuracy(self, batch_id, accuracy):
    if accuracy > self.e18B20_ACCURACY_12_BIT:
      print("accuracy out of range(0~3): %d"%accuracy)
      return False
    if batch_id == 0:
      print("batch_id param error.")
      return False
    temp = [0]*16
    id = 0
    size = 0
    ret = 0
    batch_id &= 0xFF
    i = 0
    while i <= self.SENSOR_CONNECTED_MAX_NUM:
      if (i != self.SENSOR_CONNECTED_MAX_NUM) and ((batch_id & (1 << i))):
        if size == 0:
          id = i
        temp[size] = 0x00
        temp[size + 1] = accuracy & 0xFF
        size += 2
      else:
        if size != 0:
          ret = self.write_holding_registers(self._addr, self.REG_18B20_NUM0_ACCURACY+id, temp[:size])
          size = 0
          if ret != 0:
            print("batch_id = ",bin(batch_id),"i - 1 = %d"%(i-1))
            print("Set Accuracy Error.")
            return False
      i += 1
    return True

  '''
    @brief 设置序号为id的18B20温度传感器的精度，如果不修改，默认精度为e18B20_ACCURACY_12_BIT，掉电保存，配置不丢失。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7:  协议板上id为7的18B20传感器
    @param accuracy：精度设置,范围0~3
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_18B20_accuracy(self, id, accuracy):
    if (id < 0) or (id >= self.SENSOR_CONNECTED_MAX_NUM):
      print("id out of range(0~7): id=%d"%id)
      return False
    ret = self.write_holding_register(self._addr, self.REG_18B20_NUM0_ACCURACY+id, accuracy)
    if ret == accuracy:
      return True
    return False
  
  '''
    @brief 获取序号为id的18B20温度传感器的精度。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7:  协议板上id为7的18B20传感器
    @return accuracy：精度设置,范围0~3
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @n      0xFF                       :  获取精度失败
  '''
  def get_18B20_accuracy(self, id):
    if (id < 0) or (id >= self.SENSOR_CONNECTED_MAX_NUM):
      print("id out of range(0~7): id=%d"%id)
      return 0xFF
    accuracy = self.read_holding_register(self._addr, self.REG_18B20_NUM0_ACCURACY+id)
    accuracy = accuracy & 0xFF
    return accuracy

  '''
    @brief 批量设置18B20温度传感器的温度阈值，如果不修改，默认配置为高阈值125℃，低阈值-55℃，掉电保存，配置不丢失。
    @param batchId: 18B20传感器ID集合
    @n ---------------------------------------------------------------------------------------------------------
    @n |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
    @n | eBatch_ID7 | eBatch_ID6 | eBatch_ID5 | eBatch_ID4 | eBatch_ID3 | eBatch_ID2 | eBatch_ID1 | eBatch_ID0 |
    @n |   1 << 7   |   1 << 6   |   1 << 5   |   1 << 4   |   1 << 3   |   1 << 2   |   1 << 1   |   1 << 0   |
    @n |                                         eBatch_ID_ALL                                                 |
    @n ---------------------------------------------------------------------------------------------------------
    @n     eBatch_ID0     or 1 << 0 : 批量配置中，协议板上id为0的18B20传感器需要配置
    @n     eBatch_ID1     or 1 << 1 : 批量配置中，协议板上id为1的18B20传感器需要配置
    @n     eBatch_ID2     or 1 << 2 : 批量配置中，协议板上id为2的18B20传感器需要配置
    @n     eBatch_ID3     or 1 << 3 : 批量配置中，协议板上id为3的18B20传感器需要配置
    @n     eBatch_ID4     or 1 << 4 : 批量配置中，协议板上id为4的18B20传感器需要配置
    @n     eBatch_ID5     or 1 << 5 : 批量配置中，协议板上id为5的18B20传感器需要配置
    @n     eBatch_ID6     or 1 << 6 : 批量配置中，协议板上id为6的18B20传感器需要配置
    @n     eBatch_ID7     or 1 << 7 : 批量配置中，协议板上id为7的18B20传感器需要配置
    @n     eBatch_ID_ALL  or  0xFF  : 批量配置中，协议板上所有的18B20传感器需要配置
    @param th: 设置温度的上阈值，范围-55~125℃
    @param tl: 设置温度的下阈值，范围-55~125℃
    @n note: 必须满足设置条件th > tl
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def batch_set_18b20_temperature_threshold(self, batch_id, th, tl):
    if (th > 125) or (tl < -55) or (th < tl) or (th == tl):
      print("th, tl params error: th(-55~125)=%d, tl(-55~125)=%d"%(th,tl))
      return False
    if batch_id == 0:
      print("batch_id param error.")
      return False
    if th < 0:
      th = 256 + th
    if tl < 0:
      tl = 256 + tl
    temp = [0]*16
    id = 0
    size = 0
    ret = 0
    batch_id &= 0xFF
    i = 0
    while i <= self.SENSOR_CONNECTED_MAX_NUM:
      flag = batch_id & (1 << i)
      if ((i != self.SENSOR_CONNECTED_MAX_NUM) and (flag > 0)):
        if size == 0:
          id = i
        temp[size] = th
        temp[size + 1] = tl
        size += 2
      else:
        if size != 0:
          ret = self.write_holding_registers(self._addr, self.REG_18B20_NUM0_TH_TL+id, temp[:size])
          size = 0
          if ret != 0:
            print("batch_id = ",bin(batch_id),"i - 1 = %d"%(i-1))
            print("Set threshold Error.")
            return False
      i += 1
      
    return True

  '''
    @brief 设置序号为id的温度传感器的上下温度阈值，如果不修改，默认配置为高阈值125℃，低阈值-55℃，掉电保存，配置不丢失。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7:  协议板上id为7的18B20传感器
    @param th: 设置温度的上阈值，范围-55~125℃
    @param tl: 设置温度的下阈值，范围-55~125℃
    @n note: 必须满足设置条件th > tl
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_temperature_threshold(self, id, th, tl):
    if (th > 125) or (tl < -55) or (th < tl) or (th == tl):
      print("th, tl params error: th(-55~125)=%d, tl(-55~125)=%d"%(th,tl))
      return False
    
    if id >= self.e18B20_ID_MAX:
      print("ID out of range(0~7): id=%d"%id)
      return False
    if th < 0:
      th = 256 - th
    if tl < 0:
      tl = 256 - tl
    val = (th << 8) | tl
    ret = self.write_holding_register(self._addr, self.REG_18B20_NUM0_TH_TL+id, val)
    if ret != 0:
      print("Set addr failed,ret=%d"%ret)
      return False
    return True

  '''
    @brief 获取序号为id的温度传感器的上下温度阈值。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
    @return 长度为2的整型列表:
    @n      列表索引0:  存储温度的上阈值
    @n      列表索引1:  存储温度的下阈值
  '''
  def get_temperature_threshold(self, id):
    if (id < 0) or (id >= self.SENSOR_CONNECTED_MAX_NUM):
      print("id out of range(0~7): id=%d"%id)
      return False
    val = self.read_holding_register(self._addr, self.REG_18B20_NUM0_TH_TL+id)
    
    th = (val >> 8) & 0xFF
    tl = val  & 0xFF
    if th & 0x80:
      th = -(((~(th & 0x7F)) + 1) & 0x7F)
    if tl & 0x80:
      tl = -(((~(tl & 0x7F)) + 1) & 0x7F)
    return [th, tl]
  
  '''
    @brief 获取18B20传感器温度阈值报警标志。
    @return 8个18B20当前温度阈值报警的标志位:
    @n -------------------------------------------------------------------------------------------------------------------------
    @n |     bit15    |     bit14    |     bit13    |     bit12    |     bit11    |     bit10    |     bit9     |     bit8     |
    @n -------------------------------------------------------------------------------------------------------------------------
    @n | 18b20_HL_id7 | 18b20_HL_id6 | 18b20_HL_id5 | 18b20_HL_id4 | 18b20_HL_id3 | 18b20_HL_id2 | 18b20_HL_id1 | 18b20_HL_id0 |
    @n |                                0:低于最低阈值报警， 1高于最高温度阈值报警                                                |
    @n -------------------------------------------------------------------------------------------------------------------------
    @n |     bit7     |     bit6     |     bit5     |     bit4     |     bit3     |     bit2     |     bit1     |     bit0     |
    @n -------------------------------------------------------------------------------------------------------------------------
    @n |  18b20_F_id7 |  18b20_F_id6 |  18b20_F_id5 |  18b20_F_id4 |  18b20_F_id3 |  18b20_F_id2 |  18b20_F_id1 |  18b20_F_id0 |
    @n |                                    18B20是否发生阈值报警标志位，0：没有发生阈值报警，1发生了阈值报警                       |
    @n -------------------------------------------------------------------------------------------------------------------------
    @n  低8位0~7：代表id0~id7位置的18B20传感器是否发生阈值报警标志位，0：温度在阈值范围内，未发生阈值报警，1：温度在阈值范围外，发生了阈值报警
    @n  高8位8~15：代表如果发生了阈值报警，是高于最高温度阈值，还是低于最低温度阈值，高8位只有在低8位对印位为1才会生效，0低于最低阈值，1高于最高阈值。
  '''
  def get_temperature_threshold_alarm_flag(self):
    return self.read_holding_register(self._addr, self.REG_ALARM)

  '''
    @brief 解析id对应的18B20温度传感器的温度相对于温度阈值范围的状态。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
    @param alarm_flag: 传入get_temperature_threshold_alarm_flag函数的返回值
    @return id所对应的18B20的温度相对于温度阈值的状态:
    @n      IN_THE_TEMPERATURE_THRESHOLD            or 0:   in 在温度阈值范围内
    @n      BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD  or 1:   below 低于最低温度阈值
    @n      ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD or 2:   above 高于最高温度阈值
  '''
  def parse_threshold_alarm_flag(self, id, alarm_flag):
    if ((alarm_flag >> id) & 0x01):
      if((alarm_flag >> (8+id)) & 0x01):
        return self.ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD
      return self.BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD
    return self.IN_THE_TEMPERATURE_THRESHOLD

  '''
    @brief 获取协议转换板上实际连接的18B20的数量。
    @return 18B20设备数量，范围0~8：
    @n      0：协议转换板上未连接18B20传感器
    @n      1: 协议转换板上连接了1个18B20传感器
    @n      ......
    @n      8:  协议转换板上连接了8个18B20传感器
  '''
  def get_18B20_number(self):
    val = self.read_holding_register(self._addr, self.REG_18B20_NUM)
    return val

  '''
    @brief 获取序号为id的18B20的温度数据，单位：摄氏度(℃)。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
    @return 温度:单位摄氏度，可以测量-55~125摄氏度范围内的温度
  '''
  def get_temperature_c(self, id):
    if (id < 0) or (id >= self.SENSOR_CONNECTED_MAX_NUM):
      print("id out of range(0~7): id=%d"%id)
      return 0.0
    val = self.read_holding_register(self._addr, self.REG_18B20_NUM0_TEMP+id)
    return val/16.0
  
  '''
    @brief 获取序号为id的温度传感器的ROM码。
    @param id: 范围0~7，依次对应0~7号DS18B20传感器
    @n     e18B20_ID0 or 0: 协议板上id为0的18B20传感器
    @n     e18B20_ID1 or 1: 协议板上id为1的18B20传感器
    @n     e18B20_ID2 or 2: 协议板上id为2的18B20传感器
    @n     e18B20_ID3 or 3: 协议板上id为3的18B20传感器
    @n     e18B20_ID4 or 4: 协议板上id为4的18B20传感器
    @n     e18B20_ID5 or 5: 协议板上id为5的18B20传感器
    @n     e18B20_ID6 or 6: 协议板上id为6的18B20传感器
    @n     e18B20_ID7 or 7: 协议板上id为7的18B20传感器
    @return 长度为8的ROM码列表，全为0代表获取失败或该id未被分配给18B20传感器
  '''
  def get_18B20_rom(self, id):
    if (id < 0) or (id >= self.SENSOR_CONNECTED_MAX_NUM):
      print("id out of range(0~7): id=%d"%id)
      return [0]*8
    ret = self.read_holding_registers(self._addr, self.REG_18B20_NUM0_ROM+id*4, 4)
    if ret[0] == 0:
      return ret[1:]
    return [0]*8
  
  '''
    @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
    @n 字符串为28AAAD385414016A。
    @param rom: 长度为8的ROM列表。
    @return ROM码的16进制字符串或空字符串:
  '''
  def get_rom_hex_string(self,rom):
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


