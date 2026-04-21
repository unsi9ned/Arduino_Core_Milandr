/**
  * FILE system_K1986VE1xI.h
  */

/** @addtogroup __CMSIS CMSIS
  * @{
  */

/** @defgroup K1986VE1xI K1986VE1xI
 *  @{
 */

/** @defgroup K1986VE1xI_System K1986VE1xI System
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_K1986VE1xI_H
#define __SYSTEM_K1986VE1xI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> /* standard types definitions */

/** @addtogroup __K1986VE1xI_System_Exported_Variables K1986VE1xI System Exported Variables
  * @{
  */

extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock) default value */

/** @} */ /* End of group __K1986VE1xI_System_Exported_Variables */

/** @addtogroup __K1986VE1xI_System_Exported_Functions K1986VE1xI System Exported Functions
  * @{
  */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

/** @} */ /* End of group __K1986VE1xI_System_Exported_Functions */

#ifdef __cplusplus
}
#endif

#endif /* #define __SYSTEM_K1986VE1xI_H */

/** @} */ /* End of group K1986VE1xI_System */

/** @} */ /* End of goup K1986VE1xI */

/** @} */ /* End of group __CMSIS */

/*
*
* END OF FILE system_K1986VE1xI.h */

