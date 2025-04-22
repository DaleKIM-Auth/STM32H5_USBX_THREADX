/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_threadx.h"
#include "ais25ba.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

UX_SLAVE_CLASS_CDC_ACM  *cdc_acm;

extern TX_THREAD txTDMReceiveThread;
extern TX_SEMAPHORE txCDCSemaphore;
extern volatile uint16_t *outBuffer;

UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER CDC_VCP_LineCoding =
{
  115200, /* baud rate */
  0x0,    /* stop bits - 1 */
  0x0,    /* parity - none */
  0x8     /* nb. of bits - 8 */
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  USBD_CDC_ACM_Activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Activate */
  cdc_acm = (UX_SLAVE_CLASS_CDC_ACM*) cdc_acm_instance;

  if (ux_device_class_cdc_acm_ioctl(cdc_acm, UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING,
                                    &CDC_VCP_LineCoding) != UX_SUCCESS) {
    Error_Handler();
  }
  /* USER CODE END USBD_CDC_ACM_Activate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_Deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  
  /* Reset the cdc acm instance */
  cdc_acm = UX_NULL;
  /* USER CODE END USBD_CDC_ACM_Deactivate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_ParameterChange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  /* Get the pointer to the device.  */

  /* USER CODE END USBD_CDC_ACM_ParameterChange */

  return;
}

/* USER CODE BEGIN 1 */
/**
 * @brief  Function implementing usbx_cdc_acm_thread_entry.
 * @param  thread_input: Not used
 * @retval none
 */
VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input)
{
  /* Private Variables */
  ULONG tx_actual_length;
  UX_SLAVE_DEVICE *device;
  
  UX_PARAMETER_NOT_USED(thread_input);

  device = &_ux_system_slave->ux_system_slave_device;  
  
  while(1) {
    if ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (cdc_acm != UX_NULL)) {
      tx_semaphore_get(&txCDCSemaphore, TX_WAIT_FOREVER);
      ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *)outBuffer, TDM_DATA_SIZE, &tx_actual_length);
    }
  }
}
 
/**
 * @brief  Function implementing usbx_cdc_acm_write_thread_entry.
 * @param  thread_input: Not used
 * @retval none
 */
VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input)
{
  /* Private Variables */
  ULONG rx_actual_length;
  uint8_t UserRxBuffer[64];
  UX_SLAVE_DEVICE *device;

  UX_PARAMETER_NOT_USED(thread_input);

  device = &_ux_system_slave->ux_system_slave_device;
  
  /* Infinite Loop */
  while(1) {
    if ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (cdc_acm != UX_NULL)) {
      ux_device_class_cdc_acm_read(cdc_acm, (UCHAR *)UserRxBuffer, 64, &rx_actual_length);
      if (UserRxBuffer[rx_actual_length - 1] == 0x7F){
        tx_thread_resume(&txTDMReceiveThread);        
      }
    }
  }
}
 
/* USER CODE END 1 */
