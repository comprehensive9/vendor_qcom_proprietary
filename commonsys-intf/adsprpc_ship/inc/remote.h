/*
 * Copyright (c) 2012-2014,2016,2017,2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef REMOTE_H
#define REMOTE_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t remote_handle;
typedef uint64_t remote_handle64; //! used by multi domain modules
                                  //! 64 bit handles are translated to 32 bit values
                                  //! by the transport layer
typedef uint64_t fastrpc_async_jobid; //Job id of Async job queued to DSP

typedef struct {
   void *pv;
   size_t nLen;
} remote_buf;

typedef struct {
   int32_t fd;
   uint32_t offset;
} remote_dma_handle;

typedef union {
   remote_buf     buf;
   remote_handle    h;
   remote_handle64 h64; //! used by multi domain modules
   remote_dma_handle dma;
} remote_arg;

//Async response type
enum fastrpc_async_notify_type {
   FASTRPC_ASYNC_NO_SYNC = 0,
   FASTRPC_ASYNC_CALLBACK,
   FASTRPC_ASYNC_POLL,
   FASTRPC_ASYNC_TYPE_MAX,
};

//Async call back response type, input structure
typedef struct fastrpc_async_callback {
   void (*fn)(fastrpc_async_jobid jobid, void* context, int result);
   void *context;
}fastrpc_async_callback_t;

//Async descriptor to submit async job
typedef struct fastrpc_async_descriptor {
   enum fastrpc_async_notify_type type;
   fastrpc_async_jobid jobid;
   union {
      fastrpc_async_callback_t cb;
   };
}fastrpc_async_descriptor_t;

/*Retrives method attribute from the scalars parameter*/
#define REMOTE_SCALARS_METHOD_ATTR(dwScalars)   (((dwScalars) >> 29) & 0x7)

/*Retrives method index from the scalars parameter*/
#define REMOTE_SCALARS_METHOD(dwScalars)        (((dwScalars) >> 24) & 0x1f)

/*Retrives number of input buffers from the scalars parameter*/
#define REMOTE_SCALARS_INBUFS(dwScalars)        (((dwScalars) >> 16) & 0x0ff)

/*Retrives number of output buffers from the scalars parameter*/
#define REMOTE_SCALARS_OUTBUFS(dwScalars)       (((dwScalars) >> 8) & 0x0ff)

/*Retrives number of input handles from the scalars parameter*/
#define REMOTE_SCALARS_INHANDLES(dwScalars)     (((dwScalars) >> 4) & 0x0f)

/*Retrives number of output handles from the scalars parameter*/
#define REMOTE_SCALARS_OUTHANDLES(dwScalars)    ((dwScalars) & 0x0f)

#define REMOTE_SCALARS_MAKEX(nAttr,nMethod,nIn,nOut,noIn,noOut) \
          ((((uint32_t)   (nAttr) &  0x7) << 29) | \
           (((uint32_t) (nMethod) & 0x1f) << 24) | \
           (((uint32_t)     (nIn) & 0xff) << 16) | \
           (((uint32_t)    (nOut) & 0xff) <<  8) | \
           (((uint32_t)    (noIn) & 0x0f) <<  4) | \
            ((uint32_t)   (noOut) & 0x0f))

#define REMOTE_SCALARS_MAKE(nMethod,nIn,nOut)  REMOTE_SCALARS_MAKEX(0,nMethod,nIn,nOut,0,0)

#define REMOTE_SCALARS_LENGTH(sc) (REMOTE_SCALARS_INBUFS(sc) +\
                                   REMOTE_SCALARS_OUTBUFS(sc) +\
                                   REMOTE_SCALARS_INHANDLES(sc) +\
                                   REMOTE_SCALARS_OUTHANDLES(sc))

#ifndef __QAIC_REMOTE
#define __QAIC_REMOTE(ff) ff
#endif //__QAIC_REMOTE

#ifndef __QAIC_REMOTE_EXPORT
#ifdef _WIN32
#define __QAIC_REMOTE_EXPORT __declspec(dllexport)
#else //_WIN32
#define __QAIC_REMOTE_EXPORT
#endif //_WIN32
#endif //__QAIC_REMOTE_EXPORT

#ifndef __QAIC_REMOTE_ATTRIBUTE
#define __QAIC_REMOTE_ATTRIBUTE
#endif

#define NUM_DOMAINS 4
#define NUM_SESSIONS 2
#define DOMAIN_ID_MASK 3

