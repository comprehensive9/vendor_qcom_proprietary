/******************************************************************************
 * #  Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
 * #  All Rights Reserved.
 * #  Confidential and Proprietary - Qualcomm Technologies, Inc.
 * #******************************************************************************/

#include <stdint.h>
#include "utils_internal.h"
#include "qcril_memory_management.h"
#include "QtiMutex.h"


#define MEM_PADDING "QCRILQMIqcrilqmi"
#define MEM_PADDING_SIZE 16
using namespace std;

struct qcril_heap_list_info
{
  uint32_t              mem_id;
  char                  func_name[32];
  void*                 addr;
  uint32_t              line_number;
  size_t                size;
  qcril_heap_list_info *next;
  qcril_heap_list_info *prev;
};

static qtimutex::QtiSharedMutex memory_list_mutex;
static qcril_heap_list_info *qcril_heap_memory_list = nullptr;
#ifdef QCRIL_HEAP_MEMORY_TRACK
static uint32_t heap_memory_id = 0;
//static const struct timeval HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY = {60,0};
#endif

//===========================================================================
// qcril_find_heap_memory_locked
//===========================================================================
qcril_heap_list_info **qcril_find_heap_memory_locked(void* addr)
{
  qcril_heap_list_info **i;

  for (i = &qcril_heap_memory_list; *i ; i = &((*i)->next)) {
    if ((*i)->addr == addr) {
      break;
    }
  }

  return i;
} // qcril_find_heap_memory_locked

//===========================================================================
// qcril_remove_heap_memory_from_list
//===========================================================================
qcril_heap_list_info *qcril_remove_heap_memory_from_list(void* addr)
{
  qcril_heap_list_info **i, *ret;
  lock_guard<qtimutex::QtiSharedMutex> lock(memory_list_mutex);
  i = qcril_find_heap_memory_locked(addr);

  ret = *i;
  if ( NULL != ret )
  {
    if ( NULL != ret->next )
    {
      ret->next->prev = ret->prev;
    }
    *i = ret->next;

    ret->next = NULL;
    ret->prev = NULL;
  }

  return ret;
} // qcril_remove_heap_memory_from_list

//===========================================================================
// qcril_add_heap_memory_to_list
//===========================================================================
void qcril_add_heap_memory_to_list(qcril_heap_list_info *info)
{
  qcril_heap_list_info **i;
  qcril_heap_list_info *prev = NULL;

  lock_guard<qtimutex::QtiSharedMutex> lock(memory_list_mutex);

  for (i = &qcril_heap_memory_list; *i; i = &((*i)->next))
  {
    prev = *i;
  }
  *i = info;
  info->next = NULL;
  info->prev = prev;
} // qcril_add_heap_memory_to_list

//===========================================================================
// qcril_print_heap_memory_list
//===========================================================================
void qcril_print_heap_memory_list(void *arg)
{
  qcril_heap_list_info *i;
  (void)arg;
  UTIL_LOG_INFO(" ************ print heap memory list ************");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(memory_list_mutex);
  for (i = qcril_heap_memory_list; i; i = i->next)
  {
    UTIL_LOG_INFO("\tid: %u, func: %s, line: %u, size: %u", i->mem_id, i->func_name, i->line_number, i->size);
  }
  lock.unlock();
#if 0
  qcril_setup_timed_callback(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                             qcril_print_heap_memory_list, &HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY, NULL );
#endif
} // qcril_print_heap_memory_list

