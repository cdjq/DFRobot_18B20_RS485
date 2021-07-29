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

#define REG_PID                   0x0000 //协议转换板的寄存器
#define REG_VID                   0x0001 //协议转换板的寄存器
#define REG_DEVICE_ADDR           0x0002 //协议转换板的寄存器
#define REG_UART_CTRL0            0x0003 //协议转换板的寄存器
#define EG_UART_CTRL1             0x0004 //协议转换板的寄存器
#define REG_VERSION               0x0005 //协议转换板的寄存器
#define REG_ALARM                 0x0008 //协议转换板的寄存器
#define REG_ROM_FLAG              0x0009 //协议转换板的寄存器
#define REG_18B20_NUM             0x000A //协议转换板的寄存器
#define REG_18B20_NUM0_ROM        0x000B //协议转换板的寄存器
#define REG_18B20_NUM0_TEMP       0x002B //协议转换板的寄存器
#define REG_18B20_NUM0_TH_TL      0x0033 //协议转换板的寄存器
#define REG_18B20_NUM0_ACCURACY   0x003B //协议转换板的寄存器
#define RS485_SERIAL_8N1    (1<<6) | (0<<3) | (0 << 0)

DFRobot_18B20_RS485::DFRobot_18B20_RS485(uint8_t addr,Stream *s)
  :DFRobot_RTU(s),_addr(addr){
  
}
DFRobot_18B20_RS485::DFRobot_18B20_RS485()
  :DFRobot_RTU(),_addr(TEL0144_DEFAULT_DEVICE_ADDRESS){
  
}
DFRobot_18B20_RS485::~DFRobot_18B20_RS485(){}

int DFRobot_18B20_RS485::begin(){
  delay(2000);//wait for 1s
  setTimeoutTimeMs(500);
  if(_addr > 0xF7){
      DBG("Invaild Device addr.");
  }
  if(_addr != 0){
      if(!detectDeviceAddress(_addr)){
          DBG("Device addr Error.");
          return -1;
      }
      
      if(getDevicePID() != DEVICE_PID){
          DBG("PID Error");
          return -1;
      }
      
      if(getDeviceVID() != DEVICE_VID){
          DBG("VID Error");
          return -1;
      }
  }
  
  return 0;
}

uint16_t DFRobot_18B20_RS485::getDevicePID(){
  uint16_t val = readHoldingRegister(_addr, REG_PID);
  return val;
}

uint16_t DFRobot_18B20_RS485::getDeviceVID(){
  uint16_t val = readHoldingRegister(_addr, REG_VID);
  return val;
}

bool DFRobot_18B20_RS485::setBaudrate(eBaudrateConfig_t baud){
  return setBaudrate((uint32_t)baud);
}

bool DFRobot_18B20_RS485::setBaudrate(uint32_t baud){
  uint8_t data[] = {0x00, 0x00};
  switch(baud){
      case eBAUDRATE_2400:
           data[1] = 1;
           break;
      case eBAUDRATE_4800:
           data[1] = 2;
           break;
      case eBAUDRATE_9600:
           data[1] = 3;
           break;
      case eBAUDRATE_14400:
           data[1] = 4;
           break;
      case eBAUDRATE_19200:
           data[1] = 5;
           break;
      case eBAUDRATE_38400:
           data[1] = 6;
           break;
      case eBAUDRATE_57600:
           data[1] = 7;
           break;
      case eBAUDRATE_115200:
           data[1] = 8;
           break;
      default:
           data[1] = 3;
           break;
  }
  uint8_t ret = writeHoldingRegister(_addr, REG_UART_CTRL0, data, sizeof(data));
  if(ret == 0) return true;
  else return false;
}

uint32_t DFRobot_18B20_RS485::getBaudrate(){
  uint32_t baud = 0;
  uint8_t data[4];
  uint8_t ret = readHoldingRegister(_addr, REG_UART_CTRL0, data, sizeof(data));
  if(ret == 0){
      switch(data[1]){
          case 1:
            baud = (uint32_t)eBAUDRATE_2400;
            break;
          case 2:
            baud = (uint32_t)eBAUDRATE_4800;
            break;
          case 3:
            baud = (uint32_t)eBAUDRATE_9600;
            break;
          case 4:
            baud = (uint32_t)eBAUDRATE_14400;
            break;
          case 5:
            baud = (uint32_t)eBAUDRATE_19200;
            break;
          case 6:
            baud = (uint32_t)eBAUDRATE_38400;
            break;
          case 7:
            baud = (uint32_t)eBAUDRATE_57600;
            break;
          case 8:
            baud = (uint32_t)eBAUDRATE_115200;
            break;
          default:
            baud = (uint32_t)eBAUDRATE_9600;
            break;
      }
  }
  
  return baud;
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
      delay(1000);
      ret = readHoldingRegister(newAddr, REG_DEVICE_ADDR);
      DBG(ret,HEX);
      if(ret == newAddr){
        _addr = newAddr;
        return true;
      }
  }else{
    if(ret == 0){
        _addr = newAddr;
        return true;
    }
  }
  return false;
}

