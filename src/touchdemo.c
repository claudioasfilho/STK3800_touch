/**************************************************************************//**
 * @file
 * @brief Cap sense touch demo for EFM32WG_STK3800
 * @version 4.2.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Device Suppot Library */
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_lcd.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "em_usart.h"

/* Drivers */
#include "vddcheck.h"
#include "segmentlcd.h"
#include "caplesense.h"
#include "rtcdriver.h"
#include "bsp_trace.h"

#include "retargetserial.h"

#define DEMOMODES           3

#define DEMOMODE_SCROLLTEXT 0
#define DEMOMODE_BARS       1
#define DEMOMODE_VALUES     2

typedef enum
{
  DEMO_ERROR = -1,
  DEMO_SLEEP = 0,
  DEMO_SENSE = 1,
  DEMO_SLEEP_PREPARE = 2,
  DEMO_SENSE_PREPARE = 3
} DEMO_States_TypeDef;


static volatile DEMO_States_TypeDef demoState = DEMO_SLEEP_PREPARE;
volatile uint32_t demoMode;
volatile uint32_t subMode;
volatile uint32_t subModes;

volatile int msTicks; /* counts 1ms timeTicks */

const char *message = "CAPSENSE TOUCH DEMO  ";

/* Enable or disable vboost */
bool vboost = false;

/** Timer used for bringing the system into sleep. */
RTCDRV_TimerID_t xTimerForSleep;

/* Local Prototypes */
void gpioSetup(void);
void capSenseAringUpdate(int sliderPos);
void capSenseBars(void);
void capSenseDemo(void);
void capSenseValues(void);
void capSenseScrollText(void);
void capSenseTimerFired( RTCDRV_TimerID_t id, void *user);
void capSenseScanComplete(void);
void capSenseChTrigger(void);

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB9)
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 9);

  /* Set next demo */
  demoMode = DEMOMODE_VALUES;//(demoMode + 1) % DEMOMODES;

  /* Reset submode */
  subMode = 0;
  /* Set the maximum number of subModes depending on type */
  switch (demoMode)
  {
    case (DEMOMODE_VALUES):
      subModes = 4;
      break;
    default:
      subModes = 1;
      break;
  }

}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB10)
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint32_t max;

  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 10);

  /* Select next submode */
  max = subModes;
  subMode = (subMode + 1) % max;
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt to change demo mode
 *****************************************************************************/
void gpioSetup(void)
{
  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB9 and PB10 as input */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(gpioPortB, 9, false, true, true);
  GPIO_IntConfig(gpioPortB, 10, false, true, true);

  /* Enable interrupt in core for even and odd gpio interrupts */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void USART1Setup(void) {

	CMU_ClockEnable(cmuClock_USART1, true);

	// $[USART_InitAsync]
	USART_InitAsync_TypeDef initasync = USART_INITASYNC_DEFAULT;

	initasync.baudrate = 115200;
	initasync.databits = usartDatabits8;
	initasync.parity = usartNoParity;
	initasync.stopbits = usartStopbits1;
	initasync.oversampling = usartOVS16;
#if defined( USART_INPUT_RXPRS ) && defined( USART_CTRL_MVDIS )
	initasync.mvdis = 0;
	initasync.prsRxEnable = 0;
	initasync.prsRxCh = 0;
#endif

	USART_InitAsync(USART1, &initasync);
	// [USART_InitAsync]$

	// $[USART_InitSync]
	// [USART_InitSync]$

	// $[USART_InitPrsTrigger]
	USART_PrsTriggerInit_TypeDef initprs = USART_INITPRSTRIGGER_DEFAULT;

	initprs.rxTriggerEnable = 0;
	initprs.txTriggerEnable = 0;
	initprs.prsTriggerChannel = usartPrsTriggerCh0;

	USART_InitPrsTrigger(USART1, &initprs);
	// [USART_InitPrsTrigger]$

	/* Pin PD0 is configured to Push-pull */
	GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE0_MASK)
			| GPIO_P_MODEL_MODE0_PUSHPULL;
	// [Port D Configuration]$

	// $[Port E Configuration]
	// [Port E Configuration]$

	// $[Port F Configuration]
	// [Port F Configuration]$

	// $[Route Configuration]

	/* Module PCNT0 is configured to location 1 */
	PCNT0->ROUTE = (PCNT0->ROUTE & ~_PCNT_ROUTE_LOCATION_MASK)
			| PCNT_ROUTE_LOCATION_LOC1;

	/* Module UART1 is configured to location 3 */
	UART1->ROUTE = (UART1->ROUTE & ~_UART_ROUTE_LOCATION_MASK)
			| UART_ROUTE_LOCATION_LOC3;

	/* Module USART1 is configured to location 1 */
	USART1->ROUTE = (USART1->ROUTE & ~_USART_ROUTE_LOCATION_MASK)
			| USART_ROUTE_LOCATION_LOC1;

	/* Enable signals TX */
	USART1->ROUTE |= USART_ROUTE_TXPEN;
	// [Route Configuration]$

}