#ifndef DEFAULT_DOMAIN_ID
#define DEFAULT_DOMAIN_ID 0
#endif

#define ADSP_DOMAIN_ID    0
#define MDSP_DOMAIN_ID    1
#define SDSP_DOMAIN_ID    2
#define CDSP_DOMAIN_ID    3

#define ADSP_DOMAIN "&_dom=adsp"
#define MDSP_DOMAIN "&_dom=mdsp"
#define SDSP_DOMAIN "&_dom=sdsp"
#define CDSP_DOMAIN "&_dom=cdsp"

/* All other values are reserved */

/* opens a remote_handle "name"
 * returns 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_open)(const char* name, remote_handle *ph) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_open)(const char* name, remote_handle64 *ph) __QAIC_REMOTE_ATTRIBUTE;

/* invokes the remote handle
 * see retrive macro's on dwScalars format
 * pra, contains the arguments in the following order, inbufs, outbufs, inhandles, outhandles.
 * implementors should ignore and pass values as is that the transport doesn't understand.
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_invoke)(remote_handle h, uint32_t dwScalars, remote_arg *pra) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_invoke)(remote_handle64 h, uint32_t dwScalars, remote_arg *pra) __QAIC_REMOTE_ATTRIBUTE;

/* invokes the remote handle asynchronously
 * desc, descriptor contains type of asyncjob. context and call back function(if any)
 * see retrive macro's on dwScalars format
 * pra, contains the arguments in the following order, inbufs, outbufs, inhandles, outhandles.
 * all outbufs need to be either allocated using rpcmem_alloc or registered ION buffers using register_buf
 * implementors should ignore and pass values as is that the transport doesn't understand.
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_invoke_async)(remote_handle h, fastrpc_async_descriptor_t *desc, uint32_t dwScalars, remote_arg *pra) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_invoke_async)(remote_handle64 h, fastrpc_async_descriptor_t *desc, uint32_t dwScalars, remote_arg *pra) __QAIC_REMOTE_ATTRIBUTE;

/* closes the remote handle
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_close)(remote_handle h) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_close)(remote_handle64 h) __QAIC_REMOTE_ATTRIBUTE;

/* remote handle control interface
 */
enum handle_control_req_id {
	DSPRPC_CONTROL_LATENCY	=	1,
	DSPRPC_GET_DSP_INFO		=	2,
	DSPRPC_CONTROL_WAKELOCK	=	3,
	DSPRPC_GET_DOMAIN	=	4,
};

enum remote_rpc_latency_flags {
   RPC_DISABLE_QOS = 0,
/* Control cpu low power modes based on RPC activity in 100 ms window.
 * Recommended for latency sensitive use cases.
 */
   RPC_PM_QOS = 1,
/* DSP driver predicts completion time of a method and send CPU wake up signal to reduce wake up latency.
 * Recommended for moderate latency sensitive use cases. It is more power efficient compared to pm_qos control.
 */
   RPC_ADAPTIVE_QOS = 2,
};

// Used with DSPRPC_CONTROL_LATENCY req ID
struct remote_rpc_control_latency {
/* Enable latency optimization techniques to meet requested latency. Use remote_rpc_latency_flags */
   uint32_t enable;

/* Required latency in micro-sec. 0 us is ignored and considered as default latency (appr. more than 750 us).
 * RPC latency highly impacted by cpu wake up latency which varies based on multiple parameters and concurrency in the system.
 * Driver tries to meet latency requested but not guaranteed.
 */
   uint32_t latency;
};

/**
 * @enum remote_dsp_attributes
 * @brief Different types of DSP capabilities queried via remote_handle_control
 * using DSPRPC_GET_DSP_INFO request id.
 */
enum remote_dsp_attributes {
   DOMAIN_SUPPORT              = 0,          /**< Check if DSP supported: supported = 1,
                                                unsupported = 0 */
   UNSIGNED_PD_SUPPORT         = 1,          /**< DSP unsigned PD support: supported = 1,
                                                unsupported = 0 */
   HVX_SUPPORT_64B             = 2,          /**< Number of HVX 64B support */
   HVX_SUPPORT_128B            = 3,          /**< Number of HVX 128B support */
   VTCM_PAGE                   = 4,          /**< Max page size allocation possible in VTCM */
   VTCM_COUNT                  = 5,          /**< Number of page_size blocks available */
   ARCH_VER                    = 6,          /**< Hexagon processor architecture version */
   HMX_SUPPORT_DEPTH           = 7,          /**< HMX Support Depth */
   HMX_SUPPORT_SPATIAL         = 8,          /**< HMX Support Spatial */
   ASYNC_FASTRPC_SUPPORT       = 9,          /**< Async FastRPC Support */
   FASTRPC_MAX_DSP_ATTRIBUTES  = ASYNC_FASTRPC_SUPPORT + 1, /**< Number of attributes supported */
};

