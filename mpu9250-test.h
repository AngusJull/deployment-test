#include "test-global.h"

struct mpu9250_desc_t {
    /** Records time of interrupt before a sample is read from the chip */
    uint32_t next_sample_time;

    uint32_t last_sample_time;
    int16_t last_accel_x;
    int16_t last_accel_y;
    int16_t last_accel_z;
    int16_t last_gyro_x;
    int16_t last_gyro_y;
    int16_t last_gyro_z;
    int16_t last_temp;
    int16_t last_mag_x;
    int16_t last_mag_y;
    int16_t last_mag_z;

    /** Magnetometer sensitivity adjustment values */
    uint8_t mag_asa[3];

    uint8_t samples_to_read;
    uint8_t extra_samples;

    uint8_t samples_left;

    /** Driver current state */
    enum mpu9250_state state:6;
    /** Driver state to continue to after subsequence */
    enum mpu9250_state next_state:6;

    /** Flag set to indicate that the drive should wait for the FIFO to fill and
        read data in larger chunks rather than reading each sample using the
        interrupt */
    uint8_t use_fifo:1;
    /** Flag to indicate that we the register values to be sent in the current
        state have been marshaled */
    uint8_t cmd_ready:1;
    /** Flag to indicate that we are waiting for an I2C transaction */
    uint8_t i2c_in_progress:1;
    /** Flag to indicate that we are doing a wait after an I2C transaction is
        done */
    uint8_t post_cmd_wait:1;
    /** Flag to indicate that samples should be subtracted by accumulation
        sequence */
    uint8_t acc_subtract:1;
    /** Bit that indicates the last magnetometer sample is not valid because the
        magnetic sensor overflowed */
    uint8_t last_mag_overflow:1;
    /** Flag to indicate that we currently have buffer checked out from the
        telemetry service */
    uint8_t telemetry_buffer_checked_out:1;
    /** Flag to indicate that an I2C transaction initiated from an interrupt is
        in progress (separate from i2c_in_progress to avoid affecting FSM) */
    uint8_t async_i2c_in_progress:1;
};
extern int init_mpu9250(struct mpu9250_desc_t *inst);

extern void mpu9250_service(struct mpu9250_desc_t *inst);


/**
 *  Get the sensitivity of the gyroscope.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return Gyroscope sensitivity in LSB/1000 dps
 */
static inline uint32_t mpu9250_gyro_sensitivity(const struct mpu9250_desc_t *inst)
{
    switch (inst->gyro_fsr) {
        case MPU9250_GYRO_FSR_250DPS:
            // sensitivity = (2^15) / 250 = 131.072 LSB/dps
            return 131072;
        case MPU9250_GYRO_FSR_500DPS:
            // sensitivity = (2^15) / 500 = 65.536 LSB/dps
            return 65536;
        case MPU9250_GYRO_FSR_1000DPS:
            // sensitivity = (2^15) / 1000 = 32.768 LSB/dps
            return 32768;
        case MPU9250_GYRO_FSR_2000DPS:
            // sensitivity = (2^15) / 2000 = 16.384 LSB/dps
            return 16384;
    }
}

/**
 *  Get the sensitivity of the accelerometer.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return Accelerometer sensitivity in LSB/g
 */
static inline uint16_t mpu9250_accel_sensitivity(const struct mpu9250_desc_t *inst)
{
    switch (inst->accel_fsr) {
        case MPU9250_ACCEL_FSR_2G:
            // sensitivity = (2^15) / 2 = 16384 LSB/g
            return 16384;
        case MPU9250_ACCEL_FSR_4G:
            // sensitivity = (2^15) / 4 = 8192 LSB/g
            return 8192;
        case MPU9250_ACCEL_FSR_8G:
            // sensitivity = (2^15) / 8 = 4096 LSB/g
            return 4096;
        case MPU9250_ACCEL_FSR_16G:
            // sensitivity = (2^15) / 16 = 2048 LSB/g
            return 2048;
    }
}

/**
 *  Get the sensitivity of the magnetometer.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return Magnetometer sensitivity in LSB per mT
 */
static inline uint16_t mpu9250_mag_sensitivity(const struct mpu9250_desc_t *inst)
{
    // Sensitivity in 16 bit mode is 0.15 microT/LSB -> 6 and 2/3 LSB/microT
    return 6666;
}

