/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    drivers_lsm6dso LSM6DSO 3D accelerometer/gyroscope
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the LSM6DSO 3D accelerometer/gyroscope
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the LSM6DSO 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 */

#ifndef LSM6DSO_H
#define LSM6DSO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"

/**
 * @brief   Data rate settings
 */
enum {
    LSM6DSO_DATA_RATE_POWER_DOWN = 0x0,
    LSM6DSO_DATA_RATE_1_6HZ      = 0xB,
    LSM6DSO_DATA_RATE_12_5HZ     = 0x1,
    LSM6DSO_DATA_RATE_26HZ       = 0x2,
    LSM6DSO_DATA_RATE_52HZ       = 0x3,
    LSM6DSO_DATA_RATE_104HZ      = 0x4,
    LSM6DSO_DATA_RATE_208HZ      = 0x5,
    LSM6DSO_DATA_RATE_416HZ      = 0x6,
    LSM6DSO_DATA_RATE_833HZ      = 0x7,
    LSM6DSO_DATA_RATE_1_66KHZ    = 0x8,
    LSM6DSO_DATA_RATE_3_33KHZ    = 0x9,
    LSM6DSO_DATA_RATE_6_66KHZ    = 0xa,
};

/**
 * @brief   Decimation settings
 */
enum {
    LSM6DSO_DECIMATION_NO = 0,
    LSM6DSO_DECIMATION_1,
    LSM6DSO_DECIMATION_8,
    LSM6DSO_DECIMATION_32,
};

/**
 * @brief   Accelerometer full scale
 */
enum {
    LSM6DSO_ACC_FS_2G  = 0,
    LSM6DSO_ACC_FS_16G,
    LSM6DSO_ACC_FS_4G,
    LSM6DSO_ACC_FS_8G,
    LSM6DSO_ACC_FS_MAX,
};

/**
 * @brief   Gyroscope full scale
 */
enum {
    LSM6DSO_GYRO_FS_245DPS    = 0,
    LSM6DSO_GYRO_FS_500DPS,
    LSM6DSO_GYRO_FS_1000DPS,
    LSM6DSO_GYRO_FS_2000DPS,
    LSM6DSO_GYRO_FS_MAX,
};

/**
 * @brief   LSM6DSO driver parameters
 */
typedef struct {
    i2c_t i2c;                  /**< i2c bus */
    uint8_t addr;               /**< i2c address */
    uint8_t acc_odr;            /**< accelerometer output data rate */
    uint8_t gyro_odr;           /**< gyroscope output data rate */
    uint8_t acc_fs;             /**< accelerometer full scale */
    uint8_t gyro_fs;            /**< gyroscope full scale */
    uint8_t acc_decimation;     /**< accelerometer decimation */
    uint8_t gyro_decimation;    /**< gyroscope decimation */
} lsm6dso_params_t;

/**
 * @brief   LSM6DSO device descriptor
 */
typedef struct {
    lsm6dso_params_t params; /**< driver parameters */
} lsm6dso_t;

/**
 * @brief   3D output data
 */
typedef struct {
    int16_t x;  /**< X axis */
    int16_t y;  /**< Y axis */
    int16_t z;  /**< Z axis */
} lsm6dso_3d_data_t;

/**
 * @brief   Named return values
 */
enum {
    LSM6DSO_OK = 0,             /**< all good */
    LSM6DSO_ERROR_BUS,          /**< I2C bus error */
    LSM6DSO_ERROR_CNF,          /**< Config error */
    LSM6DSO_ERROR_DEV,          /**< device error */
};

/**
 * @brief   Initialize a LSM6DSO device
 *
 * @param[out] dev     device to initialize
 * @param[in] params  driver parameters
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_init(lsm6dso_t *dev, const lsm6dso_params_t *params);

/**
 * @brief   Read accelerometer data
 *
 * @param[in] dev    device to read
 * @param[out] data  accelerometer values
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_read_acc(const lsm6dso_t *dev, lsm6dso_3d_data_t *data);

/**
 * @brief   Read gyroscope data
 *
 * @param[in] dev    device to read
 * @param[out] data  gyroscope values
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_read_gyro(const lsm6dso_t *dev, lsm6dso_3d_data_t *data);

/**
 * @brief   Read temperature data
 *
 * @note To avoid floating point data types but still provide high resolution
 *       for temperature readings, resulting values are scale by factor 100.
 *
 *
 * @param[in] dev    device to read
 * @param[out] data  temperature value, in °C x 100
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_read_temp(const lsm6dso_t *dev, int16_t *data);

/**
 * @brief   Power down accelerometer
 *
 * @param[in] dev    device to power down
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_acc_power_down(const lsm6dso_t *dev);

/**
 * @brief   Power down gyroscope
 *
 * @param[in] dev    device to power down
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_gyro_power_down(const lsm6dso_t *dev);

/**
 * @brief   Power up accelerometer
 *
 * @param[in] dev    device to power up
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_acc_power_up(const lsm6dso_t *dev);

/**
 * @brief   Power up gyroscope
 *
 * @param[in] dev    device to power up
 *
 * @return LSM6DSO_OK on success
 * @return < 0 on error
 */
int lsm6dso_gyro_power_up(const lsm6dso_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSO_H */
/** @} */