/**************************************************************************//**
 * @brief Update rings according to slider position
 * @par sliderPos The current Slider position
 *****************************************************************************/
void capSenseAringUpdate(int sliderPos)
{
  int i;
  int stop;

  if (sliderPos == -1)
  {
    /* No ring if touch slider is not touched */
    stop = -1;
  }
  else
  {
    /* Map 8 segments to 48 slider positions */
    stop = (sliderPos * 8) / 48;
  }

  /* Draw ring */
  for (i=0; i < 8; i++)
  {
    if (i <= stop )
    {
      SegmentLCD_ARing(i, 1);
    }
    else
    {
      SegmentLCD_ARing(i, 0);
    }
  }
}

/**************************************************************************//**
 * @brief Cap sense values demo
 *****************************************************************************/
void capSenseBars(void)
{
  int barNum;
  int sliderPos;
  const char bars[3] = {'{', '!', '1' };
  char msg[8];

  sliderPos = CAPLESENSE_getSliderPosition();
  SegmentLCD_Number(sliderPos);

  if (sliderPos == -1)
  {
    SegmentLCD_Write("SLIDER");
  }
  else
  {
    /* Clear the msg string */
    snprintf(msg, 7, "         ");
    /* There are 21 possible "bars" on the display, while there are 48 slider
     * positions. This maps these 48 into 21 slider positions. */
    barNum = (sliderPos * 21) / 48;

    msg[barNum / 3] = bars[barNum %3];

    SegmentLCD_Write(msg);
  }
}

/**************************************************************************//**
 * @brief Cap sense values demo
 *****************************************************************************/
void capSenseValues(void)
{
  char msg[8];
  uint8_t channel;
  uint32_t cap_value;

  switch (subMode)
  {
  case(0):
    snprintf(msg, 7, "Pad 0");
    channel = CAPLESENSE_getSegmentChannel(0);
    break;
  case(1):
    snprintf(msg, 7, "Pad 1");
    channel = CAPLESENSE_getSegmentChannel(1);
    break;
  case(2):
    snprintf(msg, 7, "Pad 2");
    channel = CAPLESENSE_getSegmentChannel(2);
    break;
  case(3):
    snprintf(msg, 7, "Pad 3");
    channel = CAPLESENSE_getSegmentChannel(3);
    break;
  default:
    snprintf(msg, 7, "Pad 0");
    channel = CAPLESENSE_getSegmentChannel(0);
    break;
  }
  SegmentLCD_Write(msg);
  cap_value = CAPLESENSE_getNormalizedVal(channel);
  SegmentLCD_Number(cap_value);
  Print_Touch(&cap_value);

}

/**************************************************************************//**
 * @brief  ScrollText demo
 *****************************************************************************/
void capSenseScrollText(void)
{
  char msg[10];
  int sliderPos;
  static int oldSliderPos = -2;

  int offset;

  sliderPos = CAPLESENSE_getSliderPosition();
  if (oldSliderPos != sliderPos)
  {
    oldSliderPos = sliderPos;
    SegmentLCD_Number(sliderPos);

    if (sliderPos == -1)
      sliderPos = 0;

    offset = ((strlen(message) - 7) * sliderPos) / 48;

    snprintf(msg, 8, "%s", message + offset);
    SegmentLCD_Write(msg);
  }
}

/**************************************************************************//**
 * @brief  Capsense demo loop
 *****************************************************************************/
