# -*- coding:utf-8 -*-
from __future__ import print_function

'''
  # demo_config.py
  #
  # @brief 配置TEL0144协议转换板的设备地址和串口波特率。TEL0144协议转换板遵循modbus RTU协议，用户可以自行将TEL0144设备的串口波特率配置为下表所示波特率，
  # @n 波特率配置后，需掉电重启才会生效，在知道配置的波特率后，用户可以自行将地址更改为1~247(0x00~0xF7)范围内的地址，下面介绍2种配置TEL0144设备地址的方法,
  # @n 地址修改，如果不是广播包，将会在从机回应后，主机的下一个包开始生效：
  # @n 1.通过modbus广播地址（0x00）修改，这种适用于用户不知道设备的地址。所有modbus从机都会处理广播包，但不会响应，用户可以通过广播包批量设置从机的参数或
  # @n 修改从机设备地址。（注意：用户不能通过广播包得知地址是否设置成功，如果想知道需要单独询问，如果用户通过广播包配置地址，总线上所有设备的地址都会被配置
  # @n 成一样的地址，批量配置地址需谨慎使用）
  # @n
  # @n 2.通过设备的地址修改，这种只适用于用户知道设备地址的情况。
  # @n 注意：上述两种修改设备地址的方法成功的前提是必须知道设备的串口参数配置，如果不修改，默认的串口参数配置为：9600波特率，8位数据位，无校验，1位停止位，
  # @n 其中用户只可以修改串口的波特率配置。
  # @n 注意：如果创建了一个广播地址设备对象，它永远只能发广播包，getDeviceAddress函数只能返回0地址，非广播地址设备对象，getDeviceAddress函数可以返回修改后的设备地址。
  # @n ---------------------------------------------
  # @n |     TEL0144协议转换板支持的串口波特率配置表  | 
  # @n --------------------------------------------- 
  # @n |  eBaudrateConfig_t      |     uint32_t    |
  # @n |   eBAUDRATE_2400        |     2400        |
  # @n |   eBAUDRATE_4800        |     4800        |
  # @n |   eBAUDRATE_9600        |     9600        |
  # @n |   eBAUDRATE_14400       |     14400       |
  # @n |   eBAUDRATE_19200       |     19200       |
  # @n |   eBAUDRATE_38400       |     38400       |
  # @n |   eBAUDRATE_57600       |     57600       |
  # @n |   eBAUDRATE_115200      |     115200      |
  # @n ---------------------------------------------
  # @n note: TEL0144支持上述波特率配置，用户可以选一种主控也支持的波特率，9600是它的默认波特率配置。
  # 
  # @n demo实验：接下来先发一个广播包把TEL0144的串口波特率配置为9600，并将其设备地址修改为0x10，在设备地址修改为16（十六进制0x10）后，在用第2种方法，将地址修到其默认的
  # @n 设备地址32(十六进制0x20)。
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
  # @license     The MIT License (MIT)
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
board = DFRobot_18B20_RS485(addr = 0, baud = 9600)   #创建一个广播地址对象board，先将TEL0144板子的设备地址更改为16（16进制0x10）
board1 = DFRobot_18B20_RS485(addr = 16, baud = 9600) #已知一个板子的设备地址为16，将改设备地址更改为32（16进制0x20）

if __name__ == "__main__":
  
  ##########方法1：通过广播地址0（16进制0x00）将设备地址更改为16(注意用广播地址修改设备地址时，总线上所有设备都会被设置成同一地址)##########
  '''
    @brief TEL0144设备或广播地址类对象（地址为广播地址0的类对象）初始化。
    @return initialization state:
    @n       0: sucess
    @n      -1: failed
  '''
  print("Initialization board...", end = " ")
  while board.begin() != 0:
    print("failed.")
    time.sleep(1);
    print("Initialization board...", end = " ")
  print("done.")

  print("Current DEVICE ADDR(range (1~247 or 0)): %d\n"%board.get_device_address())

  print("Set new device address(16) and baudrate by BROADCAST ADDRESS(0): ", end = " ")
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
  board.set_baudrate(baud = board.eBAUDRATE_9600) #通过地址为广播地址的类对象将TEL0144设备的波特率设置为9600，该配置需掉电重启后生效
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
  board.set_device_address(new_addr = 16) #通过广播地址0将TEL0144的设备地址修改为16（十六进制0x10），注意：广播包不能查询是否设置成功
  print("done")
  print("Current DEVICE ADDR(range (1~247 or 0)): %d\n"%board.get_device_address())

  ##########board1是一个设备地址为16的设备对象，查询总线上是否有设备地址为16的设备，如果有则会初始化成功##########
  print("Check if the device with address 16 exists: ", end = " ")
  while board1.begin() != 0:
    print("failed.")
    print("Check if the device with address 16 exists: ", end = " ")
  print("exist.")
  print("Current DEVICE ADDR(range (1~247 or 0)): %d\n"%board1.get_device_address())

  ##########方法1：通过广播地址0（16进制0x00）将设备地址更改为16(注意用广播地址修改设备地址时，总线上所有设备都会被设置成同一地址)##########
  print("Set new device address(32) by device address(16): ", end = " ")
  flag = board1.set_device_address(new_addr = 32)  #通过设备地址16将TEL0144的设备地址修改为32（十六进制0x20）
  if flag:
    print("sucess.")
    print("New device addr : %d"%board1.get_device_address())
    print("baudrate: %d"%board1.get_baudrate())
  else:
    print("failed.")
    print("Set new address 32 failed, current address is: %d"%board.get_device_address());