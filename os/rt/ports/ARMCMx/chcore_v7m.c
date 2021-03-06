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
 * @file    chcore_v7m.c
 * @brief   ARMv7-M architecture port code.
 *
 * @addtogroup ARMCMx_V7M_CORE
 * @{
 */

#include "ch.h"

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module interrupt handlers.                                                */
/*===========================================================================*/

#if !CORTEX_SIMPLIFIED_PRIORITY || defined(__DOXYGEN__)
/**
 * @brief   SVC vector.
 * @details The SVC vector is used for exception mode re-entering after a
 *          context switch.
 * @note    The PendSV vector is only used in advanced kernel mode.
 */
void SVC_Handler(void) {

  /* The port_extctx structure is pointed by the PSP register.*/
  struct port_extctx *ctxp = (struct port_extctx *)__get_PSP();

  /* Discarding the current exception context and positioning the stack to
     point to the real one.*/
  ctxp++;

#if CORTEX_USE_FPU
  /* Restoring the special register FPCCR.*/
  FPU->FPCCR = (uint32_t)ctxp->fpccr;
  FPU->FPCAR = FPU->FPCAR + sizeof (struct port_extctx);
#endif

  /* Writing back the modified PSP value.*/
  __set_PSP((uint32_t)ctxp);

  /* Restoring the normal interrupts status.*/
  port_unlock_from_isr();
}
#endif /* !CORTEX_SIMPLIFIED_PRIORITY */

#if CORTEX_SIMPLIFIED_PRIORITY || defined(__DOXYGEN__)
/**
 * @brief   PendSV vector.
 * @details The PendSV vector is used for exception mode re-entering after a
 *          context switch.
 * @note    The PendSV vector is only used in compact kernel mode.
 */
void PendSV_Handler(void) {

  /* The port_extctx structure is pointed by the PSP register.*/
  struct port_extctx *ctxp = (struct port_extctx *)__get_PSP();

  /* Discarding the current exception context and positioning the stack to
     point to the real one.*/
  ctxp++;

#if CORTEX_USE_FPU
  /* Restoring the special register FPCCR.*/
  FPU->FPCCR = (uint32_t)ctxp->fpccr;
  FPU->FPCAR = FPU->FPCAR + sizeof (struct port_extctx);
#endif

  /* Writing back the modified PSP value.*/
  __set_PSP((uint32_t)ctxp);
}
#endif /* CORTEX_SIMPLIFIED_PRIORITY */

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Exception exit redirection to _port_switch_from_isr().
 */
void _port_irq_epilogue(void) {

  port_lock_from_isr();
  if ((SCB->ICSR & SCB_ICSR_RETTOBASE_Msk) != 0) {

    /* The port_extctx structure is pointed by the PSP register.*/
    struct port_extctx *ctxp = (struct port_extctx *)__get_PSP();

    /* Adding an artificial exception return context, there is no need to
       populate it fully.*/
    ctxp--;

    /* Writing back the modified PSP value.*/
    __set_PSP((uint32_t)ctxp);

    /* Setting up a fake XPSR register value.*/
    ctxp->xpsr = (regarm_t)0x01000000;

    /* The exit sequence is different depending on if a preemption is
       required or not.*/
    if (chSchIsPreemptionRequired()) {
      /* Preemption is required we need to enforce a context switch.*/
      ctxp->pc = (regarm_t)_port_switch_from_isr;
#if CORTEX_USE_FPU
      /* Enforcing a lazy FPU state save by accessing the FPCSR register.*/
      (void) __get_FPSCR();
#endif
    }
    else {
      /* Preemption not required, we just need to exit the exception
         atomically.*/
      ctxp->pc = (regarm_t)_port_exit_from_isr;
    }

#if CORTEX_USE_FPU
    {
      uint32_t fpccr;

      /* Saving the special register SCB_FPCCR into the reserved offset of
         the Cortex-M4 exception frame.*/
      (ctxp + 1)->fpccr = (regarm_t)(fpccr = FPU->FPCCR);

      /* Now the FPCCR is modified in order to not restore the FPU status
         from the artificial return context.*/
      FPU->FPCCR = fpccr | FPU_FPCCR_LSPACT_Msk;
    }
#endif

    /* Note, returning without unlocking is intentional, this is done in
       order to keep the rest of the context switch atomic.*/
    return;
  }
  port_unlock_from_isr();
}

/** @} */
