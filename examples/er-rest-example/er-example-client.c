#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include "dev/button-sensor.h"
#include <stdio.h>


#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(VA_ARGS)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/* FIXME: This server address is hard-coded for Cooja and link-local for unconnected border router. */
#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7402, 0x0002, 0x0202)      /* cooja2 */
/* #define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x1) */

#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)

#define TOGGLE_INTERVAL 5

PROCESS(er_example_client, "Erbium Example Client");
AUTOSTART_PROCESSES(&er_example_client);

uip_ipaddr_t server_ipaddr;
static struct etimer et;

/* Example URIs that can be queried. */
#define NUMBER_OF_URLS 4
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
char *service_urls[NUMBER_OF_URLS] = { ".well-known/core", "/actuators/toggle", "battery/", "error/in//path" };
#if PLATFORM_HAS_BUTTON
static int uri_switch = 0;
#endif

/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}


PROCESS_THREAD(er_example_client, ev, data)
{

PROCESS_BEGIN();

    static coap_packet_t request[1];

    SERVER_NODE(&server_ipaddr);
    coap_init_engine();
    etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);
    long int x = 0 ;
    
    

    while (1) {
    	x = 0;
        PROCESS_YIELD();

        if (etimer_expired(&et)) {
            printf("Sending CoAP Request...\n");

            clock_time_t request_timestamp = clock_time();
            x = request_timestamp * 10 ;
	
            coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
            coap_set_header_uri_path(request, service_urls[1]);
            const char msg[] = "Toggle!";
            coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

            PRINT6ADDR(&server_ipaddr);
            PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

            // Send the request and record the timestamp
            COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request, client_chunk_handler);

            // Calculate and print RTT
            printf("#################################################\n");

            etimer_reset(&et);
        }
    }

    PROCESS_END();
    
}

