
/*
 * LABO 5 - UDP Multicast
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
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
    uip_ipaddr_t ipv6Friend;
    //Receivig from Node 3
    ipv6Friend.u16[0] = 0x80fe;
    ipv6Friend.u16[1] = 0;
    ipv6Friend.u16[2] = 0;
    ipv6Friend.u16[3] = 0;
    ipv6Friend.u16[4] = 0x0cc3;
    ipv6Friend.u16[5] = 0;
    ipv6Friend.u16[6] = 0;
    ipv6Friend.u16[7] = 0x0300;
    if(!memcmp(sender_addr->u16, ipv6Friend.u16, sizeof(uint16_t)*8))
    {
        LOG_INFO_6ADDR(sender_addr);
        if(strcmp((const char*)data, "led=on"))
        {
            LOG_INFO("LED ON");
        }
        else if(strcmp((const char*)data, "led=off"))
        {
            LOG_INFO("LED OFF");
        }
        else
        {
            LOG_INFO("Received data not good");
        }
    }
}

PROCESS_THREAD(udp_process, ev, data)
{
    static struct etimer timer;
    
    static uip_ipaddr_t uipBroadcast;
    uip_create_linklocal_allnodes_mcast(&uipBroadcast);
    PROCESS_BEGIN();

    /* Setup a periodic timer that expires after 5 seconds. */
    etimer_set(&timer, CLOCK_SECOND*5);
    simple_udp_register(&myConnection, UDP_CLIENT_PORT, NULL, UDP_CLIENT_PORT, receiver);

    while(1) 
    {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&timer))
        {
            /* Wait for the periodic timer to expire and then restart the timer. */
            
            etimer_reset(&timer);
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/


