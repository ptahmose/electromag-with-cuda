/***********************************************************************************************
Copyright (C) 2009-2010 - Alexandru Gagniuc - <http:\\g-tech.homeserver.com\HPC.htm>
 * This file is part of ElectroMag.

    ElectroMag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ElectroMag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ElectroMag.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************************************/

#ifndef _OPENCL_DYN_LOAD_H
#define	_OPENCL_DYN_LOAD_H

#ifdef __APPLE__
#include <OpenCL/cl_platform.h>
#else
#include "CL/cl_platform.h"
#endif

#ifdef CL_API_ENTRY
#undef CL_API_ENTRY
#define CL_API_ENTRY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

typedef struct _cl_platform_id *    cl_platform_id;
typedef struct _cl_device_id *      cl_device_id;
typedef struct _cl_context *        cl_context;
typedef struct _cl_command_queue *  cl_command_queue;
typedef struct _cl_mem *            cl_mem;
typedef struct _cl_program *        cl_program;
typedef struct _cl_kernel *         cl_kernel;
typedef struct _cl_event *          cl_event;
typedef struct _cl_sampler *        cl_sampler;

typedef cl_uint             cl_bool;                     /* WARNING!  Unlike cl_ types in cl_platform.h, cl_bool is not guaranteed to be the same size as the bool in kernels. */
typedef cl_ulong            cl_bitfield;
typedef cl_bitfield         cl_device_type;
typedef cl_uint             cl_platform_info;
typedef cl_uint             cl_device_info;
typedef cl_bitfield         cl_device_address_info;
typedef cl_bitfield         cl_device_fp_config;
typedef cl_uint             cl_device_mem_cache_type;
typedef cl_uint             cl_device_local_mem_type;
typedef cl_bitfield         cl_device_exec_capabilities;
typedef cl_bitfield         cl_command_queue_properties;

typedef intptr_t			cl_context_properties;
typedef cl_uint             cl_context_info;
typedef cl_uint             cl_command_queue_info;
typedef cl_uint             cl_channel_order;
typedef cl_uint             cl_channel_type;
typedef cl_bitfield         cl_mem_flags;
typedef cl_uint             cl_mem_object_type;
typedef cl_uint             cl_mem_info;
typedef cl_uint             cl_image_info;
typedef cl_uint             cl_addressing_mode;
typedef cl_uint             cl_filter_mode;
typedef cl_uint             cl_sampler_info;
typedef cl_bitfield         cl_map_flags;
typedef cl_uint             cl_program_info;
typedef cl_uint             cl_program_build_info;
typedef cl_int              cl_build_status;
typedef cl_uint             cl_kernel_info;
typedef cl_uint             cl_kernel_work_group_info;
typedef cl_uint             cl_event_info;
typedef cl_uint             cl_command_type;
typedef cl_uint             cl_profiling_info;

typedef struct _cl_image_format {
    cl_channel_order        image_channel_order;
    cl_channel_type         image_channel_data_type;
} cl_image_format;



/******************************************************************************/

// Error Codes
#define CL_SUCCESS                                  0
#define CL_DEVICE_NOT_FOUND                         -1
#define CL_DEVICE_NOT_AVAILABLE                     -2
#define CL_COMPILER_NOT_AVAILABLE                   -3
#define CL_MEM_OBJECT_ALLOCATION_FAILURE            -4
#define CL_OUT_OF_RESOURCES                         -5
#define CL_OUT_OF_HOST_MEMORY                       -6
#define CL_PROFILING_INFO_NOT_AVAILABLE             -7
#define CL_MEM_COPY_OVERLAP                         -8
#define CL_IMAGE_FORMAT_MISMATCH                    -9
#define CL_IMAGE_FORMAT_NOT_SUPPORTED               -10
#define CL_BUILD_PROGRAM_FAILURE                    -11
#define CL_MAP_FAILURE                              -12