/**
 *  Get the sample rate for the accelerometer and gyroscope.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return Accelerometer and gyroscope ODR in Hz
 */
static inline uint16_t mpu9250_get_ag_odr(const struct mpu9250_desc_t *inst)
{
    return (uint16_t)1000 / ((uint16_t)(inst->odr) + 1);
}

/**
 *  Get the full scale range for the accelerometer.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The full scale range in g.
 */
static inline uint8_t mpu9250_get_accel_fsr(const struct mpu9250_desc_t *inst)
{
    switch (inst->accel_fsr) {
        case MPU9250_ACCEL_FSR_2G:
            return 2;
        case MPU9250_ACCEL_FSR_4G:
            return 4;
        case MPU9250_ACCEL_FSR_8G:
            return 8;
        case MPU9250_ACCEL_FSR_16G:
            return 16;
    }
}

/**
 *
 */
static inline uint16_t mpu9250_get_gyro_fsr(const struct mpu9250_desc_t *inst)
{
    switch (inst->gyro_fsr) {
        case MPU9250_GYRO_FSR_250DPS:
            return 250;
        case MPU9250_GYRO_FSR_500DPS:
            return 500;
        case MPU9250_GYRO_FSR_1000DPS:
            return 1000;
        case MPU9250_GYRO_FSR_2000DPS:
            return 2000;
    }
}



/**
 *  Get the time of the most recent measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The value of millis for the most recent measurement
 */
static inline uint32_t mpu9250_get_last_time (const struct mpu9250_desc_t *inst)
{
    return inst->last_sample_time;
}

/**
 *  Get the most recent x axis acceleration measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent x axis acceleration measurement
 */
static inline int16_t mpu9250_get_accel_x(const struct mpu9250_desc_t *inst)
{
    return inst->last_accel_x;
}

/**
 *  Get the most recent y axis acceleration measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent y axis acceleration measurement
 */
static inline int16_t mpu9250_get_accel_y(const struct mpu9250_desc_t *inst)
{
    return inst->last_accel_y;
}

/**
 *  Get the most recent z axis acceleration measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent z axis acceleration measurement
 */
static inline int16_t mpu9250_get_accel_z(const struct mpu9250_desc_t *inst)
{
    return inst->last_accel_z;
}

/**
 *  Get the most recent temperature measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent temperature measurement in millidegrees celsius
 */
extern int32_t mpu9250_get_temperature(const struct mpu9250_desc_t *inst);

/**
 *  Get the most recent x axis angular velocity measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent x axis angular velocity measurement
 */
static inline int16_t mpu9250_get_gyro_x(const struct mpu9250_desc_t *inst)
{
    return inst->last_gyro_x;
}

/**
 *  Get the most recent y axis angular velocity measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent y axis angular velocity measurement
 */
static inline int16_t mpu9250_get_gyro_y(const struct mpu9250_desc_t *inst)
{
    return inst->last_gyro_y;
}

/**
 *  Get the most recent z axis angular velocity measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent z axis angular velocity measurement
 */
static inline int16_t mpu9250_get_gyro_z(const struct mpu9250_desc_t *inst)
{
    return inst->last_gyro_z;
}

/**
 *  Get the most recent x axis magnetic flux density measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent x axis magnetic flux density measurement
 */
static inline int16_t mpu9250_get_mag_x(const struct mpu9250_desc_t *inst)
{
    return inst->last_mag_x;
}

/**
 *  Get the most recent y axis magnetic flux density measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent y axis magnetic flux density measurement
 */
static inline int16_t mpu9250_get_mag_y(const struct mpu9250_desc_t *inst)
{
    return inst->last_mag_y;
}

/**
 *  Get the most recent z axis magnetic flux density measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return The most recent z axis magnetic flux density measurement
 */
static inline int16_t mpu9250_get_mag_z(const struct mpu9250_desc_t *inst)
{
    return inst->last_mag_z;
}

/**
 *  Get whether the magnetometer overflowed during the most recent measurement.
 *
 *  @param inst The MPU9250 driver instance
 *
 *  @return Whether the magnetometer overflowed during the most recent
 *          measurement
 */
static inline int mpu9250_get_mag_overflow(const struct mpu9250_desc_t *inst)
{
    return inst->last_mag_overflow;
}