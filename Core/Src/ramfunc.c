#include <stdint.h>
#include "ramfunc.h"

extern uint32_t __ramfunc_load__;
extern uint32_t __ramfunc_start__;
extern uint32_t __ramfunc_end__;

extern uint32_t __ramdata_load__;
extern uint32_t __ramdata_start__;
extern uint32_t __ramdata_end__;

void Copy_RamSections(void)
{
    uint32_t *src, *dst;

    // .ramfunc
    src = &__ramfunc_load__;
    dst = &__ramfunc_start__;
    while (dst < &__ramfunc_end__) {
        *dst++ = *src++;
    }

    // .ramdata (optional)
    src = &__ramdata_load__;
    dst = &__ramdata_start__;
    while (dst < &__ramdata_end__) {
        *dst++ = *src++;
    }
}