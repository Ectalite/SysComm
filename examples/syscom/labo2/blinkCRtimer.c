
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "sys/ctimer.h"
#include "sys/rtimer.h"
#include "dev/button-sensor.h"
#include "sys/clock.h"
#include <stdio.h>

typedef struct sCallbackParameter
{
  bool *bTimerOn;
  struct ctimer* myTimer;
} sCallbackParameter_t;

/*---------------------------------------------------------------------------*/
PROCESS(blinkTimer_process, "Timer");
AUTOSTART_PROCESSES(&blinkTimer_process);
void vCallback(void* pvParameter);
void vRtimerCallback();

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blinkTimer_process, ev, data)
{
  PROCESS_BEGIN();
  rtimer_init();
  SENSORS_ACTIVATE(button_sensor);
  
  static bool bTimerOn = false;
  static struct ctimer myTimer;
  static sCallbackParameter_t sParameter;
  sParameter.bTimerOn = &bTimerOn;
  sParameter.myTimer = &myTimer;
  while(1) 
  {
    PROCESS_WAIT_EVENT();
    if ((ev == sensors_event) && (data == &button_sensor)) 
    {
      printf("\nButton Pressed | ");
      if(true == bTimerOn)
      {
        printf("Timer Off\n\n");
        ctimer_stop(&myTimer);
        bTimerOn = false;
      }
      else
      {
        printf("Timer On\n\n");
        ctimer_set(&myTimer, 10*CLOCK_SECOND, &vCallback,&sParameter);
        bTimerOn = true;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void vCallback(void* pvParameter)
{
  static struct rtimer RTimer;
  sCallbackParameter_t *psParameter = (sCallbackParameter_t *)pvParameter;
  /* Wait for the periodic timer to expire and then restart the timer. */
  if(true == *(psParameter->bTimerOn))
  {
    ctimer_restart(psParameter->myTimer);
  }
  printf("TIMER event\n");
  printf("Temps passé: %lu\n",clock_seconds());
  leds_on(LEDS_GREEN);
  printf("Allumer\n");
  printf("Temps avant le set: %lu\n",clock_seconds());
  rtimer_set(&RTimer,RTIMER_NOW(),RTIMER_NOW()+2*CLOCK_SECOND,&vRtimerCallback,NULL);
}

void vRtimerCallback()
{
  leds_off(LEDS_GREEN);
  printf("Eteindre\n");
  printf("Temps après le set: %lu\n",clock_seconds());
}