/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"
#include "shell.h"
#include "l3gd20.h"

#include "usbcfg.h"

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)


static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {THD_STATE_NAMES};
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state time\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
            (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
            (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
            states[tp->p_state], (uint32_t)tp->p_time);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}


static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: test\r\n");
    return;
  }
  tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
                           TestThread, chp);
  if (tp == NULL) {
    chprintf(chp, "out of memory\r\n");
    return;
  }
  chThdWait(tp);
}

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};

/*===========================================================================*/
/* Accelerometer related.                                                    */
/*===========================================================================*/






static uint8_t ctrl_reg_values_buffer[14];
static uint8_t ctrl_reg1_config=0x0F;
static int16_t tmp_x[4],tmp_y[4],tmp_z[4];
static int16_t x_value[60],y_value[60],z_value[60];
unsigned i=0,j,lednumb;
static msg_t message=0;


/*
*Configuration @ max frequency according to MEMS
*Configuration (9 MHz, CPHA=1, CPOL=1, MSb first, 8bit word).
*/
static const SPIConfig std_spicfg = {
  NULL,
  GPIOE,                                                        /*  port of CS*/
  GPIOE_SPI1_CS,                                                /*   pin of CS*/
  SPI_CR1_BR_0|SPI_CR1_CPOL|SPI_CR1_CPHA,                       /*CR1 register*/
  SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0                    /*CR2 register*/
};


/*
* This Thread reads values from more than one register.
*/
static msg_t SPID1_Thread(void *arg) {
  (void)arg;
  chRegSetThreadName ("SPI Drive 1 thread");
  while (TRUE){


    /*
     * Cyclic implementation of data buffer
     */
    if (i==60){
      i=0;
    }


    /*
     * SPI bus acquiring and configuring
     */
    spiAcquireBus(&SPID1);
    spiStart(&SPID1, &std_spicfg);


    /*
     * Configuration of control register number 1 of L3GD20
     */
    l3gd20WriteRegister(&SPID1, L3GD20_SPI_AD_CTRL_REG1, ctrl_reg1_config);


    /*
     * Sequential read of all registers not reserved
     */
    for(j=0;j<14;j++){
      ctrl_reg_values_buffer[j]=l3gd20ReadRegister(&SPID1, L3GD20_SPI_AD_CTRL_REG1+j);
    }
    lednumb=8;


    /*
     * Reading of values from axis registers. This cycle reads 4 values per axis
     * in 1 second and puts they in a bi-dimensional buffer. It also turns on two LEDs for
     * each set of three values (one per axis) captured.
     */
    for(j=0;j<4;j++){
      tmp_x[j]=l3gd20ReadAxis (&SPID1, L3GD20_SPI_AD_OUT_X_L);
      tmp_y[j]=l3gd20ReadAxis (&SPID1, L3GD20_SPI_AD_OUT_Y_L);
      tmp_z[j]=l3gd20ReadAxis (&SPID1, L3GD20_SPI_AD_OUT_Z_L);
      palSetPad(GPIOE,lednumb);
      palSetPad(GPIOE,lednumb+4);
      chThdSleepMilliseconds(250);
      lednumb++;
    }


    /*
     * Turning off all LEDs
     */
    for(lednumb=8;lednumb<16;lednumb++){
      palClearPad(GPIOE,lednumb);
    }


    /*
     * Releasing SPI bus
     */
    spiReleaseBus (&SPID1);


    /*
     * Calculating the average from values previously acquired
     */
    x_value[i]=(tmp_x[0] + tmp_x[1] + tmp_x[2] + tmp_x[3])/4;
    y_value[i]=(tmp_y[0] + tmp_y[1] + tmp_y[2] + tmp_y[3])/4;
    z_value[i]=(tmp_z[0] + tmp_z[1] + tmp_z[2] + tmp_z[3])/4;
    i++;
  }
  return message;
}


/*
* Declaration of the working areas of threads
*/
static WORKING_AREA(waSPID1_Thread, 512);


/*
* Application entry point.
*/
int main(void) {


  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();


  /*
   * SPID1 I/O pins setup.(It bypasses board.h configurations)
   */
  palSetPadMode(GPIOA, GPIOA_SPI1_SCK ,
                PAL_MODE_ALTERNATE(5)|PAL_STM32_OSPEED_HIGHEST);   /* New SCK */
  palSetPadMode(GPIOA, GPIOA_SPI1_MISO ,
                PAL_MODE_ALTERNATE(5)|PAL_STM32_OSPEED_HIGHEST);   /* New MISO*/
  palSetPadMode(GPIOA, GPIOA_SPI1_MOSI ,
                PAL_MODE_ALTERNATE(5)|PAL_STM32_OSPEED_HIGHEST);   /* New MOSI*/
  palSetPadMode(GPIOE, GPIOE_SPI1_CS ,
                PAL_MODE_OUTPUT_PUSHPULL|PAL_STM32_OSPEED_HIGHEST);/* New CS  */


  /*
   * Starting the transmitter-receiver thread.
   */
  chThdCreateStatic(waSPID1_Thread, sizeof(waSPID1_Thread),
                    NORMALPRIO+1, SPID1_Thread, NULL);


  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
  return 0;
}
