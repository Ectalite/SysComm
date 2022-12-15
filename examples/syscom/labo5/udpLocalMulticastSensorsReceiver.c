
/*
 * LABO 5 - UDP Multicast
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h> 

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

typedef struct my_msg {
     uint8_t id;
     uint16_t counter;
     uint16_t tmp102;
     uint16_t adxl345_x_axis;
     uint16_t adxl345_y_axis;
     uint16_t adxl345_z_axis;
     uint16_t battery;
} my_msg_t;

//Callback
static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
                    uint16_t sender_port,const uip_ipaddr_t *receiver_addr, 
                    uint16_t receiver_port, const uint8_t *data, uint16_t datalen)
{
    //printf("Message received on port %u\n", (unsigned int)receiver_port);
    static my_msg_t *pmsg = (my_msg_t *)data;
    
}

PROCESS_THREAD(udp_process, ev, data)
{
    static struct etimer timer;
    
    //static uip_ipaddr_t myUipAddress = "baad::cafe";
    static uip_ipaddr_t uipBroadcast;
    uip_create_linklocal_allnodes_mcast(&uipBroadcast);
    PROCESS_BEGIN();

    /* Setup a periodic timer that expires after 5 seconds. */
    etimer_set(&timer, CLOCK_SECOND * 5);
    simple_udp_register(&myConnection, UDP_CLIENT_PORT, NULL, UDP_CLIENT_PORT, receiver);

    while(1) 
    {
        /* Wait for the periodic timer to expire and then restart the timer. */
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void readFromPacket(my_msg_t *msg)
{
    printf("--------------------------------\n");
    LOG_INFO_6ADDR(sender_addr);
    printf(" Data: ID = %d, Counter = %d, Battery = %d, Temp = %d, x = %d, y = %d, z = %d\n",
        pmsg->id, pmsg->counter, pmsg->battery, pmsg->tmp102, pmsg->adxl345_x_axis, pmsg->adxl345_y_axis, pmsg->adxl345_z_axis);
    printf("--------------------------------\n");
}