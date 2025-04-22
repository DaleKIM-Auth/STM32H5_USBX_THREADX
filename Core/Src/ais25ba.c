/*
 * AIS25BA.c
 *
 *  Created on: 2023. 4. 21.
 *      Author: gvlko
 */

#include "ais25ba.h"

/* Write data to I2C device */
uint8_t AIS25BA_I2C_Write(uint8_t *pData, uint16_t Size)
{
  if (HAL_I2C_Master_Transmit(&hi2c1, _AIS25BA_ADD<<1, pData, Size, HAL_MAX_DELAY) != HAL_OK)
  {
	  return 0;
  }

  return 1;
}

/* Read data from I2C device */
uint8_t AIS25BA_I2C_Read(uint8_t *pData, uint16_t Size)
{
  if (HAL_I2C_Master_Receive(&hi2c1, _AIS25BA_ADD<<1, pData, Size, HAL_MAX_DELAY) != HAL_OK)
  {
	  return 0;
  }

  return 1;
}

uint8_t tx_data[2] = {0x0, };
uint8_t rx_data[2] = {0x0, };
  
uint8_t InitAIS25BA(void)
{
  uint8_t status = 0;

  tx_data[0] = 0x26;	//register Address
  tx_data[1] = CTRL_REG_value;
  status += AIS25BA_I2C_Write(tx_data, 2);
  HAL_Delay(1);

  tx_data[0] = 0x25;	//register Address
  tx_data[1] = TDM_cmax_LO_value;
  status += AIS25BA_I2C_Write(tx_data, 2);
  HAL_Delay(1);

  tx_data[0] = 0x2E;	//register Address
  tx_data[1] = TDM_CTRL_REG_value;
  status += AIS25BA_I2C_Write(tx_data, 2);
  HAL_Delay(1);

  tx_data[0] = 0x2F;	//register Address
  tx_data[1] = AXES_CTRL_REG_value;
  status += AIS25BA_I2C_Write(tx_data, 2);
  HAL_Delay(1);

  tx_data[0] = 0x30;	//register Address
  tx_data[1] = CTRL_REG_FS_value;
  status += AIS25BA_I2C_Write(tx_data, 2);
  HAL_Delay(1);

  tx_data[0] = 0x26;	//register Address
  status += AIS25BA_I2C_Write(tx_data, 1);
  status += AIS25BA_I2C_Read(rx_data, 1);

  if(rx_data[0] != 0x00)	
    HAL_Delay(1);

  tx_data[0] = 0x2E;	//register Address
  status += AIS25BA_I2C_Write(tx_data, 1);
  status += AIS25BA_I2C_Read(rx_data, 1);

  return status;
}

uint8_t AIS25BA_Init(void)
{
  uint8_t status;

	status = InitAIS25BA(); // initialize the AI225BA through I2C;

	return status;
}

