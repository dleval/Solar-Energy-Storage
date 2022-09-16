/**
 * @file enphase_envoy_production.h
 * @author David Leval (dleval@dle-dev.com)
 * @brief Enphase Envoy Ethernet data request
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* Includes *************************************************************************************/
#include <Arduino.h>
#include <Ethernet.h>

/* Configurations *******************************************************************************/
#define _DEBUG_ENVOY_ENABLE    0

/* Class declaration ****************************************************************************/
class Enphase_Envoy
{
private:
    int16_t solar_prod;
    int16_t grid_cons;
    uint32_t last_connect_time;
    uint32_t request_interval_time;
    bool com_status;
    bool connect(const char* hostName);
    void disconnect(void);
    void sendRequest(const char* host, const char* resource);
    bool skipResponseHeaders(void); 

public:
    Enphase_Envoy();
    ~Enphase_Envoy();
    void begin(uint32_t interval);
    int8_t process(void);
    bool get_production_power(int16_t *prod);
    bool get_consumption_power(int16_t *cons);
    void set_request_interval(uint32_t interval);
};


