# -*- coding:utf-8 -*-
from __future__ import print_function

'''
  # demo_get_18b20_data.py
  #
  # @brief 获取单个协议转换板上连接的18B20温度传感器的数量，以及所有18B20的ROM号，精度，温度上下阈值的配置，以及当前采集的温度。
  # @n 注意：协议转换板的默认串口配置为：9600波特率，8位数据维，无校验，以及一位停止位，其中波特率可以通过set_baudrate函数更改。
  # @n  协议转换板的默认设备地址为0x20（十进制32，TEL0144_DEFAULT_DEVICE_ADDRESS），可以通过set_device_address函数将地址更改为1~247(0x01~0xF7)
  # @n  范围内的任意数值,0x00是广播地址，任何modbus从机都会处理广播包，但不会响应。
  # @n 注意：如果你想更改协议转换板的设备地址和串口波特率，可参考demo：demo_config.py
  #
  # @n connected
  # -----------------------------------------------------------------------------
  #    board   |             MCU                |         raspberry pi          |
  #     VCC    |            3.3V/5V             |            5V/3V3             |
  #     GND    |              GND               |             GND               |
  #     RX     |              TX                |          (BCM)14 TX           |
  #     TX     |              RX                |          (BCM)15 RX           |
  # -----------------------------------------------------------------------------
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
board = DFRobot_18B20_RS485(addr = 32, baud = 9600)   

if __name__ == "__main__":
  
  print("Initialization board...", end = " ")
  '''
    @brief TEL0144设备或广播地址类对象（地址为广播地址0的类对象）初始化。
    @return initialization state:
    @n       0: sucess
    @n      -1: failed
  '''
  err = board.begin()
  while board.begin() != 0:
    print("failed.")
    time.sleep(1);
    print("Initialization board...", end = " ")
  print("done.")

  print("DEVICE ADDR(range (1~247 or 0)): %d"%board.get_device_address())
  print("DEVICE PID: 0x%04X"%board.get_device_pid())
  print("DEVICE VID: 0x%04X"%board.get_device_vid())
  print("BAUDRATE: %d\n"%board.get_baudrate())

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
  board.batch_set_18b20_accuracy(batch_id = board.eBatch_ID_ALL, accuracy = board.e18B20_ACCURACY_10_BIT)

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
  board.batch_set_18b20_temperature_threshold(batch_id = board.eBatch_ID_ALL, th = 77, tl = -33)
  
  ds18b20_num = 0;
  while True:
    connected_num   = 0 #存储设备上实际连接的18B20的数量，范围0~8，最大可连接8个18B20传感器
    alarm_flag      = 0 #存储协议转换板上所有18B20的温度报警状态标志
    rom             = [0]*board.DS18B20_ROM_BYTES #存储单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
    temp_threshold  = [0]*2 #存储18B20的温度的上下阈值，temp_threshold[0]：温度的上阈值，temp_threshold[1]：温度的下阈值
    accuracy        = 0 #存储某个设备上某id位置连接的单个18B20的精度配置
    temp            = 0 #存储某个设备上某id位置连接的单个18B20的测量的温度
 
    '''
     @brief 获取协议转换板上实际连接的18B20的数量。
     @return 18B20设备数量，范围0~8：
     @n      0：协议转换板上未连接18B20传感器
     @n      1: 协议转换板上连接了1个18B20传感器
     @n      ......
     @n      8:  协议转换板上连接了8个18B20传感器
   '''
    connected_num   = board.get_18B20_number()
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
    alarm_flag = board.get_temperature_threshold_alarm_flag()

    print("18B20 connected numbers(range 0~8): %d"%connected_num)

    id_num = 0
    while id_num < board.SENSOR_CONNECTED_MAX_NUM:
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
      rom = board.get_18B20_rom(id = id_num)
      '''
        @brief 将获取到的8字节的ROM码转换为16进制表示的字符串，例：8字节的ROM号为0x28 0xAA 0xAD 0x38 0x54 0x14 0x01 0x6A转化为
        @n 字符串为28AAAD385414016A。
        @param rom: 长度为8的ROM列表。
        @return ROM码的16进制字符串或空字符串:
      '''
      rom_str = board.get_rom_hex_string(rom)
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
      temp_threshold = board.get_temperature_threshold(id = id_num)
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
      accuracy = board.get_18B20_accuracy(id = id_num)
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
      temp = board.get_temperature_c(id = id_num)
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
      threshold_alarm_flag = board.parse_threshold_alarm_flag(id = id_num, alarm_flag = alarm_flag)
      
      print("ID: %d, ROM: %s, Accuarcy(0-9bits, 1-10bits, 2-11bits, 3-12bits): %d, ALARM(0-IN, 1-BELOW, 2-ABOVE): %d, Threshold High(-55~125): %d, Threshold Low(-55~125): %d, Temperature: %f" \
      %(id_num, rom_str, accuracy, threshold_alarm_flag, temp_threshold[0],temp_threshold[1],temp))
      time.sleep(0.5)
      id_num += 1
    print("\n")
    time.sleep(1)