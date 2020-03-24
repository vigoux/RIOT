/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dso
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM6DSO 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "xtimer.h"

#include "lsm6dso.h"
#include "lsm6dso_internal.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BUS         (dev->params.i2c)
#define ADDR        (dev->params.addr)

/**
 * order in array [0, 1, 2, 3] is
 * LSM6DSO_ACC_FS_2G, LSM6DSL_ACC_FS_16G, LSM6DSL_ACC_FS_4G, LSM6DSL_ACC_FS_8G
 */
static const int16_t range_acc[] = { 2000, 16000, 4000, 8000 };

/**
 * order in array [0, 1, 2, 3] is
 * LSM6DSO_GYRO_FS_245DPS, LSM6DSL_GYRO_FS_500DPS,
 * LSM6DSO_GYRO_FS_1000DPS, LSM6DSL_GYRO_FS_2000DPS
 */
static const int16_t range_gyro[] = { 2450, 5000, 10000, 20000 };

int lsm6dso_init(lsm6dso_t *dev, const lsm6dso_params_t *params)
{
    uint8_t tmp;
    int res;

    assert(dev && params);

    dev->params = *params;

    i2c_acquire(BUS);

    /* Reboot */
    i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL3_C, LSM6DSO_CTRL3_C_BOOT, 0);

    xtimer_usleep(LSM6DSO_BOOT_WAIT);

    if (i2c_read_reg(BUS, ADDR, LSM6DSO_REG_WHO_AM_I, &tmp, 0) < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dso_init: i2c_read_reg LSM6DSO_REG_WHO_AM_I!\n");
        return -LSM6DSO_ERROR_BUS;
    }

    if (tmp != LSM6DSO_WHO_AM_I) {
		i2c_release(BUS);
        DEBUG("[ERROR] lsm6dso_init: WHO_AM_I : %d, exp: %d.\n", tmp, LSM6DSO_WHO_AM_I);
        return -LSM6DSO_ERROR_DEV;
    }

    /* Set acc odr / full scale */
    tmp = (dev->params.acc_odr << LSM6DSO_CTRL_ODR_SHIFT) |
          (dev->params.acc_fs << LSM6DSO_CTRL_FS_SHIFT);
    res = i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL1_XL, tmp, 0);

    /* Set gyro odr / full scale */
    tmp = (dev->params.gyro_odr << LSM6DSO_CTRL_ODR_SHIFT) |
          (dev->params.gyro_fs << LSM6DSO_CTRL_FS_SHIFT);
    res += i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL2_G, tmp, 0);

    /* Set continuous mode */
    uint8_t fifo_odr = MAX(dev->params.acc_odr, dev->params.gyro_odr);
    tmp = (fifo_odr << LSM6DSO_FIFO_CTRL4_FIFO_ODR_SHIFT) |
          LSM6DSO_FIFO_CTRL4_CONTINUOUS_MODE;

	// TODO
    /* tmp = (dev->params.gyro_decimation << LSM6DSO_FIFO_CTRL4_GYRO_DEC_SHIFT) | */
    /*       dev->params.acc_decimation; */

    /* res += i2c_write_reg(BUS, ADDR, LSM6DSO_REG_FIFO_CTRL4, tmp, 0); */

    res += i2c_write_reg(BUS, ADDR, LSM6DSO_REG_FIFO_CTRL4, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_init: config\n");
        return -LSM6DSO_ERROR_CNF;
    }
    return LSM6DSO_OK;
}

int lsm6dso_read_acc(const lsm6dso_t *dev, lsm6dso_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, LSM6DSO_REG_STATUS_REG, &tmp, 0);
    DEBUG("lsm6dso status: %x\n", tmp);

    res = i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTX_L_A, &tmp, 0);
    data->x = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTX_H_A, &tmp, 0);
    data->x |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTY_L_A, &tmp, 0);
    data->y = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTY_H_A, &tmp, 0);
    data->y |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTZ_L_A, &tmp, 0);
    data->z = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTZ_H_A, &tmp, 0);
    data->z |= tmp << 8;
    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_read_acc\n");
        return -LSM6DSO_ERROR_BUS;
    }

    assert(dev->params.acc_fs < LSM6DSO_ACC_FS_MAX);
    data->x = ((int32_t)data->x * range_acc[dev->params.acc_fs]) / INT16_MAX;
    data->y = ((int32_t)data->y * range_acc[dev->params.acc_fs]) / INT16_MAX;
    data->z = ((int32_t)data->z * range_acc[dev->params.acc_fs]) / INT16_MAX;

    return LSM6DSO_OK;
}

