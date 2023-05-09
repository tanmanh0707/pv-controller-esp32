#ifndef _COMMON_H_
#define _COMMON_H_
#include "configs.h"
#include "logger.h"

void CurrentMeasureTask (void *arg);

/* IO Expander */
void IOEXP_Setup();
bool IOEXP_Write(uint8_t io_num, uint8_t pin_num, uint8_t mode);
void PV_Control(uint8_t pv_num, uint8_t mode);

/* ADS current */
void ADS_Setup();
float ADS_GetAverageValue();

/* Common mutex */
void CommonMutex_Setup();
bool CommonMutex_Take();
bool CommonMutex_Give();
#endif //_COMMON_H_
