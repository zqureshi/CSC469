#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define SLEEP_NSEC   1E6L  /* 1 millisecond = 10^6 nanoseconds */
#define SLEEP_SAMPLES  5L

static uint64_t start = 0;

static inline void access_counter(unsigned *hi, unsigned *lo) {
    __asm__ volatile
        ( "rdtsc; movl %%edx, %0; movl %%eax, %1" /* format string */
        : "=r" (*hi), "=r" (*lo) /* output list */
        : /* no inputs */
        : "%edx", "%eax"); /* clobber list */
}

static inline void start_counter() {
    unsigned hi, lo;
    access_counter(&hi, &lo);
    start = ((uint64_t)hi << 32) | lo;
}

static inline uint64_t get_counter() {
    unsigned ncyc_hi, ncyc_lo;
    access_counter(&ncyc_hi, &ncyc_lo);
    return (((uint64_t)ncyc_hi << 32) | ncyc_lo) - start;
}

uint64_t inactive_periods(int num, uint64_t theshold, uint64_t *samples) {
    // TODO
    return 0;
}

int main(int argc, char const *argv[]) {
    /* Get clock resolution */
    struct timespec res;
    clock_getres(CLOCK_REALTIME, &res);

    /* Get CPU frequency by sleeping for SLEEP_SAMPLES times */
    struct timespec sleepTime = { 0, SLEEP_NSEC };
    uint64_t sleepSamples[SLEEP_SAMPLES];
    uint64_t start = 0, current = 0;

    /**
     *  As long as we don't go over 10 seconds of combined sleep time
     *  this shouldn't overflow.
     */
    uint64_t cyclePerMSec = 0;

    start_counter();
    for(uint64_t i = 0; i < SLEEP_SAMPLES;) {
        __asm__ volatile ( "cpuid;" );
        /* do some busy waiting */
        for(uint64_t j = 0; j < 1E8; j++);

        start = get_counter();
        if(nanosleep(&sleepTime, NULL) == 0) {
            current = get_counter();
            sleepSamples[i++] = current - start;
            cyclePerMSec += sleepSamples[i-1];
        }
    }
    cyclePerMSec = cyclePerMSec / SLEEP_SAMPLES;

    printf("Clock Resolution: %lds %ldns\n", res.tv_sec, res.tv_nsec);
    printf("Clock Speed: %.2Lf GHz\n", cyclePerMSec / SLEEP_NSEC);

}