#define CL_INVALID_VALUE                            -30
#define CL_INVALID_DEVICE_TYPE                      -31
#define CL_INVALID_PLATFORM                         -32
#define CL_INVALID_DEVICE                           -33
#define CL_INVALID_CONTEXT                          -34
#define CL_INVALID_QUEUE_PROPERTIES                 -35
#define CL_INVALID_COMMAND_QUEUE                    -36
#define CL_INVALID_HOST_PTR                         -37
#define CL_INVALID_MEM_OBJECT                       -38
#define CL_INVALID_IMAGE_FORMAT_DESCRIPTOR          -39
#define CL_INVALID_IMAGE_SIZE                       -40
#define CL_INVALID_SAMPLER                          -41
#define CL_INVALID_BINARY                           -42
#define CL_INVALID_BUILD_OPTIONS                    -43
#define CL_INVALID_PROGRAM                          -44
#define CL_INVALID_PROGRAM_EXECUTABLE               -45
#define CL_INVALID_KERNEL_NAME                      -46
#define CL_INVALID_KERNEL_DEFINITION                -47
#define CL_INVALID_KERNEL                           -48
#define CL_INVALID_ARG_INDEX                        -49
#define CL_INVALID_ARG_VALUE                        -50
#define CL_INVALID_ARG_SIZE                         -51
#define CL_INVALID_KERNEL_ARGS                      -52
#define CL_INVALID_WORK_DIMENSION                   -53
#define CL_INVALID_WORK_GROUP_SIZE                  -54
#define CL_INVALID_WORK_ITEM_SIZE                   -55
#define CL_INVALID_GLOBAL_OFFSET                    -56
#define CL_INVALID_EVENT_WAIT_LIST                  -57
#define CL_INVALID_EVENT                            -58
#define CL_INVALID_OPERATION                        -59
#define CL_INVALID_GL_OBJECT                        -60
#define CL_INVALID_BUFFER_SIZE                      -61
#define CL_INVALID_MIP_LEVEL                        -62
#define CL_INVALID_GLOBAL_WORK_SIZE                 -63

// OpenCL Version
#define CL_VERSION_1_0                              1

// cl_bool
#define CL_FALSE                                    0
#define CL_TRUE                                     1

// cl_platform_info
#define CL_PLATFORM_PROFILE                         0x0900
#define CL_PLATFORM_VERSION                         0x0901
#define CL_PLATFORM_NAME                            0x0902
#define CL_PLATFORM_VENDOR                          0x0903
#define CL_PLATFORM_EXTENSIONS                      0x0904

// cl_device_type - bitfield
#define CL_DEVICE_TYPE_DEFAULT                      (1 << 0)
#define CL_DEVICE_TYPE_CPU                          (1 << 1)
#define CL_DEVICE_TYPE_GPU                          (1 << 2)
#define CL_DEVICE_TYPE_ACCELERATOR                  (1 << 3)
#define CL_DEVICE_TYPE_ALL                          0xFFFFFFFF

// cl_device_info
#define CL_DEVICE_TYPE                              0x1000
#define CL_DEVICE_VENDOR_ID                         0x1001
#define CL_DEVICE_MAX_COMPUTE_UNITS                 0x1002
#define CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS          0x1003
#define CL_DEVICE_MAX_WORK_GROUP_SIZE               0x1004
#define CL_DEVICE_MAX_WORK_ITEM_SIZES               0x1005
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR       0x1006
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT      0x1007
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT        0x1008
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG       0x1009
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT      0x100A
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE     0x100B
#define CL_DEVICE_MAX_CLOCK_FREQUENCY               0x100C
#define CL_DEVICE_ADDRESS_BITS                      0x100D
#define CL_DEVICE_MAX_READ_IMAGE_ARGS               0x100E
#define CL_DEVICE_MAX_WRITE_IMAGE_ARGS              0x100F
#define CL_DEVICE_MAX_MEM_ALLOC_SIZE                0x1010
#define CL_DEVICE_IMAGE2D_MAX_WIDTH                 0x1011
#define CL_DEVICE_IMAGE2D_MAX_HEIGHT                0x1012
#define CL_DEVICE_IMAGE3D_MAX_WIDTH                 0x1013
#define CL_DEVICE_IMAGE3D_MAX_HEIGHT                0x1014
#define CL_DEVICE_IMAGE3D_MAX_DEPTH                 0x1015
#define CL_DEVICE_IMAGE_SUPPORT                     0x1016
#define CL_DEVICE_MAX_PARAMETER_SIZE                0x1017
#define CL_DEVICE_MAX_SAMPLERS                      0x1018
#define CL_DEVICE_MEM_BASE_ADDR_ALIGN               0x1019
#define CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE          0x101A
#define CL_DEVICE_SINGLE_FP_CONFIG                  0x101B
#define CL_DEVICE_GLOBAL_MEM_CACHE_TYPE             0x101C
#define CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE         0x101D
#define CL_DEVICE_GLOBAL_MEM_CACHE_SIZE             0x101E
#define CL_DEVICE_GLOBAL_MEM_SIZE                   0x101F
#define CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE          0x1020
#define CL_DEVICE_MAX_CONSTANT_ARGS                 0x1021
#define CL_DEVICE_LOCAL_MEM_TYPE                    0x1022
#define CL_DEVICE_LOCAL_MEM_SIZE                    0x1023
#define CL_DEVICE_ERROR_CORRECTION_SUPPORT          0x1024
#define CL_DEVICE_PROFILING_TIMER_RESOLUTION        0x1025
#define CL_DEVICE_ENDIAN_LITTLE                     0x1026
#define CL_DEVICE_AVAILABLE                         0x1027
#define CL_DEVICE_COMPILER_AVAILABLE                0x1028
#define CL_DEVICE_EXECUTION_CAPABILITIES            0x1029
#define CL_DEVICE_QUEUE_PROPERTIES                  0x102A
#define CL_DEVICE_NAME                              0x102B
#define CL_DEVICE_VENDOR                            0x102C
#define CL_DRIVER_VERSION                           0x102D
#define CL_DEVICE_PROFILE                           0x102E
#define CL_DEVICE_VERSION                           0x102F
#define CL_DEVICE_EXTENSIONS                        0x1030
#define CL_DEVICE_PLATFORM                          0x1031

