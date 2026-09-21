/**
 * @file main.c
 * @brief Entry point for the real target (not used in the PC build).
 *
 * Clock tree, vector table and .data/.bss init are done by the vendor
 * startup code before main() runs.
 */
#include "app/app.h"

int main(void) {
    (void)app_init();   /* on failure the LEDs already show the fault */

    for (;;) {
        app_run_once();
        /* __WFI();  sleep until the next interrupt (ADC EOC every 100 us) */
    }
}
