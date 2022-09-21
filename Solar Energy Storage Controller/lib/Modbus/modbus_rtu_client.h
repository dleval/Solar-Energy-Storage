/**
 * @file modbus_rtu_client.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* Includes *************************************************************************************/
#include <Arduino.h>

class Modbus_RTU_Client
{
private:
    /* data */
public:
    Modbus_RTU_Client(/* args */);
    ~Modbus_RTU_Client();
    enum {READ = 0, WRITE = 1};
    void begin(uint32_t baudrate);
    bool access_register(uint8_t id, uint16_t address, uint16_t nb, uint16_t *data, uint8_t rw);
};


