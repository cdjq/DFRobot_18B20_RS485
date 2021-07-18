# -*- coding:utf-8 -*-
'''
  @file DFRobot_18B20_RS485.py
  @brief 这是一款18b20单总线协议转RS485及UART协议的协议转接板驱动库。使用它，你可以通过Arduino 主控的RS485和UART接口驱动18B20
  @n 传感器。这块转换板有以下特点：
  @n 1. 最大能同时连接8个18B20传感器;
  @n 2. Arduino MCU通过UART接口或RS485接口同传感器通信。
  @n Hardware conneted table in UART detection mode
  @n --------------------------------------------------
  @n  Sensor      |           raspberry pi            |
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

  REG_PID                  = 0x0000
  REG_VID                  = 0x0001
  REG_DEVICE_ADDR          = 0x0002
  REG_UART_CTRL0           = 0x0003
  REG_UART_CTRL1            = 0x0004
  REG_VERSION              = 0x0005
  REG_ALARM                = 0x0008
  REG_ROM_FLAG             = 0x0009
  REG_18B20_NUM            = 0x000A
  REG_18B20_NUM0_ADDR      = 0x000B
  REG_18B20_NUM1_ADDR      = 0x000F
  REG_18B20_NUM2_ADDR      = 0x0013
  REG_18B20_NUM3_ADDR      = 0x0017
  REG_18B20_NUM4_ADDR      = 0x001B
  REG_18B20_NUM5_ADDR      = 0x001F
  REG_18B20_NUM6_ADDR      = 0x0023
  REG_18B20_NUM7_ADDR      = 0x0027
  REG_18B20_NUM0_TEMP      = 0x002B
  REG_18B20_NUM1_TEMP      = 0x002C
  REG_18B20_NUM2_TEMP      = 0x002D
  REG_18B20_NUM3_TEMP      = 0x002E
  REG_18B20_NUM4_TEMP      = 0x002F
  REG_18B20_NUM5_TEMP      = 0x0030
  REG_18B20_NUM6_TEMP      = 0x0031
  REG_18B20_NUM7_TEMP      = 0x0032
  REG_18B20_NUM0_TH_TL     = 0x0033
  REG_18B20_NUM1_TH_TL     = 0x0034
  REG_18B20_NUM2_TH_TL     = 0x0035
  REG_18B20_NUM3_TH_TL     = 0x0036
  REG_18B20_NUM4_TH_TL     = 0x0037
  REG_18B20_NUM5_TH_TL     = 0x0038
  REG_18B20_NUM6_TH_TL     = 0x0039
  REG_18B20_NUM7_TH_TL     = 0x003A
  REG_18B20_NUM0_ACCURACY  = 0x003B
  REG_18B20_NUM1_ACCURACY  = 0x003C
  REG_18B20_NUM2_ACCURACY  = 0x003D
  REG_18B20_NUM3_ACCURACY  = 0x003E
  REG_18B20_NUM4_ACCURACY  = 0x003F
  REG_18B20_NUM5_ACCURACY  = 0x0040
  REG_18B20_NUM6_ACCURACY  = 0x0041
  REG_18B20_NUM7_ACCURACY  = 0x0042
  
  DEVICE_PID            = 0x8090
  DEVICE_VID            = 0x3343
  NO_DEVICE_NUM         = 0
  BROADCAST_ADDRESS     = 0x00
  DS18B20_ROM_SIZE      = 8
  DEFAULT_DEVICE_ADDRESS= 0x20
  DS18B20_MAX_NUM       =8
  
  
  BAUDRATE_2400    = 2400
  BAUDRATE_4800    = 4800
  BAUDRATE_9600    = 9600
  BAUDRATE_14400   = 14400
  BAUDRATE_19200   = 19200
  BAUDRATE_38400   = 38400
  BAUDRATE_57600   = 57600
  BAUDRATE_115200  = 115200
  
  RS485_SERIAL_7E1    = (0<<6) | (1<<3) | (0 << 0)
  RS485_SERIAL_7E1_5  = (0<<6) | (1<<3) | (1 << 0)
  RS485_SERIAL_7E2    = (0<<6) | (1<<3) | (2 << 0)
  RS485_SERIAL_7O1    = (0<<6) | (2<<3) | (0 << 0)
  RS485_SERIAL_7O1_5  = (0<<6) | (2<<3) | (1 << 0)
  RS485_SERIAL_7O2    = (0<<6) | (2<<3) | (2 << 0)
  RS485_SERIAL_8N1    = (1<<6) | (0<<3) | (0 << 0)
  RS485_SERIAL_8N1_5  = (1<<6) | (0<<3) | (1 << 0)
  RS485_SERIAL_8N2    = (1<<6) | (0<<3) | (2 << 0)
  RS485_SERIAL_8E1    = (1<<6) | (1<<3) | (0 << 0)
  RS485_SERIAL_8E1_5  = (1<<6) | (1<<3) | (1 << 0)
  RS485_SERIAL_8E2    = (1<<6) | (1<<3) | (2 << 0)
  RS485_SERIAL_8O1    = (1<<6) | (2<<3) | (0 << 0)
  RS485_SERIAL_8O1_5  = (1<<6) | (2<<3) | (1 << 0)
  RS485_SERIAL_8O2    = (1<<6) | (2<<3) | (2 << 0)
  RS485_SERIAL_9N1    = (2<<6) | (0<<3) | (0 << 0)
  RS485_SERIAL_9N1_5  = (2<<6) | (0<<3) | (1 << 0)
  RS485_SERIAL_9N2    = (2<<6) | (0<<3) | (2 << 0)
  
  DS18B20_NUM0_ID    = 0
  DS18B20_NUM1_ID    = 1
  DS18B20_NUM2_ID    = 2
  DS18B20_NUM3_ID    = 3
  DS18B20_NUM4_ID    = 4
  DS18B20_NUM5_ID    = 5
  DS18B20_NUM6_ID    = 6
  DS18B20_NUM7_ID    = 7
  
  DS18B20_ACCURACY_9_BIT  = 0
  DS18B20_ACCURACY_10_BIT = 1
  DS18B20_ACCURACY_11_BIT = 2
  DS18B20_ACCURACY_12_BIT = 3

  def __init__(self, addr, baud = 9600, bits = 8, parity = 'N', stopbit = 1):
    self._addr = addr
    DFRobot_RTU.__init__(self, baud, bits, parity, stopbit)

  '''
    @brief 传感器设备初始化。
    @return initialization state:
    @n       0: sucess
    @n      -1: failed
  '''
  def begin(self):
    time.sleep(1)
    if self._addr > 0xF7:
      print("Invaild Device addr.")
    if self._addr != 0:
      if !self._detect_device_id(self._addr):
        print("Device addr Error.")
        return -1
      if self._get_pid() != self.DEVICE_VID:
        print("PID Error")
        return -1
      if self._get_vid() != self.DEVICE_VID:
        print("VID Error")
        return -1
    return 0

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
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def config_serial(self, baud, config):
    l = [0x00, 0x00, (config >> 8) & 0xFF, config & 0xFF]
    if baud == self.BAUDRATE_2400:
      l[1] = 1
    elif baud == self.BAUDRATE_4800:
      l[1] = 2
    elif baud == self.BAUDRATE_9600:
      l[1] = 3
    elif baud == self.BAUDRATE_14400:
      l[1] = 4
    elif baud == self.BAUDRATE_19200:
      l[1] = 5
    elif baud == self.BAUDRATE_38400:
      l[1] = 6
    elif baud == self.BAUDRATE_57600:
      l[1] = 7
    elif baud == self.BAUDRATE_115200:
      l[1] = 8
    else:
      l[1] = 3
    ret = self.write_holding_registers(self._addr, self.REG_UART_CTRL0, l)
    if ret == 0:
      return True
    return False

  '''
    @brief 获取串口波特率配置
    @return 返回串口波特率配置
  '''
  def get_serial_baud(self):
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
    @brief 获取串口数据位、校验位、停止位配置
    @return 返回串口数据位、校验位、停止位配置
  '''
  def get_serial_config(self):
    val = self.read_holding_register(self._addr, self.REG_UART_CTRL1)
    return val

  '''
    @brief 扫描位置0~7是否挂载18B20传感器。协议转换板最多能挂载8个18B20传感器，分别对应0~7的18B20配置
    @return 返回8位状态位，从低到高分别代表0~7个传感器是否被挂载，某位置1代表对应的序号有传感器，置0代表无传感器。
  '''
  def scan(self):
    l = self.read_holding_registers(self._addr, self.REG_ROM_FLAG, 1)
    state = 0
    if l[0] == 0:
      state = l[1] & l[2]
    return state

  '''
    @brief 设置设备地址。
    @param newAddr: 设备地址，范围1~247。
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_device_address(self, new_addr):
    if new_addr < 1 or new_addr > 0xF7:
      print("Error id")
    ret = self.write_holding_register(self._addr, self.REG_DEVICE_ADDR, new_addr)
    if(self._addr == 0):
      time.sleep(0.1)
      ret = self.read_holding_register(new_addr, self.REG_DEVICE_ADDR)
    if ret == new_addr:
      self._addr = new_addr
      return True
    return False

  '''
    @brief 获取设备地址。
    @return 返回设备地址，范围1~247。
  '''
  def get_device_address(self):
     return self._addr
  
  '''
    @brief 设置18B20精度。
    @return 精度:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_18B20_accuracy(self, id, accuracy):
    if id >= self.DS18B20_MAX_NUM:
      print("id out of range(0~7)")
      return False
    ret = self.write_holding_register(self._addr, self.REG_18B20_NUM0_ACCURACY+id, accuracy)
    if ret == accuracy:
      return True
    return False
  
  '''
    @brief 获取18B20精度。
    @return 精度:
  '''
  def get_18B20_accuracy(self, id):
    if id >= self.DS18B20_MAX_NUM:
      print("id out of range(0~7)")
      return 0xFF
    accuracy = self.read_holding_register(self._addr, self.REG_18B20_NUM0_ACCURACY+id)
    accuracy = accuracy & 0xFF
    return accuracy

  '''
    @brief 设置温度的上下阈值。
    @param id: 第几个温度传感器，范围0~7。
    @param tH: 设置温度的上阈值，范围-55~125℃
    @param tL: 设置温度的下阈值，范围-55~125℃
    @n note: 必须满足设置条件tH > tL
    @return 设置状态:
    @n      True:  设置成功
    @n      False: 设置失败
  '''
  def set_temperature_threshold(self, id, th, tl):
    if((id >= DS18B20_MAX_NUM) or (th < - 55) or (th > 125) or (tl < -55) or (tl > 125)):
      return False
    if th < 0:
      th = ((~th) + 1)
    if tl < 0:
      tl = ((~tl) + 1)
    val = (th << 8) | tl
    ret = self.write_holding_register(self._addr, self.REG_18B20_NUM0_TH_TL+id, val)
    if ret == val:
      return True
    return False
  '''
    @brief 获取温度的上下阈值。
    @param id: 第几个温度传感器，范围0~7。
    @return 16字节温度阈值:
    @n      高字节:  存储温度的上阈值
    @n      低字节:  存储温度的下阈值
  '''
  def get_temperature_threshold(self, id):
    if((id >= DS18B20_MAX_NUM) or (th < - 55) or (th > 125) or (tl < -55) or (tl > 125)):
      return False
    val = self.read_holding_register(self._addr, self.REG_18B20_NUM0_TH_TL+id)
    return val

  '''
    @brief 获取协议转换板上挂载的18B20的个数。
    @return 18B20设备数量，范围0~8：
    @n      0：协议转换板上未挂载18B20传感器
    @n      1: 协议转换板上挂载了1个18B20传感器
    @n      ......
    @n      8:  协议转换板上挂载了8个18B20传感器
  '''
  def get_18B20_number(self):
    val = self.read_holding_register(self._addr, self.REG_18B20_NUM)
    return val

  '''
    @brief 获取设备id的温度。
    @param id：18B20挂载在总线上的id号，范围0~7
    @return 温度:
  '''
  def get_temperature_c(self, id):
    if id >= self.DS18B20_MAX_NUM:
      print("id out of range(0~7)")
      return 0.0
    val = self.read_holding_register(self._addr, self.REG_18B20_NUM0_TEMP+id)
    return val/16.0
  
  '''
    @brief 获取18B20的ROM码。
    @param id: 第几个温度传感器，范围0~7。
    @param rom: 存放ROM码的指针。
    @param len: 固定长度，必须为8字节
    @return 读取状态:
    @n      0:  获取成功
    @n      others: 获取失败
  '''
  def get_18B20_rom(self, id):
    if id >= self.DS18B20_MAX_NUM:
      print("id out of range(0~7)")
      return [0]*8
    ret = self.read_holding_registers(self._addr, self.REG_18B20_NUM0_ADDR+id)
    if ret[0] = 0:
      return ret[1:]
    return ret

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