uint8_t DFRobot_18B20_RS485::getDeviceAddress(){
  return _addr;
}

bool DFRobot_18B20_RS485::set18B20Accuracy(e18B20IDNum_t id, e18B20Accuracy_t accuracy){
  return set18B20Accuracy((uint8_t)id, (uint8_t)accuracy);
}

bool DFRobot_18B20_RS485::set18B20Accuracy(uint8_t id, uint8_t accuracy){
   if(id >= (uint8_t)e18B20_ID_MAX) {
     DBG("ID out of range(0~7).");
     return false;
   }
   uint8_t ret = writeHoldingRegister(_addr, REG_18B20_NUM0_ACCURACY + id, (uint16_t)accuracy);
   if(ret != 0){
     DBG("Set addr failed.")
     DBG(ret);
     return false;
   }
   return true;
}

bool DFRobot_18B20_RS485::batchSet18B20Accuracy(e18B20BatchID_t batchId, e18B20Accuracy_t accuracy){
  return batchSet18B20Accuracy((uint8_t)batchId, (uint8_t)accuracy);
}

bool DFRobot_18B20_RS485::batchSet18B20Accuracy(uint8_t batchId, uint8_t accuracy){//批量设置
  if(accuracy > (uint8_t)e18B20_ACCURACY_12_BIT){
    DBG("accuracy out of range(0~3)");
    DBG(accuracy);
    return false;
  }
  if(batchId == 0){
    DBG("batchId param error.");
    return false;
  }
  uint8_t temp[16];
  uint8_t id;
  uint8_t size = 0;
  uint8_t ret  = 0;
  batchId &= 0xFF;
  for(int i = 0; i <= DEVICE_CONNECTED_MAX_NUM; i++){
    if((i != DEVICE_CONNECTED_MAX_NUM) && (batchId & (1 << i))){
       if(size == 0) id = i;
       temp[size] = 0x00;
       temp[size + 1] = accuracy;
       size += 2;
    }else{
       if(size != 0){
          ret = writeHoldingRegister(_addr, REG_18B20_NUM0_ACCURACY+id, temp, size);
          size = 0;
          if(ret != 0){
            DBG(batchId,BIN);
            DBG(id);  //start id
            DBG(i - 1);   //end id
            DBG("Set threshold Error.");
            return false;
          }
       }
    }
  }
  return true;
}
uint8_t DFRobot_18B20_RS485::get18B20Accuracy(uint8_t id){
  if(id >= DEVICE_CONNECTED_MAX_NUM) return 0xFF;
  uint8_t accuracy = 0xFF;
  uint16_t ret = readHoldingRegister(_addr, REG_18B20_NUM0_ACCURACY+id);
  accuracy = (uint8_t)(ret & 0xFF);
  return accuracy;
}

uint8_t DFRobot_18B20_RS485::get18B20Accuracy(e18B20IDNum_t id){
  return get18B20Accuracy((uint8_t)id);
}


//批量设置中不能有读，因为modbus从机不会响应广播包
bool DFRobot_18B20_RS485::batchSet18B20TemperatureThreshold(uint8_t batchId, int8_t tH, int8_t tL){
  if((tH < - 55) || (tH > 125) || (tL < -55) || (tL > 125) || (tL > tH)){
     DBG("tH, tL params error");
     return false;
  }
  if(batchId == 0){
    DBG("batchId param error.");
    return false;
  }
  uint8_t temp[16];
  uint8_t id;
  uint8_t size = 0;
  uint8_t ret  = 0;
  batchId &= 0xFF;
  for(int i = 0; i <= DEVICE_CONNECTED_MAX_NUM; i++){
    if((i != DEVICE_CONNECTED_MAX_NUM) && (batchId & (1 << i))){
       if(size == 0) id = i;
       temp[size] = (uint8_t)tH;
       temp[size + 1] = (uint8_t)tL;
       size += 2;
    }else{
       if(size != 0){
          ret = writeHoldingRegister(_addr, REG_18B20_NUM0_TH_TL+id, temp, size);
          size = 0;
          if(ret != 0){
            DBG(batchId,BIN);
            DBG(id);  //start id
            DBG(i - 1);   //end id
            DBG("Set threshold Error.");
            return false;
          }
       }
    }
  }
  return true;
}
bool DFRobot_18B20_RS485::batchSet18B20TemperatureThreshold(e18B20IDNum_t batchId, int8_t tH, int8_t tL){
  return batchSet18B20TemperatureThreshold((uint8_t)batchId, tH, tL);
}

