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

DFRobot_18B20_RS485::DFRobot_18B20_RS485(Stream *s)
  :_addr(BROADCAST_ADDRESS), _s(s){
  
}
DFRobot_18B20_RS485::~DFRobot_18B20_RS485(){}
/**
 * @brief 传感器设备初始化。
 * @return initialization state:
 * @n      0:  sucess
 * @n      -1：failed,未接入设备，协议转化板至少要挂载一个18B20设备
 * @n      -2: failed,接入设备过多，协议转化板最多只能挂载8个18B20设备
 */
int DFRobot_18B20_RS485::begin(uint8_t addr){
  delay(1000);//wait for 1s
  _addr = addr;
  if(getPID() != DEVICE_PID){
      DBG("PID Error");
      return -1;
  }
  
  if(getVID() != DEVICE_VID){
      DBG("VID Error");
      return -1;
  }
  return 0;
}

uint8_t DFRobot_18B20_RS485::configSerial(uint32_t baud, uint16_t config){
  uint8_t ret = 0;
  uint8_t data[] = {0x00,0x02,0x04, 0x00,0x00,(config<<8)&0xFF, config&0xFF};
  switch(baud){
      case BAUDRATE_2400:
           data[6] = 1;
           break;
      case BAUDRATE_4800:
           data[6] = 2;
           break;
      case BAUDRATE_9600:
           data[6] = 3;
           break;
      case BAUDRATE_14400:
           data[6] = 4;
           break;
      case BAUDRATE_19200:
           data[6] = 5;
           break;
      case BAUDRATE_38400:
           data[6] = 6;
           break;
      case BAUDRATE_57600:
           data[6] = 7;
           break;
      case BAUDRATE_115200:
           data[6] = 8;
           break;
      default:
           data[6] = 3;
           break;
  }
  pPacketHeader_t header = packed(eRTU_WRITE_MULTIPLE_REG_CMD, REG_UART_CTRL0, data, sizeof(data));
  ret = parsePackage(header, NULL, 0);
  return ret;
}

bool DFRobot_18B20_RS485::setDeviceAddress(uint8_t newAddr){
  if(newAddr < 1 || newAddr > 0xF7) return false;
  uint8_t data[] = {0x00, newAddr};
  pPacketHeader_t header = packed(eRTU_WRITE_REG_CMD, REG_DEVICE_ADDR, data, sizeof(data));
  if(_addr == newAddr) return true;
  else return false;
}

uint8_t DFRobot_18B20_RS485::getDeviceAddress(){
  return _addr;
}

uint8_t DFRobot_18B20_RS485::set18B20Accuracy(uint8_t id, uint8_t accuracy){
  uint8_t data[] = {0x00, accuracy};
  pPacketHeader_t header = packed(eRTU_WRITE_REG_CMD,REG_18B20_NUM0_ACCURACY, data, sizeof(data));
  return parsePackage(header, NULL, 0);
}
uint8_t DFRobot_18B20_RS485::get18B20Accuracy(uint8_t id){
  uint8_t ret = 0;
  uint8_t data[] = {0x00, 0x01};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD, REG_18B20_NUM0_ACCURACY, data, sizeof(data));
  ret = parsePackage(header, &data, 2);
  if(ret == 0){
      ret = data[1];
  }
  return ret;
}

uint8_t DFRobot_18B20_RS485::setTemperatureThreshold(uint8_t id, int8_t tH, int8_t tL){
  uint8_t ret = 0;
  if(tH <= tL || tH < -55 || tH > 125 || tL < -55 || tL > 125){
      return -1;
  }
  uint8_t data[] = {(uint8_t)tH, (uint8_t)tL};
  pPacketHeader_t header = packed(eRTU_WRITE_REG_CMD, REG_18B20_NUM0_TH_TL, data, sizeof(data));
  return parsePackage(header, NULL, 0);
}

uint8_t DFRobot_18B20_RS485::getTemperatureThreshold(uint8_t id, int8_t *tH, int8_t *tL){
  uint8_t ret = 0;
  uint8_t data[] = {0x00, 0x01};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD, REG_18B20_NUM0_TH_TL, data, sizeof(data));
  ret = parsePackage(header, &data, 2);
  if(ret == 0){
      *tH = (int8_t)data[0];
      *tL = (int8_t)data[1];
  }
  return ret;
}

uint8_t DFRobot_18B20_RS485::get18B20ROM(uint8_t id, uint8_t *rom, uint8_t len){
  uint8_t ret = 0;
  if(len != DS18B20_ROM_SIZE) return 1;
  uint8_t data[] = {0x00, 0x04};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD, REG_18B20_NUM0_ADDR, data, sizeof(data));
  ret = parsePackage(header, &rom, len);
  return ret;
}
  
/**
 * @brief 获取设备id的温度。
 * @param id：18B20挂载在总线上的id号，范围0~7
 * @return 温度:
 */
