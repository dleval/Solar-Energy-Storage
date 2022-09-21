/**
 * @file modbus_tcp_server.cpp
 * @author David Leval (dleval@dle-dev.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* Includes *************************************************************************************/
#include "modbus_tcp_server.h"
#include <ArduinoRS485.h> 
#include <ArduinoModbus.h>
#include <Ethernet.h>

EthernetServer ethServer(502);
ModbusTCPServer modbusTCPServer;
EthernetClient client;

Modbus_TCP_Server::Modbus_TCP_Server(/* args */)
{
}

Modbus_TCP_Server::~Modbus_TCP_Server()
{
}

void Modbus_TCP_Server::begin(void)
{
  // start the server
  ethServer.begin();
  // start the Modbus TCP server
  if (!modbusTCPServer.begin()) {
    Serial.println(F("Failed to start Modbus TCP Server!"));
    while (1);
  }
  //Configure register
  modbusTCPServer.configureHoldingRegisters(0x01, 1);
}

void Modbus_TCP_Server::process(void)
{
  enum{WAIT_CLIENT_CONNECT, MODBUS_POLL};
  static uint8_t state;

  switch(state)
  {
    case WAIT_CLIENT_CONNECT:
      // listen for incoming clients
      client = ethServer.available();
      if(client)
      {
        Serial.println(F("ModBus TCP : new client connected"));
        // let the Modbus TCP accept the connection 
        modbusTCPServer.accept(client);
        state = MODBUS_POLL;
      }
      break;
    case MODBUS_POLL:
      if(client.connected())
      {
        // poll for Modbus TCP requests, while client connected
        modbusTCPServer.poll();
      } else {
        Serial.println(F("ModBus TCP : client disconnected"));
        state = WAIT_CLIENT_CONNECT;
      }
      break;
    default:
      state = WAIT_CLIENT_CONNECT;
      break;
  }
}

bool Modbus_TCP_Server::access_register(uint16_t address, uint16_t* data, uint8_t rw)
{
  bool result = false;

  if(data != nullptr)
  {
    if(rw == READ)
    {
      *data = modbusTCPServer.holdingRegisterRead(address);
      result = true;
    }
    if(rw == WRITE)
    {
      modbusTCPServer.holdingRegisterWrite(address, *data);
    }
  }

  return result;
}
