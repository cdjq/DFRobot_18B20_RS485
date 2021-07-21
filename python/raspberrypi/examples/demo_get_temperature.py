# -*- coding:utf-8 -*-
from __future__ import print_function

'''
  # demo_detect.py
  #
  # @brief 获取18B20转RS485协议板上所有DS18B20的温度信息,未更改前，默认串口配置为：9600波特率，8位数据位，无校验位，1位停止位。
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

board = DFRobot_18B20_RS485(0x20)

if __name__ == "__main__":
  print("Initialization board...", end = " ")
  err = board.begin()
  while board.begin() != 0:
    print("failed.")
    time.sleep(1);
    print("Initialization board...", end = " ")
  print("done.")
  
  ds18b20_num = 0;
  while True:
    ds18b20_num = board.get_18B20_number()
    id = 0
    while id < board.DS18B20_MAX_NUM:
      t = board.get_temperature_c(id)
      print("id: %d   Temperature: %f"%(id, t))
      time.sleep(1)
      id += 1
    print("\n")
