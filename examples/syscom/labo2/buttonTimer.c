
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include "dev/button-sensor.h"
#include "sys/clock.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(blinkTimer_process, "Timer");
AUTOSTART_PROCESSES(&blinkTimer_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blinkTimer_process, ev, data)
{
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_sensor);

  static bool bLed = false;
  static bool bTimerOn = false;
  static struct etimer myTimer;
  etimer_set(&myTimer, CLOCK_SECOND);
  while(1) 
  {
    PROCESS_WAIT_EVENT();
    if ((ev == sensors_event) && (data == &button_sensor)) 
    {
      printf("\nButton Pressed | ");
      if(true == bTimerOn)
      {
        printf("Timer Off\n\n");
        etimer_restart(&myTimer);
        bTimerOn = false;
      }
      else
      {
        printf("Timer On\n\n");
        etimer_reset(&myTimer);
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
      printf("Temps passé: %lu\n",clock_seconds());
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