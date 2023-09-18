#include "os_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <iostream>

using namespace std;

mem_handle_t* dataBlocks;
mem_handle_t* freeBlocks;
int availableDiskSize;
bool isCreated = false;
int dataBlocksSize;
int freeBlocksSize;

int max_size = 256;

#pragma region AssistFunctions
void SortFreeBlocks_new()
{
    mem_handle_t* temp;
    for (int i = 0; i < freeBlocksSize; i++)
    {
        for (int j = 0; j < freeBlocksSize - i; j++)
        {
            if (freeBlocks[j].addr > freeBlocks[j + 1].addr)
            {
                temp = &freeBlocks[j + 1];
                freeBlocks[j + 1] = freeBlocks[j];
                freeBlocks[j] = *temp;
            }
        }
    }
}

void SortFreeBlocks()
{
    mem_handle_t temp = mem_handle_t(0,0);
    for (int i = 0; i < freeBlocksSize; i++)
    {
        for (int j = 0; j < freeBlocksSize - i; j++)
        {
            if (freeBlocks[j].addr > freeBlocks[j + 1].addr)
            {
                temp = freeBlocks[j + 1];
                freeBlocks[j + 1] = freeBlocks[j];
                freeBlocks[j] = temp;
            }
        }
    }
}

/*void SortDataBlocks()
{
    mem_handle_t temp = mem_handle_t(0, 0);
    for (int i = 0; i < dataBlocksSize - 1; i++)
    {
        for (int j = 0; j < dataBlocksSize - 1 - i; j++)
        {
            if (dataBlocks[j].addr > dataBlocks[j + 1].addr)
            {
                temp = dataBlocks[j + 1];
                dataBlocks[j + 1] = dataBlocks[j];
                dataBlocks[j] = temp;
            }
        }
        freeBlocksSize--;
    }
}*/
#pragma endregion

int my_create(int size, int num_pages)
{
    if (isCreated) return 0;
    availableDiskSize = size;
    dataBlocks = (mem_handle_t*)malloc(max_size*sizeof(mem_handle_t));
    freeBlocks = (mem_handle_t*)malloc(max_size*sizeof(mem_handle_t));
    dataBlocksSize = 0;
    freeBlocksSize = 1;
    freeBlocks[0].size = size;
    freeBlocks[0].addr = 0;
    isCreated = true;
    return 1;
}

int my_destroy()
{
    if (!isCreated) return 0;
    free(freeBlocks);
    free(dataBlocks);
    availableDiskSize = 0;
    isCreated = false;
    return 1;
}

mem_handle_t my_alloc (int block_size)
{
    if (!isCreated) return (mem_handle_t(0, 0));
    if (availableDiskSize == 0)
    {
        return (mem_handle_t(0, 0));
    }
    int bestBlockSize = availableDiskSize + 1;
    int bestBlockIndex = 0;
    bool isFound = false;
    for (int i = 0; i < freeBlocksSize; i++)
    {
        if (block_size <= freeBlocks[i].size)
        {
            if (freeBlocks[i].size == block_size)
            {
                bestBlockSize = freeBlocks[i].size;
                bestBlockIndex = i;
                isFound = true;
                break;
            }
            if (freeBlocks[i].size < bestBlockSize)
            {
                bestBlockSize = freeBlocks[i].size;
                bestBlockIndex = i;
            }
            isFound = true;
        }
    }
    if (isFound)
    {
        dataBlocks[dataBlocksSize].addr = freeBlocks[bestBlockIndex].addr;
        dataBlocks[dataBlocksSize].size = block_size;
        dataBlocksSize++;

        if(block_size != bestBlockSize)
        {
            freeBlocks[bestBlockIndex].addr += block_size;
            freeBlocks[bestBlockIndex].size -= block_size;
        }
        else
        {
            for (int i = 0; i < freeBlocksSize; i++)
            {
                if (freeBlocks[i].addr == bestBlockIndex)
                {
                    for (int j = i; j < freeBlocksSize - 1; j++)
                    {
                        freeBlocks[j] = freeBlocks[j+1];
                    }
                    break;
                }
            }
            freeBlocksSize--;
        }
        availableDiskSize -= block_size;
        return dataBlocks[dataBlocksSize - 1];
    }
    else
    {
        return (mem_handle_t(0, 0));
    }
}

int my_free(mem_handle_t h)
{
    if (!isCreated) return 0;
    for (int i = 0; i < dataBlocksSize; i++)
    {
        if (dataBlocks[i].addr == h.addr && dataBlocks[i].size == h.size)
        {
            availableDiskSize += dataBlocks[i].size;
            freeBlocksSize++;
            freeBlocks[freeBlocksSize - 1].addr = dataBlocks[i].addr;
            freeBlocks[freeBlocksSize - 1].size += dataBlocks[i].size;
            for (int j = i; j < dataBlocksSize - 1; j++)
            {
                dataBlocks[j] = dataBlocks[j+1];
            }
            dataBlocksSize--;
            SortFreeBlocks_new();

            int j = 0;
            while (j < freeBlocksSize - 1)
            {
                if (freeBlocks[j].addr + freeBlocks[j].size == freeBlocks[j + 1].addr)
                {
                    freeBlocks[j].size += freeBlocks[j+1].size;
                    for (int l = j+1; l < freeBlocksSize - 1; l++)
                    {
                        freeBlocks[l] = freeBlocks[l+1];
                    }
                    freeBlocksSize--;
                }
                else
                {
                    j++;
                }
            }
            return 1;
        }
    }
    return 0;
}

int my_get_max_block_size()
{
    if (!isCreated) return 0;
    int maxFreeBlockSize = 0;
    for (int i = 0; i < freeBlocksSize; i++)
    {
        if (freeBlocks[i].size > maxFreeBlockSize) maxFreeBlockSize = freeBlocks[i].size;
    }
    return(maxFreeBlockSize);
}

int my_get_free_space()
{
    if (!isCreated) return 0;
    return (availableDiskSize);
}

void my_print_blocks()
{
    if (!isCreated) return;
    printf("\n");
    for(int i = 0; i < dataBlocksSize; i++)
    {
        printf("%d %d\n", dataBlocks[i].addr, dataBlocks[i].size);
    }
}

void setup_memory_manager(memory_manager_t* mm)
{
    mm -> destroy = my_destroy;
    mm -> create = my_create;
    mm -> alloc = my_alloc;
    mm -> get_free_space = my_get_free_space;
    mm -> print_blocks = my_print_blocks;
    mm -> free = my_free;
    mm -> get_max_block_size = my_get_max_block_size;
}