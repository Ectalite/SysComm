
/*
 * LABO 8 - The Sniffing Project
 * Fichier Centrale
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h> 
#include <string.h>
#include "sys/timer.h"
#include "dev/button-sensor.h"

#define UDP_CLIENT_PORT 8765

//Debug
#include "sys/log.h"
#define LOG_MODULE "UDP Mulicast"
#define LOG_LEVEL LOG_LEVEL_INFO
/*---------------------------------------------------------------------------*/
PROCESS(udp_process, "UDP process");
AUTOSTART_PROCESSES(&udp_process);
/*---------------------------------------------------------------------------*/

static struct simple_udp_connection myConnection;

//Callback
static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
                    uint16_t sender_port,const uip_ipaddr_t *receiver_addr, 
                    uint16_t receiver_port, const uint8_t *data, uint16_t datalen)
{
    LOG_INFO_6ADDR(sender_addr);
    printf(" - %s\n",data);
}

PROCESS_THREAD(udp_process, ev, data)
{
    SENSORS_ACTIVATE(button_sensor);
    PROCESS_BEGIN();

    static bool bLed = false;

    simple_udp_register(&myConnection, UDP_CLIENT_PORT, NULL, UDP_CLIENT_PORT, receiver);

    static uip_ipaddr_t ipv6Lampadaire;
    static uip_ipaddr_t ipv6Lampadaire2;
    uip_ip6addr(&ipv6Lampadaire, 0xfe80, 0x0, 0x0, 0x0, 0xc30c, 0x0, 0x0, 0x69);
    uip_ip6addr(&ipv6Lampadaire2, 0xfe80, 0x0, 0x0, 0x0, 0xc30c, 0x0, 0x0, 0x68);
    LOG_INFO_6ADDR(&ipv6Lampadaire);
    LOG_INFO_6ADDR(&ipv6Lampadaire2);
    printf("\n");

    while(1) 
    {
        PROCESS_WAIT_EVENT();
        if((ev == sensors_event) && (data == &button_sensor))
        {
            bLed = !bLed;
            if(bLed)
            {
                simple_udp_sendto(&myConnection, "LedON", 6, &ipv6Lampadaire);
                simple_udp_sendto(&myConnection, "LedON", 6, &ipv6Lampadaire2);
                printf("LedON\n");
            }
            else
            {
                simple_udp_sendto(&myConnection, "LedOFF", 6, &ipv6Lampadaire);
                simple_udp_sendto(&myConnection, "LedOFF", 6, &ipv6Lampadaire2);
                printf("LedOFF\n");
            }
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/


