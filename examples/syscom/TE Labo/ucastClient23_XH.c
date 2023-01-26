/*---------------------------------------------------------------------------*/
/**
 * \file
 *         ucastClient23_XH.c: 
 * \author
 *         Xavier Hueber
 *         
 */
/*---------------------------------------------------------------------------*/

#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "sys/stimer.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "sys/log.h"
#define LOG_MODULE "TE2023-CLIENT"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT	8765

#ifdef LEDS_ALL
  #undef LEDS_ALL
#endif
#ifdef LEDS_BLUE
  #undef LEDS_BLUE
#endif
#define LEDS_BLUE LEDS_YELLOW
#define LEDS_ALL LEDS_GREEN|LEDS_RED|LEDS_BLUE

void Init(void);
void State1(int ev,process_data_t data);
void State2(int ev,process_data_t data);
void State3(int ev,process_data_t data);
void State4(int ev,process_data_t data);
void CallbackTimeout(void *);

/*---------------------------------------------------------------------------*/
static struct simple_udp_connection ucast_connection;

// definition du type de données transféré entre receiver et protothread
#define SIZEMAX_MSG	30
typedef struct 
{
  char msg[SIZEMAX_MSG];	// message recu
  int8_t size_msg;		// taille du message recu
} sdata_t;
//definition du type d'un enum pour gérer les états
enum eState_t {s1, s2, s3, s4};

// variable qui contient l'adresse du server
static uip_ipaddr_t nodeAddrServer;	
// variable de type Event pour indiquer la reception d'une trame
static process_event_t rcv_event = 0;

// variable globale contenant l'état du client
static enum eState_t clientState = s1;
//Declaration des etimer
static struct etimer timer;
static struct etimer state4_timer;
static struct ctimer timeout;

/*---------------------------------------------------------------------------*/
PROCESS(nodeClient_process, "Node Client Process");
AUTOSTART_PROCESSES(&nodeClient_process);
/*---------------------------------------------------------------------------*/
// fonction callback de reception des messages
static void receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  static sdata_t message;
  //on vient stocker le message à reçu et on l'envoie au travers d'un event au nodeClient_process
  memcpy(message.msg, data, datalen*sizeof(char));
  message.size_msg = datalen;
  process_post(&nodeClient_process, rcv_event, &message);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nodeClient_process, ev, data)
{
  PROCESS_BEGIN(); 
  //On active le bouton
  SENSORS_ACTIVATE(button_sensor);

  
  etimer_set(&timer, CLOCK_SECOND*5);

  //On appelle la fonction receiver lorsqu'on reçoit un message
  simple_udp_register(&ucast_connection, UDP_PORT, NULL, UDP_PORT, receiver);

  // set address server (node 1 sur Cooja)
  uip_ip6addr(&nodeAddrServer, 0xfe80, 0x0, 0x0, 0x0, 0xc30c, 0x0, 0x0, 0x1);
  LOG_INFO_("V0.2 Server address: ");
  LOG_INFO_6ADDR(&nodeAddrServer);
  LOG_INFO_("\n");

  Init();
  while(1) 
  {
    //PROCESS_WAIT_EVENT();
    switch (clientState){
      case s1:
        State1(ev, data);
        break;
      case s2:
        State2(ev, data);
        break;
      case s3:
        State3(ev, data);
        break;
      case s4:
        State4(ev, data);
        break;
      default:
        clientState = s1;
    }
    /*if(etimer_expired(&timer))
    {
         Wait for the periodic timer to expire and then restart the timer. */
        /*simple_udp_sendto(&ucast_connection, "PING", 10, &nodeAddrServer);
        printf("PING\n");
        etimer_reset(&timer);
    }*/
    /*else if(ev == eventReceived)
    {
      
    } */ 
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

//Défintion des différents états appelés par le switch case dans la boucle while:
void Init(void)
{
  struct stimer sTimer;
  //On vient allumer les LEDs et on utilise un timer bloquant pour attendre 2secondes
  leds_on(LEDS_ALL);
  printf("Node Client: start\n");
  stimer_set(&sTimer, 2);
}

void State1(int ev,process_data_t data)
{
  static bool bLed = false;
  if(etimer_expired(&timer))
  {
      /* Wait for the periodic timer to expire and then restart the timer. */
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_GREEN);
      }
      else
      {
        leds_off(LEDS_GREEN);
      }
      etimer_set(&timer, CLOCK_SECOND);
  }
  else if((ev == sensors_event) && (data == &button_sensor))
  {
    leds_off(LEDS_GREEN);
    clientState = s2;
    simple_udp_sendto(&ucast_connection, "Time?", 10, &nodeAddrServer);
  }
}

void State2(int ev,process_data_t data)
{
  static bool bLed = false;
  
  if(ctimer_expired(&timeout))
  {
    //On active un timer timeout de 5 secondes
    ctimer_set(&timeout, CLOCK_SECOND*5, &CallbackTimeout, NULL);

  }
  if(etimer_expired(&timer))
  {
      /* Wait for the periodic timer to expire and then restart the timer. */
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_GREEN);
      }
      else
      {
        leds_off(LEDS_GREEN);
      }
      etimer_set(&timer, CLOCK_SECOND/2);
  }
  if(ev == rcv_event)
  {
    ctimer_stop(&timeout);
    clientState = s3;
  }
}

void State3(int ev,process_data_t data)
{
  static bool bLed = false;
  sdata_t *message = (sdata_t*)data;
  if(etimer_expired(&timer))
  {
      /* Wait for the periodic timer to expire and then restart the timer. */
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_GREEN);
      }
      else
      {
        leds_off(LEDS_GREEN);
      }
      etimer_set(&timer, CLOCK_SECOND/4);
  }

  //on vient comparer le début du message reçu avec le message attendu
  if(!memcmp(message->msg, "ServerID", sizeof(uint8_t)*9))
  {
    printf(message->msg);
    clientState = s1;
  }
  else
  {
    printf("error\n");
    printf(message->msg);
    //On met en route le timer de 3 secondes pour le state 4
    etimer_set(&state4_timer, CLOCK_SECOND*3);
    printf("wait\n");
    clientState = s4;
  }
}

void State4(int ev,process_data_t data)
{
  static bool bLed = false;
  if(etimer_expired(&timer))
  {
      /* Wait for the periodic timer to expire and then restart the timer. */
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_RED);
      }
      else
      {
        leds_off(LEDS_RED);
      }
      etimer_set(&timer, CLOCK_SECOND/2);
  }
  if(etimer_expired(&state4_timer))
  {
    //Après 3 secondes on revient dans l'état 1
    leds_off(LEDS_RED);
    clientState = s1;
  }
}

void CallbackTimeout(void * ptr)
{
  (void)ptr;
  printf("timeout\n");
  clientState = s4;
}