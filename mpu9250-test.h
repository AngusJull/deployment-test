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
