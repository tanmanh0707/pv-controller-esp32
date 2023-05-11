#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
float g_current = 0.0;    //Amps
uint8_t g_crtPVn = 0;
/********************************************************************
 Local functions
 ********************************************************************/
void setup()
{
  LOG::Init();

  pinMode(INPUT_PIN, INPUT);
  CommonMutex_Setup();
  IOEXP_Setup();
  ADS_Setup();
}

void loop()
{
  g_current = ADS_GetAverageValue();

  /* Wait for stable readings at startup */
  if (ADS_IsStarted() == false) {
    return;    
  }

  /* Handle INPUT HIGH */
  if (digitalRead(INPUT_PIN) == HIGH)
  {
    if (CURRENT_HIGH_LOWER <= g_current && g_current <= CURRENT_HIGH_UPPER) {
      //Do nothing
    }
    else if (g_current > CURRENT_HIGH_UPPER) {
      //Turn off PVs
      if (g_crtPVn > 0) {
        PV_Control(g_crtPVn, LOW);
        g_crtPVn -= 1;  //Decrease PV
      }
      else if (g_crtPVn == 0) {
        PV_Control(g_crtPVn, LOW);
        LOCAL_PRINTF(("ALERT!!! ALL PVs HAVE BEEN TURNED OFF!!! CURRENT VALUE: %f - INPUT: %d\n", g_current, digitalRead(INPUT_PIN)));
      }
    }
    else if (g_current < CURRENT_HIGH_LOWER) {
      //Turn on PVs
      if (g_crtPVn < NUMBER_OF_PV) {      
        PV_Control(g_crtPVn, HIGH);
        g_crtPVn += 1;  //Increase PV

        if (g_crtPVn >= NUMBER_OF_PV) {
          LOCAL_PRINTF(("ALERT!!! ALL PVs HAVE BEEN TURNED ON!!! CURRENT VALUE: %f - INPUT: %d\n", g_current, digitalRead(INPUT_PIN)));
        }
      }
    }
  }

  /* Hanlde INPUT LOW */
  else
  {
    if (g_current <= CURRENT_LOW_LOWER)
    {
      if (g_crtPVn < NUMBER_OF_PV) {
        PV_Control(g_crtPVn, HIGH);
        g_crtPVn += 1;  //Increase PV

        if (g_crtPVn >= NUMBER_OF_PV) {
          LOCAL_PRINTF(("ALERT!!! ALL PVs HAVE BEEN TURNED ON!!! CURRENT VALUE: %f - INPUT: %d\n", g_current, digitalRead(INPUT_PIN)));
        }
      }
    }
    else if (g_current >= CURRENT_LOW_UPPER)
    {
      if (g_crtPVn > 0) {
        PV_Control(g_crtPVn, LOW);
        g_crtPVn -= 1;  //Decrease PV
      }
      else if (g_crtPVn == 0) {
        PV_Control(g_crtPVn, LOW);
        LOCAL_PRINTF(("ALERT!!! ALL PVs HAVE BEEN TURNED OFF!!! CURRENT VALUE: %f - INPUT: %d\n", g_current, digitalRead(INPUT_PIN)));
      }
    }
    else {
      // Do nothing
    }
  }

  vTaskDelay(pdMS_TO_TICKS(10));
}
