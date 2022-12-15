
/*
 * LABO 5 - UDP Multicast
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h> 
#include "dev/adxl345.h"
#include "dev/battery-sensor.h"
#include "dev/tmp102.h"

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
static process_event_t eventReceived;

typedef struct my_msg {
     uint8_t id;
     uint16_t counter;
     uint16_t tmp102;
     uint16_t adxl345_x_axis;
     uint16_t adxl345_y_axis;
     uint16_t adxl345_z_axis;
     uint16_t battery;
} my_msg_t;

typedef struct ReceivedMsg {
    uip_ipaddr_t *sender_addr;
    my_msg_t msg;
} ReceivedMsg_t;

//Callback
static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
                    uint16_t sender_port,const uip_ipaddr_t *receiver_addr, 
                    uint16_t receiver_port, const uint8_t *data, uint16_t datalen)
{
    //printf("Message received on port %u\n", (unsigned int)receiver_port);
    static ReceivedMsg_t Received;
    static ReceivedMsg_t *pReceived = &Received;
    memcpy(&(pReceived->msg),data,sizeof(ReceivedMsg_t));
    memcpy(&(pReceived->sender_addr),sender_addr,sizeof(uip_ipaddr_t*));
    process_post(&udp_process,eventReceived,pReceived);
    /*printf("--------------------------------\n");
    LOG_INFO_6ADDR(sender_addr);
    printf("Data: ID = %d, Counter = %d, Battery = %d, Temp = %d, x = %d, y = %d, z = %d",
        pmsg->id, pmsg->counter, pmsg->battery, pmsg->tmp102, pmsg->adxl345_x_axis, pmsg->adxl345_y_axis, pmsg->adxl345_z_axis);
    printf("--------------------------------\n");*/
}

PROCESS_THREAD(udp_process, ev, data)
{
    static struct etimer timer;

    static my_msg_t msg;
    static my_msg_t *msgPtr = &msg;
    
    //static uip_ipaddr_t myUipAddress = "baad::cafe";
    static uip_ipaddr_t uipBroadcast;
    uip_create_linklocal_allnodes_mcast(&uipBroadcast);
    PROCESS_BEGIN();

    /* Setup a periodic timer that expires after 5 seconds. */
    etimer_set(&timer, CLOCK_SECOND * 5);
    simple_udp_register(&myConnection, UDP_CLIENT_PORT, NULL, UDP_CLIENT_PORT, receiver);

    SENSORS_ACTIVATE(adxl345);
    SENSORS_ACTIVATE(tmp102);

    //Card number definition
    msg.id = (uint8_t)69;

    msg.counter = 0;

    while(1) 
    {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&timer))
        {
            /* Wait for the periodic timer to expire and then restart the timer. */
            msg.adxl345_x_axis = adxl345.value(X_AXIS);
            msg.adxl345_y_axis = adxl345.value(Y_AXIS);
            msg.adxl345_z_axis = adxl345.value(Z_AXIS);
            uint32_t battery = battery_sensor.value(0);
            msg.battery = (battery * 2500 * 2) / 4096; 
            msg.counter++;
            msg.tmp102 = tmp102_read_temp_x100();
            simple_udp_sendto(&myConnection, msgPtr, sizeof(msg), &uipBroadcast);

            etimer_reset(&timer);
        }
        else if(ev == eventReceived)
        {
            ReceivedMsg_t *pReceived = (ReceivedMsg_t*)data;
            printf("--------------------------------\n");
            LOG_INFO_6ADDR((pReceived->sender_addr));
            printf(" Data: ID = %d, Counter = %d, Battery = %d, Temp = %d, x = %d, y = %d, z = %d",
                pReceived->msg.id, pReceived->msg.counter, pReceived->msg.battery, 
                pReceived->msg.tmp102, pReceived->msg.adxl345_x_axis, 
                pReceived->msg.adxl345_y_axis, pReceived->msg.adxl345_z_axis);
            printf("--------------------------------\n");
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/