// cl_device_fp_config - bitfield
#define CL_FP_DENORM                                (1 << 0)
#define CL_FP_INF_NAN                               (1 << 1)
#define CL_FP_ROUND_TO_NEAREST                      (1 << 2)
#define CL_FP_ROUND_TO_ZERO                         (1 << 3)
#define CL_FP_ROUND_TO_INF                          (1 << 4)
#define CL_FP_FMA                                   (1 << 5)

// cl_device_mem_cache_type
#define CL_NONE                                     0x0
#define CL_READ_ONLY_CACHE                          0x1
#define CL_READ_WRITE_CACHE                         0x2

// cl_device_local_mem_type
#define CL_LOCAL                                    0x1
#define CL_GLOBAL                                   0x2

// cl_device_exec_capabilities - bitfield
#define CL_EXEC_KERNEL                              (1 << 0)
#define CL_EXEC_NATIVE_KERNEL                       (1 << 1)

// cl_command_queue_properties - bitfield
#define CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE      (1 << 0)
#define CL_QUEUE_PROFILING_ENABLE                   (1 << 1)

// cl_context_info
#define CL_CONTEXT_REFERENCE_COUNT                  0x1080
#define CL_CONTEXT_DEVICES                          0x1081
#define CL_CONTEXT_PROPERTIES                       0x1082

// cl_context_properties
#define CL_CONTEXT_PLATFORM                         0x1084

// cl_command_queue_info
#define CL_QUEUE_CONTEXT                            0x1090
#define CL_QUEUE_DEVICE                             0x1091
#define CL_QUEUE_REFERENCE_COUNT                    0x1092
#define CL_QUEUE_PROPERTIES                         0x1093

// cl_mem_flags - bitfield
#define CL_MEM_READ_WRITE                           (1 << 0)
#define CL_MEM_WRITE_ONLY                           (1 << 1)
#define CL_MEM_READ_ONLY                            (1 << 2)
#define CL_MEM_USE_HOST_PTR                         (1 << 3)
#define CL_MEM_ALLOC_HOST_PTR                       (1 << 4)
#define CL_MEM_COPY_HOST_PTR                        (1 << 5)

// cl_channel_order
#define CL_R                                        0x10B0
#define CL_A                                        0x10B1
#define CL_RG                                       0x10B2
#define CL_RA                                       0x10B3
#define CL_RGB                                      0x10B4
#define CL_RGBA                                     0x10B5
#define CL_BGRA                                     0x10B6
#define CL_ARGB                                     0x10B7
#define CL_INTENSITY                                0x10B8
#define CL_LUMINANCE                                0x10B9

// cl_channel_type
#define CL_SNORM_INT8                               0x10D0
#define CL_SNORM_INT16                              0x10D1
#define CL_UNORM_INT8                               0x10D2
#define CL_UNORM_INT16                              0x10D3
#define CL_UNORM_SHORT_565                          0x10D4
#define CL_UNORM_SHORT_555                          0x10D5
#define CL_UNORM_INT_101010                         0x10D6
#define CL_SIGNED_INT8                              0x10D7
#define CL_SIGNED_INT16                             0x10D8
#define CL_SIGNED_INT32                             0x10D9
#define CL_UNSIGNED_INT8                            0x10DA
#define CL_UNSIGNED_INT16                           0x10DB
#define CL_UNSIGNED_INT32                           0x10DC
#define CL_HALF_FLOAT                               0x10DD
#define CL_FLOAT                                    0x10DE

// cl_mem_object_type
#define CL_MEM_OBJECT_BUFFER                        0x10F0
#define CL_MEM_OBJECT_IMAGE2D                       0x10F1
#define CL_MEM_OBJECT_IMAGE3D                       0x10F2

// cl_mem_info
#define CL_MEM_TYPE                                 0x1100
#define CL_MEM_FLAGS                                0x1101
#define CL_MEM_SIZE                                 0x1102
#define CL_MEM_HOST_PTR                             0x1103
#define CL_MEM_MAP_COUNT                            0x1104
#define CL_MEM_REFERENCE_COUNT                      0x1105
#define CL_MEM_CONTEXT                              0x1106

