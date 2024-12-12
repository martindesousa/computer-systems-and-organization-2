#include "tlb.h"
#include "mlpt.h"
#include "config.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define SETS 16
#define WAYS 4

typedef struct {
    size_t page;            // Physical page number
    size_t vpn;            // Virtual page number
    int valid_bit;         // Valid bit
    int lru;           // LRU counter
} tlb_entry;

static tlb_entry tlb[SETS][WAYS];
static int debug = 0;

/** invalidate all cache lines in the TLB */
void tlb_clear() {
    for (int set = 0; set < SETS; set++)
    {
        for (int way = 0; way < WAYS; way++) {
            tlb[set][way].valid_bit = 0;
        }
    }
}

/**
 * return 0 if this virtual address does not have a valid
 * mapping in the TLB. Otherwise, return its LRU status: 1
 * if it is the most-recently used, 2 if the next-to-most,
 * etc.
 */
int tlb_peek(size_t va) {
    size_t vpn = va >> POBITS;
    int index = vpn % SETS;

    for (int way = 0; way < WAYS; way++)
    {
        if (tlb[index][way].valid_bit && tlb[index][way].vpn == vpn) {
            return tlb[index][way].lru;
        }
    }
    return 0;
}

void update_lru_vals(int index, int mru_way) { //update least recently used way values
    for (int way = 0; way < WAYS; way++) {
        if (tlb[index][way].valid_bit && tlb[index][way].lru < WAYS) {
            tlb[index][way].lru++;
        }
    }
    tlb[index][mru_way].lru = 1;

}

/**
 * If this virtual address is in the TLB, return its
 * corresponding physical address. If not, use
 * `translate(va)` to find that address, store the result
 * in the TLB, and return it. In either case, make its
 * cache line the most-recently used in its set.
 *
 * As an exception, if translate(va) returns -1, do not
 * update the TLB: just return -1.
 */
size_t tlb_translate(size_t va) {
    size_t vpn = va >> POBITS;
    int index = vpn % SETS;
    size_t offset_mask_1 = (1 << POBITS) - 1;
    size_t offset_mask_0 = ~((1 << POBITS) - 1);
    size_t offset = va & ((1 << POBITS) - 1);

    for (int way = 0; way < WAYS; way++)
    {

        if (tlb[index][way].valid_bit && tlb[index][way].vpn == vpn) { //found in the TLB
            update_lru_vals(index, way);
            if (debug == 1)
            {
                printf("CACHE HIT, NO TRANSLATE. FOUND IN tlb[%d][%d] VPN %zx, page %zx \n", index, way, tlb[index][way].vpn, tlb[index][way].page);
                printf("%zx", tlb[index][way].page);
            }
            
            return ((tlb[index][way].page & offset_mask_0) + (va & offset_mask_1));
        }
    }

    size_t page_aligned_va = (va & offset_mask_0);
    size_t page = translate(page_aligned_va); 
    if (page == -1) //translate returns -1
    {
        return -1;
    }
    page = (page & offset_mask_0) | offset;
    

    int max_lru = 0; //find least recently used way and store the tlb entry
    int lru_way = 0;
    for (int way = 0; way < WAYS; way++) {
        if (tlb[index][way].valid_bit == 0) {
            lru_way = way;
            break;
        }
        if (tlb[index][way].lru > max_lru) {
            max_lru = tlb[index][way].lru;
            lru_way = way;
        }
    }
    int cur_way = lru_way;
    tlb[index][cur_way].vpn = vpn;
    tlb[index][cur_way].page = page;
    tlb[index][cur_way].valid_bit = 1;

    if (debug == 1) {
        printf("CACHE MISS, TRANSLATED. ADDED INTO tlb[%d][%d] VPN %zx, page %zx \n", index, cur_way, vpn, page);
        
    }

    update_lru_vals(index, cur_way);
    return page;

}

