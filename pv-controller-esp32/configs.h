#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <Arduino.h>

#define PROJECT_NAME            "PV CONTROLLER"

/* Logging */
#define LOG_BAUDRATE            115200
#define LOG_SERIAL              Serial
#define DEBUG_ENABLE

/* LED built-in */
#define LED_BUILTIN_ON          HIGH
#define LED_BUILTIN_OFF         (!LED_BUILTIN_ON)

/* IO-Expander modules */
#define IO_EXPANDER_ADDR_1      0x5C
#define IO_EXPANDER_ADDR_2      0x5D
#define IO_EXPANDER_ADDR_3      0x5E
#define RELAY_DEBOUCE_TIME      300     //milisec
#define NUMBER_OF_PV            42
#define IO_EXP_BLINK_PIN        14

/* Shunt type */
#define SHUNT_TYPE_100MV

/* INPUT */
#define INPUT_PIN               10      //esp32 pin

/* Current threshold */
#define CURRENT_HIGH_LOWER      70.0    //Amps
#define CURRENT_HIGH_UPPER      80.0
#define CURRENT_LOW_LOWER       (-15.0)
#define CURRENT_LOW_UPPER       (-5.0)
#endif  //_CONFIGS_H_
