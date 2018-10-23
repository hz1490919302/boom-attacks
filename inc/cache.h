#ifndef CACHE_H
#define CACHE_H

// cache values
// TODO: check that these parameters are right
#define L1_SETS 64
#define L1_SET_BITS 6 // note: this is log2Ceil(L1_SETS)
#define L1_WAYS 4
#define L1_BLOCK_SZ_BYTES 64
#define L1_BLOCK_BITS 6 // note: this is log2Ceil(L1_BLOCK_SZ_BYTES)
#define L1_SZ_BYTES (L1_SETS*L1_WAYS*L1_BLOCK_SZ_BYTES)
#define FULL_MASK 0xFFFFFFFFFFFFFFFF
#define OFF_MASK (FULL_MASK << L1_BLOCK_BITS)
#define TAG_MASK (FULL_MASK << (L1_SET_BITS + L1_BLOCK_BITS))
#define IDX_MASK (~(TAG_MASK | OFF_MASK))

/* ----------------------------------
 * |                  Cache address |
 * ----------------------------------
 * |       tag |      idx |  offset |
 * ----------------------------------
 * | 63 <-> 12 | 11 <-> 6 | 5 <-> 0 |
 * ----------------------------------
 */

// setup array size of cache to "put" in the cache on $ flush
// guarantees contiguous set of addrs that is at least the sz of cache
uint8_t dummyMem[2 * L1_SZ_BYTES];

/**
 * Flush the cache of the address given since RV64 does not have a
 * clflush type of instruction. Clears any set that has the same idx bits
 * as the address input range
 *
 * @param addr starting address to clear the cache
 * @param sz size of the data to remove in bytes
 */
void flushCache(uint64_t addr, uint64_t sz){
    printf("addr(0x%x) sz(%d)\n", addr, sz);

    // find out the amount of blocks you want to clear
    uint64_t numBlocksClear = sz >> L1_BLOCK_BITS;
    if ((sz & IDX_MASK) != 0){
        numBlocksClear += 1;
    }

    // temp variable used for nothing
    uint8_t dummy = 0;

    // this mem address is the start of a contiguous set of memory that will fit inside of the
    // cache
    // thus it has the following properties
    // 1. dummyMem <= alignedMem < dummyMem + sizeof(dummyMem)
    // 2. alignedMem has idx = 0 and offset = 0 
    uint8_t alignedMem = (((uint64_t)&dummyMem) + L1_SZ_BYTES) & TAG_MASK;
    for (uint64_t i = 0; i < numBlocksClear; ++i){
        // offset to move across the sets that you want to flush
        uint64_t setOffset = ((((addr & IDX_MASK) >> L1_SET_BITS) + i) % L1_SETS) * L1_BLOCK_SZ_BYTES;

        // since there are L1_WAYS you need to flush the entire set
        for(uint64_t j = 0; j < L1_WAYS; ++j){
            // offset to reaccess the set
            uint64_t wayOffset = j * L1_BLOCK_SZ_BYTES * L1_SETS;

            // evict the previous cache block and put in the dummy mem
            dummy &= *((uint8_t*)(alignedMem + setOffset + wayOffset));
            printf("evict read(0x%x)\n", alignedMem + setOffset + wayOffset);
        }
    }
}

#endif
