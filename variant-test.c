/**
 * @file variant.c
 * @desc Variant specific functions and data for test variant
 * @author Samuel Dewan
 * @date 2020-09-05
 * Last Author:
 * Last Edited On:
 */

#include "variant-test.h"
#include "ms5611-test.h"
#include "mpu9250-test.h"
#include "deployment.h"

#ifdef ENABLE_ALTIMETER
struct ms5611_desc_t altimeter_g;
#endif

#ifdef ENABLE_IMU
struct mpu9250_desc_t imu_g;
#endif

#ifdef ENABLE_DEPLOYMENT_SERVICE
struct deployment_service_desc_t deployment_g;
#endif

void init_variant(void)
{

    // Init Altimeter
#ifdef ENABLE_ALTIMETER
    init_ms5611(&altimeter_g, ALTIMETER_CSB, ALTIMETER_PERIOD, 1);
#ifdef ENABLE_TELEMETRY_SERVICE
    telemetry_register_ms5611_alt(&telemetry_g, &altimeter_g);
#endif
#endif

    // Init IMU
#ifdef ENABLE_IMU
    init_mpu9250(&imu_g, IMU_ADDR, IMU_INT_PIN, IMU_GYRO_FSR,
                 IMU_GYRO_BW, IMU_ACCEL_FSR, IMU_ACCEL_BW, IMU_AG_SAMPLE_RATE,
                 IMU_MAG_SAMPLE_RATE, IMU_USE_FIFO);
#endif
    // Deployment service
#ifdef ENABLE_DEPLOYMENT_SERVICE
#ifndef ENABLE_ALTIMETER
#error  Deployment service requires altimeter
#endif
#ifndef ENABLE_IMU
#error  Deployment service requires IMU
#endif
    init_deployment(&deployment_g, &altimeter_g, &imu_g);
#endif
}

void variant_service(void)
{
#ifdef ENABLE_ALTIMETER
    ms5611_service(&altimeter_g);
#endif

#ifdef ENABLE_IMU
    mpu9250_service(&imu_g);
#endif

#ifdef ENABLE_DEPLOYMENT_SERVICE
    deployment_service(&deployment_g);
#endif
}
