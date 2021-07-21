# -*- coding:utf-8 -*-
from __future__ import print_function

'''
  # demo_config.py
  #
  # @brief 通过广播地址(RTU_BROADCAST_ADDRESS(0x00))配置协议转换板的modbus地址和串口。将modbus地址配置为0x20，并将串口配置为
  # @n 9600波特率，8位数据位，无校验位，1位停止位。（串口配置必须掉电重启后才能生效）设备地址和串口配置掉电不会丢失。
  # @n 注意：所有从机都会处理广播包，但不会响应。
  #
  # @n connected
  # -----------------------------------------------------------------------------
  # sensor pin |             MCU                |         raspberry pi          |
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

board = DFRobot_18B20_RS485(0x00)

if __name__ == "__main__":
  print("Initialization board...", end = " ")
  err = board.begin()
  while board.begin() != 0:
    print("failed.")
    time.sleep(1);
    print("Initialization board...", end = " ")
  print("done.")
  
  board.config_serial(board.BAUDRATE_9600, board.RS485_SERIAL_8N1)
  board.set_device_address(0x20)
  print("New device addr : 0x%02X"%board.get_device_address())