/*
size_t translate(size_t va) {
    if (va < 0x1234000) {
        return va + 0x20000;
    } else if (va > 0x2000000 && va < 0x2345000) {
        return va + 0x100000;
    } else {
        return -1; 
    }
}

int main() {
    // Example 1: Testing different accesses and tlb_clear
    printf("Running example 1...\n");
    debug = 1;
    tlb_clear();
    assert(tlb_peek(0) == 0);
    assert(tlb_translate(0) == 0x0020000);
    assert(tlb_peek(0) == 1);
    assert(tlb_translate(0x200) == 0x20200);
    assert(tlb_translate(0x400) == 0x20400);
    assert(tlb_peek(0) == 1);
    assert(tlb_peek(0x200) == 1);
    assert(tlb_translate(0x2001200) == 0x2101200);
    assert(tlb_translate(0x0005200) == 0x0025200);
    assert(tlb_translate(0x0008200) == 0x0028200);
    assert(tlb_translate(0x0002200) == 0x0022200);
    assert(tlb_peek(0x2001000) == 1);
    assert(tlb_peek(0x0001000) == 0);
    tlb_clear();
    assert(tlb_peek(0x2001000) == 0);
    assert(tlb_peek(0x0005000) == 0);
    assert(tlb_peek(0x0008000) == 0);
    assert(tlb_peek(0x0002000) == 0);
    assert(tlb_peek(0x0000000) == 0);
    assert(tlb_translate(0) == 0x20000);
    assert(tlb_peek(0) == 1);
    printf("Example 1 passed.\n");

    // Example 2: Testing repeated accesses within a set
    printf("Running example 2...\n");
    
    tlb_clear();
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0801200) == 0x0821200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    assert(tlb_translate(0x0501200) == 0x0521200);
    assert(tlb_translate(0x0A01200) == 0x0A21200);
    assert(tlb_peek(0x0001200) == 0);
    assert(tlb_peek(0x2101200) == 0);
    assert(tlb_peek(0x2301200) == 3);
    assert(tlb_peek(0x0501200) == 2);
    assert(tlb_peek(0x0801200) == 4);
    assert(tlb_peek(0x0A01200) == 1);
    assert(tlb_translate(0x2301800) == 0x2401800); 
    assert(tlb_peek(0x0001000) == 0);
    assert(tlb_peek(0x2101000) == 0);
    assert(tlb_peek(0x2301000) == 1);
    assert(tlb_peek(0x0501000) == 3);
    assert(tlb_peek(0x0801000) == 4);
    assert(tlb_peek(0x0A01000) == 2);
    assert(tlb_translate(0x404000) == 0x424000);
    tlb_clear();
    assert(tlb_peek(0x301000) == 0);
    assert(tlb_peek(0x501000) == 0);
    assert(tlb_peek(0x801000) == 0);
    assert(tlb_peek(0xA01000) == 0);
    assert(tlb_translate(0xA01200) == 0xA21200);
    printf("Example 2 passed.\n");

    // Example 3: Testing invalid addresses

    printf("Running example 3...\n");

    tlb_clear();
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_peek(0xA0001000) == 0);
    assert(tlb_translate(0x1200) == 0x21200);
    assert(tlb_peek(0xA0001200) == 0);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_translate(0xB0001200) == -1);
    assert(tlb_translate(0xC0001200) == -1);
    assert(tlb_translate(0xD0001200) == -1);
    assert(tlb_translate(0xE0001200) == -1);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_translate(0x1200) == 0x21200);

    // Example 4: Filling and clearing a single set

    printf("Running example 4...\n");

    tlb_clear();
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0801200) == 0x0821200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    tlb_clear();
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    assert(tlb_translate(0x0011200) == 0x0031200);
    assert(tlb_peek(0x0001200) == 4);
    assert(tlb_peek(0x2101200) == 3);
    assert(tlb_peek(0x2301200) == 2);
    assert(tlb_peek(0x0011200) == 1);
    printf("Example 4 passed.\n");

    printf("All tests passed.\n");
    return 0;
}

*/
