/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    comp.c
 * @brief   COMP Driver code.
 *
 * @addtogroup COMP
 * @{
 */

#include "ch.h"
#include "hal.h"

#if HAL_USE_COMP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   COMP Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void compInit(void) {

  comp_lld_init();
}

/**
 * @brief   Initializes the standard part of a @p COMPDriver structure.
 *
 * @param[out] compp     pointer to the @p COMPDriver object
 *
 * @init
 */
void compObjectInit(COMPDriver *compp) {

  compp->state  = COMP_STOP;
  compp->config = NULL;
}

/**
 * @brief   Configures and activates the COMP peripheral.
 *
 * @param[in] compp     pointer to the @p COMPDriver object
 * @param[in] config    pointer to the @p COMPConfig object
 *
 * @api
 */
void compStart(COMPDriver *compp, const COMPConfig *config) {

  chDbgCheck((compp != NULL) && (config != NULL), "compStart");

  chSysLock();
  chDbgAssert((compp->state == COMP_STOP) || (compp->state == COMP_ACTIVE),
              "compStart(), #1", "invalid state");
  compp->config = config;
  comp_lld_start(compp);
  compp->state = COMP_ACTIVE;
  chSysUnlock();
}

/**
 * @brief   Deactivates the COMP peripheral.
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 *
 * @api
 */
void compStop(COMPDriver *compp) {

  chDbgCheck(compp != NULL, "compStop");

  chSysLock();
  chDbgAssert((compp->state == COMP_STOP) || (compp->state == COMP_ACTIVE),
              "compStop(), #1", "invalid state");
  comp_lld_stop(compp);
  compp->state = COMP_STOP;
  chSysUnlock();
}

#endif /* HAL_USE_COMP */

/** @} */
