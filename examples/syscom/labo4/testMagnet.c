/*---------------------------------------------------------------------------*/
/**
 * \file
 *         Testing Phidgets on the Zolertia Z1 Platform.
 * \author
 *         NUP/FSA
 *         
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/sensors.h"
#include "arch/platform/z1/dev/z1-phidgets.h"
//#include "dev/sht11/sht11-sensor.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(test_phidgets_process, "Phidgets test");
AUTOSTART_PROCESSES(&test_phidgets_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_phidgets_process, ev, data)
{
  static struct etimer et;
  uint16_t mes;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(phidgets);

  printf("+++++++++++++++++++++++++++++++\n");
  printf("+ App: Test Phidgets          +\n");
  printf("+++++++++++++++++++++++++++++++\n");
  etimer_set(&et, CLOCK_SECOND*2);

  while(1) 
  {
    PROCESS_YIELD();
    //printf("Val 1:   %u\n", phidgets.value(PHIDGET3V_1));
    //printf("Val 2:   %u\n", phidgets.value(PHIDGET3V_2));
    //printf("Val 2:   %u\n", phidgets.value(PHIDGET5V_2));
    printf("Val 2:   %u\n", phidgets.value(PHIDGET5V_1));
    mes = phidgets.value(PHIDGET5V_1);
    if (mes > 3000)
      printf("Detected!\n" );
    else
      printf("---\n" );
    
    etimer_reset(&et);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
