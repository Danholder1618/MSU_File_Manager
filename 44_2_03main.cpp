#include "os_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

int main()
{
    memory_manager_t mm;
    setup_memory_manager(&mm);

    mm.create(1000,1);
    mm.create(1000,1);
    mm.alloc(1);
    mm.alloc(2);
    mm.alloc(3);
    mm.alloc(4);
    mm.alloc(100);
    mm.free(mem_handle_t(0,1));
    mm.free(mem_handle_t(1,2));
    mm.free(mem_handle_t(3,3));
    mm.print_blocks();
    mm.alloc(100);
    mm.print_blocks();
    return 0;
}