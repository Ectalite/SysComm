
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "button-hal.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "Blink Firefly");
AUTOSTART_PROCESSES(&blink_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  static bool bLed = false;

  PROCESS_BEGIN();

  //button_hal_init();

  while(1) 
  {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(ev = button_hal_release_event);
    PROCESS_WAIT_EVENT_UNTIL(ev = button_hal_press_event);
    printf("Pressed\n");

    if(bLed)
    {
      leds_on(LEDS_ALL);
      bLed = false;
      printf("Allumer\n");
    }
    else
    {
      leds_off(LEDS_ALL);
      bLed = true;
      printf("Eteindre\n");
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
