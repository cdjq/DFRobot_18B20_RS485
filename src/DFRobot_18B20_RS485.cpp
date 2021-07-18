/*!
 * @file DFRobot_18B20_RS485.cpp
 * @brief 这是一款18b20单总线协议转RS485及UART协议的协议转接板驱动库。使用它，你可以通过Arduino 主控的RS485和UART接口驱动18B20
 * @传感器。这块转换板有以下特点：
 * @n 1. 最大能同时连接8个18B20传感器;
 * @n 2. Arduino MCU通过UART接口或RS485接口同传感器通信。
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-07-05
 * @https://github.com/DFRobot/DFRobot_18B20_RS485
 */
#include <Arduino.h>
#include "DFRobot_18B20_RS485.h"

DFRobot_18B20_RS485::DFRobot_18B20_RS485(uint8_t addr,Stream *s)
  :DFRobot_RTU(s),_addr(addr){
  
}
DFRobot_18B20_RS485::~DFRobot_18B20_RS485(){}
/**
 * @brief 传感器设备初始化。
 * @return initialization state:
 * @n      0:  sucess
 * @n      -1：failed,未接入设备，协议转化板至少要挂载一个18B20设备
 * @n      -2: failed,接入设备过多，协议转化板最多只能挂载8个18B20设备
 */
int DFRobot_18B20_RS485::begin(){
  delay(2000);//wait for 1s
  if(_addr > 0xF7){
      DBG("Invaild Device addr.");
  }
  if(_addr != 0){
      if(!detectDeviceAddress(_addr)){
          DBG("Device addr Error.");
          return -1;
      }
      
      if(getPID() != DEVICE_PID){
          DBG("PID Error");
          return -1;
      }
      
      if(getVID() != DEVICE_VID){
          DBG("VID Error");
          return -1;
      }
  }
  
  return 0;
}

uint8_t DFRobot_18B20_RS485::configSerial(uint32_t baud, uint16_t config){
  uint8_t data[] = {0x00, 0x00, (uint8_t)(config>>8)&0xFF, (uint8_t)(config&0xFF)};
  switch(baud){
      case BAUDRATE_2400:
           data[1] = 1;
           break;
      case BAUDRATE_4800:
           data[1] = 2;
           break;
      case BAUDRATE_9600:
           data[1] = 3;
           break;
      case BAUDRATE_14400:
           data[1] = 4;
           break;
      case BAUDRATE_19200:
           data[1] = 5;
           break;
      case BAUDRATE_38400:
           data[1] = 6;
           break;
      case BAUDRATE_57600:
           data[1] = 7;
           break;
      case BAUDRATE_115200:
           data[1] = 8;
           break;
      default:
           data[1] = 3;
           break;
  }
  uint8_t ret = writeHoldingRegister(_addr, REG_UART_CTRL0, data, sizeof(data));
  return ret;
}

bool DFRobot_18B20_RS485::readSerialConfig(uint32_t *baud, uint16_t *config){
  if((baud == NULL) || (config == NULL)) return false;
  uint8_t data[4];
  uint8_t ret = readHoldingRegister(_addr, REG_UART_CTRL0, data, sizeof(data));
  DBG(ret,HEX);
  if(ret == 0){
      switch(data[1]){
          case 1:
            *baud = BAUDRATE_2400;
            break;
          case 2:
            *baud = BAUDRATE_4800;
            break;
          case 3:
            *baud = BAUDRATE_9600;
            break;
          case 4:
            *baud = BAUDRATE_14400;
            break;
          case 5:
            *baud = BAUDRATE_19200;
            break;
          case 6:
            *baud = BAUDRATE_38400;
            break;
          case 7:
            *baud = BAUDRATE_57600;
            break;
          case 8:
            *baud = BAUDRATE_115200;
            break;
          default:
            *baud = BAUDRATE_9600;
            break;
      }
      *config = ((data[2] << 8) & 0xFF) | data[1];
      return true;
  }
  
  return false;
}

uint8_t DFRobot_18B20_RS485::scan(){
  uint8_t temp[2];
  uint8_t state = 0;
  uint8_t ret = readHoldingRegister(_addr, REG_ROM_FLAG, &temp, sizeof(temp));
  if(ret == 0){
      state = temp[0] & temp[1];
  }
  return state;
}

bool DFRobot_18B20_RS485::setDeviceAddress(uint8_t newAddr){
  if(newAddr < 1 || newAddr > 0xF7) return false;
  uint16_t ret = writeHoldingRegister(_addr, REG_DEVICE_ADDR, (uint16_t)newAddr);
  if(_addr == 0){
      delay(100);
      ret = readHoldingRegister(newAddr, REG_DEVICE_ADDR);
  }
  if(ret == newAddr){
      _addr = newAddr;
      return true;
  }
  return false;
}

