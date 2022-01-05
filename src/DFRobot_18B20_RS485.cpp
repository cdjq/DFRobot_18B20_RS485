/*!
 * @file DFRobot_18B20_RS485.cpp
 * @brief 这是一款18b20单总线协议转RS485及cdc协议的协议转接板驱动库。通过它，你可以通过RS485接口配置和获取18B20传感器的相关配置和数据。
 * @details TEL0144协议转换板特点：
 * @n 1. 这是一个RS485从机设备，默认设备地址为0x20；
 * @n 3. 板载4个18B20传感器采集IO口，分别为D1、D2、D3、D4，每个IO口最大连接4个18B20传感器；
 * @n 2. 板载RS485接口，用户可以通过RS485接口配置或获取连接的18B20的传感器的配置和数据；
 * @n 3. 板载USB CDC接口，用户可以通过USB CDC串口来配置和读取TEL0144的相关配置和数据； 
 * @n 4. 默认串口通信配置：9600波特率, 8位数据位，无校验位，1位停止位，其中波特率可配置
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-07-05
 * @https://github.com/DFRobot/DFRobot_18B20_RS485
 */
#include <Arduino.h>
#include "DFRobot_18B20_RS485.h"

#define REG_PID                      0x0000 //< TEL0144 RS485从机设备的产品标识符寄存器地址
#define REG_VID                      0x0001 //< TEL0144 RS485从机设备的厂商标识符寄存器地址
#define REG_DEVICE_ADDR              0x0002 //< TEL0144 RS485从机设备地址寄存器地址
#define REG_UART_CTRL0               0x0003 //< TEL0144 RS485从机设备的串口配置寄存器地址
#define REG_UART_CTRL1               0x0004 //< TEL0144 RS485从机设备的串口配置寄存器地址
#define REG_VERSION                  0x0005 //< TEL0144 RS485从机设备的版本地址
#define REG_D1_CONFIG                0x0008 //< TEL0144 RS485从机设备D1引脚的相关配置
#define REG_18B20_D1_ALARM           0x000C //< TEL0144 RS485从机设备D1口连接的传感器的阈值报警状态
#define REG_18B20_NUM                0x0010 //< TEL0144 RS485从机设备实际连接的18B20传感器总数量寄存器地址
#define REG_18B20_D1_NUM0_ROM        0x0011 //< TEL0144 RS485从机设备D1采集口0号18B20传感器ROM最高位寄存器的地址
#define REG_18B20_D1_NUM0_TEMP       0x0051 //< TEL0144 RS485从机设备D1采集口0号18B20传感器温度寄存器的地址
#define REG_18B20_D1_NUM0_TH_TL      0x0061 //< TEL0144 RS485从机设备D1采集口0号18B20传感器阈值配置寄存器的地址
#define REG_18B20_D1_NUM0_ACCURACY   0x0071 //< TEL0144 RS485从机设备D1采集口0号18B20传感器精度配置寄存器的地址
#define REG_D1_CONNECTED_FLAG        0x0081 //< TEL0144 RS485从机设备D1口各序号对应的18B20传感器的连接状态寄存器
#define RS485_SERIAL_8N1    (1<<6) | (0<<3) | (0 << 0) //< TEL0144 RS485从机设备被配置位8位数据位，无校验位和1位停止位的值

DFRobot_18B20_RS485::DFRobot_18B20_RS485(uint8_t addr, Stream *s)
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
  DBG(val,HEX);
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

uint16_t DFRobot_18B20_RS485::scan(){
  uint8_t temp[8];
  uint16_t state = 0;
  uint8_t ret = readHoldingRegister(_addr, REG_D1_CONNECTED_FLAG, &temp, sizeof(temp));
  if(ret == 0){
      state = (temp[0] & temp[1]) & 0x0F;         //D1
      state |= ((temp[2] & temp[3]) & 0x0F) << 4; //D2
      state |= ((temp[4] & temp[5]) & 0x0F) << 8; //D3
      state |= ((temp[6] & temp[7]) & 0x0F) << 12;//D4
  }
  return state;
}

