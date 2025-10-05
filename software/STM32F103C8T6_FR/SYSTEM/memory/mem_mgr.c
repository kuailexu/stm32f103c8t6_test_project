#include "mem_mgr.h"
#include <string.h>

/* �ڴ��ṹ */
typedef struct mem_block {
    struct mem_block* next;
    size_t size;
    unsigned char used;  /* ʹ�ñ�־ */
    unsigned int magic;  /* ħ���� */
} mem_block_t;

/* �ڴ����Ϣ */
typedef struct {
    void* start_addr;
    size_t total_size;
    mem_block_t* free_list;
    unsigned int alloc_count;
    unsigned int free_count;
} mem_pool_t;

/* �������� */
#define MEM_MAGIC 0xDEADBEEFU
#define MEM_BLOCK_HEADER_SIZE (sizeof(mem_block_t))

/* �ڴ������ */
static mem_pool_t mem_pools[MEM_REGION_COUNT];

/* ��ʼ���ڴ�� */
static void mem_pool_init(mem_region_t region, void* start_addr, size_t size)
{
    mem_block_t* first_block;
    
    mem_pools[region].start_addr = start_addr;
    mem_pools[region].total_size = size;
    mem_pools[region].alloc_count = 0;
    mem_pools[region].free_count = 0;
    
    /* ��ʼ����һ�����п� */
    first_block = (mem_block_t*)start_addr;
    first_block->next = NULL;
    first_block->size = size - MEM_BLOCK_HEADER_SIZE;
    first_block->used = 0;
    first_block->magic = MEM_MAGIC;
    
    mem_pools[region].free_list = first_block;
    mem_pools[region].free_count = 1;
}

/* �ڴ�����ʼ�� */
mem_status_t mem_init(void)
{
    /* ��ʼ���ڲ�SRAM (64KB) */
    mem_pool_init(MEM_REGION_INTERNAL, (void*)0x20000000, 20 * 1024);
    
//    /* ��ʼ���ⲿSRAM (512KB) */
//    mem_pool_init(MEM_REGION_EXTERNAL, (void*)0x60000000, 512 * 1024);
    
    return MEM_OK;
}

/* �ڴ���� */
void* mem_alloc(mem_region_t region, size_t size)
{
    mem_pool_t* pool;
    mem_block_t* prev;
    mem_block_t* curr;
    mem_block_t* new_block;
    size_t remaining;
    
    if (region >= MEM_REGION_COUNT) {
        return NULL;
    }
    
    if (size == 0) {
        return NULL;
    }
    
    pool = &mem_pools[region];
    prev = NULL;
    curr = pool->free_list;
    
    /* 4�ֽڶ��� */
    size = (size + 3) & ~0x03;
    
    /* ���Һ��ʵĿ��п� */
    while (curr != NULL) {
        if (!curr->used && curr->size >= size) {
            /* ���ħ���� */
            if (curr->magic != MEM_MAGIC) {
                return NULL;
            }
            
            /* ����Ƿ���Ҫ�ָ�� */
            remaining = curr->size - size;
            if (remaining > (MEM_BLOCK_HEADER_SIZE + 4)) {
                /* �ָ�� */
                new_block = (mem_block_t*)((unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + size);
                new_block->next = curr->next;
                new_block->size = remaining - MEM_BLOCK_HEADER_SIZE;
                new_block->used = 0;
                new_block->magic = MEM_MAGIC;
                
                curr->next = new_block;
                curr->size = size;
                pool->free_count++;
            }
            
            /* ���Ϊ��ʹ�� */
            curr->used = 1;
            
            /* �ӿ��������Ƴ� */
            if (prev == NULL) {
                pool->free_list = curr->next;
            } else {
                prev->next = curr->next;
            }
            
            pool->alloc_count++;
            pool->free_count--;
            
            return (void*)((unsigned char*)curr + MEM_BLOCK_HEADER_SIZE);
        }
        
        prev = curr;
        curr = curr->next;
    }
    
    return NULL; /* û���㹻�ռ� */
}

/* �ڴ��ͷ� */
mem_status_t mem_free(mem_region_t region, void* ptr)
{
    mem_pool_t* pool;
    mem_block_t* block;
    mem_block_t* curr;
    
    if (region >= MEM_REGION_COUNT) {
        return MEM_INVALID_REGION;
    }
    
    if (ptr == NULL) {
        return MEM_FREE_FAILED;
    }
    
    pool = &mem_pools[region];
    block = (mem_block_t*)((unsigned char*)ptr - MEM_BLOCK_HEADER_SIZE);
    
    /* ���ħ���� */
    if (block->magic != MEM_MAGIC) {
        return MEM_CORRUPTED;
    }
    
    /* ����Ƿ��Ѿ��ǿ���״̬ */
    if (!block->used) {
        return MEM_FREE_FAILED;
    }
    
    /* ���Ϊ���� */
    block->used = 0;
    
    /* ���뵽��������ͷ�� */
    block->next = pool->free_list;
    pool->free_list = block;
    
    pool->alloc_count--;
    pool->free_count++;
    
    /* �ϲ����ڵĿ��п� */
    curr = pool->free_list;
    while (curr != NULL && curr->next != NULL) {
        if (!curr->used && !curr->next->used && 
            (unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size == (unsigned char*)curr->next) {
            /* �ϲ��� */
            curr->size += MEM_BLOCK_HEADER_SIZE + curr->next->size;
            curr->next = curr->next->next;
            pool->free_count--;
        } else {
            curr = curr->next;
        }
    }
    
    return MEM_OK;
}

/* ��ȡ�ڴ�ʹ��ͳ�� */
mem_status_t mem_get_stats(mem_region_t region, size_t* total, size_t* used, size_t* free)
{
    mem_pool_t* pool;
    mem_block_t* curr;
    
    if (region >= MEM_REGION_COUNT) {
        return MEM_INVALID_REGION;
    }
    
    pool = &mem_pools[region];
    
    if (total != NULL) {
        *total = pool->total_size;
    }
    
    if (used != NULL) {
        *used = 0;
        curr = (mem_block_t*)pool->start_addr;
        while ((unsigned char*)curr < (unsigned char*)pool->start_addr + pool->total_size) {
            if (curr->used) {
                *used += curr->size + MEM_BLOCK_HEADER_SIZE;
            }
            curr = (mem_block_t*)((unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size);
        }
    }
    
    if (free != NULL) {
        *free = 0;
        curr = pool->free_list;
        while (curr != NULL) {
            *free += curr->size + MEM_BLOCK_HEADER_SIZE;
            curr = curr->next;
        }
    }
    
    return MEM_OK;
}

/* �ڴ������Լ�� */
mem_status_t mem_check_integrity(mem_region_t region)
{
    mem_pool_t* pool;
    mem_block_t* curr;
    
    if (region >= MEM_REGION_COUNT) {
        return MEM_INVALID_REGION;
    }
    
    pool = &mem_pools[region];
    curr = (mem_block_t*)pool->start_addr;
    
    while ((unsigned char*)curr < (unsigned char*)pool->start_addr + pool->total_size) {
        /* ���ħ���� */
        if (curr->magic != MEM_MAGIC) {
            return MEM_CORRUPTED;
        }
        
        /* �����С�Ƿ���Ч */
        if (curr->size > pool->total_size || 
            (unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size > 
            (unsigned char*)pool->start_addr + pool->total_size) {
            return MEM_CORRUPTED;
        }
        
        curr = (mem_block_t*)((unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size);
    }
    
    return MEM_OK;
}