int lsm6dso_read_gyro(const lsm6dso_t *dev, lsm6dso_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, LSM6DSO_REG_STATUS_REG, &tmp, 0);
    DEBUG("lsm6dso status: %x\n", tmp);

    res = i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTX_L_G, &tmp, 0);
    data->x = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTX_H_G, &tmp, 0);
    data->x |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTY_L_G, &tmp, 0);
    data->y = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTY_H_G, &tmp, 0);
    data->y |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTZ_L_G, &tmp, 0);
    data->z = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUTZ_H_G, &tmp, 0);
    data->z |= tmp << 8;
    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_read_gyro\n");
        return -LSM6DSO_ERROR_BUS;
    }

    assert(dev->params.gyro_fs < LSM6DSO_GYRO_FS_MAX);
    data->x = ((int32_t)data->x * range_gyro[dev->params.gyro_fs]) / INT16_MAX;
    data->y = ((int32_t)data->y * range_gyro[dev->params.gyro_fs]) / INT16_MAX;
    data->z = ((int32_t)data->z * range_gyro[dev->params.gyro_fs]) / INT16_MAX;

    return LSM6DSO_OK;
}

int lsm6dso_read_temp(const lsm6dso_t *dev, int16_t *data)
{
    uint8_t tmp;
    uint16_t traw;
    /* read raw temperature */
    i2c_acquire(BUS);
    if (i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUT_TEMP_L, &tmp, 0) < 0) {
        i2c_release(BUS);
        return -LSM6DSO_ERROR_BUS;
    }
    traw = tmp;
    if (i2c_read_reg(BUS, ADDR, LSM6DSO_REG_OUT_TEMP_H, &tmp, 0) < 0) {
        i2c_release(BUS);
        return -LSM6DSO_ERROR_BUS;
    }
    traw |= (uint16_t)tmp << 8;
    i2c_release(BUS);
    /* convert temperature to degC x 100 */
    traw += LSM6DSO_TEMP_OFFSET;
    *data = (int16_t)(((int32_t)traw * 100) / LSM6DSO_TEMP_SENSITIVITY);

    return LSM6DSO_OK;
}

int lsm6dso_acc_power_down(const lsm6dso_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSO_REG_CTRL1_XL, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dso_acc_power_down\n");
        return -LSM6DSO_ERROR_BUS;
    }

    tmp &= ~(LSM6DSO_CTRL_ODR_MASK);
    res = i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL1_XL, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_acc_power_down\n");
        return -LSM6DSO_ERROR_BUS;
    }

    return LSM6DSO_OK;
}

int lsm6dso_gyro_power_down(const lsm6dso_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSO_REG_CTRL2_G, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dso_gyro_power_down\n");
        return -LSM6DSO_ERROR_BUS;
    }

    tmp &= ~(LSM6DSO_CTRL_ODR_MASK);
    res = i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL2_G, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_gyro_power_down\n");
        return -LSM6DSO_ERROR_BUS;
    }

    return LSM6DSO_OK;
}

int lsm6dso_acc_power_up(const lsm6dso_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSO_REG_CTRL1_XL, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dso_acc_power_up\n");
        return -LSM6DSO_ERROR_BUS;
    }

    tmp &= ~(LSM6DSO_CTRL_ODR_MASK);
    tmp |= dev->params.acc_odr << LSM6DSO_CTRL_ODR_SHIFT;
    res = i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL1_XL, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_acc_power_up\n");
        return -LSM6DSO_ERROR_BUS;
    }

    return LSM6DSO_OK;
}

int lsm6dso_gyro_power_up(const lsm6dso_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSO_REG_CTRL2_G, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dso_gyro_power_up\n");
        return -LSM6DSO_ERROR_BUS;
    }

    tmp &= ~(LSM6DSO_CTRL_ODR_MASK);
    tmp |= dev->params.gyro_odr << LSM6DSO_CTRL_ODR_SHIFT;
    res = i2c_write_reg(BUS, ADDR, LSM6DSO_REG_CTRL2_G, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dso_gyro_power_up\n");
        return -LSM6DSO_ERROR_BUS;
    }

    return LSM6DSO_OK;
}
