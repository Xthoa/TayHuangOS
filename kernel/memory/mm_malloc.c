/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 * 
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 * 
 * --------------------------------------------------------------------------------
 * 
 * 作者: Flysong
 * 
 * mm_malloc.c
 * 
 * malloc系函数
 * 
 */



#include "mm_malloc.h"
#include "paging.h"
#include "pmm.h"

#include <tayhuang/paging.h>

#include <process/task_manager.h>

#include <debug/logging.h>

PUBLIC void *cpmalloc(int size) {
    int pages = size / MEMUNIT_SZ + ((size % MEMUNIT_SZ) != 0);

    void *addr = find_continuous_freepages(pages);

    for (int i = 0 ; i < pages ; i ++)
        mark_used(addr + i * MEMUNIT_SZ);

    return addr;
}

//一次分配16的倍数个内存
#define HEAPUNIT_SZ (16)
//小于等于该值时不再对区块进行分割
#define HEAPDIV_MIN_SZ (128)

//chunk起始地址 + 16B 即为数据所在地址
typedef struct _chunk {
    qword size : 63;
    bool used : 1;
    struct _chunk *next;
} chunk; //16B
//堆开头16个Byte为空闲chunk头

PUBLIC void theap_init(int pid) {
    task_struct *task = find_task(pid);
    void *heap = task->mm_info->start_heap;

    chunk *whole_heap_chunk = (chunk*)(heap + 2);
    whole_heap_chunk->size = task->mm_info->end_heap - task->mm_info->start_heap - 2;
    whole_heap_chunk->used = false;
    whole_heap_chunk->next = NULL;

    chunk *free_chunk_head = (chunk*)heap;
    free_chunk_head->size = -1;
    free_chunk_head->used = false;
    free_chunk_head->next = whole_heap_chunk;
}

PUBLIC void vheap_init(void) {
    theap_init(current_task->pid);
}

PUBLIC void *tmalloc(int size, int pid) {
    task_struct *task = find_task(pid);
    void *heap = task->mm_info->start_heap;
    chunk *free_chunk = (chunk*)(heap + 2);
    chunk *last = (chunk*)heap;

    //修正大小
    int fix_size = ((size / HEAPUNIT_SZ) + (size % HEAPUNIT_SZ != 0)) * HEAPUNIT_SZ;

    //寻找符合条件的chunk
    while (free_chunk->size < (fix_size + 2) && (free_chunk->next != NULL)) {
        last = free_chunk;
        free_chunk = free_chunk->next;
    }

    if (free_chunk->size < (fix_size + 2)) {
        //TODO: extend the heap
    }

    //TODO: div the chunk

    //进行分割
    if (free_chunk->size > HEAPDIV_MIN_SZ) {
        chunk *new_chunk = ((void*)free_chunk) + 2 + fix_size;
        new_chunk->next = free_chunk->next;
        new_chunk->size = free_chunk->size - 2 - fix_size;
        new_chunk->used = false;
        last->next = new_chunk;

        free_chunk->size = fix_size + 2;
    }
    else {
        last->next = NULL;
    }

    //更改信息
    free_chunk->used = true;
    free_chunk->next = NULL;

    return ((void*)free_chunk) + 2;
}

PUBLIC void *vmalloc(int size) {
    return tmalloc(size, current_task->pid);
}

PUBLIC void cpfree(void *addr, int size) {
    int pages = size / MEMUNIT_SZ + ((size % MEMUNIT_SZ) != 0);

    for (int i = 0 ; i < pages ; i ++)
        mark_unused(addr + i * MEMUNIT_SZ);
}

PUBLIC void tfree(void *addr, int pid) {
    task_struct *task = find_task(pid);
    void *heap = task->mm_info->start_heap;

    //空闲chunk链表头
    chunk *free_chunk = (chunk*)(heap + 2);

    //当前chunk
    chunk *cur_chunk = (chunk*)(addr - 2);
    if (! cur_chunk->used) {
        lwarn ("try to free a unused chunk!");
        return;
    }

    //设为未使用
    cur_chunk->used = false;
    cur_chunk->next = NULL;

    //寻找链表尾
    while (free_chunk->next != NULL) {
        free_chunk = free_chunk->next;
    }

    free_chunk->next = cur_chunk; //插入
}

PUBLIC void vfree(void *addr) {
    return tfree(addr, current_task->pid);
}