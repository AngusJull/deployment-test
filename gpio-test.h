#ifndef gpio_test_h
#define gpio_test_h

#include "test-global.h"

#define GPIO_4  1
#define GPIO_15 1
#define GPIO_22 1
#define GPIO_23 1

inline uint8_t gpio_get_input(uint8_t pin) {return 1;}
inline uint8_t gpio_set_output(uint8_t pin, uint8_t value) {return;}

#endif /* gpio-test.h */