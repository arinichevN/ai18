#ifndef APP_H
#define APP_H

/*
 * -user_config:
 * if digitalRead(DEFAULT_CONTROL_PIN) returns BUTTON_DOWN on system startup,
 * all application and channels settings will be set to default values.
 * You can find default values in ./app/main.c, ./app/main.h, ./model/channel.h
 * files.
 */
#define DEFAULT_CONTROL_PIN 4

/*
 * -user_config:
 * this pin will be high while delay in setup function and in case of 
 * error after setup.
 */
#define INDICATOR_PIN 13

/*
 * -user_config:
 * set pin all your sensors are connected to
 */
#define SENSOR_PIN 6

/*
 * -user_config:
 * set resolution for all your DS18B20 sensors:
 */
#define SENSOR_RESOLUTION 10

/*
 * -user_config:
 * set period for sending request to DS18B20 sensors:
 */
#define READ_PERIOD_MS 1000

/*
 * -user_config:
 * uncomment the following line if you want to communicate with application
 * using full command set. When commented, your application will run with 
 * default settings, and you will be able to get and set only main parameters, 
 * but this will reduce memory usage.
 */
#define SERIAL_INTERFACE_FULL

/*
 * -user_config:
 * uncomment serial interfaces you want to use:
 */
#define WANT_SERIAL0
#define WANT_SERIAL1
#define WANT_SERIAL2
#define WANT_SERIAL3

/*
 * -user_config:
 * uncomment the following line if you need debug messages:
 */
//#define MODE_DEBUG


#endif 
