# -*- coding:utf-8 -*-
from __future__ import print_function

'''
  # demo_scan_sensor.py
  #
  # @brief 扫描协议转换板上真实连接的18B20传感器，并读出相应的配置值。
  # @n 1: ROM,挂载在协议转换板上的每一个DS18B20都有唯一64位的ROM值。
  # @n 2：阈值(Threshold),可以单独设置和读取挂载在协议转换版上每一个DS28B20的高温度阈值和低温度阈值，范围为-55~125℃
  # @n 3：精度(accuracy),可以单独设置和读取挂载在协议转换版上每一个DS28B20的精度，范围如下所示：
  # @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
  # @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
  # @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
  # @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
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
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-07-05
 * @https://github.com/DFRobot/DFRobot_18B20_RS485
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
board = DFRobot_18B20_RS485(addr = 0x20, baud = 9600)

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
    @return accuracy ：精度设置,范围0~3
    @n      e18B20_ACCURACY_9_BIT  or 0:  9位精度
    @n      e18B20_ACCURACY_10_BIT or 1:  10位精度
    @n      e18B20_ACCURACY_11_BIT or 2:  11位精度
    @n      e18B20_ACCURACY_12_BIT or 3:  12位精度
    @n      0xFF                       :  获取精度失败
  '''
  board.set_18B20_accuracy(id = board.e18B20_ID0, accuracy = board.e18B20_ACCURACY_12_BIT)
  board.set_18B20_accuracy(id = board.e18B20_ID1, accuracy = board.e18B20_ACCURACY_9_BIT)

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
  board.set_temperature_threshold(id = board.e18B20_ID0, th = 100, tl = 26)
  board.set_temperature_threshold(id = board.e18B20_ID1, th = 33, tl = 28)
  
  ds18b20_num = 0;
  while True:
    connected_state = 0 #存储设备上0~7位置是否连接18B20传感器，bit0~bit1依次代表id0~id7 18B20传感器的连接状态，1代表连接，0代表未连接
    connected_num   = 0 #存储设备上实际连接的18B20的数量，范围0~8，最大可连接8个18B20传感器
    alarm_flag      = 0 #存储协议转换板上所有18B20的温度报警状态标志
    rom             = [0]*board.DS18B20_ROM_BYTES #存储单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
    temp_threshold  = [0]*2 #存储18B20的温度的上下阈值，temp_threshold[0]：温度的上阈值，temp_threshold[1]：温度的下阈值
    accuracy        = 0 #存储某个设备上某id位置连接的单个18B20的精度配置
    temp            = 0 #存储某个设备上某id位置连接的单个18B20的测量的温度
 
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
    connected_state = board.scan()
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
      if (connected_state & (1 << id_num)):
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
