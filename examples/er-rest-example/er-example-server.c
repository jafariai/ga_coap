#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include <stdio.h>
#include <stdlib.h>

/* Resources to be activated need to be imported through the extern keyword. */
extern resource_t res_hello, res_push;
#if PLATFORM_HAS_LEDS
extern resource_t res_toggle;
#endif
#if PLATFORM_HAS_LIGHT
#include "dev/light-sensor.h"
extern resource_t res_light;
#endif

PROCESS(er_example_server, "Erbium Example Server");

AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate resources with meaningful names. */
  rest_activate_resource(&res_hello, "test/hello");
  rest_activate_resource(&res_push, "test/push");

  #if PLATFORM_HAS_LEDS
  rest_activate_resource(&res_toggle, "actuators/toggle");
  #endif

  #if PLATFORM_HAS_LIGHT
  rest_activate_resource(&res_light, "sensors/light");
  SENSORS_ACTIVATE(light_sensor);
  #endif

  /* Define application-specific events here. */
  while (1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}