/**
 * @struct fastrpc_capability
 * @brief Argument to query DSP capability with request ID DSPRPC_GET_DSP_INFO
 */
typedef struct remote_dsp_capability {
	uint32_t domain;       /**< @param[in]: DSP domain ADSP_DOMAIN_ID, SDSP_DOMAIN_ID, or CDSP_DOMAIN_ID */
	uint32_t attribute_ID; /**< @param[in]: One of the DSP/kernel attributes from enum remote_dsp_attributes */
	uint32_t capability;   /**< @param[out]: Result of the DSP/kernel capability query based on attribute_ID */
}fastrpc_capability;

/* Macro for backward compatbility. Clients can compile wakelock request code
 * in their app only when this is defined */
#define FASTRPC_WAKELOCK_CONTROL_SUPPORTED 1

// Used with DSPRPC_CONTROL_WAKELOCK req ID
struct remote_rpc_control_wakelock {
	uint32_t enable;	// enable control of wake lock
};

/* Used with DSPRPC_GET_DOMAIN request ID.
 * Get domain ID associated with an opened handle to remote interface of type remote_handle64.
 * remote_handle64_control() returns domain for a given handle
 * remote_handle_control() API returns default domain ID
 */
typedef struct remote_rpc_get_domain {
	int domain;          // @param[out]: domain ID associcated with handle
} remote_rpc_get_domain_t;

/* Set remote handle control parameters
 *
 * @param req, request ID
 * @param data, address of structure with parameters
 * @param datalen, length of data
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_control)(uint32_t req, void* data, uint32_t datalen) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_control)(remote_handle64 h, uint32_t req, void* data, uint32_t datalen) __QAIC_REMOTE_ATTRIBUTE;

/* remote session control interface
 */
enum session_control_req_id {
	FASTRPC_THREAD_PARAMS			=	1,
	DSPRPC_CONTROL_UNSIGNED_MODULE		=	2,
	FASTRPC_RELATIVE_THREAD_PRIORITY	=	4,
	FASTRPC_REMOTE_PROCESS_KILL			=	6, /* Kill remote process */
	FASTRPC_SESSION_CLOSE				=	7, /* Close all open handles of requested domain */
	FASTRPC_CONTROL_PD_DUMP				=	8, // Enable PD dump feature
	FASTRPC_REMOTE_PROCESS_EXCEPTION	=	9, // Trigger Exception in the remote process
};

// Used with FASTRPC_THREAD_PARAMS req ID
struct remote_rpc_thread_params {
	int domain;			// Remote subsystem domain ID, pass -1 to set params for all domains
	int prio;			// user thread priority (1 to 255), pass -1 to use default
	int stack_size;		// user thread stack size, pass -1 to use default
};

// Used with DSPRPC_CONTROL_UNSIGNED_MODULE req ID
struct remote_rpc_control_unsigned_module {
   int domain;				// Remote subsystem domain ID, -1 to set params for all domains
   int enable; 				// enable unsigned module loading
};

// Used with FASTRPC_RELATIVE_THREAD_PRIORITY req ID
struct remote_rpc_relative_thread_priority {
	int domain;						// Remote subsystem domain ID, pass -1 to update priority for all domains
	int relative_thread_priority;	/* the value by which the default thread priority needs to increase/decrease
									 * DSP thread priorities run from 1 to 255 with 1 being the highest thread priority.
									 * So a negative relative thread priority value will 'increase' the thread priority,
									 * a positive value will 'decrease' the thread priority.
									 */
};

/*
 * When a remote invocation does not return,
 * then call "remote_session_control" with FASTRPC_REMOTE_PROCESS_KILL requestID
 * and the appropriate remote domain ID. Once remote process is successfully
 * killed, before attempting to create new session, user is expected to
 * close all open handles for shared objects in case of domains.
 * And, user is expected to unload all shared objects including
 * libcdsprpc.so/libadsprpc.so/libmdsprpc.so/libsdsprpc.so in case of non-domains.
 */
