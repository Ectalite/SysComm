/**
 * \file
 *         Scanning 2.4 GHz radio frequencies using CC2420 and prints
 *         the values
 * \author
 *         
 */

#include "contiki.h"
#include "net/netstack.h"

#include "dev/leds.h"
#include "cc2420.h"
#include "cc2420_const.h"
#include "dev/spi.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/* This assumes that the CC2420 is always on and "stable" */
static void set_frq(int c)
{
  int f;
  /* We can read even other channels with CC2420! */
  /* Fc = 2048 + FSCTRL  ;  Fc = 2405 + 5(k-11) MHz, k=11,12, ... , 26 */
  //f = c + 357; /* Start from 2400 MHz to 2485 MHz, */
  f = 5 * (c - 11) + 357 + 0x4000;  
  /* Write the new channel value */
  CC2420_SPI_ENABLE();
  SPI_WRITE_FAST(CC2420_FSCTRL);
  SPI_WRITE_FAST((uint8_t)(f >> 8));
  SPI_WRITE_FAST((uint8_t)(f & 0xff));
  SPI_WAITFORTx_ENDED();
  SPI_WRITE(0);

  /* Send the strobe */
  SPI_WRITE(CC2420_SRXON);
  CC2420_SPI_DISABLE();
}
static void do_rssi(void)
{
  int channel;
  printf("RSSI:");
  //for(channel = 0; channel <= 85; ++channel)   {
  for(channel = 11; channel <= 26; ++channel) 
  {
    set_frq(channel);
    printf("%02d ", cc2420_rssi() + 100);
  }
  printf("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS(scanner_process, "RSSI Scanner");
AUTOSTART_PROCESSES(&scanner_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(scanner_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();    
  // switch mac layer off, and turn radio on 
  NETSTACK_MAC.off();
  cc2420_on();
  
  etimer_set(&et, 1); 

  while(1) 
  {
    PROCESS_WAIT_EVENT();
    if (etimer_expired(&et))
		{
			do_rssi();
			etimer_reset(&et);
		}  
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