bool DFRobot_18B20_RS485::setDeviceAddress(uint8_t newAddr){
  if(newAddr < 1 || newAddr > 0xF7) return false;
  uint16_t ret = writeHoldingRegister(_addr, REG_DEVICE_ADDR, (uint16_t)newAddr);
  if(_addr == 0){
      delay(1000);
      //ret = readHoldingRegister(newAddr, REG_DEVICE_ADDR);
      //DBG(ret,HEX);
      // if(ret == newAddr){
      // _addr = newAddr;
       // return true;
      //}
      return true;
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
bool DFRobot_18B20_RS485::set18B20Accuracy(uint8_t io, uint8_t id, uint16_t accuracy){
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return false;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return false;
  }
  uint8_t ret = writeHoldingRegister(_addr, REG_18B20_D1_NUM0_ACCURACY + 4 * (io - 1) + id, (uint16_t)accuracy);
  if(ret != 0){
     DBG("Set Accuracy failed.")
     DBG(ret);
     return false;
  }
   return true;

}

bool DFRobot_18B20_RS485::set18B20Accuracy(eGPIONum_t io, e18B20IDNum_t id, e18B20Accuracy_t accuracy){
  return set18B20Accuracy((uint8_t)io, (uint8_t)id, (uint16_t)accuracy);
}

bool DFRobot_18B20_RS485::batchSet18B20Accuracy(eGPIOBatchNum_t batchIo, e18B20BatchID_t batchId, e18B20Accuracy_t accuracy){
  return batchSet18B20Accuracy((uint8_t)batchIo, (uint8_t)batchId, (uint16_t)accuracy);
}

bool DFRobot_18B20_RS485::batchSet18B20Accuracy(uint8_t batchIo, uint8_t batchId, uint16_t accuracy){//批量设置
  batchIo &= 0x0F;
  batchId &= 0x0F;
  if(batchIo == 0){
    DBG("batchIo param is zero.");
    return false;
  }
  if(batchId == 0){
    DBG("batchId param error.");
    return false;
  }
  if(accuracy > (uint8_t)e18B20_ACCURACY_12_BIT){
    DBG("accuracy out of range(0~3)");
    DBG(accuracy);
    return false;
  }
  uint8_t temp[8];
  uint8_t id;
  uint8_t size = 0;
  uint8_t ret  = 0;
  for(int i = 0; i < DS18B20_CONNECT_IO_NUM; i++){
    if(batchIo & (1 << i)){
      for(int j = 0; j <= DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM; j++){
        if((j < DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM) && (batchId & (1 << j))){
          if(size == 0) id = j;
          temp[size] = 0x00;
          temp[size + 1] = accuracy;
          size += 2;
        }else{
          if(size != 0){
             ret = writeHoldingRegister(_addr, REG_18B20_D1_NUM0_ACCURACY + 4*i + id, temp, size);
             size = 0;
             if(ret != 0){
               DBG("Set Accuracy Error.");
               return false;
             }
          }
        }
      }

    }
  }
  return true;
}

uint16_t DFRobot_18B20_RS485::get18B20Accuracy(uint8_t io, uint8_t id){
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return 0xFFFF;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return 0xFFFF;
  }
  uint16_t accuracy = readHoldingRegister(_addr, REG_18B20_D1_NUM0_ACCURACY + 4*(io - 1) +id);
  return accuracy;
}

uint16_t DFRobot_18B20_RS485::get18B20Accuracy(eGPIONum_t io, e18B20IDNum_t id){
  return get18B20Accuracy((uint8_t)io, (uint8_t)id);
}