struct remote_rpc_process_clean_params {
	int domain;          // Domain ID  to recover process
};

// Used with FASTRPC_SESSION_CLOSE req ID
struct remote_rpc_session_close {
   int domain;				// Remote subsystem domain ID, -1 to close all handles for all domains
};

// Used with DSPRPC_CONTROL_PD_DUMP req ID, for enabling and disabling PD dump of User PDs on DSP
struct remote_rpc_control_pd_dump {
   int domain;				// Remote subsystem domain ID, -1 to set params for all domains
   int enable; 				// enable pd dump of user pd
};

// Used with FASTRPC_REMOTE_PROCESS_EXCEPTION req ID, to trigger exception in the User PD of DSP.
typedef struct remote_rpc_process_clean_params remote_rpc_process_exception;

/* Set remote session parameters
 *
 * @param req, request ID
 * @param data, address of structure with parameters
 * @param datalen, length of data
 * @retval, 0 on success
 * remote_session_control with FASTRPC_REMOTE_PROCESS_KILL req ID, possible error codes
 * are AEE_ENOSUCH, AEE_EBADPARM, AEE_EINVALIDDOMAIN. Other than this errors codes treated as
 * retuned from fastRPC framework.
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_session_control)(uint32_t req, void *data, uint32_t datalen) __QAIC_REMOTE_ATTRIBUTE;

/* map memory to the remote domain
 *
 * @param fd, fd assosciated with this memory
 * @param flags, flags to be used for the mapping
 * @param vaddrin, input address
 * @param size, size of buffer
 * @param vaddrout, output address
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_mmap)(int fd, uint32_t flags, uint32_t vaddrin, int size, uint32_t* vaddrout) __QAIC_REMOTE_ATTRIBUTE;

/* unmap memory from the remote domain
 *
 * @param vaddrout, remote address mapped
 * @param size, size to unmap.  Unmapping a range partially may  not be supported.
 * @retval, 0 on success, may fail if memory is still mapped
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_munmap)(uint32_t vaddrout, int size) __QAIC_REMOTE_ATTRIBUTE;

/* Memory map control flags for using with remote_mem_map() and remote_mem_unmap() */
enum remote_mem_map_flags {
/*
 * Create static memory map on remote process with default cache configuration (writeback).
 * Same remoteVirtAddr will be assigned on remote process when fastrpc call made with local virtual address.
 * @Map lifetime
 * Life time of this mapping is until user unmap using remote_mem_unmap or session close.
 * No reference counts are used. Behavior of mapping multiple times without unmap is undefined.
 * @Cache maintenance
 * Driver clean caches when virtual address passed through RPC calls defined in IDL as a pointer.
 * User is responsible for cleaing cache when remoteVirtAddr shared to DSP and accessed out of fastrpc method invocations on DSP.
 * @recommended usage
 * Map buffers which are reused for long time or until session close. This helps to reduce fastrpc latency.
 * Memory shared with remote process and accessed only by DSP.
 */
  REMOTE_MAP_MEM_STATIC      = 0,

  REMOTE_MAP_MAX_FLAG        = REMOTE_MAP_MEM_STATIC + 1,
};

