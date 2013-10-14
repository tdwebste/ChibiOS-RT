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
 * @file    STM32/comp_lld.h
 * @brief   STM32 COMP subsystem low level driver header.
 *
 * @addtogroup COMP
 * @{
 */

#ifndef _COMP_LLD_H_
#define _COMP_LLD_H_

#if HAL_USE_COMP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   COMPD1 driver enable switch.
 * @details If set to @p TRUE the support for COMPD1 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(STM32_COMP_USE_COMP1) || defined(__DOXYGEN__)
#define STM32_COMP_USE_COMP1                  FALSE
#endif

/**
 * @brief   COMPD2 driver enable switch.
 * @details If set to @p TRUE the support for COMPD2 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(STM32_COMP_USE_COMP2) || defined(__DOXYGEN__)
#define STM32_COMP_USE_COMP2                  FALSE
#endif

/**
 * @brief   COMPD1 interrupt priority level setting.
 */
#if !defined(STM32_COMP_TIM1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_COMP_TIM1_IRQ_PRIORITY         7
#endif

/**
 * @brief   COMPD2 interrupt priority level setting.
 */
#if !defined(STM32_COMP_TIM2_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_COMP_TIM2_IRQ_PRIORITY         7
#endif

/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if STM32_COMP_USE_COMP1 && !STM32_HAS_COMP1
#error "COMP1 not present in the selected device"
#endif

#if !STM32_COMP_USE_COMP1
#error "COMP driver activated but no COMP peripheral assigned"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   COMP1 inverting input selection
 */
typedef enum {
  COMP1_INPUT_SRC_14_VREFINT     = 0, /**< 1/4 of Vrefint.      */
  COMP1_INPUT_SRC_12_VREFINT     = 1, /**< 1/2 of Vrefint.      */
  COMP1_INPUT_SRC_34_VREFINT     = 2, /**< 3/4 of Vrefint.      */
  COMP1_INPUT_SRC_VREFINT        = 3, /**< Vrefint.             */
  COMP1_INPUT_SRC_DAC1_OUT1_PA4  = 4, /**< DAC1_OUT1 or PA4.    */
  COMP1_INPUT_SRC_DAC1_OUT2_PA5  = 5, /**< DAC1_OUT2 or PA5.    */
  COMP1_INPUT_SRC_PA0            = 6, /**< DAC1_OUT2 or PA5.    */
  COMP1_INPUT_SRC_DAC2_OUT1_PA6  = 7, /**< DAC1_OUT2 or PA5.    */
} comp1inputsrc_t;

/**
 * @brief   COMP2 inverting input selection
 */
typedef enum {
  COMP2_INPUT_SRC_14_VREFINT     = 0, /**< 1/4 of Vrefint.      */
  COMP2_INPUT_SRC_12_VREFINT     = 1, /**< 1/2 of Vrefint.      */
  COMP2_INPUT_SRC_34_VREFINT     = 2, /**< 3/4 of Vrefint.      */
  COMP2_INPUT_SRC_VREFINT        = 3, /**< Vrefint.             */
  COMP2_INPUT_SRC_DAC1_OUT1_PA4  = 4, /**< DAC1_OUT1 or PA4.    */
  COMP2_INPUT_SRC_DAC1_OUT2_PA5  = 5, /**< DAC1_OUT2 or PA5.    */
  COMP2_INPUT_SRC_PA2            = 6, /**< PA2.                 */
  COMP2_INPUT_SRC_DAC2_OUT1_PA6  = 7, /**< DAC2_OUT2 or PA6.    */
} comp2inputsrc_t;

/**
 * @brief   COMP channel type.
 */
typedef enum {
  COMP_CHANNEL_1 = 0,              /**< Use COMP_CH1 */
  COMP_CHANNEL_2 = 1,              /**< Use COMP_CH2 */
} compchannel_t;

/**
 * @brief   COMP Operation Mode setting
 */
typedef enum {
  COMP_PWR_HIGH_SPEED    = 0,   
  COMP_PWR_MED_SPEED     = 1,   
  COMP_PWR_LOW_PWR       = 2,   
  COMP_PWR_ULTRA_LOW_PWR = 3    
} compopmode_t;

/**
 * @brief   COMP Hysteresis
 */
typedef enum {
  COMP_HYSTERESIS_NONE = 0,   
  COMP_HYSTERESIS_LOW  = 1,   
  COMP_HYSTERESIS_MED  = 2,   
  COMP_HYSTERESIS_HI   = 3    
} comphysteresis_t;

/**
 * @brief   COMP Output polarity
 */
typedef enum {
  COMP_OUTPUT_POL_NOT_INV = 0,   
  COMP_OUTPUT_POL_INVERTED  = 1
} compoutputpolarity_t;

/**
 * @brief   COMP #1 Output Destination
 */
typedef enum {
  COMP1_OUTPUT_DEST_NONE         = 0,   /* no output selected */
  COMP1_OUTPUT_DEST_TIM15_BRK_IN = 1,   /* output to TIM15 break input */
  COMP1_OUTPUT_DEST_TIM3_IC1     = 2,   /* output to TIM3 input capture 1 */
  COMP1_OUTPUT_DEST_TIM3_OCRCI   = 3,   /* output to TIM3 OCrefclear input */
  COMP1_OUTPUT_DEST_TIM2_IC4     = 4,   /* output to TIM2 input capture 4 */
  COMP1_OUTPUT_DEST_TIM2_OCRCI   = 3,   /* output to TIM2 OCrefclear input */
  COMP1_OUTPUT_DEST_TIM5_IC4     = 4,   /* output to TIM5 input capture 4 */
  COMP1_OUTPUT_DEST_TIM5_OCRCI   = 3,   /* output to TIM5 OCrefclear input */
} comp1outputsel_t;

/**
 * @brief   COMP #2 Output Destination
 */
typedef enum {
  COMP2_OUTPUT_DEST_NONE         = 0,   /* no output selected */
  COMP2_OUTPUT_DEST_TIM16_BRK_IN = 1,   /* output to TIM16 break input */
  COMP2_OUTPUT_DEST_TIM4_IC1     = 2,   /* output to TIM4 input capture 1 */
  COMP2_OUTPUT_DEST_TIM4_OCRCI   = 3,   /* output to TIM4 OCrefclear input */
  COMP2_OUTPUT_DEST_TIM2_IC4     = 4,   /* output to TIM2 input capture 4 */
  COMP2_OUTPUT_DEST_TIM2_OCRCI   = 3,   /* output to TIM2 OCrefclear input */
  COMP2_OUTPUT_DEST_TIM3_IC4     = 4,   /* output to TIM3 input capture 1 */
  COMP2_OUTPUT_DEST_TIM3_OCRCI   = 3,   /* output to TIM3 OCrefclear input */
} comp2outputsel_t;

typedef enum {
  COMP1_EN = COMP_CSR_COMP1EN,   /* enable comparator 1 */
  COMP2_EN = COMP_CSR_COMP2EN,   /* enable comparator 2 */
} compenable_t;


/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  /**
   * @brief   Driver mode.
   */
  compenable_t comp_enable;

  /**
   * @brief   Comparator #1 inverting input source.
   */
  comp1inputsrc_t comp1_inv_in_src;
  /**
   * @brief   Comparator #2 inverting input source.
   */
  comp2inputsrc_t comp2_inv_in_src;

  /**
   * @brief   Comparator #1 Hysteresis setting
   */
  comphysteresis_t comp1_hysteresis;
  /**
   * @brief   Comparator #2 Hysteresis setting
   */
  comphysteresis_t comp2_hysteresis;

  /**
   * @brief   Lock comparator #1 config after setting.
   */
  bool_t comp1_lock;
  /**
   * @brief   Lock comparator #2 config after setting.
   */
  bool_t comp2_lock;

  /**
   * @brief   Comparator #1 Operation Mode
   */
  compopmode_t comp1_opmode;
  /**
   * @brief   Comparator #2 Operation Mode
   */
  compopmode_t comp2_opmode;

  /**
   * @brief   Comparator #1 Output Polarity
   */
  compoutputpolarity_t comp1_output_pol;
  /**
   * @brief   Comparator #2 Output Polarity
   */
  compoutputpolarity_t comp2_output_pol;


  /**
   * @brief   Comparator #1 Output Selection
   */
  comp1outputsel_t comp1_output_sel;
  /**
   * @brief   Comparator #2 Output Selection
   */
  comp2outputsel_t comp2_output_sel;

#if 0

>>>>**** NEED TO FILL IN WITH INTERRUPT CALLBACK ****<<<<<

  /**
   * @brief   Callback for pulse width measurement.
   */
  compcallback_t             width_cb;
#endif
} COMPConfig;

