
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "Blink");
AUTOSTART_PROCESSES(&blink_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  static bool bLed = false;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_sensor);

  while(1) 
  {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event) && (data == &button_sensor));
    printf("Pressed\n");

    if(bLed)
    {
      leds_on(LEDS_GREEN);
      bLed = false;
      printf("Allumer\n");
    }
    else
    {
      leds_off(LEDS_GREEN);
      bLed = true;
      printf("Eteindre\n");
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