// cl_image_info
#define CL_IMAGE_FORMAT                             0x1110
#define CL_IMAGE_ELEMENT_SIZE                       0x1111
#define CL_IMAGE_ROW_PITCH                          0x1112
#define CL_IMAGE_SLICE_PITCH                        0x1113
#define CL_IMAGE_WIDTH                              0x1114
#define CL_IMAGE_HEIGHT                             0x1115
#define CL_IMAGE_DEPTH                              0x1116

// cl_addressing_mode
#define CL_ADDRESS_NONE                             0x1130
#define CL_ADDRESS_CLAMP_TO_EDGE                    0x1131
#define CL_ADDRESS_CLAMP                            0x1132
#define CL_ADDRESS_REPEAT                           0x1133

// cl_filter_mode
#define CL_FILTER_NEAREST                           0x1140
#define CL_FILTER_LINEAR                            0x1141

// cl_sampler_info
#define CL_SAMPLER_REFERENCE_COUNT                  0x1150
#define CL_SAMPLER_CONTEXT                          0x1151
#define CL_SAMPLER_NORMALIZED_COORDS                0x1152
#define CL_SAMPLER_ADDRESSING_MODE                  0x1153
#define CL_SAMPLER_FILTER_MODE                      0x1154

// cl_map_flags - bitfield
#define CL_MAP_READ                                 (1 << 0)
#define CL_MAP_WRITE                                (1 << 1)

// cl_program_info
#define CL_PROGRAM_REFERENCE_COUNT                  0x1160
#define CL_PROGRAM_CONTEXT                          0x1161
#define CL_PROGRAM_NUM_DEVICES                      0x1162
#define CL_PROGRAM_DEVICES                          0x1163
#define CL_PROGRAM_SOURCE                           0x1164
#define CL_PROGRAM_BINARY_SIZES                     0x1165
#define CL_PROGRAM_BINARIES                         0x1166

// cl_program_build_info
#define CL_PROGRAM_BUILD_STATUS                     0x1181
#define CL_PROGRAM_BUILD_OPTIONS                    0x1182
#define CL_PROGRAM_BUILD_LOG                        0x1183

// cl_build_status
#define CL_BUILD_SUCCESS                            0
#define CL_BUILD_NONE                               -1
#define CL_BUILD_ERROR                              -2
#define CL_BUILD_IN_PROGRESS                        -3

// cl_kernel_info
#define CL_KERNEL_FUNCTION_NAME                     0x1190
#define CL_KERNEL_NUM_ARGS                          0x1191
#define CL_KERNEL_REFERENCE_COUNT                   0x1192
#define CL_KERNEL_CONTEXT                           0x1193
#define CL_KERNEL_PROGRAM                           0x1194

// cl_kernel_work_group_info
#define CL_KERNEL_WORK_GROUP_SIZE                   0x11B0
#define CL_KERNEL_COMPILE_WORK_GROUP_SIZE           0x11B1
#define CL_KERNEL_LOCAL_MEM_SIZE                    0x11B2

// cl_event_info
#define CL_EVENT_COMMAND_QUEUE                      0x11D0
#define CL_EVENT_COMMAND_TYPE                       0x11D1
#define CL_EVENT_REFERENCE_COUNT                    0x11D2
#define CL_EVENT_COMMAND_EXECUTION_STATUS           0x11D3

// cl_command_type
#define CL_COMMAND_NDRANGE_KERNEL                   0x11F0
#define CL_COMMAND_TASK                             0x11F1
#define CL_COMMAND_NATIVE_KERNEL                    0x11F2
#define CL_COMMAND_READ_BUFFER                      0x11F3
#define CL_COMMAND_WRITE_BUFFER                     0x11F4
#define CL_COMMAND_COPY_BUFFER                      0x11F5
#define CL_COMMAND_READ_IMAGE                       0x11F6
#define CL_COMMAND_WRITE_IMAGE                      0x11F7
#define CL_COMMAND_COPY_IMAGE                       0x11F8
#define CL_COMMAND_COPY_IMAGE_TO_BUFFER             0x11F9
#define CL_COMMAND_COPY_BUFFER_TO_IMAGE             0x11FA
#define CL_COMMAND_MAP_BUFFER                       0x11FB
#define CL_COMMAND_MAP_IMAGE                        0x11FC
#define CL_COMMAND_UNMAP_MEM_OBJECT                 0x11FD
#define CL_COMMAND_MARKER                           0x11FE
#define CL_COMMAND_ACQUIRE_GL_OBJECTS               0x11FF
#define CL_COMMAND_RELEASE_GL_OBJECTS               0x1200

// command execution status
#define CL_COMPLETE                                 0x0
#define CL_RUNNING                                  0x1
#define CL_SUBMITTED                                0x2
#define CL_QUEUED                                   0x3

