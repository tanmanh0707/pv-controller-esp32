#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "configs.h"

#if defined (DEBUG_ENABLE)
  #define UART_DEBUG_PRINTLN(debug)\
  { \
    LOG_SERIAL.println debug; \
  } \

  #define UART_DEBUG_PRINT(debug)\
  { \
    LOG_SERIAL.print debug; \
  } \

  #define UART_DEBUG_PRINTF(debug)\
  { \ 
    LOG_SERIAL.printf debug; \
  } \

  #define SERIAL_DEBUG(X)               LOG_SERIAL.println(X)
  #define SERIAL_PRINTLN                UART_DEBUG_PRINTLN
  #define SERIAL_PRINT                  UART_DEBUG_PRINT
  #define SERIAL_PRINTF                 UART_DEBUG_PRINTF
#else
  #define SERIAL_DEBUG(X)               {}
  #define SERIAL_PRINT(X)               {}
  #define SERIAL_PRINTLN(X)             {}
#endif  //DEBUG_ENABLE

// LOG_SERIAL.printf("[%-30s:%d] ", __FUNCTION__, __LINE__); LOG_SERIAL.printf debug
/***************************************************************************
 Function     : LOGGER_Init
 Description  : Initiate logging using Serial port
 Input        : N/A
 Output       : N/A
 Return       : N/A
***************************************************************************/
class LOG
{
public:
  static void Init() {
    LOG_SERIAL.begin( LOG_BAUDRATE );
    SERIAL_PRINTLN((""));
    SERIAL_PRINTLN(("******************************"));
#if defined (PROJECT_NAME)
    SERIAL_PRINTLN((PROJECT_NAME " STARTED"));
#else
    SERIAL_PRINTLN(("       PROJECT STARTED        "));
#endif    
    SERIAL_PRINTLN(("******************************"));
  }
};
#endif  //_LOGGING_H_
