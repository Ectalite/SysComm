
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(blinkTimer_process, "Timer");
AUTOSTART_PROCESSES(&blinkTimer_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blinkTimer_process, ev, data)
{
  static bool bLed = false;
  PROCESS_BEGIN();

  static struct etimer myTimer;
  etimer_set(&myTimer, CLOCK_SECOND);
  while(1) 
  {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&myTimer));
    /* Reset the etimer to trig again in 1 second */
    etimer_restart(&myTimer);
    printf("TIMER event\n");

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