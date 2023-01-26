/*---------------------------------------------------------------------------*/
/**
 * \file
 *         ucastServer23_XH.c: 
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
#include "sys/rtimer.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "sys/log.h"
#define LOG_MODULE "TE2023-SERVER"
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
void CallbackSend(void * ptr);

/*---------------------------------------------------------------------------*/
static struct simple_udp_connection ucast_connection;

// definition du type de données transféré entre receiver et protothread
#define SIZEMAX_MSG	30
typedef struct 
{
  char msg[SIZEMAX_MSG];	      // message recu
  int8_t size_msg;		          // taille du message recu
  uip_ipaddr_t nodeAddrClient;  // adresse du client  	
} sdata_t;
//definition du type d'un enum pour gérer les états
enum eState_t {s1, s2, s3, s4};

// variable de type Event pour indiquer la reception d'une trame
static process_event_t rcv_event = 0;

// variable globale contenant l'état du server
static enum eState_t serverState = s1;

//Déclaration des timers
static struct etimer timer;
static struct ctimer timerSend;
static struct etimer state4_timer;

/*---------------------------------------------------------------------------*/
PROCESS(nodeServer_process, "Node Server Process");
AUTOSTART_PROCESSES(&nodeServer_process);
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
  memcpy(message.nodeAddrClient.u16, sender_addr, sizeof(uint16_t)*8);
  process_post(&nodeServer_process, rcv_event, &message);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nodeServer_process, ev, data)
{
  PROCESS_BEGIN(); 
  //Declaration d'un etimer
  static struct etimer timer;
  etimer_set(&timer, CLOCK_SECOND*5);

  static uip_ipaddr_t uipBroadcast;
  uip_create_linklocal_allnodes_mcast(&uipBroadcast);

  //On appelle la fonction receiver lorsqu'on reçoit un message
  simple_udp_register(&ucast_connection, UDP_PORT, NULL, UDP_PORT, receiver);
  printf("V0.2 ServerNode");
  Init();

  while(1) 
  {
    //PROCESS_WAIT_EVENT();
    switch (serverState){
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
        serverState = s1;
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void Init(void)
{
  struct stimer sTimer;
  //On vient allumer les LEDs et on utilise un timer bloquant pour attendre 2secondes
  leds_on(LEDS_ALL);
  printf("Node Server: start\n");
  stimer_set(&sTimer, 2);
}

void State1(int ev,process_data_t data)
{
  static bool bLed = false;
  if(etimer_expired(&timer))
  {
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_BLUE);
      }
      else
      {
        leds_off(LEDS_BLUE);
      }
      etimer_set(&timer, CLOCK_SECOND/2);
  }
  if(ev == rcv_event)
  {
    serverState = s2;
  }
}

void State2(int ev,process_data_t data)
{
  static bool bLed = false;
  sdata_t *message = (sdata_t*)data;
  if(etimer_expired(&timer))
  {
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_BLUE);
      }
      else
      {
        leds_off(LEDS_BLUE);
      }
      etimer_set(&timer, CLOCK_SECOND/4);
  }
  //on vient comparer le début du message reçu avec le message attendu
  if(!strcmp(message->msg,"Time?"))
  {
    printf(message->msg);
    serverState = s3;
  }
  else
  {
    printf("error\n");
    printf(message->msg);
    leds_off(LEDS_BLUE);
    //On met en route le timer pour le state 4
    etimer_set(&state4_timer, CLOCK_SECOND*3);
    printf("wait\n");
    serverState = s4;
  }
}

void State3(int ev,process_data_t data)
{
  static bool bLed = false;
  if(etimer_expired(&timer))
  {
      bLed = !bLed;
      if(bLed)
      {
        leds_on(LEDS_BLUE);
      }
      else
      {
        leds_off(LEDS_BLUE);
      }
      etimer_set(&timer, CLOCK_SECOND/4);
  }
  //On met en route un callback timer qui va s'occuper d'envoyer le message après 3 secondes
  ctimer_set(&timerSend, CLOCK_SECOND*3, &CallbackSend, data);
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
    serverState = s1;
  }
}

void CallbackSend(void * ptr)
{
  //variable qui va s'incrémenter à chaque envoi
  static unsigned int ui32Send = 0;
  //On récupère l'adresse de la node qui nous a envoyé le message
  sdata_t *message = (sdata_t*)ptr;
  
  //Construction du message à envoyer:
  static char msg[50] = "ServerID :";
  static char dummy[10];
  sprintf(dummy, "%u", ui32Send);
  strcat(msg,dummy);
  strcat(msg,";Time: ");
  //On peut avoir le temps actuel en secondes en utilisant RTIMER_NOW()/
  sprintf(dummy, "%u", RTIMER_NOW());
  strcat(msg,dummy);

  simple_udp_sendto(&ucast_connection, msg, 50, &(message->nodeAddrClient));
  serverState = s1;
}