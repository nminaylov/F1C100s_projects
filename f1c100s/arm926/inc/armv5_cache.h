#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static inline void cache_inv_range(unsigned long start, unsigned long end) {
    extern void v5_cache_inv_range(unsigned long start, unsigned long end);
    v5_cache_inv_range(start, end);
}

static inline void cache_clean_range(unsigned long start, unsigned long end) {
    extern void v5_cache_clean_range(unsigned long start, unsigned long end);
    v5_cache_clean_range(start, end);
}

static inline void cache_flush_range(unsigned long start, unsigned long end) {
    extern void v5_cache_flush_range(unsigned long start, unsigned long end);
    v5_cache_flush_range(start, end);
}

#ifdef __cplusplus
}
#endif