/**
 * @brief   Structure representing an COMP driver.
 */
struct COMPDriver {
  /**
   * @brief Driver state.
   */
  compstate_t                state;
  /**
   * @brief Current configuration data.
   */
  const COMPConfig           *config;
#if defined(COMP_DRIVER_EXT_FIELDS)
  COMP_DRIVER_EXT_FIELDS
#endif
  /* End of the mandatory fields.*/
  /**
   * @brief Pointer to the COMP register.
   */
  COMP_TypeDef               *comp;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Returns the width of the latest pulse.
 * @details The pulse width is defined as number of ticks between the start
 *          edge and the stop edge.
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 * @return              The number of ticks.
 *
 * @notapi
 */
#define comp_lld_get_width(compp) (*((compp)->wccrp) + 1)

/**
 * @brief   Returns the width of the latest cycle.
 * @details The cycle width is defined as number of ticks between a start
 *          edge and the next start edge.
 *
 * @param[in] compp      pointer to the @p COMPDriver object
 * @return              The number of ticks.
 *
 * @notapi
 */
#define comp_lld_get_period(compp) (*((compp)->pccrp) + 1)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if STM32_COMP_USE_COMP1 && !defined(__DOXYGEN__)
extern COMPDriver COMPD1;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void comp_lld_init(void);
  void comp_lld_start(COMPDriver *compp);
  void comp_lld_stop(COMPDriver *compp);
  bool_t comp_lld_comp1val(COMPDriver *compp);
  bool_t comp_lld_comp2val(COMPDriver *compp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_COMP */

#endif /* _COMP_LLD_H_ */

/** @} */
