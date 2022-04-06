#ifndef H_TIMER_PC_EMUL_PLATFORM_H
#define H_TIMER_PC_EMUL_PLATFORM_H
/* PC Emulation of TIMER peripheral */

#ifdef PC

#include <stdint.h>
#include "iob_timer_swreg.h"
#include <time.h>

/* convert clock values from PC CLOCK FREQ to EMBEDDED FREQ */
#define PC_TO_FREQ_FACTOR ((1.0*FREQ)/CLOCKS_PER_SEC)

static clock_t start, end, time_counter, counter_reg;
static int timer_enable;

static void pc_timer_reset(int value) {	
    // use only reg width
    int rst_int = (value & 0x01);
    if(rst_int){
        start = end = 0;
        time_counter = 0;
        timer_enable = 0;
    }
    return;
}

static void pc_timer_enable(int value){
    // use only reg width
    int en_int = (value & 0x01);
    // manage transitions
    // 0 -> 1
    if(timer_enable == 0 && en_int == 1){
        // start counting time
        start = clock();
    } else if(timer_enable == 1 && en_int == 0){
        // accumulate enable interval
        end = clock();
        timer_enable += (end - start);
        start = end = 0; // reset aux clock values
    }
    // store enable en_int
    timer_enable = en_int;
    return;
}

static void pc_timer_sample(int value) {	
    // use only reg width
    int sample_int = (value & 0x01);
    if(sample_int){
        counter_reg = time_counter;
        if(start != 0)
            counter_reg += (clock() - start);
    }
    return;
}

static int pc_timer_data_high(){
    /* convert clock from PC CLOCKS_PER_CYCLE to FREQ */
    double counter_freq = (1.0*counter_reg)*PC_TO_FREQ_FACTOR;
    return ( (int) (((unsigned long long) counter_freq) >> 32));
}

static int pc_timer_data_low(){
    /* convert clock from PC CLOCKS_PER_CYCLE to FREQ */
    double counter_freq = (1.0*counter_reg)*PC_TO_FREQ_FACTOR;
    return ( (int) (((unsigned long long) counter_freq) & 0xFFFFFFFF));
}

static void io_set_int(int addr, int64_t value) {
    switch(addr){
        case TIMER_RESET:
            pc_timer_reset(value);
            break;
        case TIMER_ENABLE:
            pc_timer_enable(value);
            break;
        case TIMER_SAMPLE:
            pc_timer_sample(value);
            break;
        default:
            // do nothing
            break;
    }
    return;
}

static int64_t io_get_int(int addr) {
    int ret_val = 0;
    switch(addr){
        case TIMER_DATA_HIGH:
            ret_val = pc_timer_data_high();
            break;
        case TIMER_DATA_LOW:
            ret_val = pc_timer_data_low();
            break;
        default:
            // do nothing
            break;
    }
    return ret_val;
}

#endif //ifdef PC

#endif //ifndef H_TIMER_PC_EMUL_PLATFORM_H
