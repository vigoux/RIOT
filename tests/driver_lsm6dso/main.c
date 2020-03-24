/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the LSM6DSO accelerometer/gyroscope driver.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "lsm6dso.h"
#include "lsm6dso_params.h"

#define SLEEP_USEC  (500UL * US_PER_MS)

int main(void)
{
    lsm6dso_t dev;
    int16_t temp_value;
    lsm6dso_3d_data_t mag_value;
    lsm6dso_3d_data_t acc_value;

    puts("LSM6DSO test application");
    printf("Initializing LSM6DSO sensor at I2C_%i... ", lsm6dso_params->i2c);

    if (lsm6dso_init(&dev, lsm6dso_params) != LSM6DSO_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    puts("Powering down LSM6DSO sensor...");
    if (lsm6dso_acc_power_down(&dev) != LSM6DSO_OK) {
        puts("[ERROR]");
        return 1;
    }
    if (lsm6dso_gyro_power_down(&dev) != LSM6DSO_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    xtimer_sleep(1);

    puts("Powering up LSM6DSO sensor...");
    if (lsm6dso_acc_power_up(&dev) != LSM6DSO_OK) {
        puts("[ERROR]");
        return 1;
    }
    if (lsm6dso_gyro_power_up(&dev) != LSM6DSO_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    while (1) {
        if (lsm6dso_read_acc(&dev, &acc_value) == LSM6DSO_OK) {
            printf("Accelerometer x: %i y: %i z: %i\n", acc_value.x,
                                                        acc_value.y,
                                                        acc_value.z);
        }
        else {
            puts("[ERROR] reading accelerometer!\n");
        }

        if (lsm6dso_read_gyro(&dev, &mag_value) == LSM6DSO_OK) {
            printf("Gyroscope x: %i y: %i z: %i\n", mag_value.x,
                                                    mag_value.y,
                                                    mag_value.z);
        }
        else {
            puts("[ERROR] reading gyroscope!\n");
        }

        if (lsm6dso_read_temp(&dev, &temp_value) == LSM6DSO_OK) {
            printf("Temperature [in °C x 100]: %i \n", temp_value);
        }
        else {
            puts("[ERROR] reading temperature!\n");
        }

        puts("");
        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
