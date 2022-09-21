/**
 * @file enphase_envoy_production.cpp
 * @author David Leval (dleval@dle-dev.com)
 * @brief Enphase Envoy Ethernet data request
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* Includes *************************************************************************************/
#include "enphase_envoy_production.h"
#include <ArduinoJson.h>

/* Debug Macro **********************************************************************************/
#if _DEBUG_ENVOY_ENABLE != 0
#define _DEBUG_PRINT(x)         Serial.print(x)
#define _DEBUG_PRINTLN(x)       Serial.println(x)
#define _DEBUG_JSON_PRETTY(x)   serializeJsonPretty(x, Serial)
#else
#define _DEBUG_PRINT(x)     
#define _DEBUG_PRINTLN(x)   
#define _DEBUG_JSON_PRETTY(x)
#endif

/* External object ******************************************************************************/
extern EthernetClient eth_client;

/* Parameters ***********************************************************************************/
const char* server = "192.168.3.5";
const char* resource = "/production.json";

const unsigned long HTTP_TIMEOUT = 1000;  // max respone time from server
const uint32_t HTTP_REQUEST_INTERVAL_MIN = 1500;

/* Private static variables *********************************************************************/
StaticJsonDocument<2048> json_doc;
DeserializationError error;

/* Class constructor ****************************************************************************/
Enphase_Envoy::Enphase_Envoy()
{
    request_interval_time = 10 * 1000;
    com_status = false;
}

Enphase_Envoy::~Enphase_Envoy()
{
}

/* Public Functions *****************************************************************************/

void Enphase_Envoy::begin(uint32_t interval)
{
    set_request_interval(interval);
}

bool Enphase_Envoy::process(void)
{
    enum{SEND_REQUEST, SKIP_HEADER, READ_RESPONSE, WAIT_CYCLE, ERROR_STATE};
    static uint8_t state = SEND_REQUEST;
    bool ret = false;

    switch(state)
    {
        case SEND_REQUEST:
            last_connect_time = millis();
            if (connect(server))
            {
                sendRequest(server, resource);
                state = SKIP_HEADER;
            }
            else state = ERROR_STATE;
            break;
        case SKIP_HEADER:
            if(skipResponseHeaders()) state = READ_RESPONSE;
            else state = ERROR_STATE;
            break;
        case READ_RESPONSE:
            error = deserializeJson(json_doc, eth_client);
            _DEBUG_PRINT(F("Envoy : perf.:"));
            _DEBUG_PRINTLN(millis() - last_connect_time);
            if (error) {
                _DEBUG_PRINT(F("Envoy : deserializeJson() failed: "));
                _DEBUG_PRINTLN(error.f_str());
                state = ERROR_STATE;
            }
            else {
                _DEBUG_PRINTLN(F("Envoy : Response:"));
                _DEBUG_JSON_PRETTY(json_doc);
                json_extract_data();
                com_status = true;
                ret = true;
            }
            disconnect();
            state = WAIT_CYCLE;
            
        case WAIT_CYCLE:
            if (millis() - last_connect_time > request_interval_time) state = SEND_REQUEST;
            break;

        case ERROR_STATE:
            _DEBUG_PRINTLN(F("Envoy : Error"));
            com_status = false;
            disconnect();
            state = WAIT_CYCLE;
            break;

        default:
            state = WAIT_CYCLE;
    }
    return ret;
}

bool Enphase_Envoy::get_production_power(int16_t *prod)
{
    if((com_status == true) && (prod != nullptr))
    {
        *prod = solar_prod;
    }
    return com_status;
}

bool Enphase_Envoy::get_consumption_power(int16_t *cons)
{
    if((com_status == true) && (cons != nullptr))
    {
        *cons = grid_cons;
    }
    return com_status;
}

void Enphase_Envoy::set_request_interval(uint32_t interval)
{
    if(interval < HTTP_REQUEST_INTERVAL_MIN)
    {
        interval = HTTP_REQUEST_INTERVAL_MIN;
    }
    request_interval_time = interval;
}

/* Private functions ****************************************************************************/

// Open connection to the HTTP server
bool Enphase_Envoy::connect(const char* hostName) {
  _DEBUG_PRINT(F("Envoy : Connect to "));
  _DEBUG_PRINTLN(hostName);
  bool ok = eth_client.connect(hostName, 80);
//   _DEBUG_PRINTLN(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Close the connection with the HTTP server
void Enphase_Envoy::disconnect(void) {
  _DEBUG_PRINTLN(F("Envoy : Disconnect"));
  eth_client.stop();
}

// Send the HTTP GET request to the server
void Enphase_Envoy::sendRequest(const char* host, const char* resource) {
    char buffer_a[40];
    char buffer_b[40];
    sprintf(buffer_a, "GET %s HTTP/1.1", resource);
    sprintf(buffer_b, "Host: %s", host);
    eth_client.println(buffer_a);
    eth_client.println(buffer_b);
    eth_client.println("Connection: close");
    eth_client.println();
}

// Skip HTTP headers so that we are at the beginning of the response's body
bool Enphase_Envoy::skipResponseHeaders(void) {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";
  eth_client.setTimeout(HTTP_TIMEOUT);
  bool ok = eth_client.find(endOfHeaders);
  if (!ok) {
    _DEBUG_PRINTLN(F("Envoy : No response or invalid response!"));
  }
  // end of size number
  eth_client.find('\n');
  return ok;
}

void Enphase_Envoy::json_extract_data(void)
{
    solar_prod = (int16_t)json_doc["production"][1]["wNow"];
    grid_cons = (int16_t)json_doc["consumption"][0]["wNow"];
}

