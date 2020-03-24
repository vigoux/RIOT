/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dsl
 * @{
 *
 * @file
 * @brief       Internal configuration for LSM6DSO devices
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 */

#ifndef LSM6DSO_INTERNAL_H
#define LSM6DSO_INTERNAL_H

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LSM6DSO registers
 * @{
 */
#define LSM6DSO_REG_FUNC_CFG_ACCESS         (0x01)
#define LSM6DSO_REG_PIN_CTRL         (0x02)
/* RESERVED */
#define LSM6DSO_REG_FIFO_CTRL1              (0x07)
#define LSM6DSO_REG_FIFO_CTRL2              (0x08)
#define LSM6DSO_REG_FIFO_CTRL3              (0x09)
#define LSM6DSO_REG_FIFO_CTRL4              (0x0A)
#define LSM6DSO_REG_COUNTER_BDR_REG1 (0x0B)
#define LSM6DSO_REG_COUNTER_BDR_REG2 (0x0C)
#define LSM6DSO_REG_INT1_CTRL               (0x0D)
#define LSM6DSO_REG_INT2_CTRL               (0x0E)
#define LSM6DSO_REG_WHO_AM_I                (0x0F)
#define LSM6DSO_REG_CTRL1_XL                (0x10)
#define LSM6DSO_REG_CTRL2_G                 (0x11)
#define LSM6DSO_REG_CTRL3_C                 (0x12)
#define LSM6DSO_REG_CTRL4_C                 (0x13)
#define LSM6DSO_REG_CTRL5_C                 (0x14)
#define LSM6DSO_REG_CTRL6_C                 (0x15)
#define LSM6DSO_REG_CTRL7_G                 (0x16)
#define LSM6DSO_REG_CTRL8_XL                (0x17)
#define LSM6DSO_REG_CTRL9_XL                (0x18)
#define LSM6DSO_REG_CTRL10_C                (0x19)
#define LSM6DSO_REG_ALL_INT_SRC (0x1A)
#define LSM6DSO_REG_WAKE_UP_SRC             (0x1B)
#define LSM6DSO_REG_TAP_SRC                 (0x1C)
#define LSM6DSO_REG_D6D_SRC                 (0x1D)
#define LSM6DSO_REG_STATUS_REG              (0x1E)
#define LSM6DSO_REG_OUT_TEMP_L              (0x20)
#define LSM6DSO_REG_OUT_TEMP_H              (0x21)
/* RESERVED */
#define LSM6DSO_REG_OUTX_L_G                (0x22)
#define LSM6DSO_REG_OUTX_H_G                (0x23)
#define LSM6DSO_REG_OUTY_L_G                (0x24)
#define LSM6DSO_REG_OUTY_H_G                (0x25)
#define LSM6DSO_REG_OUTZ_L_G                (0x26)
#define LSM6DSO_REG_OUTZ_H_G                (0x27)

#define LSM6DSO_REG_OUTX_L_A               (0x28)
#define LSM6DSO_REG_OUTX_H_A               (0x29)
#define LSM6DSO_REG_OUTY_L_A               (0x2A)
#define LSM6DSO_REG_OUTY_H_A               (0x2B)
#define LSM6DSO_REG_OUTZ_L_A               (0x2C)
#define LSM6DSO_REG_OUTZ_H_A               (0x2D)
/* RESERVED */
#define LSM6DSO_REG_EMB_FUNC_STATUS_MAINPAGE (0x35)
#define LSM6DSO_REG_FSM_STATUS_A_MAINPAGE (0x36)
#define LSM6DSO_REG_FSM_STATUS_B_MAINPAGE (0x37)
/* RESERVED */
#define LSM6DSO_REG_STATUS_MASTER_MAINPAGE (0x39)
#define LSM6DSO_REG_FIFO_STATUS1            (0x3A)
#define LSM6DSO_REG_FIFO_STATUS2            (0x3B)
/* RESERVED */
#define LSM6DSO_REG_TIMESTAMP0          (0x40)
#define LSM6DSO_REG_TIMESTAMP1          (0x41)
#define LSM6DSO_REG_TIMESTAMP2          (0x42)
#define LSM6DSO_REG_TIMESTAMP3 (0x43)
/* RESERVED */
#define LSM6DSO_REG_TAP_CFG0 (0x56)
#define LSM6DSO_REG_TAP_CFG1 (0x57)
#define LSM6DSO_REG_TAP_CFG2 (0x58)
#define LSM6DSO_REG_TAP_THS_6D              (0x59)
#define LSM6DSO_REG_INT_DUR2                (0x5A)
#define LSM6DSO_REG_WAKE_UP_THS             (0x5B)
#define LSM6DSO_REG_WAKE_UP_DUR             (0x5C)
#define LSM6DSO_REG_FREE_FALL               (0x5D)
#define LSM6DSO_REG_MD1_CFG                 (0x5E)
#define LSM6DSO_REG_MD2_CFG                 (0x5F)
/* RESERVED */
#define LSM6DSO_REG_I3C_BUS_AVB (0x62)
#define LSM6DSO_REG_INTERNAL_FREQ_FINE (0x63)
/* RESERVED */
#define LSM6DSO_REG_INT_OIS (0x6F)
#define LSM6DSO_REG_CTRL1_OIS (0x70)
#define LSM6DSO_REG_CTRL2_OIS (0x71)
#define LSM6DSO_REG_CTRL3_OIS (0x72)
#define LSM6DSO_REG_X_OFS_USR (0x73)
/** @} */

/** WHO_AM_I value */
#define LSM6DSO_WHO_AM_I                    (0b01101000)

/**
 * @name    CTRL_x registers
 * @{
 */
#define LSM6DSO_CTRL_ODR_SHIFT              (4)
#define LSM6DSO_CTRL_ODR_MASK               (0xF0)
#define LSM6DSO_CTRL_FS_SHIFT               (2)
#define LSM6DSO_CTRL_FS_MASK                (0x0C)

#define LSM6DSO_CTRL3_C_BOOT                (0x80)
/** @} */

/**
 * @name    FIFO_CTRL_x registers
 * @{
 */
#define LSM6DSO_FIFO_CTRL4_CONTINUOUS_MODE  (0x6)
#define LSM6DSO_FIFO_CTRL4_FIFO_ODR_SHIFT   (4)

// TODO
/* #define LSM6DSO_FIFO_CTRL4_DEC_SHIFT   (6) */
/** @} */

/**
 * @brief	Offset for temperature calculation
 */
#define LSM6DSO_TEMP_OFFSET                 (0x1900)
#define LSM6DSO_TEMP_SENSITIVITY (0x100)

/**
 * @brief 	Reboot wait interval in us (15ms)
 */
#define LSM6DSO_BOOT_WAIT                   (15 * US_PER_MS)

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSO_INTERNAL_H */
/** @} */
