
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "button-hal.h"
#include "sys/etimer.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(buttonTimer_process, "Timer");
AUTOSTART_PROCESSES(&buttonTimer_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(buttonTimer_process, ev, data)
{
  static bool bLed = false;
  static bool bTimerOn = false;
  PROCESS_BEGIN();

  static struct etimer myTimer;
  etimer_set(&myTimer, CLOCK_SECOND);
  etimer_stop(&myTimer);
  while(1) 
  {
    PROCESS_WAIT_EVENT();
    if (ev == button_hal_press_event) 
    {
      printf("Button Pressed | ");
      if(true == bTimerOn)
      {
        printf("Timer Off\n");
        etimer_restart(&myTimer);
        bTimerOn = false;
      }
      else
      {
        printf("Timer On\n");
        etimer_stop(&myTimer);
        bTimerOn = true;
      }
    }
    if(etimer_expired(&myTimer))
    {
      /* Wait for the periodic timer to expire and then restart the timer. */
      if(true == bTimerOn)
      {
        etimer_restart(&myTimer);
      }
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
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/