// cl_profiling_info
#define CL_PROFILING_COMMAND_QUEUED                 0x1280
#define CL_PROFILING_COMMAND_SUBMIT                 0x1281
#define CL_PROFILING_COMMAND_START                  0x1282
#define CL_PROFILING_COMMAND_END                    0x1283

/********************************************************************************************************/


// Platform API
typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetPlatformIDs(cl_uint          /* num_entries */,
                 cl_platform_id * /* platforms */,
                 cl_uint *        /* num_platforms */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetPlatformInfo(cl_platform_id   /* platform */,
                  cl_platform_info /* param_name */,
                  size_t           /* param_value_size */,
                  void *           /* param_value */,
                  size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Device APIs
typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetDeviceIDs(cl_platform_id   /* platform */,
               cl_device_type   /* device_type */,
               cl_uint          /* num_entries */,
               cl_device_id *   /* devices */,
               cl_uint *        /* num_devices */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetDeviceInfo(cl_device_id    /* device */,
                cl_device_info  /* param_name */,
                size_t          /* param_value_size */,
                void *          /* param_value */,
                size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Context APIs
typedef CL_API_ENTRY cl_context CL_API_CALL
__clCreateContext(const cl_context_properties * /* properties */,
                cl_uint                       /* num_devices */,
                const cl_device_id *          /* devices */,
                void (*pfn_notify)(const char *, const void *, size_t, void *) /* pfn_notify */,
                void *                        /* user_data */,
                cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_context CL_API_CALL
__clCreateContextFromType(const cl_context_properties * /* properties */,
                        cl_device_type                /* device_type */,
                        void (*pfn_notify)(const char *, const void *, size_t, void *) /* pfn_notify */,
                        void *                        /* user_data */,
                        cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainContext(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseContext(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetContextInfo(cl_context         /* context */,
                 cl_context_info    /* param_name */,
                 size_t             /* param_value_size */,
                 void *             /* param_value */,
                 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Command Queue APIs
typedef CL_API_ENTRY cl_command_queue CL_API_CALL
__clCreateCommandQueue(cl_context                     /* context */,
                     cl_device_id                   /* device */,
                     cl_command_queue_properties    /* properties */,
                     cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetCommandQueueInfo(cl_command_queue      /* command_queue */,
                      cl_command_queue_info /* param_name */,
                      size_t                /* param_value_size */,
                      void *                /* param_value */,
                      size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clSetCommandQueueProperty(cl_command_queue              /* command_queue */,
                          cl_command_queue_properties   /* properties */,
                          cl_bool                        /* enable */,
                          cl_command_queue_properties * /* old_properties */) CL_API_SUFFIX__VERSION_1_0;

// Memory Object APIs
typedef CL_API_ENTRY cl_mem CL_API_CALL
__clCreateBuffer(cl_context   /* context */,
               cl_mem_flags /* flags */,
               size_t       /* size */,
               void *       /* host_ptr */,
               cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem CL_API_CALL
__clCreateImage2D(cl_context              /* context */,
                cl_mem_flags            /* flags */,
                const cl_image_format * /* image_format */,
                size_t                  /* image_width */,
                size_t                  /* image_height */,
                size_t                  /* image_row_pitch */,
                void *                  /* host_ptr */,
                cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem CL_API_CALL
__clCreateImage3D(cl_context              /* context */,
                cl_mem_flags            /* flags */,
                const cl_image_format * /* image_format */,
                size_t                  /* image_width */,
                size_t                  /* image_height */,
                size_t                  /* image_depth */,
                size_t                  /* image_row_pitch */,
                size_t                  /* image_slice_pitch */,
                void *                  /* host_ptr */,
                cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetSupportedImageFormats(cl_context           /* context */,
                           cl_mem_flags         /* flags */,
                           cl_mem_object_type   /* image_type */,
                           cl_uint              /* num_entries */,
                           cl_image_format *    /* image_formats */,
                           cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetMemObjectInfo(cl_mem           /* memobj */,
                   cl_mem_info      /* param_name */,
                   size_t           /* param_value_size */,
                   void *           /* param_value */,
                   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetImageInfo(cl_mem           /* image */,
               cl_image_info    /* param_name */,
               size_t           /* param_value_size */,
               void *           /* param_value */,
               size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Sampler APIs
typedef CL_API_ENTRY cl_sampler CL_API_CALL
__clCreateSampler(cl_context          /* context */,
                cl_bool             /* normalized_coords */,
                cl_addressing_mode  /* addressing_mode */,
                cl_filter_mode      /* filter_mode */,
                cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetSamplerInfo(cl_sampler         /* sampler */,
                 cl_sampler_info    /* param_name */,
                 size_t             /* param_value_size */,
                 void *             /* param_value */,
                 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Program Object APIs
typedef CL_API_ENTRY cl_program CL_API_CALL
__clCreateProgramWithSource(cl_context        /* context */,
                          cl_uint           /* count */,
                          const char **     /* strings */,
                          const size_t *    /* lengths */,
                          cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_program CL_API_CALL
__clCreateProgramWithBinary(cl_context                     /* context */,
                          cl_uint                        /* num_devices */,
                          const cl_device_id *           /* device_list */,
                          const size_t *                 /* lengths */,
                          const unsigned char **         /* binaries */,
                          cl_int *                       /* binary_status */,
                          cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clBuildProgram(cl_program           /* program */,
               cl_uint              /* num_devices */,
               const cl_device_id * /* device_list */,
               const char *         /* options */,
               void (*pfn_notify)(cl_program /* program */, void * /* user_data */),
               void *               /* user_data */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clUnloadCompiler(void) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetProgramInfo(cl_program         /* program */,
                 cl_program_info    /* param_name */,
                 size_t             /* param_value_size */,
                 void *             /* param_value */,
                 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetProgramBuildInfo(cl_program            /* program */,
                      cl_device_id          /* device */,
                      cl_program_build_info /* param_name */,
                      size_t                /* param_value_size */,
                      void *                /* param_value */,
                      size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Kernel Object APIs
typedef CL_API_ENTRY cl_kernel CL_API_CALL
__clCreateKernel(cl_program      /* program */,
               const char *    /* kernel_name */,
               cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clCreateKernelsInProgram(cl_program     /* program */,
                         cl_uint        /* num_kernels */,
                         cl_kernel *    /* kernels */,
                         cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainKernel(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseKernel(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clSetKernelArg(cl_kernel    /* kernel */,
               cl_uint      /* arg_index */,
               size_t       /* arg_size */,
               const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetKernelInfo(cl_kernel       /* kernel */,
                cl_kernel_info  /* param_name */,
                size_t          /* param_value_size */,
                void *          /* param_value */,
                size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetKernelWorkGroupInfo(cl_kernel                  /* kernel */,
                         cl_device_id               /* device */,
                         cl_kernel_work_group_info  /* param_name */,
                         size_t                     /* param_value_size */,
                         void *                     /* param_value */,
                         size_t *                   /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Event Object APIs
typedef CL_API_ENTRY cl_int CL_API_CALL
__clWaitForEvents(cl_uint             /* num_events */,
                const cl_event *    /* event_list */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetEventInfo(cl_event         /* event */,
               cl_event_info    /* param_name */,
               size_t           /* param_value_size */,
               void *           /* param_value */,
               size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clRetainEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clReleaseEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;

// Profiling APIs
typedef CL_API_ENTRY cl_int CL_API_CALL
__clGetEventProfilingInfo(cl_event            /* event */,
                        cl_profiling_info   /* param_name */,
                        size_t              /* param_value_size */,
                        void *              /* param_value */,
                        size_t *            /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

// Flush and Finish APIs
typedef CL_API_ENTRY cl_int CL_API_CALL
__clFlush(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clFinish(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

// Enqueued Commands APIs
typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueReadBuffer(cl_command_queue    /* command_queue */,
                    cl_mem              /* buffer */,
                    cl_bool             /* blocking_read */,
                    size_t              /* offset */,
                    size_t              /* cb */,
                    void *              /* ptr */,
                    cl_uint             /* num_events_in_wait_list */,
                    const cl_event *    /* event_wait_list */,
                    cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueWriteBuffer(cl_command_queue   /* command_queue */,
                     cl_mem             /* buffer */,
                     cl_bool            /* blocking_write */,
                     size_t             /* offset */,
                     size_t             /* cb */,
                     const void *       /* ptr */,
                     cl_uint            /* num_events_in_wait_list */,
                     const cl_event *   /* event_wait_list */,
                     cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueCopyBuffer(cl_command_queue    /* command_queue */,
                    cl_mem              /* src_buffer */,
                    cl_mem              /* dst_buffer */,
                    size_t              /* src_offset */,
                    size_t              /* dst_offset */,
                    size_t              /* cb */,
                    cl_uint             /* num_events_in_wait_list */,
                    const cl_event *    /* event_wait_list */,
                    cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueReadImage(cl_command_queue     /* command_queue */,
                   cl_mem               /* image */,
                   cl_bool              /* blocking_read */,
                   const size_t *       /* origin[3] */,
                   const size_t *       /* region[3] */,
                   size_t               /* row_pitch */,
                   size_t               /* slice_pitch */,
                   void *               /* ptr */,
                   cl_uint              /* num_events_in_wait_list */,
                   const cl_event *     /* event_wait_list */,
                   cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueWriteImage(cl_command_queue    /* command_queue */,
                    cl_mem              /* image */,
                    cl_bool             /* blocking_write */,
                    const size_t *      /* origin[3] */,
                    const size_t *      /* region[3] */,
                    size_t              /* input_row_pitch */,
                    size_t              /* input_slice_pitch */,
                    const void *        /* ptr */,
                    cl_uint             /* num_events_in_wait_list */,
                    const cl_event *    /* event_wait_list */,
                    cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueCopyImage(cl_command_queue     /* command_queue */,
                   cl_mem               /* src_image */,
                   cl_mem               /* dst_image */,
                   const size_t *       /* src_origin[3] */,
                   const size_t *       /* dst_origin[3] */,
                   const size_t *       /* region[3] */,
                   cl_uint              /* num_events_in_wait_list */,
                   const cl_event *     /* event_wait_list */,
                   cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueCopyImageToBuffer(cl_command_queue /* command_queue */,
                           cl_mem           /* src_image */,
                           cl_mem           /* dst_buffer */,
                           const size_t *   /* src_origin[3] */,
                           const size_t *   /* region[3] */,
                           size_t           /* dst_offset */,
                           cl_uint          /* num_events_in_wait_list */,
                           const cl_event * /* event_wait_list */,
                           cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueCopyBufferToImage(cl_command_queue /* command_queue */,
                           cl_mem           /* src_buffer */,
                           cl_mem           /* dst_image */,
                           size_t           /* src_offset */,
                           const size_t *   /* dst_origin[3] */,
                           const size_t *   /* region[3] */,
                           cl_uint          /* num_events_in_wait_list */,
                           const cl_event * /* event_wait_list */,
                           cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY void * CL_API_CALL
__clEnqueueMapBuffer(cl_command_queue /* command_queue */,
                   cl_mem           /* buffer */,
                   cl_bool          /* blocking_map */,
                   cl_map_flags     /* map_flags */,
                   size_t           /* offset */,
                   size_t           /* cb */,
                   cl_uint          /* num_events_in_wait_list */,
                   const cl_event * /* event_wait_list */,
                   cl_event *       /* event */,
                   cl_int *         /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY void * CL_API_CALL
__clEnqueueMapImage(cl_command_queue  /* command_queue */,
                  cl_mem            /* image */,
                  cl_bool           /* blocking_map */,
                  cl_map_flags      /* map_flags */,
                  const size_t *    /* origin[3] */,
                  const size_t *    /* region[3] */,
                  size_t *          /* image_row_pitch */,
                  size_t *          /* image_slice_pitch */,
                  cl_uint           /* num_events_in_wait_list */,
                  const cl_event *  /* event_wait_list */,
                  cl_event *        /* event */,
                  cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueUnmapMemObject(cl_command_queue /* command_queue */,
                        cl_mem           /* memobj */,
                        void *           /* mapped_ptr */,
                        cl_uint          /* num_events_in_wait_list */,
                        const cl_event *  /* event_wait_list */,
                        cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueNDRangeKernel(cl_command_queue /* command_queue */,
                       cl_kernel        /* kernel */,
                       cl_uint          /* work_dim */,
                       const size_t *   /* global_work_offset */,
                       const size_t *   /* global_work_size */,
                       const size_t *   /* local_work_size */,
                       cl_uint          /* num_events_in_wait_list */,
                       const cl_event * /* event_wait_list */,
                       cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueTask(cl_command_queue  /* command_queue */,
              cl_kernel         /* kernel */,
              cl_uint           /* num_events_in_wait_list */,
              const cl_event *  /* event_wait_list */,
              cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueNativeKernel(cl_command_queue  /* command_queue */,
					  void (*user_func)(void *),
                      void *            /* args */,
                      size_t            /* cb_args */,
                      cl_uint           /* num_mem_objects */,
                      const cl_mem *    /* mem_list */,
                      const void **     /* args_mem_loc */,
                      cl_uint           /* num_events_in_wait_list */,
                      const cl_event *  /* event_wait_list */,
                      cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueMarker(cl_command_queue    /* command_queue */,
                cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueWaitForEvents(cl_command_queue /* command_queue */,
                       cl_uint          /* num_events */,
                       const cl_event * /* event_list */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int CL_API_CALL
__clEnqueueBarrier(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

// Extension function access
//
// Returns the extension function address for the given function name,
// or NULL if a valid function can not be found.  The client must
// check to make sure the address is not NULL, before using or
// calling the returned function address.
//
typedef CL_API_ENTRY void * CL_API_CALL __clGetExtensionFunctionAddress(const char * /* func_name */) CL_API_SUFFIX__VERSION_1_0;


extern __clGetPlatformIDs                   *clGetPlatformIDs;
extern __clGetPlatformInfo                  *clGetPlatformInfo;
extern __clGetDeviceIDs                     *clGetDeviceIDs;
extern __clGetDeviceInfo                    *clGetDeviceInfo;
extern __clCreateContext                    *clCreateContext;
extern __clCreateContextFromType            *clCreateContextFromType;
extern __clRetainContext                    *clRetainContext;
extern __clReleaseContext                   *clReleaseContext;
extern __clGetContextInfo                   *clGetContextInfo;
extern __clCreateCommandQueue               *clCreateCommandQueue;
extern __clRetainCommandQueue               *clRetainCommandQueue;
extern __clReleaseCommandQueue              *clReleaseCommandQueue;
extern __clGetCommandQueueInfo              *clGetCommandQueueInfo;
extern __clSetCommandQueueProperty          *clSetCommandQueueProperty;
extern __clCreateBuffer                     *clCreateBuffer;
extern __clCreateImage2D                    *clCreateImage2D;
extern __clCreateImage3D                    *clCreateImage3D;
extern __clRetainMemObject                  *clRetainMemObject;
extern __clReleaseMemObject                 *clReleaseMemObject;
extern __clGetSupportedImageFormats         *clGetSupportedImageFormats;
extern __clGetMemObjectInfo                 *clGetMemObjectInfo;
extern __clGetImageInfo                     *clGetImageInfo;
extern __clCreateSampler                    *clCreateSampler;
extern __clRetainSampler                    *clRetainSampler;
extern __clReleaseSampler                   *clReleaseSampler;
extern __clGetSamplerInfo                   *clGetSamplerInfo;
extern __clCreateProgramWithSource          *clCreateProgramWithSource;
extern __clCreateProgramWithBinary          *clCreateProgramWithBinary;
extern __clRetainProgram                    *clRetainProgram;
extern __clReleaseProgram                   *clReleaseProgram;
extern __clBuildProgram                     *clBuildProgram;
extern __clUnloadCompiler                   *clUnloadCompiler;
extern __clGetProgramInfo                   *clGetProgramInfo;
extern __clGetProgramBuildInfo              *clGetProgramBuildInfo;
extern __clCreateKernel                     *clCreateKernel;
extern __clCreateKernelsInProgram           *clCreateKernelsInProgram;
extern __clRetainKernel                     *clRetainKernel;
extern __clReleaseKernel                    *clReleaseKernel;
extern __clSetKernelArg                     *clSetKernelArg;
extern __clGetKernelInfo                    *clGetKernelInfo;
extern __clGetKernelWorkGroupInfo           *clGetKernelWorkGroupInfo;
extern __clWaitForEvents                    *clWaitForEvents;
extern __clGetEventInfo                     *clGetEventInfo;
extern __clRetainEvent                      *clRetainEvent;
extern __clReleaseEvent                     *clReleaseEvent;
extern __clGetEventProfilingInfo            *clGetEventProfilingInfo;
extern __clFlush                            *clFlush;
extern __clFinish                           *clFinish;
extern __clEnqueueReadBuffer                *clEnqueueReadBuffer;
extern __clEnqueueWriteBuffer               *clEnqueueWriteBuffer;
extern __clEnqueueCopyBuffer                *clEnqueueCopyBuffer;
extern __clEnqueueReadImage                 *clEnqueueReadImage;
extern __clEnqueueWriteImage                *clEnqueueWriteImage;
extern __clEnqueueCopyImage                 *clEnqueueCopyImage;
extern __clEnqueueCopyImageToBuffer         *clEnqueueCopyImageToBuffer;
extern __clEnqueueCopyBufferToImage         *clEnqueueCopyBufferToImage;
extern __clEnqueueMapBuffer                 *clEnqueueMapBuffer;
extern __clEnqueueMapImage                  *clEnqueueMapImage;
extern __clEnqueueUnmapMemObject            *clEnqueueUnmapMemObject;
extern __clEnqueueNDRangeKernel             *clEnqueueNDRangeKernel;
extern __clEnqueueTask                      *clEnqueueTask;
extern __clEnqueueNativeKernel              *clEnqueueNativeKernel;
extern __clEnqueueMarker                    *clEnqueueMarker;
extern __clEnqueueWaitForEvents             *clEnqueueWaitForEvents;
extern __clEnqueueBarrier                   *clEnqueueBarrier;
extern __clGetExtensionFunctionAddress      *clGetExtensionFunctionAddress;

/// Loads the OpenCL library if available
///
/// This function must be called prior to calling anu other cl*() function
/// @return Returns CL_DEVICE_NOT_AVAILABLE if the OpenCL Library is not found
/// @return CL_DEVICE_NOT_AVAILABLE  if a library function is not found.
/// @return and CL_SUCCESS if all modules are loaded succesfully.
extern cl_int clLibLoad();


#ifdef __cplusplus
}
#endif



#endif	/* _OPENCL_DYN_LOAD_H */

