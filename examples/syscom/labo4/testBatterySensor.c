
/*
 * \file
 *         Test Battery Sensor
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
PROCESS(battery_process, "Battery process");
AUTOSTART_PROCESSES(&battery_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(battery_process, ev, data)
{
  static struct etimer timer;
  uint32_t mes;

  PROCESS_BEGIN();
  printf("+++++++++++++++++++++++++++++++\n");
  printf("+ App: Test Battery Sensor    +\n");
  printf("+++++++++++++++++++++++++++++++\n");

  SENSORS_ACTIVATE(battery_sensor);

  /* Setup a periodic timer that expires after 10 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 10);

  while(1) 
  {
      /* Convert the battery reading from ADC units to mV (powered over USB) */
    mes = battery_sensor.value(0);
    mes = (mes * 2500 * 2) / 4096;  

    /* Print the sensor data */
    LOG_INFO("batt: %u.%03d V\n", getQuotient(mes, 1000), getRemainder(mes, 1000));
    
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
