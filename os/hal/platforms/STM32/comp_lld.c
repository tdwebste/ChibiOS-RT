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
/*
   Concepts and parts of this file have been contributed by Fabio Utzig and
   Xo Wang.
 */

/**
 * @file    STM32/comp_lld.c
 * @brief   STM32 COMP subsystem low level driver header.
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

/**
 * @brief   COMPD1 driver identifier.
 * @note    The driver COMPD1 allocates the complex timer TIM1 when enabled.
 */
#if STM32_COMP_USE_COMP1 || defined(__DOXYGEN__)
COMPDriver COMPD1;
#endif


/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if 0

Comparator #1, #2 use EXTI line 21, 22

Need to plug into EXTI interrupt handler

/**
 * @brief   Shared IRQ handler.
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 */
static void comp_lld_serve_interrupt(COMPDriver *compp) {
  uint16_t sr;

  sr  = icup->comp->SR;
  sr &= icup->comp->DIER;
  icup->comp->SR = ~sr;
  if (icup->config->channel == ICU_CHANNEL_1) {
    if ((sr & COMP_SR_CC1IF) != 0)
      _icu_isr_invoke_period_cb(icup);
    if ((sr & COMP_SR_CC2IF) != 0)
      _icu_isr_invoke_width_cb(icup);
  } else {
    if ((sr & COMP_SR_CC1IF) != 0)
      _icu_isr_invoke_width_cb(icup);
    if ((sr & COMP_SR_CC2IF) != 0)
      _icu_isr_invoke_period_cb(icup);
  }
  if ((sr & COMP_SR_UIF) != 0)
    _icu_isr_invoke_overflow_cb(icup);
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if STM32_COMP_USE_COMP1
#if !defined(STM32_COMP1_UP_HANDLER)
#error "STM32_COMP1_UP_HANDLER not defined"
#endif
/**
 * @brief   COMP1 compare interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(STM32_COMP1_UP_HANDLER) {

  CH_IRQ_PROLOGUE();

  comp_lld_serve_interrupt(&COMPD1);

  CH_IRQ_EPILOGUE();
}

#endif /* STM32_COMP_USE_COMP1 */
#endif /* #if 0  EXTI INTERRUPT*/


/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level COMP driver initialization.
 *
 * @notapi
 */
void comp_lld_init(void) {

#if STM32_COMP_USE_COMP1
  /* Driver initialization.*/
  compObjectInit(&COMPD1);
  COMPD1.comp = COMP;
#endif

}

static void disable_comp(COMPDriver *compdp) {

    compdp->comp->CSR &= ~(COMP_CSR_COMP1EN | COMP_CSR_COMP2EN);

}

/**
 * @brief   Configures and activates the COMP peripheral.
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 *
 * @notapi
 */
void comp_lld_start(COMPDriver *compdp) {
  chDbgAssert(compdp, "comp_lld_start(), #1", "invalid input");

  if (compdp->state != COMP_STOP) {
    chDbgAssert(compdp->comp->CSR & COMP_CSR_COMP1LOCK ||
		compdp->comp->CSR & COMP_CSR_COMP2LOCK, 
		"comp_lld_start(), #1", "cannot reconfigure, locked");

    /* Driver re-configuration scenario, it must be stopped first.*/
    disable_comp(compdp);
  }

  /* COMP configuration.*/
  compdp->comp->CSR |=
    compdp->config->comp_enable              |
    (compdp->config->comp1_inv_in_src <<  4) |
    (compdp->config->comp2_inv_in_src << 20) |
    (compdp->config->comp1_hysteresis << 12) |
    (compdp->config->comp2_hysteresis << 28) |
    (compdp->config->comp1_lock       << 15) |
    (compdp->config->comp2_lock       << 31) |
    (compdp->config->comp1_opmode     <<  2) |
    (compdp->config->comp2_opmode     << 18) |
    (compdp->config->comp1_output_pol << 11) |
    (compdp->config->comp2_output_pol << 27) |
    (compdp->config->comp1_output_sel <<  8) |
    (compdp->config->comp2_output_sel << 24);
}

/**
 * @brief   Deactivates the COMP peripheral.
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 *
 * @notapi
 */
void comp_lld_stop(COMPDriver *compdp) {

  if (compdp->state == COMP_ACTIVE) {
    chDbgAssert(compdp->comp->CSR & COMP_CSR_COMP1LOCK ||
		compdp->comp->CSR & COMP_CSR_COMP2LOCK, 
		"comp_lld_start(), #1", "cannot reconfigure, locked");
    
    disable_comp(compdp);
  }
}

/**
 * @brief   Returns the value of the output of the comparator
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 *
 * @notapi
 */
bool_t comp_lld_comp1val(COMPDriver *compp) {
  return (compp->comp->CSR & COMP_CSR_COMP1OUT);
}
bool_t comp_lld_comp2val(COMPDriver *compp) {
  return (compp->comp->CSR & COMP_CSR_COMP2OUT);
}


#endif /* HAL_USE_COMP */

/** @} */
