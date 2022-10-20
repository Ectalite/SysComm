
/*
 * \file
 *         A very simple Contiki application
 * \author
 *         NUP, FSA
 */

#include "contiki.h"
#include "dev/leds.h"
#include "sys/ctimer.h"
#include "button-hal.h"
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

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blinkTimer_process, ev, data)
{
  PROCESS_BEGIN();
  
  static bool bTimerOn = false;
  static struct ctimer myTimer;
  static sCallbackParameter_t sParameter;
  sParameter.bTimerOn = &bTimerOn;
  sParameter.myTimer = &myTimer;
  ctimer_set(&myTimer, CLOCK_SECOND, &vCallback,&sParameter);
  while(1) 
  {
    PROCESS_WAIT_EVENT();
    if (ev == button_hal_press_event) 
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
        ctimer_reset(&myTimer);
        bTimerOn = true;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void vCallback(void* pvParameter)
{
  static bool bLed = false;
  sCallbackParameter_t *psParameter = (sCallbackParameter_t *)pvParameter;
  /* Wait for the periodic timer to expire and then restart the timer. */
  if(true == *(psParameter->bTimerOn))
  {
    ctimer_restart(psParameter->myTimer);
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