
/*
 * \file
 *         Test Temperature Sensor
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
/* Sensors to send data */
#include "dev/battery-sensor.h"
#include "dev/i2cmaster.h"
#include "dev/tmp102.h"

#include <stdio.h> 

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO


/*---------------------------------------------------------------------------*/
uint16_t getQuotient(uint16_t dividend, uint16_t divisor)
{
  return dividend/divisor;
}
/*---------------------------------------------------------------------------*/
uint16_t getRemainder(uint16_t dividend, uint16_t divisor)
{
  //uint16_t quotient = dividend/divisor;
  return dividend%divisor;
}
/*---------------------------------------------------------------------------*/
PROCESS(temperature_process, "Temperature process");
AUTOSTART_PROCESSES(&temperature_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(temperature_process, ev, data)
{
  static struct etimer timer;
  uint32_t mes;

  PROCESS_BEGIN();
  printf("++++++++++++++++++++++++++++++++++\n");
  printf("+ App: Test Temperature Sensor   +\n");
  printf("++++++++++++++++++++++++++++++++++\n");

  //tmp102_init();
  SENSORS_ACTIVATE(tmp102);
  
  /* Setup a periodic timer that expires after 10 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 10);

  while(1) 
  {
      /* Reading temperature sensor */
    mes = tmp102_read_temp_x100();
        
    /* Print the sensor data */
    LOG_INFO("Temperature: %u.%02d C\n", getQuotient(mes, 100), getRemainder(mes, 100));
    
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
