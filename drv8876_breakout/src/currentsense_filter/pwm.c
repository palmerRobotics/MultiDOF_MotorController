#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "iir.h"



const uint nfault = 1; //GPIO1
const uint nsleep = 2; //GPIO2
const uint ph = 3; //GPIO3
const uint en = 4; //GPIO4
const uint pmode = 6; //GPIO5

const uint16_t rollover = 6249; //20kHz, 528uV pwm resolution
volatile IIRfilter LPbutter;

bool repeating_timer_callback(struct repeating_timer *timer) {
    // !! VERIFY THAT CURRENT SENSING VOLTAGE WON'T BLOW INPUT PIN !! done
    const float conversion_factor = 3.3f / (1 << 12);
    
    uint16_t raw_val = adc_read();
    //adc read will never exceed 2^12 = 4096. Can't lose data converting from uint to int
    int dir_val = raw_val;
    
    //If the rotation direction is reverse, negate current reading (since it's always positive, regardless of direction)
    if ( gpio_get(ph) == 0) {
        dir_val = raw_val * -1;
    }
    
    //TODO: should probably base control loop and filter off of adc value rather than V. Less computation. Will need to change parameter type of filterSignal()
    float dir_V = dir_val * conversion_factor;

    float filtered_V = filterSignal(&LPbutter, dir_V);
    
    printf("%f|%f\n", dir_V, filtered_V);
    return true;
}

int main() {
    stdio_init_all(); // !Required for the Pico to show up in /dev as ttyACM0 
    sleep_ms(5000); //Give me a little bit of time to open /dev/ttyACM0

    // Initialize the analog inputs
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    // Initialize the GPIO pins
    gpio_init(nfault);
    gpio_init(nsleep);
    gpio_init(ph);
    gpio_init(pmode);

    // Tell GPIO 6 it's are allocated to the PWM
    gpio_set_function(en, GPIO_FUNC_PWM);

    gpio_set_dir(nfault, GPIO_IN);
    gpio_set_dir(nsleep, GPIO_OUT);
    gpio_set_dir(ph, GPIO_OUT);
    gpio_set_dir(pmode, GPIO_OUT);     

    // Find out which PWM slice is connected to GPIO 5
    uint slice_num = pwm_gpio_to_slice_num(en);

    // Set pwm frequency
    pwm_set_wrap(slice_num, rollover);
    // Set duty cycle to 0%
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);

    // Set h-bridge to ph/en control mode
    gpio_put(pmode,0);

    sleep_ms(10);
    // Enable the h-bridge
    gpio_put(nsleep,1);

    //imode and vref are tied to ground and 3.3V, respectively. No need to set them in software

    // IIR filter order and a & b coefficents determined via MATLAB
    const char order = 2;
    float b[MAX_ORDER] = {0.0279, 0.0557, 0.0279};
    float a[MAX_ORDER] = {1.0, -1.4755, 0.5869};

    // Initialize filter order and coefficients
    initFilter(&LPbutter, order, b, a);

    // Set the PWM running
    pwm_set_enabled(slice_num, true);

    printf("Raw V | Filtered V\n");

    // Set up timer-based interrupt for measuring current
    struct repeating_timer current_sensing_timer;
    //.01 sec period (100Hz) for measuring motor current. Will need to change to 5kHz for deployment
    add_repeating_timer_ms(10, repeating_timer_callback, NULL, &current_sensing_timer);

    // Set forward motor rotation direction
    gpio_put(ph,1);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, rollover);

    // Uncomment contents of while loop to toggle motor rotation direction every 5 secs
    while(1) {
        /*
        gpio_put(ph,1);
        pwm_set_chan_level(slice_num, PWM_CHAN_A, rollover);
        sleep_ms(5000);
        gpio_put(ph,0);
        pwm_set_chan_level(slice_num, PWM_CHAN_A, rollover);
        sleep_ms(5000);
        */


    }
}