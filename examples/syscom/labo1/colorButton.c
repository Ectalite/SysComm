
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

//#undef LEDS_GREEN
#undef LEDS_BLUE
//#define LEDS_GREEN 
#define LEDS_BLUE LEDS_YELLOW

typedef enum eColor {Red, Blue, Green} eColor_t;

/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "Blink");
AUTOSTART_PROCESSES(&blink_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  static eColor_t eColor = Red;
  leds_on(LEDS_RED);

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_sensor);

  while(1) 
  {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event) && (data == &button_sensor));
    printf("Pressed");

    
    
    switch (eColor)
    {
    case Red:
      leds_on(LEDS_RED);
      leds_off(LEDS_GREEN);
      eColor = Blue;
      printf(" Red\n");
      break;
    case Blue:
      leds_on(LEDS_BLUE);
      leds_off(LEDS_RED);
      eColor = Green;
      printf(" Blue\n");
      break;
    case Green:
      leds_on(LEDS_GREEN);
      leds_off(LEDS_BLUE);
      leds_off(LEDS_RED);
      eColor = Red;
      printf(" Green\n");
      break;
    default:
      leds_off(LEDS_GREEN);
      leds_off(LEDS_RED);
      leds_off(LEDS_BLUE);
      break;
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
