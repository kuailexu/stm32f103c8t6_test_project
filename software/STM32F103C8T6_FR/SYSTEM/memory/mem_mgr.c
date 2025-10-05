#include "mem_mgr.h"
#include <string.h>

/* 内存块结构 */
typedef struct mem_block {
    struct mem_block* next;
    size_t size;
    unsigned char used;  /* 使用标志 */
    unsigned int magic;  /* 魔术字 */
} mem_block_t;

/* 内存池信息 */
typedef struct {
    void* start_addr;
    size_t total_size;
    mem_block_t* free_list;
    unsigned int alloc_count;
    unsigned int free_count;
} mem_pool_t;

/* 常量定义 */
#define MEM_MAGIC 0xDEADBEEFU
#define MEM_BLOCK_HEADER_SIZE (sizeof(mem_block_t))

/* 内存池数组 */
static mem_pool_t mem_pools[MEM_REGION_COUNT];

/* 初始化内存池 */
static void mem_pool_init(mem_region_t region, void* start_addr, size_t size)
{
    mem_block_t* first_block;
    
    mem_pools[region].start_addr = start_addr;
    mem_pools[region].total_size = size;
    mem_pools[region].alloc_count = 0;
    mem_pools[region].free_count = 0;
    
    /* 初始化第一个空闲块 */
    first_block = (mem_block_t*)start_addr;
    first_block->next = NULL;
    first_block->size = size - MEM_BLOCK_HEADER_SIZE;
    first_block->used = 0;
    first_block->magic = MEM_MAGIC;
    
    mem_pools[region].free_list = first_block;
    mem_pools[region].free_count = 1;
}

/* 内存管理初始化 */
mem_status_t mem_init(void)
{
    /* 初始化内部SRAM (64KB) */
    mem_pool_init(MEM_REGION_INTERNAL, (void*)0x20000000, 20 * 1024);
    
//    /* 初始化外部SRAM (512KB) */
//    mem_pool_init(MEM_REGION_EXTERNAL, (void*)0x60000000, 512 * 1024);
    
    return MEM_OK;
}

/* 内存分配 */
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
    
    /* 4字节对齐 */
    size = (size + 3) & ~0x03;
    
    /* 查找合适的空闲块 */
    while (curr != NULL) {
        if (!curr->used && curr->size >= size) {
            /* 检查魔术字 */
            if (curr->magic != MEM_MAGIC) {
                return NULL;
            }
            
            /* 检查是否需要分割块 */
            remaining = curr->size - size;
            if (remaining > (MEM_BLOCK_HEADER_SIZE + 4)) {
                /* 分割块 */
                new_block = (mem_block_t*)((unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + size);
                new_block->next = curr->next;
                new_block->size = remaining - MEM_BLOCK_HEADER_SIZE;
                new_block->used = 0;
                new_block->magic = MEM_MAGIC;
                
                curr->next = new_block;
                curr->size = size;
                pool->free_count++;
            }
            
            /* 标记为已使用 */
            curr->used = 1;
            
            /* 从空闲链表移除 */
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
    
    return NULL; /* 没有足够空间 */
}

/* 内存释放 */
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
    
    /* 检查魔术字 */
    if (block->magic != MEM_MAGIC) {
        return MEM_CORRUPTED;
    }
    
    /* 检查是否已经是空闲状态 */
    if (!block->used) {
        return MEM_FREE_FAILED;
    }
    
    /* 标记为空闲 */
    block->used = 0;
    
    /* 插入到空闲链表头部 */
    block->next = pool->free_list;
    pool->free_list = block;
    
    pool->alloc_count--;
    pool->free_count++;
    
    /* 合并相邻的空闲块 */
    curr = pool->free_list;
    while (curr != NULL && curr->next != NULL) {
        if (!curr->used && !curr->next->used && 
            (unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size == (unsigned char*)curr->next) {
            /* 合并块 */
            curr->size += MEM_BLOCK_HEADER_SIZE + curr->next->size;
            curr->next = curr->next->next;
            pool->free_count--;
        } else {
            curr = curr->next;
        }
    }
    
    return MEM_OK;
}

/* 获取内存使用统计 */
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

/* 内存完整性检查 */
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
        /* 检查魔术字 */
        if (curr->magic != MEM_MAGIC) {
            return MEM_CORRUPTED;
        }
        
        /* 检查块大小是否有效 */
        if (curr->size > pool->total_size || 
            (unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size > 
            (unsigned char*)pool->start_addr + pool->total_size) {
            return MEM_CORRUPTED;
        }
        
        curr = (mem_block_t*)((unsigned char*)curr + MEM_BLOCK_HEADER_SIZE + curr->size);
    }
    
    return MEM_OK;
}