bool DFRobot_18B20_RS485::setTemperatureThreshold(uint8_t io, uint8_t id, int8_t tH, int8_t tL){
  if((tH < - 55) || (tH > 125) || (tL < -55) || (tL > 125) || (tL > tH)) {
    DBG("Temperature threshold is out of range -55~125");
    return false;
  }
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return false;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return false;
  }
  uint16_t  temp = (((uint8_t)tH) << 8) | (uint8_t)tL;
  uint8_t ret = writeHoldingRegister(_addr, REG_18B20_D1_NUM0_TH_TL + (io-1)*4 + id, temp);
  if(ret != 0){
     DBG("Set Temperature threshold failed.")
     return false;
  }
  return true;
}

bool DFRobot_18B20_RS485::setTemperatureThreshold(eGPIONum_t io, e18B20IDNum_t id, int8_t tH, int8_t tL){
  return setTemperatureThreshold((uint8_t)io,(uint8_t)id, tH, tL);
}

//批量设置中不能有读，因为modbus从机不会响应广播包
bool DFRobot_18B20_RS485::batchSet18B20TemperatureThreshold(uint8_t batchIo, uint8_t batchId, int8_t tH, int8_t tL){
  batchIo &= 0x0F;
  batchId &= 0x0F;
  if(batchIo == 0){
    DBG("batchIo param is zero.");
    return false;
  }
  if(batchId == 0){
    DBG("batchId param error.");
    return false;
  }
  if((tH < - 55) || (tH > 125) || (tL < -55) || (tL > 125) || (tL > tH)){
     DBG("tH, tL params error");
     return false;
  }

  uint8_t temp[8];
  uint8_t id;
  uint8_t size = 0;
  uint8_t ret  = 0;
  for(int i = 0; i < DS18B20_CONNECT_IO_NUM; i++){
    if(batchIo & (1 << i)){
      for(int j = 0; j <= DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM; j++){
        if((j < DS18B20_CONNECTED_TO_EACH_IO_MAX_NUM) && (batchId & (1 << j))){
          if(size == 0) id = j;
          temp[size] = (uint8_t)tH;
          temp[size + 1] = (uint8_t)tL;
          size += 2;
        }else{
          if(size != 0){
             ret = writeHoldingRegister(_addr, REG_18B20_D1_NUM0_TH_TL + i*4 + id, temp, size);
             size = 0;
             if(ret != 0){
               DBG("Set Accuracy Error.");
               return false;
             }
          }
        }
      }

    }
  }
  return true;
}

bool DFRobot_18B20_RS485::batchSet18B20TemperatureThreshold(eGPIONum_t batchIo, e18B20IDNum_t batchId, int8_t tH, int8_t tL){
  return batchSet18B20TemperatureThreshold((uint8_t)batchIo, (uint8_t)batchId, tH, tL);
}

bool DFRobot_18B20_RS485::getTemperatureThreshold(eGPIONum_t io, e18B20IDNum_t id, int8_t *tH, int8_t *tL){
  return getTemperatureThreshold((uint8_t)io, (uint8_t) id, tH, tL);
}

bool DFRobot_18B20_RS485::getTemperatureThreshold(uint8_t io, uint8_t id, int8_t *tH, int8_t *tL){
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return false;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return false;
  }
  uint8_t temp[2];
  uint8_t ret = readHoldingRegister(_addr,REG_18B20_D1_NUM0_TH_TL + (io - 1)*4 + id, &temp, sizeof(temp));
  if(ret == 0){
      if(tH != NULL) *tH = (int8_t)temp[0];
      if(tL != NULL) *tL = (int8_t)temp[1];
      return true;
  }
  return false;
}

uint32_t DFRobot_18B20_RS485::getTemperatureThresholdAlarmFlag(){
  uint8_t temp[8];
  uint32_t state = 0;
  uint8_t ret = readHoldingRegister(_addr, REG_18B20_D1_ALARM, &temp, sizeof(temp));
  //for(int i = 0; i < 8; i++){
  //  Serial.print(temp[i], HEX);
  //  Serial.print(" ");
  //}
  //Serial.print(" ret=");Serial.print(ret);
  if(ret == 0){
      
      state =  (temp[0] & 0x0F) | (temp[1] << 4);         //D1
      state |= (((temp[2] & 0x0F) | (temp[3] << 4)) << 8); //D2
      state |= (((temp[4] & 0x0F) | (temp[5] << 4)) << 16); //D3
      state |= (((temp[6] & 0x0F) | (temp[7] << 4)) << 24);//D4
  }
  //Serial.print(" state=");Serial.println(state,HEX);
  return state;
}