uint8_t DFRobot_18B20_RS485::getDeviceAddress(){
  return _addr;
}

bool DFRobot_18B20_RS485::set18B20Accuracy(uint8_t id, uint8_t accuracy){
  if(id >= DS18B20_MAX_NUM) return false;
  uint16_t ret = writeHoldingRegister(_addr, REG_18B20_NUM0_ACCURACY+id, (uint16_t)accuracy);
  if(ret == accuracy){
      return true;
  }else return false;
}
uint8_t DFRobot_18B20_RS485::get18B20Accuracy(uint8_t id){
  if(id >= DS18B20_MAX_NUM) return 0xFF;
  uint8_t accuracy = 0xFF;
  uint16_t ret = readCoilsRegister(_addr, REG_18B20_NUM0_ACCURACY+id);
  accuracy = (uint8_t)(ret & 0xFF);
  return accuracy;
}

bool DFRobot_18B20_RS485::setTemperatureThreshold(uint8_t id, int8_t tH, int8_t tL){
  if((id >= DS18B20_MAX_NUM) || (tH < - 55) || (tH > 125) || (tL < -55) || (tL > 125)) return false;
  uint16_t val = (((uint8_t)tH) << 8)|(uint8_t)tL;
  uint16_t ret = writeHoldingRegister(_addr, REG_18B20_NUM0_TH_TL+id, val);
  if(ret == val){
      return true;
  }else return false;
}

bool DFRobot_18B20_RS485::getTemperatureThreshold(uint8_t id, int8_t *tH, int8_t *tL){
  if(id >= DS18B20_MAX_NUM) return false;
  uint8_t temp[2];
  uint8_t ret = readHoldingRegister(_addr, REG_18B20_NUM0_TH_TL+id, &temp, sizeof(temp));
  if(ret == 0){
      *tH = (int8_t)temp[0];
      *tL = (int8_t)temp[1];
      return true;
  }
  return false;
}

bool DFRobot_18B20_RS485::get18B20ROM(uint8_t id, uint8_t *rom, uint8_t len){
  if((id >= DS18B20_MAX_NUM) || (rom == NULL) || (len != DS18B20_ROM_SIZE)) return false;
  uint8_t ret = readHoldingRegister(_addr, REG_18B20_NUM0_ADDR+id, rom, len);
  if(ret == 0){
      return true;
  }
  
  return false;
}
  
/**
 * @brief 获取设备id的温度。
 * @param id：18B20挂载在总线上的id号，范围0~7
 * @return 温度:
 */
float DFRobot_18B20_RS485::getTemperatureC(uint8_t id){
  if(id >= DS18B20_MAX_NUM) return 0.0;
  uint16_t temp = readHoldingRegister(_addr, REG_18B20_NUM0_TEMP+id);
  return temp/16.0;
}
/**
 * @brief 获取协议转换板上挂载的18B20的个数。
 * @return 18B20设备数量，范围0~8：
 * @n      0：协议转换板上未挂载18B20传感器
 * @n      1: 协议转换板上挂载了1个18B20传感器
 * @n      ......
 * @n      8:  协议转换板上挂载了8个18B20传感器
 */
uint8_t DFRobot_18B20_RS485::get18B20Number(){
  uint8_t num = (uint8_t)readHoldingRegister(_addr, REG_18B20_NUM);
  return num;
}
/**
 * @brief 判断设备是否存在？如果存在将存储该设备的64位ROM码。
 * @param id：18B20挂载在总线上的id号，范围0~7
 * @param rom: 64位（8字节）18B20的ROM码，用来区分同一总线上的设备。
 * @return 设备存在状态
 * @n      true:  该设备存在
 * @n      false：该设备不存在
 */
bool DFRobot_18B20_RS485::exist(uint8_t id, uint8_t rom[8]){
  return false;
}

uint16_t DFRobot_18B20_RS485::getPID(){
  uint16_t val = readHoldingRegister(_addr, REG_PID);
  return val;
}

uint16_t DFRobot_18B20_RS485::getVID(){
  uint16_t ret = readHoldingRegister(_addr, REG_VID);
  return ret;
}

bool DFRobot_18B20_RS485::detectDeviceAddress(uint8_t addr){
  uint16_t ret = readHoldingRegister(addr, REG_DEVICE_ADDR);
  if((ret & 0xFF) == addr){
      return true;
  }
  return false;
}

DFRobot_18B20_UART::DFRobot_18B20_UART(uint8_t addr, Stream *s)
  :DFRobot_18B20_RS485(addr, s){
  
}