void capSenseDemo(void)
{
  int32_t slider;
  bool oldBoost = vboost;

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForSleep);

  /* Setup capSense callbacks. */
  CAPLESENSE_setupCallbacks(&capSenseScanComplete, &capSenseChTrigger);

  /* Main loop */
  while (1)
  {

    switch(demoState)
    {
      case DEMO_SLEEP_PREPARE:
      {
        /* Setup LESENSE in sleep mode. */
        CAPLESENSE_setupLESENSE(true);
        /* Disable LCD to avoid excessive current consumption */
        SegmentLCD_Disable();
        /* Disable Vdd check. */
        VDDCHECK_Disable();
        /* Go to sleep state. */
        demoState = DEMO_SLEEP;
      }
      break;

      case DEMO_SLEEP:
      {
        /* Go to sleep and wait until the measurement completes. */
        CAPLESENSE_Sleep();
      }
      break;

      case DEMO_SENSE_PREPARE:
      {
        /* Setup LESENSE in high-accuracy sense mode. */
        CAPLESENSE_setupLESENSE(false);
        /* Enable vboost */
        SegmentLCD_Init(vboost);
        /* Go to sense state. */
        demoState = DEMO_SENSE;
      }
      break;

      case DEMO_SENSE:
      {
        /* Go to sleep and wait until the measurement completes. */
        CAPLESENSE_Sleep();

        /* Get slider position. */
        slider = CAPLESENSE_getSliderPosition();
        if (-1 != slider)
        {
          RTCDRV_StartTimer( xTimerForSleep, rtcdrvTimerTypeOneshot, 1000, capSenseTimerFired, NULL);
        }
        capSenseAringUpdate(slider);

        /* Check for change in input voltage. Enable vboost if necessary */
        /* Initialize voltage comparator */
        VDDCHECK_Init();

        /* Check if voltage is below 3V, if so use voltage boost */
        if (VDDCHECK_LowVoltage(2.9))
        {
          vboost = true;
          if (oldBoost != vboost)
          {
            /* Enable vboost */
            SegmentLCD_Init(vboost);
            /* Use antenna symbol to signify enabling of vboost */
            SegmentLCD_Symbol(LCD_SYMBOL_ANT, vboost);
          }
          oldBoost = vboost;
        }
        else
        {
          vboost = false;
        }

        switch (demoMode)
        {
        case (DEMOMODE_VALUES):
          capSenseValues();
          break;
        case (DEMOMODE_SCROLLTEXT):
          capSenseScrollText();
          break;
        case (DEMOMODE_BARS):
          capSenseBars();
          break;

        default:  capSenseValues();
                  break;
          break;
        }
      }
      break;

      default:
      {
        ;
      }
      break;
    }
  }
}


/**************************************************************************//**
 * @brief  Callback for timer overflow.
 *****************************************************************************/
void capSenseTimerFired( RTCDRV_TimerID_t id, void *user)
{
  ( void)id;
  ( void)user;
  demoState = DEMO_SLEEP_PREPARE;
}

/**************************************************************************//**
 * @brief  Callback for sensor scan complete.
 *****************************************************************************/
void capSenseScanComplete(void)
{
  ;
}

/**************************************************************************//**
 * @brief  Callback for sensor channel triggered.
 *****************************************************************************/
void capSenseChTrigger(void)
{
  demoState = DEMO_SENSE_PREPARE;
}

/**************************************************************************//**
 * @brief  It Transmits on USART1 whatever it is on the string buffer
 *****************************************************************************/
static char string[32];

void USART_TxString()
{

	  for (int i=0; i<32;i++)
	  {
	    USART_Tx(USART1, string[i]);
	  }
}


/**************************************************************************//**
 * @brief  It Transmits on USART1 whatever it is on the string buffer
 *****************************************************************************/

void Init_TxString()
{
	  for (int i=0; i<sizeof(string);i++)
	 	  {
	 	    string[i]=0;
	 	  }
}


void Print_Touch(uint32_t *random)
{
	  sprintf(string,"Touch:%d\n\r",*random);
	  USART_TxString();
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  /* Initialize capsense */
  CAPLESENSE_Init(true);



  /* Initialize GPIO interrupts */
  gpioSetup();

  USART1Setup();
  /* Start capsense demo. */


  capSenseDemo();

  return 0;
}
