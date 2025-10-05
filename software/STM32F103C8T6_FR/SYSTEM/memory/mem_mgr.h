#ifndef __MEM_MGR_H__
#define __MEM_MGR_H__

#include <stdint.h>
#include <stddef.h>

/* �ڴ������� */
typedef enum {
    MEM_REGION_INTERNAL,  /* �ڲ�SRAM */
    MEM_REGION_EXTERNAL,  /* �ⲿSRAM */
    MEM_REGION_COUNT
} mem_region_t;

/* �ڴ��������� */
typedef enum {
    MEM_OK = 0,
    MEM_INVALID_REGION,
    MEM_INVALID_SIZE,
    MEM_ALLOC_FAILED,
    MEM_FREE_FAILED,
    MEM_CORRUPTED
} mem_status_t;

/* �������� */
mem_status_t mem_init(void);
void* mem_alloc(mem_region_t region, size_t size);
mem_status_t mem_free(mem_region_t region, void* ptr);
mem_status_t mem_get_stats(mem_region_t region, size_t* total, size_t* used, size_t* free);
mem_status_t mem_check_integrity(mem_region_t region);

#endif /* __MEM_MGR_H__ */
