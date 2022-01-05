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
  #将TEL0144设备上所有18B20传感器的精度都配置为10位采集精度
  board.batch_set_18b20_accuracy(batch_io = board.eBatch_ALL, batch_id = board.eBatch_ID_ALL, accuracy = board.e18B20_ACCURACY_10_BIT)

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
  #将TEL0144设备上所有18B20传感器的温度阈值范围设置为 高阈值77，低阈值-33，如果低于最低，或高于最高，会发生报警
  board.batch_set_18b20_temperature_threshold(batch_io = board.eBatch_ALL, batch_id = board.eBatch_ID_ALL, th = 77, tl = -33)
  
  while True:
    #记录TEL0144设备上所有IO口连接的18B20传感器数量之和，最大16个
    total_connected = 0
    #记录TEL0144设备上单独IO口连接的18B20传感器数量，最大4个
    io_connected    = 0
    #记录TEL0144设备各IO口的每个位置是否连接18B20传感器的情况，TEL0144有4个IO口，每个IO口最多可连接4个传感器，用16位数据分别代表各IO口上传感器的连接情况，0->未连接，1->连接
    connected_state = 0 #存储设备上0~7位置是否连接18B20传感器，bit0~bit1依次代表id0~id7 18B20传感器的连接状态，1代表连接，0代表未连接  
    #记录单个18B20的ROM数据， DS18B20_ROM_BYTES = 8字节
    rom             = [0]*board.DS18B20_ROM_BYTES
    #记录18B20的温度的上下阈值，temp_threshold[0]：温度的上阈值，temp_threshold[1]：温度的下阈值
    temp_threshold  = [0]*2
    #记录18B20的精度配置
    accuracy  = 0
    #记录18B20传感器采集到的温度数据
    temp  = 0
    #记录TEL0144设备所有18B20的温度报警状态标志
    alarm_flag = 0
    
    '''
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
    #scan 扫描TEL0144设备上各IO口各id位置是否连接了18B20传感器，1：连接，0：未连接
    connected_state = board.scan()
    
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
    #get_18B20_number 扫描TEL0144设备上总共连接了多少个18B20传感器，最大可连接16个
    total_connected = board.get_18B20_number(board.eBatch_ALL)
    
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
    #查询TEL0144板子上各个IO口每个id是否发生温度阈值报警标志
    alarm_flag = board.get_temperature_threshold_alarm_flag()

    print("18B20 connected numbers(range 0~16): %d"%total_connected)
    print("|-----------------------------------------------------------------------------------------------|")
    print("|              18B20 connecte table of TEL0144(0->not connected, 1->connected)                  |")
    print("|-----------------------------------------------------------------------------------------------|")
    print("|           D4          |           D3          |           D2          |           D1          |")
    print("| b15 | b14 | b13 | b12 | b11 | b10 | b09 | b08 | b07 | b06 | b05 | b04 | b03 | b02 | b01 | b00 |")
    print("| id3 | id2 | id1 | id0 | id3 | id2 | id1 | id0 | id3 | id2 | id1 | id0 | id3 | id2 | id1 | id0 |")

    count = board.DS18B20_CONNECT_IO_NUM*board.DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM
    while count > 0:
      if(connected_state & (1 << (count - 1))):
        print("|  1 ", end = " ")
      else:
        print("|  0 ", end = " ")
      count -= 1
    print("|\n|-----------------------------------------------------------------------------------------------|\n")

    io = 1
    while io <= board.DS18B20_CONNECT_IO_NUM:
      io_connected = board.get_18B20_number(1 << (io - 1))
      print("IO: D%d\tconnected(range 0~4):%d"%(io, io_connected))
      id = 0
      while id < board.DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM:
        rom = board.get_18B20_rom(io = io, id = id)
        rom_str = board.get_rom_hex_string(rom)
        temp_threshold = board.get_temperature_threshold(io = io, id = id)
        threshold_alarm_flag = board.parse_threshold_alarm_flag(io = io, id = id, alarm_flag = alarm_flag)
        accuracy = board.get_18B20_accuracy(io = io, id = id)
        temp = board.get_temperature_c(io = io, id = id)
        print("  ID: %d, ROM: %s, Accuarcy(0-9bits, 1-10bits, 2-11bits, 3-12bits): %d, ALARM(0-IN, 1-BELOW, 2-ABOVE): %d, Threshold High(-55~125): %d, Threshold Low(-55~125): %d, Temperature: %f" \
        %(id, rom_str, accuracy, threshold_alarm_flag, temp_threshold[0],temp_threshold[1],temp))
        time.sleep(1)
        id += 1
      io += 1
    print("\n")
    time.sleep(1)