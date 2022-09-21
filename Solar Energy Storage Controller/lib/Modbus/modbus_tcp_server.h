/**
 * @file modbus_tcp_server.h
 * @author David Leval (dleval@dle-dev.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* Includes *************************************************************************************/
#include <Arduino.h>


class Modbus_TCP_Server
{
private:
    /* data */
public:
    Modbus_TCP_Server(/* args */);
    ~Modbus_TCP_Server();
    enum {READ = 0, WRITE = 1};
    void begin(void);
    void process(void);
    bool access_register(uint16_t address, uint16_t* data, uint8_t rw);
};


