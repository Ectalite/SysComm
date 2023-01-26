
/*
 * LABO 5 - UDP Multicast
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "dev/button-sensor.h"
#include <stdio.h> 
#include <string.h>
#include "sys/timer.h"

#define UDP_CLIENT_PORT 8765

//Debug
#include "sys/log.h"
#define LOG_MODULE "UDP Mulicast"
#define LOG_LEVEL LOG_LEVEL_INFO
/*---------------------------------------------------------------------------*/
PROCESS(udp_process, "UDP process");
AUTOSTART_PROCESSES(&udp_process);
/*---------------------------------------------------------------------------*/

//Global  connection
static struct simple_udp_connection myConnection;

//Callback
static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
                    uint16_t sender_port,const uip_ipaddr_t *receiver_addr, 
                    uint16_t receiver_port, const uint8_t *data, uint16_t datalen)
{

}

PROCESS_THREAD(udp_process, ev, data)
{
    static struct etimer timer;
    
    PROCESS_BEGIN();
    SENSORS_ACTIVATE(button_sensor);

    /* Setup a periodic timer that expires after 5 seconds. */
    etimer_set(&timer, CLOCK_SECOND*5);
    simple_udp_register(&myConnection, UDP_CLIENT_PORT, NULL, UDP_CLIENT_PORT, receiver);

    //Sending to 4 from 2
    static uip_ipaddr_t ipv6NextFriend;
    uip_ip6addr(&ipv6NextFriend, 0xfe80, 0x0, 0x0, 0x0, 0xc30c, 0x0, 0x0, 0x04);
    LOG_INFO_6ADDR(&ipv6NextFriend);
    printf("\n");

    static bool bLED = false;
    static char cSend[15] ;

    while(1) 
    {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&timer))
        {
            /* Wait for the periodic timer to expire and then restart the timer. */
            etimer_reset(&timer);
        }
        else if((ev == sensors_event) && (data == &button_sensor))
        {
            bLED = !bLED;
            LOG_INFO("Clicked");

            if(bLED)
            {
                strcpy(cSend, "led=on");
            }
            else
            {
                strcpy(cSend, "led=off");
            }

            simple_udp_sendto(&myConnection, cSend, 15, &ipv6NextFriend);
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/


