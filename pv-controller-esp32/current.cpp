#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "common.h"
#include <Wire.h>
#include "Adafruit_ADS1X15.h"

/********************************************************************
 Definitions and macros
 ********************************************************************/
#define LOCAL_PRINT     SERIAL_PRINT
#define LOCAL_PRINTF    SERIAL_PRINTF
#define LOCAL_PRINTLN   SERIAL_PRINTLN

#define NUM_AMP_VAL     16
/********************************************************************
 Local Variables
 ********************************************************************/
static SemaphoreHandle_t g_ampMutex = NULL;
static TaskHandle_t CurrentMeasureTaskHandle;
static TaskHandle_t IoexpBlinkTaskHandle;
static Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
static float amp_list[NUM_AMP_VAL];
static uint8_t amp_pos = 0;
static bool isStarted = false;
static unsigned long g_ads_time = 0;
/********************************************************************
 Local functions
 ********************************************************************/
void CurrentMeasureTask (void *arg)
{
  int16_t results = 0;
  float amps = 0.0;

  LOCAL_PRINTLN(("Setting up current measurement..."));
  CommonMutex_Take();
  ads.setGain(GAIN_SIXTEEN);    // 16x gain +/- 0.256V 1 bit = 0.125mV 0.0078125mV
  ads.begin();
  CommonMutex_Give();
  LOCAL_PRINTLN(("Current measurement started!"));

  while (1)
  {
    CommonMutex_Take();
    results = ads.readADC_Differential_0_1();
    CommonMutex_Give();

    amps = ((float)results * 256.0) / 32768.0;//100mv shunt
#if defined (SHUNT_TYPE_75MV)
    amps = amps * 1.333;  //75mv shunt
#endif
#if defined (SHUNT_TYPE_50MV)
    amps = amps * 2;      //50mv shunt
#endif

    xSemaphoreTake(g_ampMutex, 1000);
    amp_list[amp_pos++] = amps;
    xSemaphoreGive( g_ampMutex );

    amp_pos = (amp_pos >= NUM_AMP_VAL)? 0 : amp_pos;

    /* Wait for the first 16 readings. This take effect at startup only */
    if (isStarted == false) {
      if (amp_pos == (NUM_AMP_VAL - 1)) {
        isStarted = true;
        LOCAL_PRINTLN(("Current measurement is stable now!"));
      }
    }

    g_ads_time = millis();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void IOEXP_BlinkTask (void *arg)
{
  uint8_t i = 0;
  uint8_t blink_state = HIGH;
  uint8_t led_builtin_state = LOW;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  LOCAL_PRINTLN(("Blinking task started!"));
  while (1)
  {
    for (i = 0; i < 3; i++)
    {
      IOEXP_Write(i + 1, IO_EXP_BLINK_PIN, blink_state);
    }
    blink_state = (blink_state == HIGH)? LOW : HIGH;

    // ADS Led-builtin indicator
    if (millis() - g_ads_time > 1000) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      digitalWrite(LED_BUILTIN, led_builtin_state);
      led_builtin_state = (led_builtin_state == HIGH)? LOW : HIGH;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

bool ADS_IsStarted()
{
  return isStarted;
}

void ADS_Setup()
{
  g_ampMutex = xSemaphoreCreateMutex();
  BaseType_t ret = xTaskCreatePinnedToCore( CurrentMeasureTask,         /* Task function. */
                                            "CurrentMeasure",           /* name of task. */
                                            8192,                       /* Stack size of task */
                                            NULL,                       /* parameter of the task */
                                            1,                          /* priority of the task */
                                            &CurrentMeasureTaskHandle,  /* Task handle to keep track of created task */
                                            0);                         /* Core */
  
  if (ret != pdPASS) {
    LOCAL_PRINTLN(("ADS setup failed!!!"));    
  }

  ret = xTaskCreatePinnedToCore( IOEXP_BlinkTask,            /* Task function. */
                                 "IOEXP_BlinkTask",          /* name of task. */
                                 8192,                       /* Stack size of task */
                                 NULL,                       /* parameter of the task */
                                 1,                          /* priority of the task */
                                 &IoexpBlinkTaskHandle,      /* Task handle to keep track of created task */
                                 0);                         /* Core */
  if (ret != pdPASS) {
    LOCAL_PRINTLN(("Blinking setup failed!!!"));    
  }
}

float ADS_GetAverageValue()
{
  uint8_t i = 0;
  float average = 0.0;

  /* Wait for stable readings at startup */
  if (isStarted == false) {
    return 0.0;
  }

  xSemaphoreTake(g_ampMutex, 1000);
  for (i = 0; i < NUM_AMP_VAL; i++) {
    average += amp_list[i];
  }
  xSemaphoreGive( g_ampMutex );

  return (average / (float)NUM_AMP_VAL);
}