/*
 * Map memory to the remote process on a selected DSP domain
 * @domain: DSP domain ID. Use -1 for using default domain.
 *          Default domain is selected based on library lib(a/m/s/c)dsprpc.so library linked to application.
 * @fd: file descriptor of memory
 * @flags: enum remote_mem_map_flags type of flag
 * @virtAddr: virtual address of buffer
 * @size: buffer length
 * @remoteVirtAddr[out]: remote process virtual address
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_mem_map)(int domain, int fd, int flags, uint64_t virtAddr, size_t size, uint64_t* remoteVirtAddr) __QAIC_REMOTE_ATTRIBUTE;

/*
 * Unmap memory to the remote process on a selected DSP domain
 * @domain: DSP domain ID. Use -1 for using default domain. Get domain from multi-domain handle if required.
 * @remoteVirtAddr: remote process virtual address received from remote_mem_map
 * @size: buffer length
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_mem_unmap)(int domain, uint64_t remoteVirtAddr, size_t size) __QAIC_REMOTE_ATTRIBUTE;

enum remote_buf_attributes {
   FASTRPC_ATTR_NON_COHERENT   = 2,          /* Attribute to map a buffer as dma non-coherent,
                                                Driver perform cache maintenance.*/
   FASTRPC_ATTR_COHERENT       = 4,          /* Attribute to map a buffer as dma coherent,
                                                Driver skips cache maintenenace
                                                It will be ignored if a device is marked as dma-coherent in device tree.*/
   FASTRPC_ATTR_KEEP_MAP       = 8,          /* Attribute to keep the buffer persistant
                                                until unmap is called explicitly.*/
   FASTRPC_ATTR_NOMAP          = 16,          /* Attribute for secure buffers to skip
                                                smmu mapping in fastrpc driver*/
   FASTRPC_ATTR_FORCE_NOFLUSH  = 32,          /* Attribute to map buffer such that flush by driver is skipped for that particular buffer
                                                client has to perform cache maintenance*/
   FASTRPC_ATTR_FORCE_NOINVALIDATE = 64,      /* Attribute to map buffer such that invalidate by driver is skipped for that particular buffer
                                                client has to perform cache maintenance */
   FASTRPC_ATTR_TRY_MAP_STATIC = 128,         /* Attribute for persistent mapping a buffer
                                                 to remote DSP process during buffer registration
                                                 with FastRPC driver. This buffer will be automatically
                                                 mapped during fastrpc session open and unmapped either
                                                 at unregister or session close. FastRPC library tries
                                                 to map buffers and ignore errors in case of failure.
                                                 pre-mapping a buffer reduces the FastRPC latency.
                                                 This flag is recommended only for buffers used with
                                                 latency critical rpc calls */
};

/* Register a file descriptor for a buffer.  This is only valid on
 * android with ION allocated memory.  Users of fastrpc should register
 * a buffer allocated with ION to enable sharing that buffer to the
 * dsp via the smmu.  Some versions of libadsprpc.so lack this
 * function, so users should set this symbol as weak.
 *
 * #pragma weak  remote_register_buf
 * #pragma weak  remote_register_buf_attr
 *
 * @param buf, virtual address of the buffer
 * @param size, size of the buffer
 * @fd, the file descriptor, callers can use -1 to deregister.
 * @attr, map buffer as coherent or non-coherent
 */
__QAIC_REMOTE_EXPORT void __QAIC_REMOTE(remote_register_buf)(void* buf, int size, int fd) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT void __QAIC_REMOTE(remote_register_buf_attr)(void* buf, int size, int fd, int attr) __QAIC_REMOTE_ATTRIBUTE;

/* Register a dma handle with fastrpc.  This is only valid on
 * android with ION allocated memory.  Users of fastrpc should register
 * a file descriptor allocated with ION to enable sharing that memory to the
 * dsp via the smmu.  Some versions of libadsprpc.so lack this
 * function, so users should set this symbol as weak.
 *
 * #pragma weak  remote_register_dma_handle
 * #pragma weak  remote_register_dma_handle_attr
 *
 * @fd, the file descriptor, callers can use -1 to deregister.
 * @param len, size of the buffer
 * @attr, map buffer as coherent or non-coherent or no-map
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_register_dma_handle)(int fd, uint32_t len) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_register_dma_handle_attr)(int fd, uint32_t len, uint32_t attr) __QAIC_REMOTE_ATTRIBUTE;

/*
 * This is the default mode for the driver.  While the driver is in parallel
 * mode it will try to invalidate output buffers after it transfers control
 * to the dsp.  This allows the invalidate operations to overlap with the
 * dsp processing the call.  This mode should be used when output buffers
 * are only read on the application processor and only written on the aDSP.
 */
#define REMOTE_MODE_PARALLEL  0

/*
 * When operating in SERIAL mode the driver will invalidate output buffers
 * before calling into the dsp.  This mode should be used when output
 * buffers have been written to somewhere besides the aDSP.
 */
#define REMOTE_MODE_SERIAL    1

/*
 * Internal transport prefix
 */
#define ITRANSPORT_PREFIX "'\":;./\\"

/*
 * Set the mode of operation.
 *
 * Some versions of libadsprpc.so lack this function, so users should set
 * this symbol as weak.
 *
 * #pragma weak  remote_set_mode
 *
 * @param mode, the mode
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_set_mode)(uint32_t mode) __QAIC_REMOTE_ATTRIBUTE;

/* Register a file descriptor.  This can be used when users do not have
 * a mapping to pass to the RPC layer.  The generated address is a mapping
 * with PROT_NONE, any access to this memory will fail, so it should only
 * be used as an ID to identify this file descriptor to the RPC layer.
 *
 * To deregister use remote_register_buf(addr, size, -1).
 *
 * #pragma weak  remote_register_fd
 *
 * @param fd, the file descriptor.
 * @param size, size to of the buffer
 * @retval, (void*)-1 on failure, address on success.
 */