bool DFRobot_18B20_RS485::setTemperatureThreshold(e18B20IDNum_t id, int8_t tH, int8_t tL){
  return setTemperatureThreshold((uint8_t)id, tH, tL);
}

bool DFRobot_18B20_RS485::setTemperatureThreshold(uint8_t id, int8_t tH, int8_t tL){
  if((tH < - 55) || (tH > 125) || (tL < -55) || (tL > 125) || (tL > tH)) return false;

  if(id >= (uint8_t)e18B20_ID_MAX) {
     DBG("ID out of range(0~7).");
     return false;
   }
   uint16_t  temp = (((uint8_t)tH) << 8) | (uint8_t)tL;
   uint8_t ret = writeHoldingRegister(_addr, REG_18B20_NUM0_TH_TL + id, temp);
   if(ret != 0){
     DBG("Set addr failed.")
     DBG(ret);
     return false;
   }
   return true;
}

bool DFRobot_18B20_RS485::getTemperatureThreshold(e18B20IDNum_t id, int8_t *tH, int8_t *tL){
  return getTemperatureThreshold((uint8_t) id, tH, tL);
}

bool DFRobot_18B20_RS485::getTemperatureThreshold(uint8_t id, int8_t *tH, int8_t *tL){
  if(id >= DEVICE_CONNECTED_MAX_NUM) return false;
  uint8_t temp[2];
  uint8_t ret = readHoldingRegister(_addr, REG_18B20_NUM0_TH_TL+id, &temp, sizeof(temp));
  if(ret == 0){
      if(tH != NULL) *tH = (int8_t)temp[0];
      if(tL != NULL) *tL = (int8_t)temp[1];
      return true;
  }
  return false;
}

uint16_t DFRobot_18B20_RS485::getTemperatureThresholdAlarmFlag(){
  uint16_t flag = readHoldingRegister(_addr, REG_ALARM);
  return flag;
}

uint8_t DFRobot_18B20_RS485::parseThresholdAlarmFlag(uint8_t id, uint16_t alarmFlag){
  if((alarmFlag >> id) & 0x01){
    if((alarmFlag >> (8+id)) & 0x01){
      return ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD;
    }
    return BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD;
  }
  return IN_THE_TEMPERATURE_THRESHOLD;
}
uint8_t DFRobot_18B20_RS485::parseThresholdAlarmFlag(e18B20IDNum_t id, uint16_t alarmFlag){
  return parseThresholdAlarmFlag((uint8_t)id, alarmFlag);
}

uint8_t DFRobot_18B20_RS485::get18B20ROM(e18B20IDNum_t id,  uint8_t (&rom)[8]){
  return get18B20ROM((uint8_t)id, rom);
}

uint8_t DFRobot_18B20_RS485::get18B20ROM(uint8_t id, uint8_t (&rom)[8]){
  if(id >= DEVICE_CONNECTED_MAX_NUM){
    DBG("id param out of range(0~7)");
    DBG(id)
    return 0;
  }
  if(rom == NULL){
    DBG("rom is NULL");
    return 0;
  }
  uint8_t ret = readHoldingRegister(_addr, REG_18B20_NUM0_ROM+id*4, rom, DS18B20_ROM_BYTES);
  if(ret == 0){
      return DS18B20_ROM_BYTES;
  }
  
  return 0;
}

String DFRobot_18B20_RS485::getROMHexString(uint8_t rom[8]){
  String rslt = "";
  String code = "";
  char hexarr[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  uint8_t n;
  for(int i = 0; i < 8; i++){
    n = rom[i];
    if(n == 0){
      rslt += "00";
      continue;
    }
    code = "";
    while(n){
      int i = n % 16;
      code = hexarr[i] + code;
      n /= 16;
    }
    if(code.length() < 2){
      code = '0'+code;
    }
    rslt += code;
  }
  return rslt;
}

float DFRobot_18B20_RS485::getTemperatureC(e18B20IDNum_t id){
  return getTemperatureC((uint8_t)id);
}
  
float DFRobot_18B20_RS485::getTemperatureC(uint8_t id){
  if(id >= DEVICE_CONNECTED_MAX_NUM) return 0.0;
  uint16_t temp = readHoldingRegister(_addr, REG_18B20_NUM0_TEMP+id);
  return temp/16.0;
}

uint8_t DFRobot_18B20_RS485::get18B20Number(){
  uint8_t num = (uint8_t)readHoldingRegister(_addr, REG_18B20_NUM);
  return num;
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

DFRobot_18B20_UART::DFRobot_18B20_UART()
  :DFRobot_18B20_RS485(){
  
}