
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

typedef enum eColor {Red, Blue, Green} eColor_t;

/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "Blink Firefly");
AUTOSTART_PROCESSES(&blink_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  static eColor_t eColor = Red;
  leds_on(LEDS_RED);

  PROCESS_BEGIN();

  while(1) 
  {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(ev = button_hal_release_event);
    PROCESS_WAIT_EVENT_UNTIL(ev = button_hal_press_event);
    printf("Pressed\n");
    leds_off(LEDS_ALL);
    
    switch (eColor)
    {
    case Red:
      leds_on(LEDS_RED);
      eColor = Blue;
      break;
    case Blue:
      leds_on(LEDS_BLUE);
      eColor = Green;
      break;
    case Green:
      leds_on(LEDS_GREEN);
      eColor = Red;
      break;
    default:
      leds_off(LEDS_ALL);
      break;
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
