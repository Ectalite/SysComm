
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "button-hal.h"
#include "process.h"
#include <stdio.h> 
/*---------------------------------------------------------------------------*/
PROCESS(Main_process, "Main process");
PROCESS(Toggle_process, "Toggle process");
PROCESS(Color_process, "Color process");
AUTOSTART_PROCESSES(&Main_process, &Toggle_process, &Color_process);

static process_event_t event1;
static process_event_t event2;
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(Main_process, ev, data)
{
  static struct etimer timer;
  static leds_mask_t Led = LEDS_BLUE;
  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 1 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 1);

  while(1) 
  {
    PROCESS_WAIT_EVENT();
    printf("Got event");
    if(etimer_expired(&timer))
    {
      printf(" - TIMER\n");
      process_post(&Toggle_process,event1,&Led);
      etimer_reset(&timer);
    }
    if(ev == button_hal_press_event)
    {
      printf(" - BUTTON\n");
      process_post(&Color_process,event2,&Led);
    }
    /* Wait for the periodic timer to expire and then restart the timer. */
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(Toggle_process, ev, data)
{
  static leds_mask_t* pLed;
  static bool bLedStatus = 0;
  PROCESS_BEGIN();

  while(1) 
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == event1);
    leds_off(*pLed);
    printf("ON/OFF LED\n");
    pLed = (leds_mask_t*)data;
    if (bLedStatus)
    {
      leds_on(*pLed);
    }
    else
    {
      leds_off(LEDS_ALL);
    } 
    bLedStatus = !bLedStatus;
  }

  PROCESS_END();
}

PROCESS_THREAD(Color_process, ev, data)
{
  static leds_mask_t* pLed;
  PROCESS_BEGIN();

  while(1) 
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == event2);
    printf("Button pressed\n");
    pLed = (leds_mask_t*)data;
    switch (*pLed)
    {
    case LEDS_BLUE:
      *pLed = LEDS_GREEN;
      break;
    case LEDS_GREEN:
      *pLed = LEDS_RED;
      break;
    case LEDS_RED:
      *pLed = LEDS_BLUE;
      break;
    default:
      *pLed = LEDS_GREEN;
      break;
    }
  }
  PROCESS_END();
}