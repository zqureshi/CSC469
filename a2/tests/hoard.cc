#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gtest/gtest.h>

extern "C" {
#include "mm_thread.h"
#include "memlib.h"
#include "malloc.h"
#include "debug.h"
}

/* Align pointer to closest page boundary downwards */
#define PAGE_ALIGN(p)    ((void *)(((unsigned long)(p) / getpagesize()) * getpagesize()))
#define mm_usage() (mem_usage() + 1)

TEST(Hoard, PageSize) {
    ASSERT_EQ(4096, getpagesize());
}

TEST(Hoard, Processors) {
    TRACE("# of Processors: %d", getNumProcessors());
}

TEST(Hoard, PageBoundary) {
    ASSERT_EQ(0, (int)PAGE_ALIGN(127));
    ASSERT_EQ(getpagesize(), (int)PAGE_ALIGN(getpagesize() + 127));
}

TEST(Hoard, Init) {
    mm_init();
    TRACE("Memory Usage: %d", mm_usage());
    ASSERT_EQ(getpagesize(), mm_usage());
}

TEST(Hoard, InitTwice) {
    ASSERT_EQ(-1, mm_init());
}

TEST(Hoard, AllocSinglePage) {
    int j;
    for(j = 0; j < 10; j++) {
        const unsigned int NUM_ALLOCS = 600;
        int i, *nums[NUM_ALLOCS];
        for(i = 0; i < NUM_ALLOCS; i++) {
            nums[i] = (int *) mm_malloc(8);
            *(nums[i]) = i;
        }

        /* Verify data integrity */
        for(i = NUM_ALLOCS-1; i >= 0; i--) {
            ASSERT_EQ(i, *(nums[i]));
            mm_free(nums[i]);
        }

        TRACE("Memory Usage: %d", mm_usage());
    }
}

TEST(Hoard, GreaterThanPageSize) {
    char *ptr = (char *)mm_malloc(4096);
    mm_free(ptr);
}
