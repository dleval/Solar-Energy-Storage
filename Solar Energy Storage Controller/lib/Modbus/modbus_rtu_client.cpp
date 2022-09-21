/**
 * @file modbus_rtu_client.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* Includes *************************************************************************************/
#include "modbus_rtu_client.h"
#include <ArduinoRS485.h> 
#include <ArduinoModbus.h>

extern RS485Class rs485_modbus;
ModbusRTUClientClass modbus_rtu(rs485_modbus);

Modbus_RTU_Client::Modbus_RTU_Client(/* args */)
{
}

Modbus_RTU_Client::~Modbus_RTU_Client()
{
}

void Modbus_RTU_Client::begin(uint32_t baudrate)
{
  if (!modbus_rtu.begin(baudrate)) 
  {
    Serial.println(F("Failed to start Modbus RTU Client!"));
  }
}

bool Modbus_RTU_Client::access_register(uint8_t id, uint16_t address, uint16_t len, uint16_t *data, uint8_t rw)
{
  bool result = false;
  uint16_t index = 0;

  if(rw == READ)
  {
    Serial.print(F("ModBus RTU : Read register "));
    if (!ModbusRTUClient.requestFrom(id, HOLDING_REGISTERS, address, len)) {
      Serial.print(F("failed! "));
      Serial.println(ModbusRTUClient.lastError());
    } else {
      Serial.println(F("success"));
      while (ModbusRTUClient.available()) {
        data[index++] = ModbusRTUClient.read();
      }
      result = true;
    }
  }

  if(rw == WRITE)
  {
    Serial.print(F("ModBus RTU : Write register "));
    ModbusRTUClient.beginTransmission(id, HOLDING_REGISTERS, address, len);
    for (index = 0; index < len; index++) ModbusRTUClient.write(data[index]);
    if (!ModbusRTUClient.endTransmission()) {
      Serial.print(F("failed! "));
      Serial.println(ModbusRTUClient.lastError());
    } else {
      Serial.println(F("success"));
      result = true;
    }
  }

  return result;
}
