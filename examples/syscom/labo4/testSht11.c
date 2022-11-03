/*---------------------------------------------------------------------------*/
/**
 * \file
 *         Testing the SHT11 sensor on the Zolertia Z1 Platform.
 * \author
 *         NUP/FSA
 *         
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "dev/sensor/sht11/sht11.h"
#include "dev/sensor/sht11/sht11-sensor.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(test_sht11_process, "SHT11 test");
AUTOSTART_PROCESSES(&test_sht11_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_sht11_process, ev, data)
{
  static struct etimer et;
  uint16_t mes;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(sht11_sensor);

  printf("+++++++++++++++++++++++++++++++\n");
  printf("+ App: Test SHT11 sensor      +\n");
  printf("+++++++++++++++++++++++++++++++\n");
  etimer_set(&et, CLOCK_SECOND*2);

  while(1) 
  {
    PROCESS_YIELD();
    printf("Temperature:   %u degrees Celsius\n",
           (unsigned)(-39.60 + 0.01 * sht11_sensor.value(SHT11_SENSOR_TEMP)));
    mes = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
    printf("Rel. humidity: %u%%\n",
           (unsigned)(-4 + 0.0405 * mes - 2.8e-6 * (mes * mes)));
    etimer_reset(&et);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
