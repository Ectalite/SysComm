/*---------------------------------------------------------------------------*/
/**
 * \file
 *         A quick program for testing the light ziglet driver in the Z1 platform
 * \author
 *         Antonio Lignan <alinan@zolertia.com>
 */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
#include "arch/platform/z1/dev/light-ziglet.h"
/*---------------------------------------------------------------------------*/
#define SENSOR_READ_INTERVAL (CLOCK_SECOND / 2)
/*---------------------------------------------------------------------------*/
PROCESS(test_process, "Test light ziglet process");
AUTOSTART_PROCESSES(&test_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize driver and set a slower data rate */
  light_ziglet_init();
  i2c_setrate(I2C_PRESC_100KHZ_LSB, I2C_PRESC_100KHZ_MSB);

  while(1) {
    etimer_set(&et, SENSOR_READ_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    printf("Light = %u\n", light_ziglet_read());
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
