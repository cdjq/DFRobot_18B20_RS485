# -*- coding:utf-8 -*-

'''
  @file DFRobot_RTU.py
  @brief Modbus RTU libary for Arduino. 
  
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @licence     The MIT License (MIT)
  @author [Arya](xue.peng@dfrobot.com)
  @version  V1.0
  @date  2021-07-16
  @https://github.com/DFRobot/DFRobot_RTU
'''
import sys
import serial
import time

class DFRobot_RTU:
  
  _packet_header = {"id": 0, "cmd": 1, "cs": 0}
  
  '''Enum constant'''
  eRTU_EXCEPTION_ILLEGAL_FUNCTION     = 0x01
  eRTU_EXCEPTION_ILLEGAL_DATA_ADDRESS = 0x02
  eRTU_EXCEPTION_ILLEGAL_DATA_VALUE   = 0x03
  eRTU_EXCEPTION_SLAVE_FAILURE        = 0x04
  eRTU_EXCEPTION_CRC_ERROR            = 0x08
  eRTU_RECV_ERROR                     = 0x09
  eRTU_MEMORY_ERROR                   = 0x0A
  eRTU_ID_ERROR                       = 0x0B
  
  eCMD_READ_COILS           = 0x01
  eCMD_READ_DISCRETE        = 0x02
  eCMD_READ_HOLDING         = 0x03
  eCMD_WRITE_COILS          = 0x05
  eCMD_WRITE_HOLDING        = 0x06
  eCMD_WRITE_MULTI_COILS    = 0x0F
  eCMD_WRITE_MULTI_HOLDING  = 0x10

  def __init__(self, baud, bits, parity, stopbit):
    '''
      @brief Serial initialization.
      @param baud:  The UART baudrate of raspberry pi
      @param bits:  The UART data bits of raspberry pi
      @param parity:  The UART parity bits of raspberry pi
      @param stopbit:  The UART stopbit bits of raspberry pi.
    '''
    self._ser = serial.Serial("/dev/ttyAMA0",baud, bits, parity, stopbit)

  def read_coils_register_bit(self, id, reg, position):
    '''
      @brief Read the bit of the coils Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Coils register address.
      @param position: The bit position of Coils register value. Range: 0~15
      @return Return the positon bit of Coils register value.
      @n      True:  The position of Coils register bit is 1.
      @n      False:  The position of Coils register bit is 0.
    '''
    val = self.read_coils_register(id, reg)
    if (val & (1 << position)):
      return True
    else:
      return False
    
  def read_discrete_inputs_register_bit(self, id, reg, position):
    '''
      @brief Read the bit of the discrete inputs register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Discrete inputs register.
      @param position: The bit position of discrete inputs register value. Range: 0~15
      @return Return the positon bit of discrete inputs register value.
      @n      True:  The position of Coils register bit is 1.
      @n      False:  The position of Coils register bit is 0.
    '''
    val = self.read_discrete_inputs_register(id, reg)
    if (val & (1 << position)):
      return True
    else:
      return False
    
  def read_coils_register(self, id, reg):
    '''
      @brief Read a coils Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Coils register address.
      @return Return the value of the coils register value.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), 0x00, 0x01]
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_READ_COILS, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_READ_COILS,2)
    if (l[0] == 0) and len(l) == 8:
      l[0] = ((l[4] << 8) | l[5]) & 0xFFFF
    else:
      l[0] = 0
    return l[0]
      
  def read_discrete_inputs_register(self, id, reg):
    '''
      @brief Read a discrete input register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Discrete input register address.
      @return Return the value of the discrete input register.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), 0x00, 0x01]
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_READ_DISCRETE, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_READ_DISCRETE,2)
    if (l[0] == 0) and len(l) == 8:
      l[0] = ((l[4] << 8) | l[5]) & 0xFFFF
    else:
      l[0] = 0
    return l[0]
      
  def read_holding_register(self, id, reg):
    '''
      @brief Read a holding Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Holding register address.
      @return Return the value of the holding register value.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), 0x00, 0x01]
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_READ_HOLDING, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_READ_HOLDING,2)
    if (l[0] == 0) and len(l) == 8:
      l[0] = ((l[4] << 8) | l[5]) & 0xFFFF
    else:
      l[0] = 0
    return l[0]
      
  def write_coils_register(self, id, reg, val):
    '''
      @brief Write a coils Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Coils register address.
      @param val: write value.
      @return Return the value of the coils register write.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (val >> 8)&0xFF, (val & 0xFF)]
    val = 0
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_WRITE_COILS, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_WRITE_COILS,reg)
    if (l[0] == 0) and len(l) == 9:
      val = ((l[5] << 8) | l[6]) & 0xFFFF
    return val
      

  def write_holding_register(self, id, reg, val):
    '''
      @brief Write a holding register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Holding register address.
      @param val: The value of the register value which will be write.
      @return Return the value of the holding register.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (val >> 8)&0xFF, (val & 0xFF)]
    val = 0
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_WRITE_HOLDING, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_WRITE_HOLDING,reg)
    if (l[0] == 0) and len(l) == 9:
      val = ((l[5] << 8) | l[6]) & 0xFFFF
    return val
      
  def read_coils_registers(self, id, reg, size):
    '''
      @brief Read multiple coils Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Read the start address of the coil register.
      @param len: Number of read coil registers.
      @return list: format as follow:
      @n      list[0]: Exception code:
      @n               0 : sucess.
      @n               1 or eRTU_EXCEPTION_ILLEGAL_FUNCTION : Illegal function.
      @n               2 or eRTU_EXCEPTION_ILLEGAL_DATA_ADDRESS: Illegal data address.
      @n               3 or eRTU_EXCEPTION_ILLEGAL_DATA_VALUE:  Illegal data value.
      @n               4 or eRTU_EXCEPTION_SLAVE_FAILURE:  Slave failure.
      @n               8 or eRTU_EXCEPTION_CRC_ERROR:  CRC check error.
      @n               9 or eRTU_RECV_ERROR:  Receive packet error.
      @n               10 or eRTU_MEMORY_ERROR: Memory error.
      @n               11 or eRTU_ID_ERROR: Broadcasr address or error ID
      @n      list[1:]: The value of the coil register list.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (size >> 8) & 0xFF, size & 0xFF]
    if(id > 0xF7):
      print("device addr error.")
      return [self.eRTU_ID_ERROR]
    l = self._packed(id, self.eCMD_READ_COILS, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_READ_COILS,size*2)
    if ((l[0] == 0) and (len(l) == (5+size+1))):
      la = [l[0]] + l[4: len(l)-2]
      return la
    return [l[0]]
    
  def read_discrete_inputs_registers(self, id, reg, size):
    '''
      @brief Read multiple discrete inputs register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Read the start address of the discrete inputs register.
      @param len: Number of read discrete inputs register.
      @return list: format as follow:
      @n      list[0]: Exception code:
      @n               0 : sucess.
      @n               1 or eRTU_EXCEPTION_ILLEGAL_FUNCTION : Illegal function.
      @n               2 or eRTU_EXCEPTION_ILLEGAL_DATA_ADDRESS: Illegal data address.
      @n               3 or eRTU_EXCEPTION_ILLEGAL_DATA_VALUE:  Illegal data value.
      @n               4 or eRTU_EXCEPTION_SLAVE_FAILURE:  Slave failure.
      @n               8 or eRTU_EXCEPTION_CRC_ERROR:  CRC check error.
      @n               9 or eRTU_RECV_ERROR:  Receive packet error.
      @n               10 or eRTU_MEMORY_ERROR: Memory error.
      @n               11 or eRTU_ID_ERROR: Broadcasr address or error ID
      @n      list[1:]: The value list of the discrete inputs register.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (size >> 8) & 0xFF, size & 0xFF]
    if(id > 0xF7):
      print("device addr error.")
      return [self.eRTU_ID_ERROR]
    l = self._packed(id, self.eCMD_READ_DISCRETE, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_READ_DISCRETE,size*2)
    if (l[0] == 0) and (len(l) == (5+size+1)):
      la = [l[0]] + l[4: len(l)-2]
      return la
    return [l[0]]
    
  def read_holding_registers(self, id, reg, size):
    '''
      @brief Read multiple holding register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Read the start address of the holding register.
      @param len: Number of read holding register.
      @return list: format as follow:
      @n      list[0]: Exception code:
      @n               0 : sucess.
      @n               1 or eRTU_EXCEPTION_ILLEGAL_FUNCTION : Illegal function.
      @n               2 or eRTU_EXCEPTION_ILLEGAL_DATA_ADDRESS: Illegal data address.
      @n               3 or eRTU_EXCEPTION_ILLEGAL_DATA_VALUE:  Illegal data value.
      @n               4 or eRTU_EXCEPTION_SLAVE_FAILURE:  Slave failure.
      @n               8 or eRTU_EXCEPTION_CRC_ERROR:  CRC check error.
      @n               9 or eRTU_RECV_ERROR:  Receive packet error.
      @n               10 or eRTU_MEMORY_ERROR: Memory error.
      @n               11 or eRTU_ID_ERROR: Broadcasr address or error ID
      @n      list[1:]: The value list of the holding register.
    '''
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (size >> 8) & 0xFF, size & 0xFF]
    if(id > 0xF7):
      print("device addr error.")
      return [self.eRTU_ID_ERROR]
    l = self._packed(id, self.eCMD_READ_HOLDING, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_READ_HOLDING,size*2)
    if (l[0] == 0) and (len(l) == (5+size+1)):
      la = [l[0]] + l[4: len(l)-2]
      return la
    return [l[0]]
    
  def write_coils_registers(self, id, reg, data):
    '''
      @brief Write multiple coils Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Write the start address of the coils register.
      @param data: The list of storage coils Registers' value which will be write.
      @return Exception code:
      @n      0 : sucess.
      @n      1 or eRTU_EXCEPTION_ILLEGAL_FUNCTION : Illegal function.
      @n      2 or eRTU_EXCEPTION_ILLEGAL_DATA_ADDRESS: Illegal data address.
      @n      3 or eRTU_EXCEPTION_ILLEGAL_DATA_VALUE:  Illegal data value.
      @n      4 or eRTU_EXCEPTION_SLAVE_FAILURE:  Slave failure.
      @n      8 or eRTU_EXCEPTION_CRC_ERROR:  CRC check error.
      @n      9 or eRTU_RECV_ERROR:  Receive packet error.
      @n      10 or eRTU_MEMORY_ERROR: Memory error.
      @n      11 or eRTU_ID_ERROR: Broadcasr address or error ID
    '''
    size = len(data)
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (((size/2) >> 8) & 0xFF), ((size/2) & 0xFF), size] + data
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_WRITE_MULTI_COILS, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_WRITE_MULTI_COILS,reg)
    if (l[0] == 0) and len(l) == 9:
      val = ((l[5] << 8) | l[6]) & 0xFFFF
    return l[0]
      
  def write_holding_registers(self, id, reg, data):
    '''
      @brief Write multiple holding Register.
      @param id:  modbus device ID. Range: 0x00 ~ 0xF7(0~247), 0x00 is broadcasr address, which all slaves will process broadcast packets, 
      @n          but will not answer.
      @param reg: Write the start address of the holding register.
      @param data: The list of storage holding Registers' value which will be write.
      @return Exception code:
      @n      0 : sucess.
      @n      1 or eRTU_EXCEPTION_ILLEGAL_FUNCTION : Illegal function.
      @n      2 or eRTU_EXCEPTION_ILLEGAL_DATA_ADDRESS: Illegal data address.
      @n      3 or eRTU_EXCEPTION_ILLEGAL_DATA_VALUE:  Illegal data value.
      @n      4 or eRTU_EXCEPTION_SLAVE_FAILURE:  Slave failure.
      @n      8 or eRTU_EXCEPTION_CRC_ERROR:  CRC check error.
      @n      9 or eRTU_RECV_ERROR:  Receive packet error.
      @n      10 or eRTU_MEMORY_ERROR: Memory error.
      @n      11 or eRTU_ID_ERROR: Broadcasr address or error ID
    '''
    size = len(data)/2
    l = [(reg >> 8)&0xFF, (reg & 0xFF), (((size/2) >> 8) & 0xFF), ((size/2) & 0xFF), size] + data
    if(id > 0xF7):
      print("device addr error.")
      return 0
    l = self._packed(id, self.eCMD_WRITE_MULTI_HOLDING, l)
    self._send_package(l)
    l = self.recv_and_parse_package(id, self.eCMD_WRITE_MULTI_HOLDING,reg)
    if (l[0] == 0) and len(l) == 9:
      val = ((l[5] << 8) | l[6]) & 0xFFFF
    return l[0]
      
  def _calculate_crc(self, data):
    crc = 0xFFFF
    length = len(data)
    #print("len=%d"%length)
    pos = 0
    while pos < length:
      crc ^= (data[pos] | 0x0000)
      #print("pos=%d, %02x"%(pos,data[pos]))
      i = 8;
      while i != 0:
        if (crc & 0x0001) != 0:
          crc = (crc >> 1)&0xFFFF
          crc ^= 0xA001
        else:
          crc = (crc >> 1)&0xFFFF
        i -= 1
      pos += 1
    crc = (((crc & 0x00FF) << 8) | ((crc & 0xFF00) >> 8)) & 0xFFFF
    #print("crc=%x"%crc)
    return crc

  def _packed(self, id, cmd, l):
    length = 4+len(l)
    #print(len(l))
    package = [0]*length
    package[0] = id
    package[1] = cmd
    package[2:length-2] = l
    #lin = ['%02X' % i for i in package]
    #print(" ".join(lin))

    crc = self._calculate_crc(package[:len(package)-2])
    package[length-2] = (crc >> 8) & 0xFF
    package[length-1] = crc & 0xFF
    
    lin = ['%02X' % i for i in package]
    print(" ".join(lin))
    return package;

  def _send_package(self, l):
    if len(l):
      self._ser.write(l)

  def recv_and_parse_package(self, id, cmd, val):
    package = [self.eRTU_ID_ERROR]
    if (id < 1) or (id > 0xF7):
      return package
    head = [0]*4
    index = 0
    t = time.time()
    remain = 0
    while remain < 4:
      if self._ser.inWaiting():
        data = self._ser.read(1)
        try: 
          head[index] = ord(data)
        except:
          head[index] = data
        #print("%d = %02X"%(index, head[index]))
        index += 1
        if (index == 1) and (head[0] != id):
          index = 0
        elif (index == 2) and ((head[1] & 0x7F) != cmd):
          index = 0
        remain = index
        t = time.time()
      if time.time() - t > 0.1:
        #print("time out.")
        return [self.eRTU_RECV_ERROR]
      if(index == 4):
        if head[1] & 0x80:
          index = 5
        else:
          if head[1] < 4:
            if head[2] != (val & 0xFF):
              index = 0
              remain = 0
            else:
              index = 5 + head[2]
          else:
            if (((head[2] << 8) | head[3]) & 0xFFFF) != val:
              index = 0
              remain = 0
            else:
              index = 8
        if index > 0:
          package = [0]*(index + 1)
          package[1:5] = head
          remain = index - 4
          index = 5
          t = time.time()
          while remain > 0:
            if self._ser.inWaiting():
              data = self._ser.read(1)
              try: 
                package[index] = ord(data)
              except:
                package[index] = data
              index += 1
              remain -= 1
              t = time.time()
            if(time.time() - t > 0.1):
              print("time out1.")
              return [self.eRTU_RECV_ERROR]
          crc = ((package[len(package) - 2] << 8) | package[len(package) - 1]) & 0xFFFF
          if crc != self._calculate_crc(package[1:len(package) - 2]):
            print("CRC ERROR")
            return [self.eRTU_RECV_ERROR]
          if package[2] & 0x80:
            package[0] = package[3]
          else:
            package[0] = 0
          #lin = ['%02X' % i for i in package]
          #print(" ".join(lin))
          return package


