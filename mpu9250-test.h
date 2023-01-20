#include "test-global.h"

struct mpu9250_desc_t {
    /** I2C instance used by this sensor */
    struct sercom_i2c_desc_t *i2c_inst;

    /** Telemetry service instance */
    struct telemetry_service_desc_t *telem;

    /** Buffer used for I2C transaction data */
    uint8_t buffer[MPU9250_BUFFER_LENGTH];

    /** Pointer to buffer to be used when reading samples from sensor, could be
        a buffer provided by the telemetry service or this driver instances own
        buffer */
    uint8_t *telem_buffer;

    /** The millis value when we started waiting for something */
    uint32_t wait_start;

    /** Values used when averaging samples for self test and offset
        calibration */
    int32_t accel_accumulators[3];
    /** Values used when averaging samples for self test and offset
        calibration */
    int32_t gyro_accumulators[3];

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

    /** Sensor I2C address */
    uint8_t mpu9250_addr;


    /** I2C transaction id */
    uint8_t t_id;

    uint8_t retry_count;

    /** Value to be loaded into sample rate register to set ODR */
    uint8_t odr;

    /** ODR for magnetometer */
    enum ak8963_odr mag_odr:1;
    /** Full scale range for gyroscope */
    enum mpu9250_gyro_fsr gyro_fsr:2;
    /** Full scale range for accelerometer */
    enum mpu9250_accel_fsr accel_fsr:2;
    /** LPF bandwidth for gyroscope */
    enum mpu9250_gyro_bw gyro_bw:7;
    /** LPF bandwidth for accelerometer */
    enum mpu9250_accel_bw accel_bw:7;

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
