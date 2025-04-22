/*
 * AIS25BA_TDM.h
 *
 *  Created on: 2023. 4. 21.
 *      Author: gvlko
 */

#ifndef __AIS25BA_TDM_H__
#define __AIS25BA_TDM_H__

#include "main.h"

#define _Z_AXIS_EN            0x80
#define _Y_AXIS_EN            0x40
#define _X_AXIS_EN            0x20
#define _ODR_AUTO_EN          0x01

#define _AIS25BA_ADD          0x19

#define TDM_cmax_HI_value     0x00
#define TDM_cmax_LO_value     0x7f
#define CTRL_REG_value        0x00

#define TDM_CTRL_REG_value    0x04

#define AXES_CTRL_REG_value   0xE0

#define CTRL_REG_FS_value     0x00

#define AUDIO_FRAME_SIZE      6

#define _X_AXIS               0
#define _Y_AXIS               1
#define _Z_AXIS               2

#define _STUCK_LIMIT_CNT      3

#define TDM_ACCELO_AXIS       3
#define TDM_DATA_SIZE         (1024 * TDM_ACCELO_AXIS)

extern I2C_HandleTypeDef hi2c1;
/*
 *
 *  INITIALIZATION
 *
 */
uint8_t AIS25BA_Init(void);

/*
 *
 * LOW-LEVEL REGISTER FUNCTIONS
 *
 */
uint8_t AIS25BA_I2C_Write(uint8_t *pData, uint16_t Size);
uint8_t AIS25BA_I2C_Read(uint8_t *pData, uint16_t Size);

#endif
