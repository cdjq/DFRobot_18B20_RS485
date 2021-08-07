# -*- coding:utf-8 -*-
from __future__ import print_function

'''
  # demo_cascade.py
  #
  # @brief TEL0144协议转换板支持多块板子级联，这个demo演示了如何使用主控操控2块级联的TEL0144板子。
  # @n 准备工作：在运行此demo之前，需要保证2块板子的串口配置一致，这里使用的默认串口配置（9600波特率，8位数据位，无校验，1位停止位），并
  # @n 将2块板子设置为不同的设备地址(1~247范围内)，设置设备的串口通信参数和设备地址可以参考demo_config.py demo.
  #  
  # @n  实验演示：将板1的设备地址设置为16（十六进制0x10），板2设置为32（十六进制0x20），循环采集2块板子上18B20的连接数量，ROM号，温度上下阈值，精度及温度。
  # @n  在这里你可以创建一个广播对象，批量修改总线上所有设备的配置（此方法需谨慎使用，最好不要用来批量修改设备地址，会导致总线上所有设备地址一致）。
  #
  # @n connected
  # -----------------------------------------------------------------------------
  #    board   |             MCU                |         raspberry pi          |
  #     VCC    |            3.3V/5V             |            5V/3V3             |
  #     GND    |              GND               |             GND               |
  #     RX     |              TX                |          (BCM)14 TX           |
  #     TX     |              RX                |          (BCM)15 RX           |
  # -----------------------------------------------------------------------------
  # @n TEL0144 RS485接口级联方式：总线上所有TEL0144板子A连A，B连B。如果主控有RS485接口，则直接把其中一块板子的A/B接口连接到主控的A，B接口上 ；
  # @n 如果主控上没有RS485接口，则需要一块TTL转RS485转换板，将转换板的RS485接口连接到级联的其中一块TEL0144板子的RS485接口上，再将主控的串口和
  # @n 转换板的TTL接口相连接。
  #
  # @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  # @licence     The MIT License (MIT)
  # @author [Arya](xue.peng@dfrobot.com)
  # @version  V1.0
  # @date  2021-07-05
  # @https://github.com/DFRobot/DFRobot_18B20_RS485
'''

import sys
import os
import time

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
from DFRobot_18B20_RS485 import *

CASCADE_DEVICE_NUM = 2  #协议转换板的级联数量，这里表示总线上有2个设备级联

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
broadcast = DFRobot_18B20_RS485(addr = 0, baud = 9600) #创建一个广播地址的对象，可批量配置总线上的所有设备


board = [0]*2  #创建长度为2的列表，用来保存DFRobot_18B20_RS485对象
modbus_device_addr = [16,32]#定义长度为2的列表，存放协议转换板的设备地址，分别为16或32

if __name__ == "__main__":
  
  '''初始化类对象列表'''
  index = 0
  while index < CASCADE_DEVICE_NUM:
    board[index] = DFRobot_18B20_RS485(addr = modbus_device_addr[index], baud = 9600)
    index += 1
  
  '''初始化类对象列表里的对象'''
  index = 0
  cascade_num = 0
  while index < CASCADE_DEVICE_NUM:
    print("Initialization Modbus device [%d]"% modbus_device_addr[index], end = " ")
    if board[index].begin() != 0:
      print("failed.")
    else:
      cascade_num += 1
      print("done")
    index += 1

  if cascade_num != CASCADE_DEVICE_NUM:
    print("Initialization Modbus device Lists failed.")
    while True:
      pass
  
  broadcast.begin()
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
  broadcast.batch_set_18b20_accuracy(batch_id = broadcast.eBatch_ID_ALL, accuracy = broadcast.e18B20_ACCURACY_11_BIT) #将总线上所有设备上连接的和没连接的18B20传感器的进度设置为11位，（0-9bits，1-10bits，2-11bits，3-12bits）

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
  board[0].batch_set_18b20_temperature_threshold(batch_id = board[0].eBatch_ID_ALL, th = 100, tl = -44) #将总线上设备地址为16的协议板上18B20传感器的温度阈值设置为高阈值100℃，低阈值-44℃
  board[1].batch_set_18b20_temperature_threshold(batch_id = board[0].eBatch_ID_ALL, th = 100, tl = -44) #将总线上设备地址为32的协议板上id为0的18B20传感器的温度阈值设置为高阈值100℃，低阈值-44℃
  
  ds18b20_num = 0;
  while True:
    device_addr     = 0 #存储TEL0144设备的设备地址
    connected_num   = 0 #存储设备上实际连接的18B20的数量，范围0~8，最大可连接8个18B20传感器
    connected_state = 0 #存储设备上0~7位置是否连接18B20传感器，bit0~bit1依次代表id0~id7 18B20传感器的连接状态，1代表连接，0代表未连接
    alarm_flag      = 0 #存储协议转换板上所有18B20的温度报警状态标志
    rom             = [0]*broadcast.DS18B20_ROM_BYTES #存储单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
    temp_threshold  = [0]*2 #存储18B20的温度的上下阈值，temp_threshold[0]：温度的上阈值，temp_threshold[1]：温度的下阈值
    accuracy        = 0 #存储某个设备上某id位置连接的单个18B20的精度配置
    temp            = 0 #存储某个设备上某id位置连接的单个18B20的测量的温度
 
    index = 0
    while index < CASCADE_DEVICE_NUM:
      device_addr   = board[index].get_device_address()
      connected_num = board[index].get_18B20_number()
      connected_state = board[index].scan()
      alarm_flag = board[index].get_temperature_threshold_alarm_flag()
      print("--------------borad [%d] info--------------"%index)
      print("Device Address(range 001~247 or 0): %s,   18B20 connected numbers(range 0~8): %d"%(str(device_addr).zfill(3),connected_num))

      id_num = 0
      while id_num < 8:
        rom = board[index].get_18B20_rom(id = id_num)
        rom_str = board[index].get_rom_hex_string(rom)
        temp_threshold = board[index].get_temperature_threshold(id = id_num)
        accuracy = board[index].get_18B20_accuracy(id = id_num)
        temp = board[index].get_temperature_c(id = id_num)

        print("ID(0~7): %d,"%id_num, end = " ")
        print("ROM: %s,"%rom_str, end = " ")
        print("connected(0-disconnected, 1-connected): %d,"%((connected_state >> id_num)&0x01), end = " ")
        print("accuarcy(0-9bits, 1-10bits, 2-11bits, 3-12bits): %d,"%accuracy, end = " ")
        print("Threshold High(-55~125): %d, Threshold Low(-55~125): %d,"%(temp_threshold[0], temp_threshold[1]), end = " ")
        print("Temperature: %.2f"%temp)
        time.sleep(0.5)
        id_num += 1
      index += 1
      print("\n")
      time.sleep(1)