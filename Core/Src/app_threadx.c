/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ais25ba.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define BUFFER_SWAP    1
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD tx_app_thread;
/* USER CODE BEGIN PV */

uint16_t tdmBuffer1[TDM_DATA_SIZE];
uint16_t tdmBuffer2[TDM_DATA_SIZE];

volatile uint16_t *inBuffer = tdmBuffer1;
volatile uint16_t *outBuffer = NULL;
volatile uint32_t bToggle;

TX_THREAD txTDMReceiveThread;
TX_SEMAPHORE txTDMSemaphore;
TX_SEMAPHORE txCDCSemaphore;
TX_QUEUE  txTDMQueue;

extern SAI_HandleTypeDef hsai_BlockA1;
extern SAI_HandleTypeDef hsai_BlockA2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */
  CHAR *pointer;

  /* Allocate the stack for tx app thread  */
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* Create tx app thread.  */
  if (tx_thread_create(&tx_app_thread, "tx app thread", tx_app_thread_entry, 0, pointer,
                       TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                       TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN App_ThreadX_Init */
  /* Create TDM data receive thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, TX_TDM_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
    return TX_POOL_ERROR;
  }

  if (tx_thread_create(&txTDMReceiveThread, "tx tdm receive thread", txTDMReceiveThreadEntry, 0, pointer,
                       TX_TDM_THREAD_STACK_SIZE, TX_TDM_THREAD_PRIO, TX_TDM_THREAD_PREEMPTION_THRESHOLD,
                       TX_NO_TIME_SLICE, TX_DONT_START) != TX_SUCCESS) {
    return TX_THREAD_ERROR;
  }

  if (tx_semaphore_create(&txTDMSemaphore, "tdm semaphore", 1) != TX_SUCCESS){
    return TX_SEMAPHORE_ERROR;
  }

  if (tx_semaphore_create(&txCDCSemaphore, "usb cdc semaphore", 0) != TX_SUCCESS){
    return TX_SEMAPHORE_ERROR;
  }
  /* USER CODE END App_ThreadX_Init */

  return ret;
}
/**
  * @brief  Function implementing the tx_app_thread_entry thread.
  * @param  thread_input: Hardcoded to 0.
  * @retval None
  */
void tx_app_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_app_thread_entry */
  TX_THREAD_NOT_USED(thread_input);
  
  for(;;) {
    tx_thread_sleep(1);
  }
  /* USER CODE END tx_app_thread_entry */
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
void txTDMReceiveThreadEntry(ULONG thread_input)
{
  TX_THREAD_NOT_USED(thread_input);
  
  for(;;) {    
    tx_semaphore_get(&txTDMSemaphore, TX_WAIT_FOREVER);
    
    if (HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)inBuffer, TDM_DATA_SIZE) != HAL_OK){
      /* Need to add Error handler */
      Error_Handler();
    } 
    //tx_thread_sleep(1);
  }
}

/**
  * @brief Rx Transfer half completed callback.
  * @param  hsai pointer to a SAI_HandleTypeDef structure that contains
  *              the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* toggling buffer */
  bToggle ^= BUFFER_SWAP;
  
  /* swap buffer */
  if(bToggle == 0){
    inBuffer = tdmBuffer1;
    outBuffer = tdmBuffer2;
  } else {
    inBuffer = tdmBuffer2;
    outBuffer = tdmBuffer1;
  }
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hsai pointer to a SAI_HandleTypeDef structure that contains
  *              the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  tx_semaphore_put(&txTDMSemaphore);
  tx_semaphore_put(&txCDCSemaphore);
}
/* USER CODE END 1 */