uint8_t DFRobot_18B20_RS485::parseThresholdAlarmFlag(uint8_t io, uint8_t id, uint32_t alarmFlag){
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return OCCUR_ERROR_TEMPERATURE_THRESHOLD;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return OCCUR_ERROR_TEMPERATURE_THRESHOLD;
  }
  uint8_t temp =  (alarmFlag >> (8*(io - 1))) & 0xFF;
  if(temp & (1 << id)){
    if(temp & (1 << (4+id))){
      return ABOVE_THE_HIGHEST_TEMPERATURE_THRESHOLD;
    }
    return BELOW_THE_LOWEST_TEMPERATURE_THRESHOLD;
  }
  return IN_THE_TEMPERATURE_THRESHOLD;
}
uint8_t DFRobot_18B20_RS485::parseThresholdAlarmFlag(eGPIONum_t io, e18B20IDNum_t id, uint32_t alarmFlag){
  return parseThresholdAlarmFlag((uint8_t)io, (uint8_t)id, alarmFlag);
}

uint8_t DFRobot_18B20_RS485::get18B20ROM(eGPIONum_t io, e18B20IDNum_t id,  uint8_t (&rom)[8]){
  return get18B20ROM((uint8_t)io, (uint8_t)id, rom);
}

uint8_t DFRobot_18B20_RS485::get18B20ROM(uint8_t io, uint8_t id, uint8_t (&rom)[8]){
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return 0;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return 0;
  }
  if(rom == NULL){
    DBG("rom is NULL");
    return 0;
  }
  uint8_t ret = readHoldingRegister(_addr, REG_18B20_D1_NUM0_ROM + (io-1)*16 + id*4, rom, DS18B20_ROM_BYTES);
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
      int j = n % 16;
      code = hexarr[j] + code;
      n /= 16;
    }
    if(code.length() < 2){
      code = '0'+code;
    }
    rslt += code;
  }
  return rslt;
}

float DFRobot_18B20_RS485::getTemperatureC(eGPIONum_t io, e18B20IDNum_t id){
  return getTemperatureC((uint8_t)io, (uint8_t)id);
}
  
float DFRobot_18B20_RS485::getTemperatureC(uint8_t io, uint8_t id){
  if((io > (uint8_t)eD4) && (io < (uint8_t)eD1)) {
    DBG("io is out of range(1~4)");
    return 0;
  }
  if(id > (uint8_t)eID3) {
    DBG("id is out of range(0~3)");
    return 0;
  }
  uint16_t temp = readHoldingRegister(_addr, REG_18B20_D1_NUM0_TEMP + 4*(io - 1) + id);
  return temp/16.0;
}

uint8_t DFRobot_18B20_RS485::get18B20Number(uint8_t batchIo){
  uint8_t num = 0;
  batchIo &= 0x0F;
  if(batchIo == 0){
    DBG("batchIo param is zero.");
    return 0;
  }
  if(batchIo == (uint8_t)eBatch_ALL){
     num = (uint8_t)readHoldingRegister(_addr, REG_18B20_NUM);
     return num;

  }
  uint8_t temp[8] = {0};
  uint8_t ret = readHoldingRegister(_addr, REG_D1_CONFIG, &temp, sizeof(temp));
  for(int i = 0; i < DS18B20_CONNECT_IO_NUM; i++){
    if(batchIo & (1 << i)){
      num += (temp[2*i + 1] >> 4) & 0x0F;
    }
  }
  return num;
}

uint8_t DFRobot_18B20_RS485::get18B20Number(eGPIOBatchNum_t batchIo){
  return get18B20Number((uint8_t)batchIo);
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