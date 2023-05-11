#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "common.h"
#include "pcf8575.h"

/********************************************************************
 Definitions and macros
 ********************************************************************/
#define LOCAL_PRINT     SERIAL_PRINT
#define LOCAL_PRINTF    SERIAL_PRINTF
#define LOCAL_PRINTLN   SERIAL_PRINTLN

/********************************************************************
 Local Variables
 ********************************************************************/
static unsigned long last_write_time[NUMBER_OF_PV] = {0};
static unsigned long time_diff = 0;

PCF8575 IOExpander_1(IO_EXPANDER_ADDR_1);
PCF8575 IOExpander_2(IO_EXPANDER_ADDR_2);
PCF8575 IOExpander_3(IO_EXPANDER_ADDR_3);
/********************************************************************
 Local functions
 ********************************************************************/
void IOEXP_Setup()
{
  LOCAL_PRINTLN(("Setting up IO Expander..."));

  for (int i = 0; i < 16; i++) {
    IOExpander_1.pinMode(i, OUTPUT);
    IOExpander_2.pinMode(i, OUTPUT);
    IOExpander_3.pinMode(i, OUTPUT);
  }

  IOExpander_1.begin();
  IOExpander_2.begin();
  IOExpander_3.begin();

  IOEXP_Write(1, IO_EXP_BLINK_PIN, HIGH);
  IOEXP_Write(2, IO_EXP_BLINK_PIN, HIGH);
  IOEXP_Write(3, IO_EXP_BLINK_PIN, HIGH);

  LOCAL_PRINTLN(("IO Expander started!"));
}

bool IOEXP_Write(uint8_t io_num, uint8_t pin_num, uint8_t mode)
{
  LOCAL_PRINTF(("  IO(%d) - Pin(%02d) - State(%d)\n", io_num, pin_num, mode));

  if (CommonMutex_Take() == false) { return false; }
  switch (io_num) {
    case 1: IOExpander_1.digitalWrite(pin_num, mode); break;
    case 2: IOExpander_2.digitalWrite(pin_num, mode); break;
    case 3: IOExpander_3.digitalWrite(pin_num, mode); break;
    default: LOCAL_PRINTF(("[%s] Invalid argument. io_num: %d - pin_num: %d - mode: %d\n", __FUNCTION__, io_num, pin_num, mode)); break;
  }
  CommonMutex_Give();

  return true;
}

void PV_Control(uint8_t pv_num, uint8_t mode)
{
  time_diff = millis() - last_write_time[pv_num];
  if (time_diff < RELAY_DEBOUCE_TIME) {
    vTaskDelay(pdMS_TO_TICKS(time_diff));
  }
  LOCAL_PRINTF(("PV(%02d) - %s\n", pv_num, (mode == HIGH)? "ON":"OFF"));

  IOEXP_Write((pv_num / 14) + 1, (pv_num % 14), mode);
  last_write_time[pv_num] = millis();
}
