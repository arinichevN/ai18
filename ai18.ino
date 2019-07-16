#include <OneWire.h>
#include <DallasTemperature.h>
#include "channel.h"

//edit this
#define SERIAL_RATE 4800
#define SERIAL_CONFIG SERIAL_8N1
#define SERIAL_TIMEOUT_MS 300
#define CHANNEL_COUNT 2
#define ONE_WIRE_PIN 2
#define SENSOR_RESOLUTION 12
#define READ_DELAY_MS 1000

//edit this
//for each channel
#define ID1 5
#define SENSOR_IND1 0
#define ID2 6
#define SENSOR_IND2 1

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
Channel channels[CHANNEL_COUNT];
PUART puart;
Ton tmr;

void setup()
{
  Serial.begin(SERIAL_RATE, SERIAL_CONFIG);
  Serial.setTimeout(SERIAL_TIMEOUT_MS);
  sensors.begin();
  sensors.setResolution(SENSOR_RESOLUTION);
  sensors.setWaitForConversion(0);
  ton_setInterval(&tmr, READ_DELAY_MS);
  puart_init(&puart);
  
  //edit this
  //for each channel
  channel_init(&channels[0], ID1, SENSOR_IND1);
  channel_init(&channels[1], ID2, SENSOR_IND2);
}


void loop()
{
  if (ton(&tmr)) {
    ton_reset(&tmr);
    sensors.requestTemperatures();
    unsigned long t_end = sensors.millisToWaitForConversion(SENSOR_RESOLUTION) + millis();
    while(millis() < t_end){
		puart_server(&puart, channels, CHANNEL_COUNT );
	}
    for (int i = 0; i < CHANNEL_COUNT; i++) {
      channel_control(&channels[i]);
      puart_server(&puart, channels, CHANNEL_COUNT );
    }
  }
  puart_server(&puart, channels, CHANNEL_COUNT );
}
