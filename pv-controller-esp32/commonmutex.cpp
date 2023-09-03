#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "common.h"

/********************************************************************
 Definitions and macros
 ********************************************************************/
#define LOCAL_PRINT     SERIAL_PRINT
#define LOCAL_PRINTF    SERIAL_PRINTF
#define LOCAL_PRINTLN   SERIAL_PRINTLN

/********************************************************************
 Local Variables
 ********************************************************************/
static SemaphoreHandle_t g_mutex = NULL;

/********************************************************************
 Local functions
 ********************************************************************/
void CommonMutex_Setup()
{
  g_mutex = xSemaphoreCreateMutex();

  if (g_mutex == NULL) {
    LOCAL_PRINTLN(("Common Mutex failed to create!!!"));
  }
}

bool CommonMutex_Take()
{
  if (xSemaphoreTake(g_mutex, 1000) == pdFALSE) {
    /* Semaphore obtained failed */
//    LOCAL_PRINTF(("[%s:%d] Common mutex obtained failed\n", __FUNCTION__, __LINE__));
    return false;
  }

  return true;
}

bool CommonMutex_Give()
{
  if (xSemaphoreGive(g_mutex) == pdFALSE) {
    /* Semaphore obtained failed */
    LOCAL_PRINTF(("[%s:%d] Common mutex released failed\n", __FUNCTION__, __LINE__));
    return false;
  }

  return true;
}
