/* my_test.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/
#include "FreeRTOS.h" /* Must come first. */
//
#include <stdint.h>
#include <stdio.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

//
#include "my_debug.h"
#include "crash.h"

void stack_overflow() {
    char big[643 * 4] = {0};
    printf("sizeof big: %zu\n", sizeof big);
}

void trigger_hard_fault() {
    void (*bad_instruction)(void) = (void *)0xE0000000;
    bad_instruction();
}

void hang() {
    for (;;) sleep_ms(10);
}

// uint32_t foo __attribute__((section(".noinit")));
uint32_t foo __attribute__((section(".uninitialized_data")));

void test_hex_dump() {
    size_t n = 1024;
    void *p = pvPortMalloc(n * sizeof(uint32_t));
    //void hexdump_32(const char *s, void *p, size_t n);
    hexdump_32("test hex dump", p, n);
    vPortFree(p);
    p = pvPortMallocStack(n * sizeof(uint32_t));
    hexdump_32("test hex dump", p, n);
}

void test_compare_buffers_32() {
    // bool compare_buffers_32(const char *s0, const void *p0, const char *s1,
                        //const void *p1, const size_t n);
    uint32_t a0[24] = {0};
    //memset(a0, 0, sizeof(a0));
    uint32_t a1[24] = {0xFFFFFFFF};
    //memset(a1, 1, sizeof(a1));
    compare_buffers_32("a0", a0, "a1", a1, sizeof a0);
}

void my_test(int v) {
    switch (v) {
        case 1:
            SYSTEM_RESET();
        case 2:
            stack_overflow();
            break;
        case 3:
            __asm("bkpt 1");
            break;
        case 4:
            trigger_hard_fault();
            break;
        case 5: 
            ASSERT_ALWAYS(false);
            break;
        case 6:
            ASSERT_CASE_NOT(9999);
            break;
        case 7:
            hang();
            break;
        case 8:
            printf("foo: %lx\n", foo++);
            break;
        case 9:
            test_hex_dump();
            break;
        case 10:
            test_compare_buffers_32();
            break;
        default:
            printf("Bad test number: %d\n", v);
    }
}

/* [] END OF FILE */
