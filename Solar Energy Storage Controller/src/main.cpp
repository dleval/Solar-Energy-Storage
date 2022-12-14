/**
 * @file main.cpp
 * @author David Leval (dleval@dle-dev.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoRS485.h> 
#include "enphase_envoy_production.h"
#include "modbus_rtu_client.h"
#include "modbus_tcp_server.h"

// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

// Initialize the Ethernet client library (port 80 is default for HTTP)
EthernetClient eth_client;

RS485Class rs485_modbus(Serial1, 18, 8, 9);

Enphase_Envoy envoy;
Modbus_RTU_Client MB_RTU_client;
Modbus_TCP_Server MB_TCP_server;

void setup_Ethernet(void)
{
  // start the Ethernet connection:
    Serial.println(F("Initialize Ethernet with DHCP:"));
    if (Ethernet.begin(mac) == 0) {
        Serial.println(F("Failed to configure Ethernet using DHCP"));
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println(F("Ethernet shield not found. :("));
        while (true) {
            delay(1); // do nothing, no point running without Ethernet hardware
        }
        }
        if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println(F("Eth. cable not connected."));
        }
        // try to configure using IP address instead of DHCP:
        Ethernet.begin(mac, ip, myDns);
    } else {
        Serial.print(F("  DHCP assigned IP "));
        Serial.println(Ethernet.localIP());
    }
    // give the Ethernet shield a second to initialize:
    delay(1000);
    Serial.println(F("Eth. OK"));
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  setup_Ethernet();
  envoy.begin(2000);
  MB_RTU_client.begin(38400);
  MB_TCP_server.begin();
}

void loop() {
  int16_t envoy_meas;

  if (envoy.process())
  {
    if(envoy.get_production_power(&envoy_meas)) {
      Serial.print(F("Solar production : "));
      Serial.print(envoy_meas);
      Serial.println(F(" W"));
    } else Serial.println(F("Solar production not avail."));
    if(envoy.get_consumption_power(&envoy_meas)) {
      Serial.print(F("Grid consumption : "));
      Serial.print(envoy_meas);
      Serial.println(F(" W"));
    } else Serial.println(F("Grid consumption not avail."));
  }

  MB_TCP_server.process();
  uint16_t mb_register_data;
  if(MB_TCP_server.access_register(0x01, &mb_register_data, Modbus_TCP_Server::READ))
  {
    if(mb_register_data) digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
  }
 
}
