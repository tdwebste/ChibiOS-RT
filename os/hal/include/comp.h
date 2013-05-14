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
 * @file    comp.h
 * @brief   COMP Driver macros and structures.
 *
 * @addtogroup COMP
 * @{
 */

#ifndef _COMP_H_
#define _COMP_H_

#if HAL_USE_COMP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  COMP_UNINIT = 0,                   /**< Not initialized.                   */
  COMP_STOP = 1,                     /**< Stopped.                           */
  COMP_ACTIVE = 2,                   /**< Active.                            */
} compstate_t;

/**
 * @brief   Type of a structure representing an COMP driver.
 */
typedef struct COMPDriver COMPDriver;

#include "comp_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @name    Macro Functions
 * @{
 */
/** @} */

/**
 * @name    Low Level driver helper macros
 * @{
 */
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void compInit(void);
  void compObjectInit(COMPDriver *compp);
  void compStart(COMPDriver *compp, const COMPConfig *config);
  void compStop(COMPDriver *compp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_COMP */

#endif /* _COMP_H_ */

/** @} */
