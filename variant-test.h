/**
 * @file variant.h
 * @desc Rocket variant
 * @author Samuel Dewan
 * @date 2020-09-05
 * Last Author:
 * Last Edited On:
 */

#ifndef variant_h
#define variant_h

#include "test-global.h"
#if !defined(telemetry_h) // don't want to include telemetry.h yet
#define telemtry_h_skipped
#define telemetry_h
#endif
#include "ms5611-test.h"
#include "mpu9250-test.h"
#ifdef telemtry_h_skipped
#undef telemetry_h
#undef telemtry_h_skipped
#endif
#include "deployment.h"

/* String to identify this configuration */
#define VARIANT_STRING "Rocket"

/* USB Strings */
#define USB_MANUFACTURER_STRING u"CU InSpace"
#define USB_PRODUCT_STRING u"CU InSpace MCU Board - Rocket"


//
//
//  Header Pins
//
//

#define ARMED_SENSE_PIN         GPIO_15
#define DROGUE_EMATCH_PIN       GPIO_22
#define MAIN_EMATCH_PIN         GPIO_23


//
//
//  Deployment configuration
//
//

/* Acceleration threashold to trigger transition into powered ascent state in
   g */
#define DEPLOYMENT_POWERED_ASCENT_ACCEL_THREASHOLD  4
/* Backup altitude threashold to trigger transition into powered ascent state in
   meters */
#define DEPLOYMENT_POWERED_ASCENT_ALT_THREASHOLD    100
/* Acceleration threashold to trigger transition into coasting ascent state in
   g */
#define DEPLOYMENT_COASTING_ASCENT_ACCEL_THREASHOLD 1
/* Backup altitude threashold to trigger transition into coasting ascent state
   in meters */
#define DEPLOYMENT_COASTING_ASCENT_ALT_THREASHOLD   2000
/* Mininum altitude threashold for transition into coasting ascent state in
   meters */
#define DEPLOYMENT_COASTING_ASCENT_ALT_MINIMUM      500
/* Number of consecutive samples below the maximum altitude we have seen
   required to deploy drogue chute */
#define DEPLOYMENT_DESCENDING_SAMPLE_THREASHOLD     5
/* Amount of change in altitude required to indicate that we are still moving in
   meters */
#define DEPLOYMENT_LANDED_ALT_CHANGE                0.5f
/* Number of consecutive samples of staying still required before we can be
   sure that we have landed */
#define DEPLOYMENT_LANDED_SAMPLE_THREASHOLD         100

/* Length of time that current is applied to ematches in milliseconds */
#define DEPLOYMENT_EMATCH_FIRE_DURATION             500

/* The altitude at which the drogue parachute will be deployed*/
#define DROGUE_DEPLOY_ALTITUDE                      1000

/* The altitude at which the main parachute will be deployed*/
#define MAIN_DEPLOY_ALTITUDE                        500

//
//
//  Altimeter
//
//

/* Altimeter enabled if defined */
#define ENABLE_ALTIMETER
/* Altimeter CSB setting */
#define ALTIMETER_CSB 0
/* Altimeter sample period in milliseconds */
#define ALTIMETER_PERIOD MS_TO_MILLIS(100)
extern struct ms5611_desc_t altimeter_g;

//
//
//  IMU
//
//

/* IMU enabled if defined */
#define ENABLE_IMU
/* I2C sddress for IMU */
#define IMU_ADDR 0b1101000U
/* IMU interrupt pin */
#define IMU_INT_PIN GPIO_4

#define IMU_GYRO_FSR            MPU9250_GYRO_FSR_2000DPS
#define IMU_GYRO_BW             MPU9250_GYRO_BW_41HZ
#define IMU_ACCEL_FSR           MPU9250_ACCEL_FSR_16G
#define IMU_ACCEL_BW            MPU9250_ACCEL_BW_45HZ
#define IMU_AG_SAMPLE_RATE      100
#define IMU_MAG_SAMPLE_RATE     AK8963_ODR_100HZ
#define IMU_USE_FIFO            1

#ifdef ENABLE_IMU
extern struct mpu9250_desc_t imu_g;
#endif
//
//
//  Deployment
//
//

#define ENABLE_DEPLOYMENT_SERVICE

#ifdef ENABLE_DEPLOYMENT_SERVICE
extern struct deployment_service_desc_t deployment_g;
#endif

#endif /* variant_h */