float DFRobot_18B20_RS485::getTemperatureC(uint8_t id){
  uint8_t ret = 0;
  float temp = 0;
  uint8_t data[] = {0x00, 0x01};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD, REG_18B20_NUM0_TEMP+id, data, sizeof(data));
  ret = parsePackage(header, &data, 2);
  if(ret == 0){
      temp = (data[0] << 8) | data[1];
  }
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
  uint16_t ret = 0;
  uint8_t data[] = {0x00, 0x01};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD, REG_18B20_NUM, data, sizeof(data));
  parsePackage(header, &ret, 2);
  return ret;
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
  uint16_t ret = 0;
  uint8_t data[] = {0x00, 0x01};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD, REG_PID, data, sizeof(data));
  parsePackage(header, &ret, 2);
  return ret;
}

uint16_t DFRobot_18B20_RS485::getVID(){
  uint16_t ret = 0;
  uint8_t data[] = {0x00, 0x01};
  pPacketHeader_t header = packed(eRTU_READ_REG_CMD,REG_VID, data, sizeof(data));
  parsePackage(header, &ret, 2);
  return ret;
}

uint8_t DFRobot_18B20_RS485::parsePackage(pPacketHeader_t head, void *data, uint16_t len){
  eStatusCode_t ret = eRTU_OK;
  if(head != NULL){
      switch(head->cmd){
          case eRTU_WRITE_REG_CMD:
          case eRTU_WRITE_MULTIPLE_REG_CMD:
               memcpy(data, &head->payload[2], len);
               break;
          case eRTU_READ_REG_CMD:
               if(head->payload[0] == len){
                   memcpy(data, &head->payload[1], len);
               }
               break;
          default:
               ret = (eStatusCode_t)head->payload[2];
               break;
      }
      free(head);
  }else{
      eStatusCode_t ret = eRTU_RECV_ERROR;
  }
  return (uint8_t)ret;
}

pPacketHeader_t DFRobot_18B20_RS485::packed(eMoudbusCmd_t cmd, uint16_t reg, uint8_t *data, uint16_t len){
  packed((uint8_t)cmd, reg, data, len);
}

pPacketHeader_t DFRobot_18B20_RS485::packed(uint8_t cmd, uint16_t reg, uint8_t *data, uint16_t len){
  pPacketHeader_t header = NULL;
  uint16_t length = sizeof(sPacketHeader_t) + len+2;
  uint16_t crc = 0;
  if((header = (pPacketHeader_t)malloc(length)) == NULL){
      DBG("Memory ERROR");
      return NULL;
  }
  header->payload[0] = (reg >> 8) & 0xFF;
  header->payload[1] = reg  & 0xFF;
  header->addr = _addr;
  header->cmd = cmd;
  memcpy(header->payload+2, data, len);
  crc = calculateCRC((uint8_t *)header, length-2);
  header->payload[length-4] = (crc >> 8) & 0xFF;
  header->payload[length-3] = crc & 0xFF;
  _s->write((uint8_t *)header, length);
  delay(5);
  switch(cmd){
      case eRTU_WRITE_REG_CMD:
      case eRTU_WRITE_MULTIPLE_REG_CMD:
           length = 8;
           break;
      case eRTU_READ_REG_CMD:
           length = 5 + ((header->payload[2] << 8) | header->payload[3]);
           break;
  }
  free(header);
  return recvPackage(cmd, length);
}

pPacketHeader_t DFRobot_18B20_RS485::recvPackage(uint8_t cmd, uint16_t len){
  uint8_t head[2] = {0,0};
  uint8_t index = 0;
  uint16_t crc = 0;
  pPacketHeader_t header = NULL;
  while(_s->available()){
      head[index++] = (uint8_t)_s->read();
LOOP:
      if((index == 1) && (head[0] != 0)){
          index = 0;
      }else if((index == 2) && ((head[1] & 0x7F) != cmd)){
          index = 1;
          head[0] = head[1];
          
          goto LOOP;
      }else if((index == 2) && ((head[1] & 0x7F) == cmd)){
          
          break;
      }
  }
  if(head[1] & 0x80) len = 5;
  
  if((header = (pPacketHeader_t)malloc(len)) == NULL){
      DBG("Memory ERROR");
      return NULL;
  }
  memcpy((uint8_t *)header, head, 2);
  len -= 2;
  index = 0;
  while(len--){
      if(_s->available()){
          header->payload[index++] = (uint8_t)_s->read();
      }else{
          free(header);
          return NULL;
      }
  }
  crc = (header->payload[len-4] << 8) | header->payload[len-3];
  if(crc != calculateCRC((uint8_t *)header, len-2)){
      free(header);
      DBG("CRC ERROR");
      return NULL;
  }
  if(_addr == 0) _addr = head[0];
  return header;
}

uint16_t DFRobot_18B20_RS485::calculateCRC(uint8_t *data, uint8_t len){
  uint16_t crc = 0xFFFF;
  for( uint8_t pos = 0; pos < len; pos++){
    crc ^= (uint16_t)data[ pos ];
    for(uint8_t i = 8; i != 0; i--){
      if((crc & 0x0001) != 0){
        crc >>= 1;
        crc ^= 0xA001;
      }else{
         crc >>= 1;
      }
    }
  }
  crc = ((crc & 0x00FF) << 8) | ((crc & 0xFF00) >> 8);
  return crc;
}

DFRobot_18B20_UART::DFRobot_18B20_UART(Stream *s)
  :DFRobot_18B20_RS485(s){
  
}