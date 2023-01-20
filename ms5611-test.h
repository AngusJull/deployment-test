/**
 * @file ms5611.h
 * @desc Driver for MS5611 barometric pressure sensor
 * @author Samuel Dewan
 * @date 2019-06-04
 * Last Author:
 * Last Edited On:
 */

#ifndef ms5611_h
#define ms5611_h

#include "test-global.h"

enum ms5611_state {
    MS5611_RESET,
    MS5611_RESET_WAIT,
    MS5611_READ_C1,
    MS5611_READ_C2,
    MS5611_READ_C3,
    MS5611_READ_C4,
    MS5611_READ_C5,
    MS5611_READ_C6,
    MS5611_IDLE,
    MS5611_CONVERT_PRES,
    MS5611_CONVERT_PRES_WAIT,
    MS5611_READ_PRES,
    MS5611_CONVERT_TEMP,
    MS5611_CONVERT_TEMP_WAIT,
    MS5611_READ_TEMP,
    MS5611_FAILED
};

struct ms5611_desc_t {
    /** Time of last reading from sensor */
    uint32_t last_reading_time;
    /** Temperature compensated pressure read from sensor */
    int32_t pressure;
    /** Temperature read from sensor */
    int32_t temperature;
    /** Altitude calculated from sensor */
    float altitude;
    
    /** Pressure used as 0 for altitude calculations */
    float p0;
    /** Digital pressure value from ADC  */
    uint32_t d1;
    /** Digital tempuratue value from ADC */
    uint32_t d2;

    /** Conversion start time */
    uint32_t conv_start_time;
    
    /** Time between readings of the sensor */
    uint32_t period;
    
    /** Values read from sensor PROM */
    uint16_t prom_values[6];
    /** I2C address for sensor */
    uint8_t address;
    /** I2C transaction id */
    uint8_t t_id;
    /** Current driver state */
    enum ms5611_state state:4;
    /** Currently waiting for an I2C transaction to complete */
    uint8_t i2c_in_progress:1;
    /** Flag to indicate whether altitude should be calculated */
    uint8_t calc_altitude:1;
    /** Flag to indicate whether p0 has been initialized */
    uint8_t p0_set:1;
};

/**
 * Initialize an instance of the MS5611 driver.
 */
extern void init_ms5611 (struct ms5611_desc_t *inst);
/**
 * Service to be run in each iteration of the main loop.
 *
 * @param inst The MS5611 driver instance for which the service should be run
 */
extern void ms5611_service (struct ms5611_desc_t *inst);

#endif /* ms5611_h */
