#include <stddef.h>

/**
 * Page table base register.
 * Declared here so tester code can look at it; because it is extern
 * you'll need to define it (without extern) in exactly one .c file.
 */
extern size_t ptbr;

/**
 * Given a virtual address, return the physical address.
 * Return a value consisting of all 1 bits
 * if this virtual address does not have a physical address.
 */
size_t translate(size_t va);

/**
 * Use posix_memalign to create page tables and other pages sufficient
 * to have a mapping between the given virtual address and some physical address.
 * If there already is such a page, does nothing.
 */
void page_allocate(size_t va);

/**
 * Use the virtual address to find an associated physical page address. 
 * Clears the page if there is a valid associated page table entry for a page. 
 * If the virtual address does not correspond with a valid page table entry, does nothing.
 */
void page_deallocate(size_t va);