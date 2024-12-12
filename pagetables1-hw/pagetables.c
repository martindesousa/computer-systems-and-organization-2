#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "config.h"
#include "mlpt.h"

#define PTE_BYTE_SIZE 8 // page table entry size in bytes
#define PAGE_SIZE (1 << POBITS) // page size

#define INDEX_BITS (POBITS - 3) //number of bits used for indexing each level
#define INDEX_MASK ((1 << INDEX_BITS) - 1) //mask of size index_bits
#define OFFSET_MASK ((1 << POBITS) - 1) //mask of size pobits

size_t ptbr = 0;  

// Helper method used in page_allocate
void* allocate_page_table() {
    void* page = NULL;
    int errorcode = posix_memalign(&page, PAGE_SIZE, PAGE_SIZE);  // Allocate a page
    if (errorcode != 0) {
        perror("Error allocating memory for page");
        return NULL;
    }

    // Clear the allocated page
    size_t* page_table = (size_t*)page;
    for (size_t i = 0; i < PAGE_SIZE / PTE_BYTE_SIZE; ++i) {
        page_table[i] = 0;
    }
    return page;
}

void page_allocate(size_t va) {
    size_t vpn = va >> POBITS;  // Extract virtual page number from virtual address

    if (ptbr == 0) {
        ptbr = (size_t)allocate_page_table();  // Allocate root page table
    }

    size_t* current_table = (size_t*)ptbr;
    size_t index;  // Index used for navigating the part of the VPN used for the current level
    size_t pte;

    for (int level = 0; level < LEVELS - 1; ++level) {
        size_t level_shift_bits = (LEVELS - level - 1) * INDEX_BITS;  // Shift based on level and index size
        index = (vpn >> level_shift_bits) & INDEX_MASK;  // Extract index from VPN
        pte = current_table[index];  // get the page table entry

        if (!(pte & 1)) {  // If entry does not have valid bit set
            void* new_table = allocate_page_table();  // Allocate a new page table

            current_table[index] = ((size_t)new_table & ~(OFFSET_MASK)) | 1;  // store the new table with valid bit set
            pte = current_table[index];  
        }

        current_table = (size_t*)(pte & ~(OFFSET_MASK));  //table is set to physical page number for next level
    }

    index = vpn & INDEX_MASK;  //Extract index on final level
    pte = current_table[index];

    if (!(pte & 1)) {  
        void* physical_page = allocate_page_table();  // Allocate new physical page

        size_t ppn = (size_t)physical_page >> POBITS;  // Get the physical page number
        current_table[index] = (ppn << POBITS) | 1;  // Store PPN with valid bit set
    }
}


size_t translate(size_t va) {

    if (ptbr == 0) {

        return 0xFFFFFFFFFFFFFFFF;  //return 64 bit string of 1s
    }

    size_t *current_table = (size_t *)ptbr;
    size_t vpn = va >> POBITS;  
    size_t index;
    size_t pte; 

    for (int level = 0; level < LEVELS - 1; ++level) {

        size_t level_shift_bits = (LEVELS - level - 1) * INDEX_BITS;
        index = (vpn >> level_shift_bits & (INDEX_MASK)); 
        pte = current_table[index];

        if (!(pte & 1)) {  
            return 0xFFFFFFFFFFFFFFFF; 
        }

        current_table = (size_t *)(pte & ~(OFFSET_MASK));  //table is set to physical page number for next level
    }

    index = vpn & (INDEX_MASK); 
    pte = current_table[index]; 

    if (!(pte & 1)) {
        return 0xFFFFFFFFFFFFFFFF;  
    }

    size_t offset = va & (OFFSET_MASK);
    size_t ppn = pte >> POBITS; //physical page number from taking off offset bits
    size_t pa = (ppn << POBITS) | offset; //physical address
    return pa;
}

void page_deallocate(size_t va) {
    if (ptbr == 0) {
        return; //do nothing if no page table register set (nothing to deallocate)
    }

    size_t *current_table = (size_t *)ptbr;
    size_t vpn = va >> POBITS;
    size_t index;
    size_t pte;

    for (int level = 0; level < LEVELS - 1; ++level) {
        size_t level_shift_bits = (LEVELS - level - 1) * INDEX_BITS;
        index = (vpn >> level_shift_bits) & (INDEX_MASK);
        pte = current_table[index];

        if (!(pte & 1)) { 
            return; //do nothing if the associated page table entry is not valid (nothing to deallocate at this virtual address)
        }

        current_table = (size_t *)(pte & ~(OFFSET_MASK));
    }

    index = vpn & (INDEX_MASK);
    pte = current_table[index];

    if (pte & 1) {  // Valid entry. Free the page associated here. 
        void* page_to_free = (void*)(pte & ~(OFFSET_MASK));
        free(page_to_free); // Free the physical page
        current_table[index] = 0; // Clear entry
    }
}


int main() {
    /*
    // 0 pages have been allocated
    assert(ptbr == 0);
    
    page_allocate(0x456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x456789abcd00);
    // no new pages allocated (still 5)
    
    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x456780000000);

    // 2 new pages allocated (now 8; 5 page table, 3 data)
    */
}
