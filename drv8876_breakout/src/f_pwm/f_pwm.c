#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

#define FIVE_SEC_IN_MS 5000

const uint nfault = 1; //GPIO1
const uint nsleep = 2; //GPIO2
const uint ph = 3; //GPIO3
const uint en = 4; //GPIO4
const uint pmode = 6; //GPIO5

//*** PWM frequencies. Uncomment the frequency you want to use***//
const uint16_t rollover = 6249; //20kHz, 528uV pwm resolution
//const uint16_t rollover = 4165; //30kHz, 792uV pwm resoltuion
//const uint16_t rollover = 3124; //40kHz, 1.05mV pwm resolution

int main() {
    stdio_init_all(); //Required for the Pico to show up in /dev as ttyACM0 

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

    // Set the PWM running
    pwm_set_enabled(slice_num, true);

    while(1){
        
        printf("Starting duty cycle ramp up...");
        //printf("     DC%%    |     DCC\r\n");
        for(int i = 0; i < FIVE_SEC_IN_MS; i++){
            float duty_cycle = (float)i / (float)FIVE_SEC_IN_MS;
            //printf("%11f | ", duty_cycle);
            float duty_count = (float)rollover * duty_cycle;
            //printf("%11f\r\n", duty_count);
            pwm_set_chan_level(slice_num, PWM_CHAN_A, (uint16_t)duty_count);
            //printf("duty cycle count: %f\r\n", duty_count);
            sleep_ms(1);
            i++;
        }
        printf("Finished\r\n");
        sleep_ms(1000);

        printf("Starting duty cycle ramp down...");
        for(int i=FIVE_SEC_IN_MS; i>0; i--){
            float duty_cycle = (float)i/(float)FIVE_SEC_IN_MS;
            pwm_set_chan_level(slice_num, PWM_CHAN_A, (float)rollover*duty_cycle);
            //printf("duty cycle count: %f\r\n", rollover*duty_cycle);
            sleep_ms(1);
            i--;
        }
        printf("Finished\r\n");

        sleep_ms(1000);
        printf("Starting duty cycle & direction change...\r\n");
        for(int i=25;i<101;i+=25){
            printf("Running at %d%% duty cycle...\r\n", i);
            float duty_cycle = (float)i/100.0; //convert to a percentage
            float duty_count = (float)rollover*duty_cycle;
            pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_count); //set pwm duty cycle
            //printf("duty cycle count: %d\r\n", rollover*duty_cycle);
            for(int j=0; j < 2; j++){
                gpio_put(ph, 1);
                sleep_ms(3000);
                gpio_put(ph, 0);
                sleep_ms(3000);
                j++;
            }
        }
        printf("...Finished\r\n");

        pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
        printf("All tests finished, repeating procedure...\r\n");
        sleep_ms(1000);
    }
}