/*===========================================================================

  FUNCTION:  qcril_malloc_adv

===========================================================================*/
/*!
    @brief
    Allocate memory from heap.

    @return
    Pointer to allocated memory region.
*/
/*=========================================================================*/
void *qcril_malloc_adv
(
  size_t size,
  const char* func_name,
  int line_num
)
{
  void *mem_ptr = NULL;

  /*-----------------------------------------------------------------------*/
#ifdef QCRIL_HEAP_MEMORY_TRACK
  mem_ptr = (qcril_heap_list_info *)malloc(size + 2 * MEM_PADDING_SIZE);

  if ( NULL != mem_ptr )
  {
    qcril_heap_list_info *heap_memory_info = (qcril_heap_list_info *)malloc(sizeof(*heap_memory_info));
    if ( NULL != heap_memory_info )
    {
      memcpy(mem_ptr, MEM_PADDING, MEM_PADDING_SIZE);
      mem_ptr = (void*)((char*)mem_ptr + MEM_PADDING_SIZE);
      memset( mem_ptr, 0, size );
      memcpy((void*)((char*)mem_ptr+size), MEM_PADDING, MEM_PADDING_SIZE);

      heap_memory_info->addr = mem_ptr;
      strlcpy(heap_memory_info->func_name, func_name,
              (sizeof(heap_memory_info->func_name) / sizeof(heap_memory_info->func_name[0])));
      heap_memory_info->line_number = line_num;
      heap_memory_info->size = size;
      std::unique_lock<qtimutex::QtiSharedMutex> lock(memory_list_mutex);
      heap_memory_info->mem_id = heap_memory_id++;
      UTIL_LOG_INFO( "malloc new memory: id: %d, func: %s", heap_memory_info->mem_id, heap_memory_info->func_name );
      lock.unlock();

      UTIL_LOG_INFO("Adding new addr %p to memory list", mem_ptr);
      qcril_add_heap_memory_to_list(heap_memory_info);
    }
    else
    {
      memset( mem_ptr, 0, size );
      UTIL_LOG_ERROR( "Fail to allocate memory" );
    }
  }
  else
  {
    UTIL_LOG_ERROR( "Fail to allocate memory" );
  }
#else
  (void)func_name;
  (void)line_num;
  mem_ptr = malloc( size );
  if ( NULL != mem_ptr )
  {
    memset( mem_ptr, 0, size );
  }
  else
  {
    UTIL_LOG_ERROR( "Fail to allocate memory" );
  }
#endif

  return mem_ptr;
} /* qcril_malloc_adv */


/*===========================================================================

  FUNCTION:  qcril_free_adv

===========================================================================*/
/*!
    @brief
    Free specified memory region.

    @return
    None.
*/
/*=========================================================================*/
void qcril_free_adv
(
  void *mem_ptr,
  const char* func_name,
  int line_num
)
{

  /*-----------------------------------------------------------------------*/

  if ( mem_ptr == NULL )
  {
      return;
  }

  /*-----------------------------------------------------------------------*/

#ifdef QCRIL_HEAP_MEMORY_TRACK
  UTIL_LOG_INFO("removing heap memory: %p from list", mem_ptr);
  qcril_heap_list_info *heap_mem_info = qcril_remove_heap_memory_from_list(mem_ptr);
  if (NULL == heap_mem_info)
  {
    UTIL_LOG_ERROR("Memory %p not found in list. Called from %s line %d", mem_ptr, func_name, line_num);
  }
  else
  {
    // check buffer underflow
    if ( 0 != memcmp((void*)(((char*)mem_ptr) - MEM_PADDING_SIZE), (void*)MEM_PADDING, MEM_PADDING_SIZE ) )
    {
      UTIL_LOG_FATAL("memory underflow!");
    }
    // check buffer overflow
    if ( 0 != memcmp((void*)((char*)(mem_ptr) + heap_mem_info->size), (void*)MEM_PADDING, MEM_PADDING_SIZE) )
    {
      UTIL_LOG_FATAL("memory overflow!");
    }

    mem_ptr = (void*)(((char*)mem_ptr) - MEM_PADDING_SIZE);
    free(heap_mem_info);
  }
  if (NULL != mem_ptr)
  {
    free( mem_ptr );
  }
  else
  {
    UTIL_LOG_DEBUG("Memory pointer is NULL after shifting the padding size");
  }
#else
  (void)func_name;
  (void)line_num;
  if (NULL != mem_ptr)
  {
    free( mem_ptr );
  }
#endif

  return;
} /* qcril_free_adv */