__QAIC_REMOTE_EXPORT void *__QAIC_REMOTE(remote_register_fd)(int fd, int size) __QAIC_REMOTE_ATTRIBUTE;

/* Get status of Async job.  This can be used to query the status of a Async job
 *
 * @param jobid, jobid returned during Async job submission.
 * @param timeout_us, timeout in micro seconds
 *                    timeout = 0, returns immediately with status/result
 *                    timeout > 0, waits for specified time and then returns with status/result
 *                    timeout < 0. waits indefinetely until job completes
 * @param result, integer pointer for the result of the job
 *                0 on success
 *                error code on failure
 * @retval, 0 on job completion and result of job is part of @param result
 *          AEE_EBUSY, if job status is pending and is not returned from DSP
 *          AEE_EBADPARM, if job id is invalid
 *          AEE_EFAILED, FastRPC internal error
 *
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(fastrpc_async_get_status)(fastrpc_async_jobid jobid, int timeout_us, int *result);

/* Release Async job.  Release async job after receiving status either through callback/poll
 *
 * @param jobid, jobid returned during Async job submission.
 * @retval, 0 on success
 *          AEE_EBUSY, if job status is pending and is not returned from DSP
 *          AEE_EBADPARM, if job id is invalid
 *
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(fastrpc_release_async_job)(fastrpc_async_jobid jobid);

enum fastrpc_map_flags {
	/**
	 * Map memory pages with RW- permission and CACHE WRITEBACK.
	 * Driver will clean cache when buffer passed in a FastRPC call.
	 * Same remote virtual address will be assigned for subsequent
	 * FastRPC calls.
	 */
	FASTRPC_MAP_STATIC = 0,

	/* Reserved */
	FASTRPC_MAP_RESERVED,

	/**
	 * Map memory pages with RW- permission and CACHE WRITEBACK.
	 * Mapping tagged with a file descriptor. User is responsible for
	 * maintenance of CPU and DSP caches for the buffer. Get virtual address
	 * of buffer on DSP using HAP_mmap_get() and HAP_mmap_put() functions.
	 */
	FASTRPC_MAP_FD = 2,

	/**
	 * Mapping delayed until user calls HAP_mmap() and HAP_munmap()
	 * functions on DSP. User is responsible for maintenance of CPU and DSP
	 * caches for the buffer. Delayed mapping is useful for users to map
	 * buffer on DSP with other than default permissions and cache modes
	 * using HAP_mmap() and HAP_munmap() functions.
	 */
	FASTRPC_MAP_FD_DELAYED,
	FASTRPC_MAP_MAX,
};

/**
 * Creates a mapping on remote process for an ION buffer with file descriptor. New fastrpc session
 * will be opened if not already opened for the domain.
 *
 * @param domain, DSP domain ID of a fastrpc session
 * @param fd, ION memory file descriptor
 * @param addr, buffer virtual address on cpu
 * @param offset, offset from the beginining of the buffer
 * @param length, size of buffer in bytes
 * @param flags, controls mapping functionality on DSP. Refer fastrpc_map_flags enum definition for more information.
 *
 * @return, 0 on success, error code on failure.
 *          AEE_EALREADY Buffer already mapped. Multiple mappings for the same buffer are not supported.
 *          AEE_EBADPARM Bad parameters
 *          AEE_EFAILED Failed to map buffer
 *          AEE_ENOMEMORY Out of memory (internal error)
 *          AEE_EUNSUPPORTED Unsupported API on the target
 */
int fastrpc_mmap(int domain, int fd, void *addr, int offset, size_t length, enum fastrpc_map_flags flags);

/**
 * Removes a mapping associated with file descriptor.
 *
 * @param domain, DSP domain ID of a fastrpc session
 * @param fd, file descriptor
 * @param addr, buffer virtual address used for mapping creation
 * @param length, buffer length
 *
 * @return, 0 on success, error code on failure.
 *          AEE_EBADPARM Bad parameters
 *          AEE_EINVALIDFD Mapping not found for specified fd
 *          AEE_EFAILED Failed to map buffer
 *          AEE_EUNSUPPORTED Unsupported API on the target
 */
int fastrpc_munmap(int domain, int fd, void *addr, size_t length);

#ifdef __cplusplus
}
#endif

#endif // REMOTE_H
