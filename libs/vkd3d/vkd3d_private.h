/*
 * Copyright 2016 Józef Kucia for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __VKD3D_PRIVATE_H
#define __VKD3D_PRIVATE_H

#define COBJMACROS
#define VK_NO_PROTOTYPES

#include "vkd3d_common.h"
#include "vkd3d_memory.h"
#include "vkd3d_utf8.h"
#include "hashmap.h"
#include "list.h"
#include "rbtree.h"

#include "vkd3d.h"
#include "vkd3d_build.h"
#include "vkd3d_version.h"
#include "vkd3d_shader.h"
#include "vkd3d_threads.h"
#include "vkd3d_platform.h"
#include "vkd3d_swapchain_factory.h"
#include "vkd3d_command_list_vkd3d_ext.h"
#include "vkd3d_device_vkd3d_ext.h"
#include "vkd3d_string.h"
#include "vkd3d_file_utils.h"
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>

#define VK_CALL(f) (vk_procs->f)

#define MAKE_MAGIC(a,b,c,d) (((uint32_t)a) | (((uint32_t)b) << 8) | (((uint32_t)c) << 16) | (((uint32_t)d) << 24))

#define VKD3D_MAX_COMPATIBLE_FORMAT_COUNT 10u
#define VKD3D_MAX_SHADER_STAGES           5u
#define VKD3D_MAX_VK_SYNC_OBJECTS         4u

#define VKD3D_MAX_DESCRIPTOR_SETS 11u
#define VKD3D_MAX_BINDLESS_DESCRIPTOR_SETS 8u
#define VKD3D_MAX_MUTABLE_DESCRIPTOR_TYPES 6u

#define VKD3D_TILE_SIZE 65536

typedef ID3D12Fence1 d3d12_fence_iface;

struct d3d12_command_list;
struct d3d12_device;
struct d3d12_resource;

struct vkd3d_bindless_set_info;
struct vkd3d_dynamic_state;

struct vkd3d_vk_global_procs
{
    PFN_vkCreateInstance vkCreateInstance;
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
};

#define DECLARE_VK_PFN(name) PFN_##name name;
struct vkd3d_vk_instance_procs
{
#define VK_INSTANCE_PFN     DECLARE_VK_PFN
#define VK_INSTANCE_EXT_PFN DECLARE_VK_PFN
#include "vulkan_procs.h"
};

struct vkd3d_vk_device_procs
{
#define VK_INSTANCE_PFN     DECLARE_VK_PFN
#define VK_INSTANCE_EXT_PFN DECLARE_VK_PFN
#define VK_DEVICE_PFN       DECLARE_VK_PFN
#define VK_DEVICE_EXT_PFN   DECLARE_VK_PFN
#include "vulkan_procs.h"
};
#undef DECLARE_VK_PFN

HRESULT hresult_from_errno(int rc);
HRESULT hresult_from_vk_result(VkResult vr);
HRESULT hresult_from_vkd3d_result(int vkd3d_result);

struct vkd3d_vulkan_info
{
    /* EXT instance extensions */
    bool EXT_debug_utils;

    /* KHR device extensions */
    bool KHR_buffer_device_address;
    bool KHR_draw_indirect_count;
    bool KHR_image_format_list;
    bool KHR_push_descriptor;
    bool KHR_timeline_semaphore;
    bool KHR_shader_float16_int8;
    bool KHR_shader_subgroup_extended_types;
    bool KHR_ray_tracing_pipeline;
    bool KHR_acceleration_structure;
    bool KHR_deferred_host_operations;
    bool KHR_pipeline_library;
    bool KHR_ray_query;
    bool KHR_spirv_1_4;
    bool KHR_shader_float_controls;
    bool KHR_fragment_shading_rate;
    bool KHR_create_renderpass2;
    bool KHR_sampler_mirror_clamp_to_edge;
    bool KHR_separate_depth_stencil_layouts;
    bool KHR_shader_integer_dot_product;
    bool KHR_format_feature_flags2;
    bool KHR_shader_atomic_int64;
    bool KHR_bind_memory2;
    bool KHR_copy_commands2;
    bool KHR_dynamic_rendering;
    bool KHR_depth_stencil_resolve;
    bool KHR_driver_properties;
    bool KHR_uniform_buffer_standard_layout;
    bool KHR_maintenance4;
    bool KHR_ray_tracing_maintenance1;
    bool KHR_fragment_shader_barycentric;
    bool KHR_external_memory_win32;
    bool KHR_external_semaphore_win32;
    /* EXT device extensions */
    bool EXT_calibrated_timestamps;
    bool EXT_conditional_rendering;
    bool EXT_conservative_rasterization;
    bool EXT_custom_border_color;
    bool EXT_depth_clip_enable;
    bool EXT_descriptor_indexing;
    bool EXT_image_view_min_lod;
    bool EXT_inline_uniform_block;
    bool EXT_robustness2;
    bool EXT_sampler_filter_minmax;
    bool EXT_shader_demote_to_helper_invocation;
    bool EXT_shader_stencil_export;
    bool EXT_shader_viewport_index_layer;
    bool EXT_subgroup_size_control;
    bool EXT_texel_buffer_alignment;
    bool EXT_transform_feedback;
    bool EXT_vertex_attribute_divisor;
    bool EXT_extended_dynamic_state;
    bool EXT_extended_dynamic_state2;
    bool EXT_external_memory_host;
    bool EXT_4444_formats;
    bool EXT_shader_image_atomic_int64;
    bool EXT_scalar_block_layout;
    bool EXT_pipeline_creation_feedback;
    bool EXT_mesh_shader;
    /* AMD device extensions */
    bool AMD_buffer_marker;
    bool AMD_device_coherent_memory;
    bool AMD_shader_core_properties;
    bool AMD_shader_core_properties2;
    /* NV device extensions */
    bool NV_shader_sm_builtins;
    bool NVX_binary_import;
    bool NVX_image_view_handle;
    bool NV_fragment_shader_barycentric;
    bool NV_compute_shader_derivatives;
    bool NV_device_diagnostic_checkpoints;
    bool NV_device_generated_commands;
    /* VALVE extensions */
    bool VALVE_mutable_descriptor_type;
    bool VALVE_descriptor_set_host_mapping;

    bool rasterization_stream;
    bool transform_feedback_queries;

    bool vertex_attrib_zero_divisor;
    unsigned int max_vertex_attrib_divisor;

    VkPhysicalDeviceLimits device_limits;
    VkPhysicalDeviceSparseProperties sparse_properties;

    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texel_buffer_alignment_properties;

    unsigned int shader_extension_count;
    enum vkd3d_shader_target_extension shader_extensions[VKD3D_SHADER_TARGET_EXTENSION_COUNT];
};

struct vkd3d_instance
{
    VkInstance vk_instance;
    uint32_t instance_version;
    struct vkd3d_vk_instance_procs vk_procs;

    PFN_vkd3d_signal_event signal_event;
    PFN_vkd3d_create_thread create_thread;
    PFN_vkd3d_join_thread join_thread;

    struct vkd3d_vulkan_info vk_info;
    struct vkd3d_vk_global_procs vk_global_procs;
    void *libvulkan;

    VkDebugUtilsMessengerEXT vk_debug_callback;

    LONG refcount;
};

extern uint64_t vkd3d_config_flags;
extern struct vkd3d_shader_quirk_info vkd3d_shader_quirk_info;

union vkd3d_thread_handle
{
    pthread_t pthread;
    void *handle;
};

HRESULT vkd3d_create_thread(struct vkd3d_instance *instance,
        PFN_vkd3d_thread thread_main, void *data, union vkd3d_thread_handle *thread);
HRESULT vkd3d_join_thread(struct vkd3d_instance *instance, union vkd3d_thread_handle *thread);

struct vkd3d_waiting_fence
{
    d3d12_fence_iface *fence;
    VkSemaphore submission_timeline;
    uint64_t value;
    LONG **submission_counters;
    size_t num_submission_counts;
    bool signal;
};

struct vkd3d_fence_worker
{
    union vkd3d_thread_handle thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool should_exit;

    uint32_t enqueued_fence_count;
    struct vkd3d_waiting_fence *enqueued_fences;
    size_t enqueued_fences_size;

    struct d3d12_device *device;
};

HRESULT vkd3d_fence_worker_start(struct vkd3d_fence_worker *worker,
        struct d3d12_device *device);
HRESULT vkd3d_fence_worker_stop(struct vkd3d_fence_worker *worker,
        struct d3d12_device *device);

/* 2 MiB is a good threshold, because it's huge page size. */
#define VKD3D_VA_BLOCK_SIZE_BITS (21)
#define VKD3D_VA_BLOCK_SIZE (1ull << VKD3D_VA_BLOCK_SIZE_BITS)
#define VKD3D_VA_LO_MASK (VKD3D_VA_BLOCK_SIZE - 1)

#define VKD3D_VA_BLOCK_BITS (20)
#define VKD3D_VA_BLOCK_COUNT (1ull << VKD3D_VA_BLOCK_BITS)
#define VKD3D_VA_BLOCK_MASK (VKD3D_VA_BLOCK_COUNT - 1)

#define VKD3D_VA_NEXT_BITS (12)
#define VKD3D_VA_NEXT_COUNT (1ull << VKD3D_VA_NEXT_BITS)
#define VKD3D_VA_NEXT_MASK (VKD3D_VA_NEXT_COUNT - 1)

struct vkd3d_unique_resource;

struct vkd3d_va_entry
{
    DECLSPEC_ALIGN(8) VkDeviceAddress va;
    const struct vkd3d_unique_resource *resource;
};

struct vkd3d_va_block
{
    struct vkd3d_va_entry l;
    struct vkd3d_va_entry r;
};

struct vkd3d_va_tree
{
    struct vkd3d_va_block blocks[VKD3D_VA_BLOCK_COUNT];
    struct vkd3d_va_tree *next[VKD3D_VA_NEXT_COUNT];
};

struct vkd3d_va_range
{
    VkDeviceAddress base;
    VkDeviceSize size;
};

struct vkd3d_va_allocator
{
    pthread_mutex_t mutex;

    struct vkd3d_va_range *free_ranges;
    size_t free_ranges_size;
    size_t free_range_count;

    VkDeviceAddress next_va;
};

struct vkd3d_va_map
{
    struct vkd3d_va_tree va_tree;
    struct vkd3d_va_allocator va_allocator;

    pthread_mutex_t mutex;

    struct vkd3d_unique_resource **small_entries;
    size_t small_entries_size;
    size_t small_entries_count;
};

void vkd3d_va_map_insert(struct vkd3d_va_map *va_map, struct vkd3d_unique_resource *resource);
void vkd3d_va_map_remove(struct vkd3d_va_map *va_map, const struct vkd3d_unique_resource *resource);
const struct vkd3d_unique_resource *vkd3d_va_map_deref(struct vkd3d_va_map *va_map, VkDeviceAddress va);
VkAccelerationStructureKHR vkd3d_va_map_place_acceleration_structure(struct vkd3d_va_map *va_map,
        struct d3d12_device *device,
        VkDeviceAddress va);
VkDeviceAddress vkd3d_va_map_alloc_fake_va(struct vkd3d_va_map *va_map, VkDeviceSize size);
void vkd3d_va_map_free_fake_va(struct vkd3d_va_map *va_map, VkDeviceAddress va, VkDeviceSize size);
void vkd3d_va_map_init(struct vkd3d_va_map *va_map);
void vkd3d_va_map_cleanup(struct vkd3d_va_map *va_map);

struct vkd3d_gpu_va_allocation
{
    D3D12_GPU_VIRTUAL_ADDRESS base;
    size_t size;
    void *ptr;
};

struct vkd3d_gpu_va_slab
{
    size_t size;
    void *ptr;
};

struct vkd3d_private_store
{
    pthread_mutex_t mutex;

    struct list content;
};

struct vkd3d_private_data
{
    struct list entry;

    GUID tag;
    unsigned int size;
    bool is_object;
    union
    {
        BYTE data[1];
        IUnknown *object;
    };
};

static inline void vkd3d_private_data_destroy(struct vkd3d_private_data *data)
{
    if (data->is_object)
        IUnknown_Release(data->object);
    list_remove(&data->entry);
    vkd3d_free(data);
}

static inline HRESULT vkd3d_private_store_init(struct vkd3d_private_store *store)
{
    int rc;

    list_init(&store->content);

    if ((rc = pthread_mutex_init(&store->mutex, NULL)))
        ERR("Failed to initialize mutex, error %d.\n", rc);

    return hresult_from_errno(rc);
}

static inline void vkd3d_private_store_destroy(struct vkd3d_private_store *store)
{
    struct vkd3d_private_data *data, *cursor;

    LIST_FOR_EACH_ENTRY_SAFE(data, cursor, &store->content, struct vkd3d_private_data, entry)
    {
        vkd3d_private_data_destroy(data);
    }

    pthread_mutex_destroy(&store->mutex);
}

static inline HRESULT vkd3d_private_data_lock(struct vkd3d_private_store *store)
{
    int rc;
    if ((rc = pthread_mutex_lock(&store->mutex)))
    {
        ERR("Failed to lock mutex, error %d.\n", rc);
        return hresult_from_errno(rc);
    }

    return S_OK;
}

static inline void vkd3d_private_data_unlock(struct vkd3d_private_store *store)
{
    pthread_mutex_unlock(&store->mutex);
}

HRESULT vkd3d_get_private_data(struct vkd3d_private_store *store,
        const GUID *tag, unsigned int *out_size, void *out);

HRESULT vkd3d_private_store_set_private_data(struct vkd3d_private_store *store,
        const GUID *tag, const void *data, unsigned int data_size, bool is_object);

typedef void(*vkd3d_set_name_callback)(void *, const char *);

static inline bool vkd3d_private_data_object_name_ptr(REFGUID guid,
    UINT data_size, const void *data, const char **out_name)
{
    if (out_name)
        *out_name = NULL;

    /* This is also handled in the object_name implementation
     * but this avoids an additional, needless allocation
     * and some games may spam SetName.
     */
    if (!(vkd3d_config_flags & VKD3D_CONFIG_FLAG_DEBUG_UTILS))
        return false;

    if (IsEqualGUID(guid, &WKPDID_D3DDebugObjectName))
    {
        const char *name = (const char *)data;

        if (!data || !data_size)
            return true;

        if (out_name)
            *out_name = name[data_size - 1] != '\0'
                ? vkd3d_strdup_n(name, data_size)
                : name;

        return true;
    }
    else if (IsEqualGUID(guid, &WKPDID_D3DDebugObjectNameW))
    {
        const WCHAR *name = (const WCHAR *)data;

        if (!data || data_size < sizeof(WCHAR))
            return true;

        if (out_name)
            *out_name = vkd3d_strdup_w_utf8(name, data_size / sizeof(WCHAR));
        return true;
    }

    return false;
}

static inline HRESULT vkd3d_set_private_data(struct vkd3d_private_store *store,
        const GUID *tag, unsigned int data_size, const void *data,
        vkd3d_set_name_callback set_name_callback, void *calling_object)
{
    const char *name;
    HRESULT hr;

    if (FAILED(hr = vkd3d_private_data_lock(store)))
        return hr;

    if (FAILED(hr = vkd3d_private_store_set_private_data(store, tag, data, data_size, false)))
    {
        vkd3d_private_data_unlock(store);
        return hr;
    }

    if (set_name_callback && vkd3d_private_data_object_name_ptr(tag, data_size, data, &name))
    {
        set_name_callback(calling_object, name);
        if (name && name != data)
            vkd3d_free((void *)name);
    }

    vkd3d_private_data_unlock(store);
    return hr;
}

static inline HRESULT vkd3d_set_private_data_interface(struct vkd3d_private_store *store,
        const GUID *tag, const IUnknown *object,
        vkd3d_set_name_callback set_name_callback, void *calling_object)
{
    const void *data = object ? object : (void *)&object;
    HRESULT hr;

    if (FAILED(hr = vkd3d_private_data_lock(store)))
        return hr;

    if (FAILED(hr = vkd3d_private_store_set_private_data(store, tag, data, sizeof(object), !!object)))
    {
        vkd3d_private_data_unlock(store);
        return hr;
    }

    if (set_name_callback && vkd3d_private_data_object_name_ptr(tag, 0, NULL, NULL))
        set_name_callback(calling_object, NULL);

    vkd3d_private_data_unlock(store);
    return hr;
}

HRESULT STDMETHODCALLTYPE d3d12_object_SetName(ID3D12Object *iface, const WCHAR *name);

/* ID3D12Fence */
struct d3d12_fence_value
{
    uint64_t virtual_value;
    uint64_t physical_value;
    const struct vkd3d_queue *signalling_queue;
};

enum vkd3d_waiting_event_type
{
    VKD3D_WAITING_EVENT_TYPE_EVENT,
    VKD3D_WAITING_EVENT_TYPE_SEMAPHORE,
};

struct d3d12_fence
{
    d3d12_fence_iface ID3D12Fence_iface;
    LONG refcount_internal;
    LONG refcount;

    D3D12_FENCE_FLAGS d3d12_flags;

    VkSemaphore timeline_semaphore;

    uint64_t max_pending_virtual_timeline_value;
    uint64_t virtual_value;
    uint64_t physical_value;
    uint64_t counter;
    struct d3d12_fence_value *pending_updates;
    size_t pending_updates_count;
    size_t pending_updates_size;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t null_event_cond;

    struct vkd3d_waiting_event
    {
        uint64_t value;
        HANDLE event;
        enum vkd3d_waiting_event_type type;
        bool *latch;
    } *events;
    size_t events_size;
    size_t event_count;

    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

static inline struct d3d12_fence *impl_from_ID3D12Fence1(ID3D12Fence1 *iface)
{
    extern CONST_VTBL struct ID3D12Fence1Vtbl d3d12_fence_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_fence_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_fence, ID3D12Fence_iface);
}

static inline struct d3d12_fence *impl_from_ID3D12Fence(ID3D12Fence *iface)
{
    return impl_from_ID3D12Fence1((ID3D12Fence1 *)iface);
}

HRESULT d3d12_fence_create(struct d3d12_device *device,
        uint64_t initial_value, D3D12_FENCE_FLAGS flags, struct d3d12_fence **fence);
HRESULT d3d12_fence_set_event_on_completion(struct d3d12_fence *fence,
        UINT64 value, HANDLE event, enum vkd3d_waiting_event_type type);

struct d3d12_shared_fence
{
    d3d12_fence_iface ID3D12Fence_iface;
    LONG refcount_internal;
    LONG refcount;

    D3D12_FENCE_FLAGS d3d12_flags;

    VkSemaphore timeline_semaphore;

    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

static inline struct d3d12_shared_fence *shared_impl_from_ID3D12Fence1(ID3D12Fence1 *iface)
{
    extern CONST_VTBL struct ID3D12Fence1Vtbl d3d12_shared_fence_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_shared_fence_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_shared_fence, ID3D12Fence_iface);
}

static inline struct d3d12_shared_fence *shared_impl_from_ID3D12Fence(ID3D12Fence *iface)
{
    return shared_impl_from_ID3D12Fence1((ID3D12Fence1 *)iface);
}

HRESULT d3d12_shared_fence_create(struct d3d12_device *device,
        uint64_t initial_value, D3D12_FENCE_FLAGS flags, struct d3d12_shared_fence **fence);

static inline bool is_shared_ID3D12Fence1(ID3D12Fence1 *iface)
{
    extern CONST_VTBL struct ID3D12Fence1Vtbl d3d12_shared_fence_vtbl;
    extern CONST_VTBL struct ID3D12Fence1Vtbl d3d12_fence_vtbl;
    assert(iface->lpVtbl ==  &d3d12_shared_fence_vtbl || iface->lpVtbl == &d3d12_fence_vtbl);

    return iface->lpVtbl ==  &d3d12_shared_fence_vtbl;
}

static inline bool is_shared_ID3D12Fence(ID3D12Fence *iface)
{
    return is_shared_ID3D12Fence1((ID3D12Fence1 *)iface);
}

enum vkd3d_allocation_flag
{
    VKD3D_ALLOCATION_FLAG_GLOBAL_BUFFER     = (1u << 0),
    VKD3D_ALLOCATION_FLAG_GPU_ADDRESS       = (1u << 1),
    VKD3D_ALLOCATION_FLAG_CPU_ACCESS        = (1u << 2),
    VKD3D_ALLOCATION_FLAG_ALLOW_WRITE_WATCH = (1u << 3),
    VKD3D_ALLOCATION_FLAG_NO_FALLBACK       = (1u << 4),
    VKD3D_ALLOCATION_FLAG_DEDICATED         = (1u << 5),
    /* Intended for internal allocation of scratch buffers.
     * They are never suballocated since we do that ourselves,
     * and we do not consume space in the VA map. */
    VKD3D_ALLOCATION_FLAG_INTERNAL_SCRATCH  = (1u << 6),
};

#define VKD3D_MEMORY_CHUNK_SIZE (VKD3D_VA_BLOCK_SIZE * 8)

struct vkd3d_memory_chunk;

struct vkd3d_allocate_memory_info
{
    VkMemoryRequirements memory_requirements;
    D3D12_HEAP_PROPERTIES heap_properties;
    D3D12_HEAP_FLAGS heap_flags;
    void *host_ptr;
    const void *pNext;
    uint32_t flags;
    VkMemoryPropertyFlags optional_memory_properties;
};

struct vkd3d_allocate_heap_memory_info
{
    D3D12_HEAP_DESC heap_desc;
    void *host_ptr;
    uint32_t extra_allocation_flags;
};

struct vkd3d_allocate_resource_memory_info
{
    D3D12_HEAP_PROPERTIES heap_properties;
    D3D12_HEAP_FLAGS heap_flags;
    VkBuffer vk_buffer;
    VkImage vk_image;
    void *host_ptr;
};

struct vkd3d_view_map;

struct vkd3d_unique_resource
{
    union
    {
        VkBuffer vk_buffer;
        VkImage vk_image;
    };
    uint64_t cookie;
    VkDeviceAddress va;
    VkDeviceSize size;

    /* This is used to handle views when we cannot bind it to a
     * specific ID3D12Resource, i.e. RTAS. Only allocated as needed. */
    struct vkd3d_view_map *view_map;
};

struct vkd3d_device_memory_allocation
{
    VkDeviceMemory vk_memory;
    uint32_t vk_memory_type;
    VkDeviceSize size;
};

struct vkd3d_memory_allocation
{
    struct vkd3d_unique_resource resource;
    struct vkd3d_device_memory_allocation device_allocation;
    VkDeviceSize offset;
    void *cpu_address;

    D3D12_HEAP_TYPE heap_type;
    D3D12_HEAP_FLAGS heap_flags;
    uint32_t flags;

    uint64_t clear_semaphore_value;

    struct vkd3d_memory_chunk *chunk;
};

static inline void vkd3d_memory_allocation_slice(struct vkd3d_memory_allocation *dst,
        const struct vkd3d_memory_allocation *src, VkDeviceSize offset, VkDeviceSize size)
{
    *dst = *src;
    dst->offset += offset;
    dst->resource.size = size;
    dst->resource.va += offset;

    if (dst->cpu_address)
        dst->cpu_address = void_ptr_offset(dst->cpu_address, offset);
}

struct vkd3d_memory_free_range
{
    VkDeviceSize offset;
    VkDeviceSize length;
};

struct vkd3d_memory_chunk
{
    struct vkd3d_memory_allocation allocation;
    struct vkd3d_memory_free_range *free_ranges;
    size_t free_ranges_size;
    size_t free_ranges_count;
};

#define VKD3D_MEMORY_CLEAR_COMMAND_BUFFER_COUNT (16u)

struct vkd3d_memory_clear_queue
{
    pthread_mutex_t mutex;

    VkCommandBuffer vk_command_buffers[VKD3D_MEMORY_CLEAR_COMMAND_BUFFER_COUNT];
    VkCommandPool vk_command_pool;
    VkSemaphore vk_semaphore;

    UINT64 last_known_value;
    UINT64 next_signal_value;

    VkDeviceSize num_bytes_pending;
    uint32_t command_buffer_index;

    struct vkd3d_memory_allocation **allocations;
    size_t allocations_size;
    size_t allocations_count;
};

struct vkd3d_memory_allocator
{
    pthread_mutex_t mutex;

    struct vkd3d_memory_chunk **chunks;
    size_t chunks_size;
    size_t chunks_count;

    struct vkd3d_va_map va_map;

    struct vkd3d_queue *vkd3d_queue;
    struct vkd3d_memory_clear_queue clear_queue;
};

void vkd3d_free_memory(struct d3d12_device *device, struct vkd3d_memory_allocator *allocator,
        const struct vkd3d_memory_allocation *allocation);
HRESULT vkd3d_allocate_memory(struct d3d12_device *device, struct vkd3d_memory_allocator *allocator,
        const struct vkd3d_allocate_memory_info *info, struct vkd3d_memory_allocation *allocation);
HRESULT vkd3d_allocate_heap_memory(struct d3d12_device *device, struct vkd3d_memory_allocator *allocator,
        const struct vkd3d_allocate_heap_memory_info *info, struct vkd3d_memory_allocation *allocation);

HRESULT vkd3d_memory_allocator_init(struct vkd3d_memory_allocator *allocator, struct d3d12_device *device);
void vkd3d_memory_allocator_cleanup(struct vkd3d_memory_allocator *allocator, struct d3d12_device *device);
HRESULT vkd3d_memory_allocator_flush_clears(struct vkd3d_memory_allocator *allocator, struct d3d12_device *device);

/* ID3D12Heap */
typedef ID3D12Heap1 d3d12_heap_iface;

struct d3d12_heap
{
    d3d12_heap_iface ID3D12Heap_iface;
    LONG refcount;

    D3D12_HEAP_DESC desc;
    struct vkd3d_memory_allocation allocation;

    struct d3d12_device *device;
    struct vkd3d_private_store private_store;
};

HRESULT d3d12_heap_create(struct d3d12_device *device, const D3D12_HEAP_DESC *desc,
        void *host_address, struct d3d12_heap **heap);
HRESULT d3d12_device_validate_custom_heap_type(struct d3d12_device *device,
        const D3D12_HEAP_PROPERTIES *heap_properties);

static inline struct d3d12_heap *impl_from_ID3D12Heap1(ID3D12Heap1 *iface)
{
    extern CONST_VTBL struct ID3D12Heap1Vtbl d3d12_heap_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_heap_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_heap, ID3D12Heap_iface);
}

static inline struct d3d12_heap *impl_from_ID3D12Heap(ID3D12Heap *iface)
{
    return impl_from_ID3D12Heap1((ID3D12Heap1 *)iface);
}

enum vkd3d_resource_flag
{
    VKD3D_RESOURCE_COMMITTED              = (1u << 0),
    VKD3D_RESOURCE_PLACED                 = (1u << 1),
    VKD3D_RESOURCE_RESERVED               = (1u << 2),
    VKD3D_RESOURCE_ALLOCATION             = (1u << 3),
    VKD3D_RESOURCE_LINEAR_TILING          = (1u << 4),
    VKD3D_RESOURCE_EXTERNAL               = (1u << 5),
    VKD3D_RESOURCE_ACCELERATION_STRUCTURE = (1u << 6),
    VKD3D_RESOURCE_SIMULTANEOUS_ACCESS    = (1u << 7),
};

struct d3d12_sparse_image_region
{
    VkImageSubresource subresource;
    uint32_t subresource_index;
    VkOffset3D offset;
    VkExtent3D extent;
};

struct d3d12_sparse_buffer_region
{
    VkDeviceSize offset;
    VkDeviceSize length;
};

struct d3d12_sparse_tile
{
    union
    {
        struct d3d12_sparse_image_region image;
        struct d3d12_sparse_buffer_region buffer;
    };
    VkDeviceMemory vk_memory;
    VkDeviceSize vk_offset;
};

struct d3d12_sparse_info
{
    uint32_t tile_count;
    uint32_t tiling_count;
    struct d3d12_sparse_tile *tiles;
    D3D12_TILE_SHAPE tile_shape;
    D3D12_PACKED_MIP_INFO packed_mips;
    D3D12_SUBRESOURCE_TILING *tilings;
    struct vkd3d_device_memory_allocation vk_metadata_memory;
};

struct vkd3d_view_map
{
    spinlock_t spinlock;
    struct hash_map map;
#ifdef VKD3D_ENABLE_DESCRIPTOR_QA
    uint64_t resource_cookie;
#endif
};

HRESULT vkd3d_view_map_init(struct vkd3d_view_map *view_map);
void vkd3d_view_map_destroy(struct vkd3d_view_map *view_map, struct d3d12_device *device);

/* ID3D12Resource */
typedef ID3D12Resource2 d3d12_resource_iface;

struct d3d12_resource
{
    d3d12_resource_iface ID3D12Resource_iface;
    LONG refcount;
    LONG internal_refcount;

    D3D12_RESOURCE_DESC1 desc;
    D3D12_HEAP_PROPERTIES heap_properties;
    D3D12_HEAP_FLAGS heap_flags;
    struct vkd3d_memory_allocation mem;
    struct vkd3d_unique_resource res;

    struct d3d12_heap *heap;

    uint32_t flags;

    /* To keep track of initial layout. */
    VkImageLayout common_layout;
    D3D12_RESOURCE_STATES initial_state;
    uint32_t initial_layout_transition;

    struct d3d12_sparse_info sparse;
    struct vkd3d_view_map view_map;

    struct d3d12_device *device;

    const struct vkd3d_format *format;

    VkImageView vrs_view;

    struct vkd3d_private_store private_store;
};

static inline bool d3d12_resource_is_buffer(const struct d3d12_resource *resource)
{
    return resource->desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER;
}

static inline bool d3d12_resource_is_acceleration_structure(const struct d3d12_resource *resource)
{
    return !!(resource->flags & VKD3D_RESOURCE_ACCELERATION_STRUCTURE);
}

static inline bool d3d12_resource_is_texture(const struct d3d12_resource *resource)
{
    return resource->desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER;
}

static inline VkImageLayout d3d12_resource_pick_layout(const struct d3d12_resource *resource, VkImageLayout layout)
{
    return resource->flags & (VKD3D_RESOURCE_LINEAR_TILING | VKD3D_RESOURCE_SIMULTANEOUS_ACCESS) ?
            resource->common_layout : layout;
}

LONG64 vkd3d_allocate_cookie();

bool d3d12_resource_is_cpu_accessible(const struct d3d12_resource *resource);
void d3d12_resource_promote_desc(const D3D12_RESOURCE_DESC *desc, D3D12_RESOURCE_DESC1 *desc1);
HRESULT d3d12_resource_validate_desc(const D3D12_RESOURCE_DESC1 *desc, struct d3d12_device *device);
VkImageSubresource d3d12_resource_get_vk_subresource(const struct d3d12_resource *resource,
        uint32_t subresource_idx, bool all_aspects);
VkImageAspectFlags vk_image_aspect_flags_from_d3d12(
        const struct vkd3d_format *format, uint32_t plane_idx);
VkImageSubresource vk_image_subresource_from_d3d12(
        const struct vkd3d_format *format, uint32_t subresource_idx,
        unsigned int miplevel_count, unsigned int layer_count,
        bool all_aspects);

HRESULT d3d12_resource_create_committed(struct d3d12_device *device, const D3D12_RESOURCE_DESC1 *desc,
        const D3D12_HEAP_PROPERTIES *heap_properties, D3D12_HEAP_FLAGS heap_flags, D3D12_RESOURCE_STATES initial_state,
        const D3D12_CLEAR_VALUE *optimized_clear_value, HANDLE shared_handle, struct d3d12_resource **resource);
HRESULT d3d12_resource_create_placed(struct d3d12_device *device, const D3D12_RESOURCE_DESC1 *desc,
        struct d3d12_heap *heap, uint64_t heap_offset, D3D12_RESOURCE_STATES initial_state,
        const D3D12_CLEAR_VALUE *optimized_clear_value, struct d3d12_resource **resource);
HRESULT d3d12_resource_create_reserved(struct d3d12_device *device,
        const D3D12_RESOURCE_DESC1 *desc, D3D12_RESOURCE_STATES initial_state,
        const D3D12_CLEAR_VALUE *optimized_clear_value, struct d3d12_resource **resource);

static inline struct d3d12_resource *impl_from_ID3D12Resource2(ID3D12Resource2 *iface)
{
    extern CONST_VTBL struct ID3D12Resource2Vtbl d3d12_resource_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_resource_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_resource, ID3D12Resource_iface);
}

static inline struct d3d12_resource *impl_from_ID3D12Resource(ID3D12Resource *iface)
{
    return impl_from_ID3D12Resource2((ID3D12Resource2 *)iface);
}

HRESULT vkd3d_allocate_device_memory(struct d3d12_device *device,
        VkDeviceSize size, VkMemoryPropertyFlags type_flags, uint32_t type_mask,
        void *pNext, struct vkd3d_device_memory_allocation *allocation);
void vkd3d_free_device_memory(struct d3d12_device *device,
        const struct vkd3d_device_memory_allocation *allocation);
HRESULT vkd3d_allocate_buffer_memory(struct d3d12_device *device, VkBuffer vk_buffer,
        VkMemoryPropertyFlags type_flags,
        struct vkd3d_device_memory_allocation *allocation);
HRESULT vkd3d_allocate_image_memory(struct d3d12_device *device, VkImage vk_image,
        VkMemoryPropertyFlags type_flags,
        struct vkd3d_device_memory_allocation *allocation);
HRESULT vkd3d_create_buffer(struct d3d12_device *device,
        const D3D12_HEAP_PROPERTIES *heap_properties, D3D12_HEAP_FLAGS heap_flags,
        const D3D12_RESOURCE_DESC1 *desc, VkBuffer *vk_buffer);
HRESULT vkd3d_get_image_allocation_info(struct d3d12_device *device,
        const D3D12_RESOURCE_DESC1 *desc, D3D12_RESOURCE_ALLOCATION_INFO *allocation_info);

enum vkd3d_view_type
{
    VKD3D_VIEW_TYPE_BUFFER,
    VKD3D_VIEW_TYPE_IMAGE,
    VKD3D_VIEW_TYPE_SAMPLER,
    VKD3D_VIEW_TYPE_ACCELERATION_STRUCTURE
};

struct vkd3d_view
{
    LONG refcount;
    enum vkd3d_view_type type;
    uint64_t cookie;

    union
    {
        VkBufferView vk_buffer_view;
        VkImageView vk_image_view;
        VkSampler vk_sampler;
        VkAccelerationStructureKHR vk_acceleration_structure;
    };
    const struct vkd3d_format *format;
    union
    {
        struct
        {
            VkDeviceSize offset;
            VkDeviceSize size;
        } buffer;
        struct
        {
            VkImageViewType vk_view_type;
            unsigned int miplevel_idx;
            unsigned int layer_idx;
            unsigned int layer_count;
        } texture;
    } info;
};

void vkd3d_view_decref(struct vkd3d_view *view, struct d3d12_device *device);
void vkd3d_view_incref(struct vkd3d_view *view);

struct vkd3d_buffer_view_desc
{
    VkBuffer buffer;
    const struct vkd3d_format *format;
    VkDeviceSize offset;
    VkDeviceSize size;
};

struct vkd3d_texture_view_desc
{
    VkImage image;
    VkImageViewType view_type;
    VkImageAspectFlags aspect_mask;
    VkImageUsageFlags image_usage;
    const struct vkd3d_format *format;
    unsigned int miplevel_idx;
    unsigned int miplevel_count;
    unsigned int layer_idx;
    unsigned int layer_count;
    float miplevel_clamp;
    VkComponentMapping components;
    bool allowed_swizzle;
};

bool vkd3d_create_buffer_view(struct d3d12_device *device,
        const struct vkd3d_buffer_view_desc *desc, struct vkd3d_view **view);
bool vkd3d_create_raw_r32ui_vk_buffer_view(struct d3d12_device *device,
        VkBuffer vk_buffer, VkDeviceSize offset, VkDeviceSize range, VkBufferView *vk_view);
bool vkd3d_create_acceleration_structure_view(struct d3d12_device *device,
        const struct vkd3d_buffer_view_desc *desc, struct vkd3d_view **view);
bool vkd3d_create_texture_view(struct d3d12_device *device,
        const struct vkd3d_texture_view_desc *desc, struct vkd3d_view **view);

enum vkd3d_descriptor_flag
{
    VKD3D_DESCRIPTOR_FLAG_VIEW              = (1 << 0),
    VKD3D_DESCRIPTOR_FLAG_RAW_VA_AUX_BUFFER = (1 << 1),
    VKD3D_DESCRIPTOR_FLAG_BUFFER_OFFSET     = (1 << 2),
    VKD3D_DESCRIPTOR_FLAG_OFFSET_RANGE      = (1 << 3),
    VKD3D_DESCRIPTOR_FLAG_NON_NULL          = (1 << 4),
    VKD3D_DESCRIPTOR_FLAG_SINGLE_DESCRIPTOR = (1 << 5),
};

struct vkd3d_descriptor_binding
{
    uint8_t set;
    uint8_t binding;
};

#define VKD3D_RESOURCE_DESC_INCREMENT_LOG2 5
#define VKD3D_RESOURCE_DESC_INCREMENT (1u << VKD3D_RESOURCE_DESC_INCREMENT_LOG2)

/* Arrange data so that it can pack as tightly as possible.
 * When we copy descriptors, we must copy both structures.
 * In copy_desc_range we scan through the entire metadata_binding, so
 * this data structure should be small. */
struct vkd3d_descriptor_metadata_types
{
    VkDescriptorType current_null_type;
    uint8_t set_info_mask;
    uint8_t flags;
    /* If SINGLE_DESCRIPTOR is set, use the embedded write info instead
     * to avoid missing caches. */
    struct vkd3d_descriptor_binding single_binding;
};
STATIC_ASSERT(sizeof(struct vkd3d_descriptor_metadata_types) == 8);
/* Our use of 8-bit mask relies on MAX_BINDLESS_DESCRIPTOR_SETS fitting. */
STATIC_ASSERT(VKD3D_MAX_BINDLESS_DESCRIPTOR_SETS <= 8);

struct vkd3d_descriptor_metadata_view
{
    uint64_t cookie;
    union
    {
        VkDescriptorBufferInfo buffer;
        struct vkd3d_view *view;
    } info;
};
STATIC_ASSERT(sizeof(struct vkd3d_descriptor_metadata_view) == 32);

typedef uintptr_t vkd3d_cpu_descriptor_va_t;

void d3d12_desc_copy(vkd3d_cpu_descriptor_va_t dst, vkd3d_cpu_descriptor_va_t src,
        unsigned int count, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, struct d3d12_device *device);
void d3d12_desc_copy_single(vkd3d_cpu_descriptor_va_t dst,
        vkd3d_cpu_descriptor_va_t src, struct d3d12_device *device);
void d3d12_desc_create_cbv(vkd3d_cpu_descriptor_va_t descriptor,
        struct d3d12_device *device, const D3D12_CONSTANT_BUFFER_VIEW_DESC *desc);
void d3d12_desc_create_srv(vkd3d_cpu_descriptor_va_t descriptor,
        struct d3d12_device *device, struct d3d12_resource *resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC *desc);
void d3d12_desc_create_uav(vkd3d_cpu_descriptor_va_t descriptor, struct d3d12_device *device,
        struct d3d12_resource *resource, struct d3d12_resource *counter_resource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC *desc);
void d3d12_desc_create_sampler(vkd3d_cpu_descriptor_va_t sampler,
        struct d3d12_device *device, const D3D12_SAMPLER_DESC *desc);

bool vkd3d_create_vk_buffer_view(struct d3d12_device *device,
        VkBuffer vk_buffer, const struct vkd3d_format *format,
        VkDeviceSize offset, VkDeviceSize range, VkBufferView *vk_view);
bool vkd3d_create_raw_buffer_view(struct d3d12_device *device,
        D3D12_GPU_VIRTUAL_ADDRESS gpu_address, VkBufferView *vk_buffer_view);
HRESULT d3d12_create_static_sampler(struct d3d12_device *device,
        const D3D12_STATIC_SAMPLER_DESC *desc, VkSampler *vk_sampler);

#define D3D12_DESC_ALIGNMENT 64
struct d3d12_rtv_desc
{
    DECLSPEC_ALIGN(D3D12_DESC_ALIGNMENT) VkSampleCountFlagBits sample_count;
    const struct vkd3d_format *format;
    unsigned int width;
    unsigned int height;
    unsigned int layer_count;
    struct vkd3d_view *view;
    struct d3d12_resource *resource;
};
STATIC_ASSERT(sizeof(struct d3d12_rtv_desc) == D3D12_DESC_ALIGNMENT);

void d3d12_rtv_desc_copy(struct d3d12_rtv_desc *dst, struct d3d12_rtv_desc *src, unsigned int count);

static inline struct d3d12_rtv_desc *d3d12_rtv_desc_from_cpu_handle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle)
{
    return (struct d3d12_rtv_desc *)cpu_handle.ptr;
}

void d3d12_rtv_desc_create_rtv(struct d3d12_rtv_desc *rtv_desc, struct d3d12_device *device,
        struct d3d12_resource *resource, const D3D12_RENDER_TARGET_VIEW_DESC *desc);

void d3d12_rtv_desc_create_dsv(struct d3d12_rtv_desc *dsv_desc, struct d3d12_device *device,
        struct d3d12_resource *resource, const D3D12_DEPTH_STENCIL_VIEW_DESC *desc);

struct vkd3d_bound_buffer_range
{
    uint32_t byte_offset;
    uint32_t byte_count;
    uint32_t element_offset;
    uint32_t element_count;
};

struct vkd3d_host_visible_buffer_range
{
    VkDescriptorBufferInfo descriptor;
    void *host_ptr;
};

union vkd3d_descriptor_info
{
    VkBufferView buffer_view;
    VkDescriptorBufferInfo buffer;
    VkDescriptorImageInfo image;
    VkDeviceAddress va;
};

/* ID3D12DescriptorHeap */
struct d3d12_null_descriptor_template
{
    struct VkWriteDescriptorSet writes[VKD3D_MAX_BINDLESS_DESCRIPTOR_SETS];
    VkDescriptorBufferInfo buffer;
    VkDescriptorImageInfo image;
    VkBufferView buffer_view;
    unsigned int num_writes;
    unsigned int set_info_mask;
    bool has_mutable_descriptors;
};

typedef void (*pfn_vkd3d_host_mapping_copy_template)(void * restrict dst, const void * restrict src,
        size_t dst_index, size_t src_index, size_t count);
typedef void (*pfn_vkd3d_host_mapping_copy_template_single)(void * restrict dst, const void * restrict src,
        size_t dst_index, size_t src_index);

struct d3d12_descriptor_heap_set
{
    VkDescriptorSet vk_descriptor_set;
    void *mapped_set;
    pfn_vkd3d_host_mapping_copy_template copy_template;
    pfn_vkd3d_host_mapping_copy_template_single copy_template_single;
};

struct d3d12_descriptor_heap
{
    ID3D12DescriptorHeap ID3D12DescriptorHeap_iface;
    LONG refcount;

    uint64_t gpu_va;
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_va;

    VkDescriptorPool vk_descriptor_pool;

    struct d3d12_descriptor_heap_set sets[VKD3D_MAX_BINDLESS_DESCRIPTOR_SETS];

    struct vkd3d_device_memory_allocation device_allocation;
    VkBuffer vk_buffer;
    void *host_memory;

    struct vkd3d_host_visible_buffer_range raw_va_aux_buffer;
    struct vkd3d_host_visible_buffer_range buffer_ranges;
#ifdef VKD3D_ENABLE_DESCRIPTOR_QA
    struct vkd3d_host_visible_buffer_range descriptor_heap_info;
    uint64_t cookie;
#endif

    struct d3d12_null_descriptor_template null_descriptor_template;

    struct d3d12_device *device;

    struct vkd3d_private_store private_store;

    /* Here we pack metadata data structures for CBV_SRV_UAV and SAMPLER.
     * For RTV/DSV heaps, we just encode rtv_desc structs inline. */
    DECLSPEC_ALIGN(D3D12_DESC_ALIGNMENT) BYTE descriptors[];
};

HRESULT d3d12_descriptor_heap_create(struct d3d12_device *device,
        const D3D12_DESCRIPTOR_HEAP_DESC *desc, struct d3d12_descriptor_heap **descriptor_heap);
void d3d12_descriptor_heap_cleanup(struct d3d12_descriptor_heap *descriptor_heap);

static inline struct d3d12_descriptor_heap *impl_from_ID3D12DescriptorHeap(ID3D12DescriptorHeap *iface)
{
    extern CONST_VTBL struct ID3D12DescriptorHeapVtbl d3d12_descriptor_heap_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_descriptor_heap_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_descriptor_heap, ID3D12DescriptorHeap_iface);
}

/* Decodes descriptor heap VA (for resources only) and its offset.
 * Somewhat cursed, but avoids any de-referencing to achieve this result.
 * See d3d12_descriptor_heap_create for comments on how this works. */

struct d3d12_desc_split
{
    struct d3d12_descriptor_heap *heap;
    struct vkd3d_descriptor_metadata_types *types;
    struct vkd3d_descriptor_metadata_view *view;
    uint32_t offset;
};

static inline struct d3d12_desc_split d3d12_desc_decode_va(vkd3d_cpu_descriptor_va_t va)
{
    uintptr_t num_bits_descriptors;
    struct d3d12_desc_split split;
    uintptr_t heap_offset;
    uintptr_t heap_va;

    /* 5 LSBs encode number of bits for descriptors.
     * Over that, we have the heap offset (increment size is 32).
     * Above that, we have the d3d12_descriptor_heap, which is allocated with enough alignment
     * to contain these twiddle bits. */

    num_bits_descriptors = va & (VKD3D_RESOURCE_DESC_INCREMENT - 1);
    heap_offset = (va >> VKD3D_RESOURCE_DESC_INCREMENT_LOG2) & (((size_t)1 << num_bits_descriptors) - 1);
    split.offset = (uint32_t)heap_offset;

    heap_va = va & ~(((size_t)1 << (num_bits_descriptors + VKD3D_RESOURCE_DESC_INCREMENT_LOG2)) - 1);
    split.heap = (struct d3d12_descriptor_heap *)heap_va;
    heap_va += offsetof(struct d3d12_descriptor_heap, descriptors);
    split.types = (struct vkd3d_descriptor_metadata_types *)heap_va;
    split.types += heap_offset;
    heap_va += sizeof(struct vkd3d_descriptor_metadata_types) << num_bits_descriptors;
    split.view = (struct vkd3d_descriptor_metadata_view *)heap_va;
    split.view += heap_offset;

    return split;
}

static inline uint32_t d3d12_desc_heap_offset_from_gpu_handle(D3D12_GPU_DESCRIPTOR_HANDLE handle)
{
    return (uint32_t)handle.ptr / VKD3D_RESOURCE_DESC_INCREMENT;
}

/* ID3D12QueryHeap */
struct d3d12_query_heap
{
    ID3D12QueryHeap ID3D12QueryHeap_iface;
    LONG refcount;

    D3D12_QUERY_HEAP_DESC desc;
    VkQueryPool vk_query_pool;
    struct vkd3d_device_memory_allocation device_allocation;
    VkBuffer vk_buffer;
    uint32_t initialized;

    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

HRESULT d3d12_query_heap_create(struct d3d12_device *device, const D3D12_QUERY_HEAP_DESC *desc,
        struct d3d12_query_heap **heap);

static inline struct d3d12_query_heap *impl_from_ID3D12QueryHeap(ID3D12QueryHeap *iface)
{
    extern CONST_VTBL struct ID3D12QueryHeapVtbl d3d12_query_heap_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_query_heap_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_query_heap, ID3D12QueryHeap_iface);
}

static inline size_t d3d12_query_heap_type_get_data_size(D3D12_QUERY_HEAP_TYPE heap_type)
{
    switch (heap_type)
    {
        case D3D12_QUERY_HEAP_TYPE_OCCLUSION:
        case D3D12_QUERY_HEAP_TYPE_TIMESTAMP:
        case D3D12_QUERY_HEAP_TYPE_COPY_QUEUE_TIMESTAMP:
            return sizeof(uint64_t);
        case D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS:
            return sizeof(D3D12_QUERY_DATA_PIPELINE_STATISTICS);
        case D3D12_QUERY_HEAP_TYPE_SO_STATISTICS:
            return sizeof(D3D12_QUERY_DATA_SO_STATISTICS);
        default:
            ERR("Unhandled query pool type %u.\n", heap_type);
            return 0;
    }
}

static inline bool d3d12_query_heap_type_is_inline(D3D12_QUERY_HEAP_TYPE heap_type)
{
    return heap_type == D3D12_QUERY_HEAP_TYPE_OCCLUSION ||
            heap_type == D3D12_QUERY_HEAP_TYPE_SO_STATISTICS;
}

enum vkd3d_root_signature_flag
{
    VKD3D_ROOT_SIGNATURE_USE_ROOT_DESCRIPTOR_SET    = 0x00000001u,
    VKD3D_ROOT_SIGNATURE_USE_INLINE_UNIFORM_BLOCK   = 0x00000002u,
    VKD3D_ROOT_SIGNATURE_USE_RAW_VA_AUX_BUFFER      = 0x00000004u,
    VKD3D_ROOT_SIGNATURE_USE_SSBO_OFFSET_BUFFER     = 0x00000008u,
    VKD3D_ROOT_SIGNATURE_USE_TYPED_OFFSET_BUFFER    = 0x00000010u,
};

enum vkd3d_pipeline_type
{
    VKD3D_PIPELINE_TYPE_NONE,
    VKD3D_PIPELINE_TYPE_GRAPHICS,
    VKD3D_PIPELINE_TYPE_MESH_GRAPHICS,
    VKD3D_PIPELINE_TYPE_COMPUTE,
    VKD3D_PIPELINE_TYPE_RAY_TRACING,
};

static inline VkPipelineBindPoint vk_bind_point_from_pipeline_type(enum vkd3d_pipeline_type pipeline_type)
{
    switch (pipeline_type)
    {
        case VKD3D_PIPELINE_TYPE_NONE:
          break;
        case VKD3D_PIPELINE_TYPE_GRAPHICS:
        case VKD3D_PIPELINE_TYPE_MESH_GRAPHICS:
            return VK_PIPELINE_BIND_POINT_GRAPHICS;
        case VKD3D_PIPELINE_TYPE_COMPUTE:
            return VK_PIPELINE_BIND_POINT_COMPUTE;
        case VKD3D_PIPELINE_TYPE_RAY_TRACING:
            return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
    }

    return VK_PIPELINE_BIND_POINT_MAX_ENUM;
}

/* ID3D12RootSignature */
struct d3d12_bind_point_layout
{
    VkPipelineLayout vk_pipeline_layout;
    VkShaderStageFlags vk_push_stages;
};

#define VKD3D_MAX_HOISTED_DESCRIPTORS 16
struct vkd3d_descriptor_hoist_desc
{
    uint32_t table_index;
    uint32_t table_offset;
    uint32_t parameter_index;
};

struct vkd3d_descriptor_hoist_info
{
    struct vkd3d_descriptor_hoist_desc desc[VKD3D_MAX_HOISTED_DESCRIPTORS];
    unsigned int num_desc;
};

struct d3d12_root_signature
{
    ID3D12RootSignature ID3D12RootSignature_iface;
    LONG refcount;
    LONG internal_refcount;

    vkd3d_shader_hash_t compatibility_hash;

    struct d3d12_bind_point_layout graphics, mesh, compute, raygen;
    VkDescriptorSetLayout vk_sampler_descriptor_layout;
    VkDescriptorSetLayout vk_root_descriptor_layout;

    VkDescriptorPool vk_sampler_pool;
    VkDescriptorSet vk_sampler_set;

    struct vkd3d_shader_root_parameter *parameters;
    unsigned int parameter_count;

    uint32_t sampler_descriptor_set;
    uint32_t root_descriptor_set;

    uint64_t descriptor_table_mask;
    uint64_t root_constant_mask;
    uint64_t root_descriptor_raw_va_mask;
    uint64_t root_descriptor_push_mask;

    D3D12_ROOT_SIGNATURE_FLAGS d3d12_flags;
    unsigned int flags; /* vkd3d_root_signature_flag */

    unsigned int binding_count;
    struct vkd3d_shader_resource_binding *bindings;

    unsigned int root_constant_count;
    struct vkd3d_shader_push_constant_buffer *root_constants;

    /* Use one global push constant range */
    VkPushConstantRange push_constant_range;
    struct vkd3d_shader_descriptor_binding push_constant_ubo_binding;
    struct vkd3d_shader_descriptor_binding raw_va_aux_buffer_binding;
    struct vkd3d_shader_descriptor_binding offset_buffer_binding;
#ifdef VKD3D_ENABLE_DESCRIPTOR_QA
    struct vkd3d_shader_descriptor_binding descriptor_qa_heap_binding;
    struct vkd3d_shader_descriptor_binding descriptor_qa_global_info;
#endif

    VkDescriptorSetLayout set_layouts[VKD3D_MAX_DESCRIPTOR_SETS];
    uint32_t num_set_layouts;

    uint32_t descriptor_table_offset;
    uint32_t descriptor_table_count;

    unsigned int static_sampler_count;
    D3D12_STATIC_SAMPLER_DESC *static_samplers_desc;
    VkSampler *static_samplers;

    struct vkd3d_descriptor_hoist_info hoist_info;

    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

HRESULT d3d12_root_signature_create(struct d3d12_device *device, const void *bytecode,
        size_t bytecode_length, struct d3d12_root_signature **root_signature);
HRESULT d3d12_root_signature_create_raw(struct d3d12_device *device, const void *payload,
        size_t payload_size, struct d3d12_root_signature **root_signature);
HRESULT d3d12_root_signature_create_empty(struct d3d12_device *device,
        struct d3d12_root_signature **root_signature);
/* Private ref counts, for pipeline library. */
void d3d12_root_signature_inc_ref(struct d3d12_root_signature *state);
void d3d12_root_signature_dec_ref(struct d3d12_root_signature *state);

static inline struct d3d12_root_signature *impl_from_ID3D12RootSignature(ID3D12RootSignature *iface)
{
    extern CONST_VTBL struct ID3D12RootSignatureVtbl d3d12_root_signature_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_root_signature_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_root_signature, ID3D12RootSignature_iface);
}

unsigned int d3d12_root_signature_get_shader_interface_flags(const struct d3d12_root_signature *root_signature);
HRESULT d3d12_root_signature_create_local_static_samplers_layout(struct d3d12_root_signature *root_signature,
        VkDescriptorSetLayout vk_set_layout, VkPipelineLayout *vk_pipeline_layout);
HRESULT vkd3d_create_pipeline_layout(struct d3d12_device *device,
        unsigned int set_layout_count, const VkDescriptorSetLayout *set_layouts,
        unsigned int push_constant_count, const VkPushConstantRange *push_constants,
        VkPipelineLayout *pipeline_layout);

int vkd3d_parse_root_signature_v_1_0(const struct vkd3d_shader_code *dxbc,
        struct vkd3d_versioned_root_signature_desc *desc,
        vkd3d_shader_hash_t *compatibility_hash);
int vkd3d_parse_root_signature_v_1_1(const struct vkd3d_shader_code *dxbc,
        struct vkd3d_versioned_root_signature_desc *desc,
        vkd3d_shader_hash_t *compatibility_hash);
int vkd3d_parse_root_signature_v_1_1_from_raw_payload(const struct vkd3d_shader_code *dxbc,
        struct vkd3d_versioned_root_signature_desc *desc,
        vkd3d_shader_hash_t *compatibility_hash);

VkShaderStageFlags vkd3d_vk_stage_flags_from_visibility(D3D12_SHADER_VISIBILITY visibility);
enum vkd3d_shader_visibility vkd3d_shader_visibility_from_d3d12(D3D12_SHADER_VISIBILITY visibility);
HRESULT vkd3d_create_descriptor_set_layout(struct d3d12_device *device,
        VkDescriptorSetLayoutCreateFlags flags, unsigned int binding_count,
        const VkDescriptorSetLayoutBinding *bindings, VkDescriptorSetLayout *set_layout);

static inline const struct d3d12_bind_point_layout *d3d12_root_signature_get_layout(
        const struct d3d12_root_signature *root_signature, enum vkd3d_pipeline_type pipeline_type)
{
    switch (pipeline_type)
    {
        case VKD3D_PIPELINE_TYPE_NONE:
            return NULL;

        case VKD3D_PIPELINE_TYPE_GRAPHICS:
            return &root_signature->graphics;

        case VKD3D_PIPELINE_TYPE_MESH_GRAPHICS:
            return &root_signature->mesh;

        case VKD3D_PIPELINE_TYPE_COMPUTE:
            return &root_signature->compute;

        case VKD3D_PIPELINE_TYPE_RAY_TRACING:
            return &root_signature->raygen;
    }

    return NULL;
}

enum vkd3d_dynamic_state_flag
{
    VKD3D_DYNAMIC_STATE_VIEWPORT              = (1 << 0),
    VKD3D_DYNAMIC_STATE_SCISSOR               = (1 << 1),
    VKD3D_DYNAMIC_STATE_BLEND_CONSTANTS       = (1 << 2),
    VKD3D_DYNAMIC_STATE_STENCIL_REFERENCE     = (1 << 3),
    VKD3D_DYNAMIC_STATE_DEPTH_BOUNDS          = (1 << 4),
    VKD3D_DYNAMIC_STATE_TOPOLOGY              = (1 << 5),
    VKD3D_DYNAMIC_STATE_VERTEX_BUFFER         = (1 << 6),
    VKD3D_DYNAMIC_STATE_VERTEX_BUFFER_STRIDE  = (1 << 7),
    VKD3D_DYNAMIC_STATE_FRAGMENT_SHADING_RATE = (1 << 8),
    VKD3D_DYNAMIC_STATE_PRIMITIVE_RESTART     = (1 << 9),
};

struct vkd3d_shader_debug_ring_spec_constants
{
    uint64_t hash;
    uint64_t atomic_bda;
    uint64_t host_bda;
    uint32_t ring_words;
};

#define VKD3D_SHADER_DEBUG_RING_SPEC_INFO_MAP_ENTRIES 4
struct vkd3d_shader_debug_ring_spec_info
{
    struct vkd3d_shader_debug_ring_spec_constants constants;
    VkSpecializationMapEntry map_entries[VKD3D_SHADER_DEBUG_RING_SPEC_INFO_MAP_ENTRIES];
    VkSpecializationInfo spec_info;
};

enum vkd3d_plane_optimal_flag
{
    VKD3D_DEPTH_PLANE_OPTIMAL = (1 << 0),
    VKD3D_STENCIL_PLANE_OPTIMAL = (1 << 1),
    VKD3D_DEPTH_STENCIL_PLANE_GENERAL = (1 << 2),
};

struct d3d12_graphics_pipeline_state
{
    struct vkd3d_shader_debug_ring_spec_info spec_info[VKD3D_MAX_SHADER_STAGES];
    VkPipelineShaderStageCreateInfo stages[VKD3D_MAX_SHADER_STAGES];
    struct vkd3d_shader_code code[VKD3D_MAX_SHADER_STAGES];
    size_t stage_count;

    VkVertexInputAttributeDescription attributes[D3D12_VS_INPUT_REGISTER_COUNT];
    VkVertexInputRate input_rates[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    VkVertexInputBindingDivisorDescriptionEXT instance_divisors[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    VkVertexInputBindingDescription attribute_bindings[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    uint32_t minimum_vertex_buffer_dynamic_stride[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    uint32_t vertex_buffer_stride_align_mask[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    size_t instance_divisor_count;
    size_t attribute_binding_count;
    size_t attribute_count;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type;
    uint32_t vertex_buffer_mask;

    VkPipelineColorBlendAttachmentState blend_attachments[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
    unsigned int rt_count;
    unsigned int null_attachment_mask;
    unsigned int rtv_active_mask;
    unsigned int patch_vertex_count;
    const struct vkd3d_format *dsv_format;
    VkFormat rtv_formats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
    uint32_t dsv_plane_optimal_mask;

    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE index_buffer_strip_cut_value;
    VkPipelineRasterizationStateCreateInfo rs_desc;
    VkPipelineMultisampleStateCreateInfo ms_desc;
    VkPipelineDepthStencilStateCreateInfo ds_desc;
    VkPipelineColorBlendStateCreateInfo blend_desc;

    VkSampleMask sample_mask[2];
    VkPipelineRasterizationConservativeStateCreateInfoEXT rs_conservative_info;
    VkPipelineRasterizationDepthClipStateCreateInfoEXT rs_depth_clip_info;
    VkPipelineRasterizationStateStreamCreateInfoEXT rs_stream_info;

    uint32_t dynamic_state_flags; /* vkd3d_dynamic_state_flag */

    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
    struct list compiled_fallback_pipelines;

    bool xfb_enabled;
};

static inline unsigned int dsv_attachment_mask(const struct d3d12_graphics_pipeline_state *graphics)
{
    return 1u << graphics->rt_count;
}

struct d3d12_compute_pipeline_state
{
    VkPipeline vk_pipeline;
    struct vkd3d_shader_code code;
};

/* To be able to load a pipeline from cache, this information must match exactly,
 * otherwise, we must regard the PSO as incompatible (which is invalid usage and must be validated). */
struct vkd3d_pipeline_cache_compatibility
{
    uint64_t state_desc_compat_hash;
    uint64_t root_signature_compat_hash;
    uint64_t dxbc_blob_hashes[VKD3D_MAX_SHADER_STAGES];
};

/* ID3D12PipelineState */
struct d3d12_pipeline_state
{
    ID3D12PipelineState ID3D12PipelineState_iface;
    LONG refcount;
    LONG internal_refcount;

    union
    {
        struct d3d12_graphics_pipeline_state graphics;
        struct d3d12_compute_pipeline_state compute;
    };

    enum vkd3d_pipeline_type pipeline_type;
    VkPipelineCache vk_pso_cache;
    spinlock_t lock;

    struct vkd3d_pipeline_cache_compatibility pipeline_cache_compat;
    struct d3d12_root_signature *private_root_signature;
    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

static inline bool d3d12_pipeline_state_is_compute(const struct d3d12_pipeline_state *state)
{
    return state && state->pipeline_type == VKD3D_PIPELINE_TYPE_COMPUTE;
}

static inline bool d3d12_pipeline_state_is_graphics(const struct d3d12_pipeline_state *state)
{
    return state && (state->pipeline_type == VKD3D_PIPELINE_TYPE_GRAPHICS ||
            state->pipeline_type == VKD3D_PIPELINE_TYPE_MESH_GRAPHICS);
}

/* This returns true for invalid D3D12 API usage. Game intends to use depth-stencil tests,
 * but we don't know the format until bind time. Some games like SottR rely on this to work ... somehow. */
static inline bool d3d12_graphics_pipeline_state_has_unknown_dsv_format_with_test(
        const struct d3d12_graphics_pipeline_state *graphics)
{
    return graphics->null_attachment_mask & dsv_attachment_mask(graphics);
}

/* Private ref counts, for pipeline library. */
ULONG d3d12_pipeline_state_inc_public_ref(struct d3d12_pipeline_state *state);
void d3d12_pipeline_state_inc_ref(struct d3d12_pipeline_state *state);
void d3d12_pipeline_state_dec_ref(struct d3d12_pipeline_state *state);

struct d3d12_cached_pipeline_state
{
    D3D12_CACHED_PIPELINE_STATE blob;
    /* For cached PSO if that blob comes from a library.
     * Might need it to resolve references. */
    struct d3d12_pipeline_library *library;
};

struct d3d12_pipeline_state_desc
{
    ID3D12RootSignature *root_signature;
    D3D12_SHADER_BYTECODE vs;
    D3D12_SHADER_BYTECODE ps;
    D3D12_SHADER_BYTECODE ds;
    D3D12_SHADER_BYTECODE hs;
    D3D12_SHADER_BYTECODE gs;
    D3D12_SHADER_BYTECODE cs;
    D3D12_STREAM_OUTPUT_DESC stream_output;
    D3D12_BLEND_DESC blend_state;
    UINT sample_mask;
    D3D12_RASTERIZER_DESC rasterizer_state;
    D3D12_DEPTH_STENCIL_DESC1 depth_stencil_state;
    D3D12_INPUT_LAYOUT_DESC input_layout;
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE strip_cut_value;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type;
    D3D12_RT_FORMAT_ARRAY rtv_formats;
    DXGI_FORMAT dsv_format;
    DXGI_SAMPLE_DESC sample_desc;
    D3D12_VIEW_INSTANCING_DESC view_instancing_desc;
    UINT node_mask;
    struct d3d12_cached_pipeline_state cached_pso;
    D3D12_PIPELINE_STATE_FLAGS flags;
};

HRESULT vkd3d_pipeline_state_desc_from_d3d12_graphics_desc(struct d3d12_pipeline_state_desc *desc,
        const D3D12_GRAPHICS_PIPELINE_STATE_DESC *d3d12_desc);
HRESULT vkd3d_pipeline_state_desc_from_d3d12_compute_desc(struct d3d12_pipeline_state_desc *desc,
        const D3D12_COMPUTE_PIPELINE_STATE_DESC *d3d12_desc);
HRESULT vkd3d_pipeline_state_desc_from_d3d12_stream_desc(struct d3d12_pipeline_state_desc *desc,
        const D3D12_PIPELINE_STATE_STREAM_DESC *d3d12_desc, VkPipelineBindPoint *vk_bind_point);

static inline bool vk_primitive_topology_supports_restart(VkPrimitiveTopology topology)
{
    switch (topology)
    {
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
        case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
        case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
            return true;

        default:
            return false;
    }
}

struct vkd3d_pipeline_key
{
    D3D12_PRIMITIVE_TOPOLOGY topology;
    uint32_t viewport_count;
    uint32_t strides[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    VkFormat dsv_format;

    bool dynamic_stride;
    bool dynamic_topology;
};

bool d3d12_pipeline_state_has_replaced_shaders(struct d3d12_pipeline_state *state);
HRESULT d3d12_pipeline_state_create(struct d3d12_device *device, VkPipelineBindPoint bind_point,
        const struct d3d12_pipeline_state_desc *desc, struct d3d12_pipeline_state **state);
VkPipeline d3d12_pipeline_state_get_or_create_pipeline(struct d3d12_pipeline_state *state,
        const struct vkd3d_dynamic_state *dyn_state, const struct vkd3d_format *dsv_format,
        uint32_t *dynamic_state_flags);
VkPipeline d3d12_pipeline_state_get_pipeline(struct d3d12_pipeline_state *state,
        const struct vkd3d_dynamic_state *dyn_state, const struct vkd3d_format *dsv_format,
        uint32_t *dynamic_state_flags);
VkPipeline d3d12_pipeline_state_create_pipeline_variant(struct d3d12_pipeline_state *state,
        const struct vkd3d_pipeline_key *key, const struct vkd3d_format *dsv_format,
        VkPipelineCache vk_cache, uint32_t *dynamic_state_flags);

static inline struct d3d12_pipeline_state *impl_from_ID3D12PipelineState(ID3D12PipelineState *iface)
{
    extern CONST_VTBL struct ID3D12PipelineStateVtbl d3d12_pipeline_state_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_pipeline_state_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_pipeline_state, ID3D12PipelineState_iface);
}

/* ID3D12PipelineLibrary */
typedef ID3D12PipelineLibrary1 d3d12_pipeline_library_iface;

struct vkd3d_pipeline_library_disk_cache_item
{
    struct d3d12_pipeline_state *state;
};

struct vkd3d_pipeline_library_disk_cache
{
    /* This memory is generally mapped with MapViewOfFile() or mmap(),
     * and must remain mapped for the duration of the library. */
    struct vkd3d_memory_mapped_file mapped_file;
    struct d3d12_pipeline_library *library;

    pthread_t thread;
    condvar_reltime_t cond;
    pthread_mutex_t lock;
    bool thread_active;

    struct vkd3d_pipeline_library_disk_cache_item *items;
    size_t items_count;
    size_t items_size;

    char read_path[VKD3D_PATH_MAX];
    char write_path[VKD3D_PATH_MAX];

    /* The stream archive is designed to be safe against concurrent readers and writers, ala Fossilize.
     * There is a read-only portion, and a write-only portion which can be merged back to the read-only archive
     * on demand. */
    FILE *stream_archive_write_file;
    bool stream_archive_attempted_write;
};

struct d3d12_pipeline_library
{
    d3d12_pipeline_library_iface ID3D12PipelineLibrary_iface;
    LONG refcount;
    LONG internal_refcount;
    uint32_t flags;

    struct d3d12_device *device;

    rwlock_t mutex;
    /* driver_cache_map and spirv_cache_map can be touched in serialize_pipeline_state.
     * Use the internal mutex when touching the internal caches
     * so we don't need a big lock on the outside when serializing. */
    rwlock_t internal_hashmap_mutex;
    struct hash_map pso_map;
    struct hash_map driver_cache_map;
    struct hash_map spirv_cache_map;

    size_t total_name_table_size;
    size_t total_blob_size;

    /* Non-owned pointer. Calls back into the disk cache when blobs are added. */
    struct vkd3d_pipeline_library_disk_cache *disk_cache_listener;
    /* Useful if parsing a huge archive in the disk thread from a cold cache.
     * If we want to tear down device immediately after device creation (not too uncommon),
     * we can end up blocking for a long time. */
    uint32_t stream_archive_cancellation_point;

    struct vkd3d_private_store private_store;
};

enum vkd3d_pipeline_library_flags
{
    VKD3D_PIPELINE_LIBRARY_FLAG_SAVE_FULL_SPIRV = 1 << 0,
    VKD3D_PIPELINE_LIBRARY_FLAG_SAVE_PSO_BLOB = 1 << 1,
    VKD3D_PIPELINE_LIBRARY_FLAG_INTERNAL_KEYS = 1 << 2,
    VKD3D_PIPELINE_LIBRARY_FLAG_USE_PIPELINE_CACHE_UUID = 1 << 3,
    VKD3D_PIPELINE_LIBRARY_FLAG_STREAM_ARCHIVE = 1 << 4,
    /* We expect to parse archive from thread, so consider thread safety and cancellation points. */
    VKD3D_PIPELINE_LIBRARY_FLAG_STREAM_ARCHIVE_PARSE_ASYNC = 1 << 5,
};

HRESULT d3d12_pipeline_library_create(struct d3d12_device *device, const void *blob,
        size_t blob_length, uint32_t flags, /* vkd3d_pipeline_library_flags */
        struct d3d12_pipeline_library **pipeline_library);

VkResult vkd3d_create_pipeline_cache(struct d3d12_device *device,
        size_t size, const void *data, VkPipelineCache *cache);
HRESULT vkd3d_create_pipeline_cache_from_d3d12_desc(struct d3d12_device *device,
        const struct d3d12_cached_pipeline_state *state, VkPipelineCache *cache);
HRESULT vkd3d_get_cached_spirv_code_from_d3d12_desc(
        const struct d3d12_cached_pipeline_state *state,
        VkShaderStageFlagBits stage,
        struct vkd3d_shader_code *spirv_code);
VkResult vkd3d_serialize_pipeline_state(struct d3d12_pipeline_library *pipeline_library,
        const struct d3d12_pipeline_state *state, size_t *size, void *data);
HRESULT d3d12_cached_pipeline_state_validate(struct d3d12_device *device,
        const struct d3d12_cached_pipeline_state *state,
        const struct vkd3d_pipeline_cache_compatibility *compat);
bool d3d12_cached_pipeline_state_is_dummy(const struct d3d12_cached_pipeline_state *state);
void vkd3d_pipeline_cache_compat_from_state_desc(struct vkd3d_pipeline_cache_compatibility *compat,
        const struct d3d12_pipeline_state_desc *desc);

ULONG d3d12_pipeline_library_inc_public_ref(struct d3d12_pipeline_library *state);
ULONG d3d12_pipeline_library_dec_public_ref(struct d3d12_pipeline_library *state);
void d3d12_pipeline_library_inc_ref(struct d3d12_pipeline_library *state);
void d3d12_pipeline_library_dec_ref(struct d3d12_pipeline_library *state);

/* For internal on-disk pipeline cache fallback. The key to Load/StorePipeline is implied by the PSO cache compatibility. */
HRESULT vkd3d_pipeline_library_store_pipeline_to_disk_cache(struct vkd3d_pipeline_library_disk_cache *pipeline_library,
        struct d3d12_pipeline_state *state);
HRESULT vkd3d_pipeline_library_find_cached_blob_from_disk_cache(struct vkd3d_pipeline_library_disk_cache *pipeline_library,
        const struct vkd3d_pipeline_cache_compatibility *compat,
        struct d3d12_cached_pipeline_state *cached_state);
void vkd3d_pipeline_library_disk_cache_notify_blob_insert(struct vkd3d_pipeline_library_disk_cache *disk_cache,
        uint64_t hash, uint32_t type /* vkd3d_serialized_pipeline_stream_entry_type */,
        const void *data, size_t size);

/* Called on device init. */
HRESULT vkd3d_pipeline_library_init_disk_cache(struct vkd3d_pipeline_library_disk_cache *cache,
        struct d3d12_device *device);
/* Called on device destroy. */
void vkd3d_pipeline_library_flush_disk_cache(struct vkd3d_pipeline_library_disk_cache *cache);

struct vkd3d_buffer
{
    VkBuffer vk_buffer;
    VkDeviceMemory vk_memory;
};

struct d3d12_descriptor_pool_cache
{
    VkDescriptorPool vk_descriptor_pool;
    VkDescriptorPool *free_descriptor_pools;
    size_t free_descriptor_pools_size;
    size_t free_descriptor_pool_count;

    VkDescriptorPool *descriptor_pools;
    size_t descriptor_pools_size;
    size_t descriptor_pool_count;
};

enum vkd3d_descriptor_pool_types
{
    VKD3D_DESCRIPTOR_POOL_TYPE_STATIC = 0,
    VKD3D_DESCRIPTOR_POOL_TYPE_COUNT
};

#define VKD3D_SCRATCH_BUFFER_SIZE (1ull << 20)
#define VKD3D_SCRATCH_BUFFER_COUNT (32u)

struct vkd3d_scratch_buffer
{
    struct vkd3d_memory_allocation allocation;
    VkDeviceSize offset;
};

#define VKD3D_QUERY_TYPE_INDEX_OCCLUSION (0u)
#define VKD3D_QUERY_TYPE_INDEX_PIPELINE_STATISTICS (1u)
#define VKD3D_QUERY_TYPE_INDEX_TRANSFORM_FEEDBACK (2u)
#define VKD3D_QUERY_TYPE_INDEX_RT_COMPACTED_SIZE (3u)
#define VKD3D_QUERY_TYPE_INDEX_RT_SERIALIZE_SIZE (4u)
#define VKD3D_QUERY_TYPE_INDEX_RT_CURRENT_SIZE (5u)
#define VKD3D_QUERY_TYPE_INDEX_RT_SERIALIZE_SIZE_BOTTOM_LEVEL_POINTERS (6u)
#define VKD3D_VIRTUAL_QUERY_TYPE_COUNT (7u)
#define VKD3D_VIRTUAL_QUERY_POOL_COUNT (128u)

struct vkd3d_query_pool
{
    VkQueryPool vk_query_pool;
    uint32_t type_index;
    uint32_t query_count;
    uint32_t next_index;
};

struct d3d12_command_allocator_scratch_pool
{
    struct vkd3d_scratch_buffer *scratch_buffers;
    size_t scratch_buffers_size;
    size_t scratch_buffer_count;
};

enum vkd3d_scratch_pool_kind
{
    VKD3D_SCRATCH_POOL_KIND_DEVICE_STORAGE = 0,
    VKD3D_SCRATCH_POOL_KIND_INDIRECT_PREPROCESS,
    VKD3D_SCRATCH_POOL_KIND_COUNT
};

/* ID3D12CommandAllocator */
struct d3d12_command_allocator
{
    ID3D12CommandAllocator ID3D12CommandAllocator_iface;
    LONG refcount;

    D3D12_COMMAND_LIST_TYPE type;
    VkQueueFlags vk_queue_flags;
    uint32_t vk_family_index;

    VkCommandPool vk_command_pool;

    struct d3d12_descriptor_pool_cache descriptor_pool_caches[VKD3D_DESCRIPTOR_POOL_TYPE_COUNT];

    struct vkd3d_view **views;
    size_t views_size;
    size_t view_count;

    VkBufferView *buffer_views;
    size_t buffer_views_size;
    size_t buffer_view_count;

    VkCommandBuffer *command_buffers;
    size_t command_buffers_size;
    size_t command_buffer_count;

    struct d3d12_command_allocator_scratch_pool scratch_pools[VKD3D_SCRATCH_POOL_KIND_COUNT];

    struct vkd3d_query_pool *query_pools;
    size_t query_pools_size;
    size_t query_pool_count;

    struct vkd3d_query_pool active_query_pools[VKD3D_VIRTUAL_QUERY_TYPE_COUNT];

    LONG outstanding_submissions_count;

    struct d3d12_command_list *current_command_list;
    struct d3d12_device *device;

    struct vkd3d_private_store private_store;

#ifdef VKD3D_ENABLE_BREADCRUMBS
    unsigned int *breadcrumb_context_indices;
    size_t breadcrumb_context_index_size;
    size_t breadcrumb_context_index_count;
#endif
};

HRESULT d3d12_command_allocator_create(struct d3d12_device *device,
        D3D12_COMMAND_LIST_TYPE type, struct d3d12_command_allocator **allocator);
bool d3d12_command_allocator_allocate_query_from_type_index(
        struct d3d12_command_allocator *allocator,
        uint32_t type_index, VkQueryPool *query_pool, uint32_t *query_index);

enum vkd3d_pipeline_dirty_flag
{
    VKD3D_PIPELINE_DIRTY_STATIC_SAMPLER_SET       = 0x00000001u,
    VKD3D_PIPELINE_DIRTY_DESCRIPTOR_TABLE_OFFSETS = 0x00000002u,
    VKD3D_PIPELINE_DIRTY_HOISTED_DESCRIPTORS      = 0x00000004u,
};

struct vkd3d_root_descriptor_info
{
    VkDescriptorType vk_descriptor_type;
    union vkd3d_descriptor_info info;
};

struct vkd3d_pipeline_bindings
{
    const struct d3d12_root_signature *root_signature;

    VkDescriptorSet static_sampler_set;
    uint32_t dirty_flags; /* vkd3d_pipeline_dirty_flags */

    uint32_t descriptor_tables[D3D12_MAX_ROOT_COST];
    uint64_t descriptor_table_active_mask;
    uint64_t descriptor_heap_dirty_mask;

    /* Needed when VK_KHR_push_descriptor is not available. */
    struct vkd3d_root_descriptor_info root_descriptors[D3D12_MAX_ROOT_COST];
    uint64_t root_descriptor_dirty_mask;
    uint64_t root_descriptor_active_mask;

    uint32_t root_constants[D3D12_MAX_ROOT_COST];
    uint64_t root_constant_dirty_mask;
};

struct vkd3d_dynamic_state
{
    uint32_t active_flags; /* vkd3d_dynamic_state_flags */
    uint32_t dirty_flags; /* vkd3d_dynamic_state_flags */
    uint32_t dirty_vbos;
    uint32_t dirty_vbo_strides;

    uint32_t viewport_count;
    VkViewport viewports[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    VkRect2D scissors[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

    float blend_constants[4];
    uint32_t stencil_reference;

    float min_depth_bounds;
    float max_depth_bounds;

    VkBuffer vertex_buffers[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    VkDeviceSize vertex_offsets[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    VkDeviceSize vertex_sizes[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    VkDeviceSize vertex_strides[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

    D3D12_PRIMITIVE_TOPOLOGY primitive_topology;
    VkPrimitiveTopology vk_primitive_topology;

    struct
    {
        VkExtent2D fragment_size;
        VkFragmentShadingRateCombinerOpKHR combiner_ops[D3D12_RS_SET_SHADING_RATE_COMBINER_COUNT];
    } fragment_shading_rate;

    uint32_t pipeline_stack_size;
};

/* ID3D12CommandList */
typedef ID3D12GraphicsCommandList6 d3d12_command_list_iface;

enum vkd3d_initial_transition_type
{
    VKD3D_INITIAL_TRANSITION_TYPE_RESOURCE,
    VKD3D_INITIAL_TRANSITION_TYPE_QUERY_HEAP,
};

struct vkd3d_initial_transition
{
    enum vkd3d_initial_transition_type type;
    union
    {
        struct
        {
            struct d3d12_resource *resource;
            bool perform_initial_transition;
        } resource;
        struct d3d12_query_heap *query_heap;
    };
};

enum vkd3d_active_query_state
{
    VKD3D_ACTIVE_QUERY_RESET,
    VKD3D_ACTIVE_QUERY_BEGUN,
    VKD3D_ACTIVE_QUERY_ENDED,
};

struct vkd3d_active_query
{
    struct d3d12_query_heap *heap;
    uint32_t index;
    D3D12_QUERY_TYPE type;
    VkQueryPool vk_pool;
    uint32_t vk_index;
    enum vkd3d_active_query_state state;
    uint32_t resolve_index;
};

enum vkd3d_query_range_flag
{
    VKD3D_QUERY_RANGE_RESET = 0x1,
};

struct vkd3d_query_range
{
    VkQueryPool vk_pool;
    uint32_t index;
    uint32_t count;
    uint32_t flags;
};

enum vkd3d_rendering_flags
{
    VKD3D_RENDERING_ACTIVE    = (1u << 0),
    VKD3D_RENDERING_SUSPENDED = (1u << 1),
    VKD3D_RENDERING_CURRENT   = (1u << 2),
};

struct vkd3d_rendering_info
{
    VkRenderingInfoKHR info;
    VkRenderingAttachmentInfoKHR rtv[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
    VkRenderingAttachmentInfoKHR dsv;
    VkRenderingFragmentShadingRateAttachmentInfoKHR vrs;
    uint32_t state_flags;
    uint32_t rtv_mask;
};

/* ID3D12CommandListExt */
typedef ID3D12GraphicsCommandListExt d3d12_command_list_vkd3d_ext_iface;

struct d3d12_state_object;

struct d3d12_resource_tracking
{
    const struct d3d12_resource *resource;
    uint32_t plane_optimal_mask;
};

#define VKD3D_BUFFER_COPY_TRACKING_BUFFER_COUNT 4
struct d3d12_buffer_copy_tracked_buffer
{
    /* Need to track on VkBuffer level to handle aliasing. For ID3D12Heap, all resources share one VkBuffer. */
    VkBuffer vk_buffer;
    VkDeviceSize hazard_begin;
    VkDeviceSize hazard_end;
};

enum vkd3d_batch_type
{
    VKD3D_BATCH_TYPE_NONE,
    VKD3D_BATCH_TYPE_COPY_BUFFER_TO_IMAGE,
    VKD3D_BATCH_TYPE_COPY_IMAGE_TO_BUFFER,
    VKD3D_BATCH_TYPE_COPY_IMAGE,
};

struct vkd3d_image_copy_info
{
    D3D12_TEXTURE_COPY_LOCATION src, dst;
    const struct vkd3d_format *src_format, *dst_format;
    enum vkd3d_batch_type batch_type;
    union
    {
        VkBufferImageCopy2KHR buffer_image;
        VkImageCopy2KHR image;
    } copy;
    /* TODO: split d3d12_command_list_copy_image too, so this can be a local variable of before_copy_texture_region. */
    bool writes_full_subresource;
    VkImageLayout src_layout;
    VkImageLayout dst_layout;
};

#define VKD3D_COPY_TEXTURE_REGION_MAX_BATCH_SIZE 16

struct d3d12_transfer_batch_state
{
    enum vkd3d_batch_type batch_type;
    struct vkd3d_image_copy_info batch[VKD3D_COPY_TEXTURE_REGION_MAX_BATCH_SIZE];
    size_t batch_len;
};

struct d3d12_command_list
{
    d3d12_command_list_iface ID3D12GraphicsCommandList_iface;
    d3d12_command_list_vkd3d_ext_iface ID3D12GraphicsCommandListExt_iface;
    LONG refcount;

    D3D12_COMMAND_LIST_TYPE type;
    VkQueueFlags vk_queue_flags;

    bool is_recording;
    bool is_valid;
    bool debug_capture;
    bool has_replaced_shaders;

    struct
    {
        VkBuffer buffer;
        VkDeviceSize offset;
        DXGI_FORMAT dxgi_format;
        VkIndexType vk_type;
        bool is_dirty;
    } index_buffer;

    struct
    {
        bool has_observed_transition_to_indirect;
        bool has_emitted_indirect_to_compute_barrier;
    } execute_indirect;

    VkCommandBuffer vk_command_buffer;
    VkCommandBuffer vk_init_commands;

    struct d3d12_rtv_desc rtvs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
    struct d3d12_rtv_desc dsv;
    uint32_t dsv_plane_optimal_mask;
    VkImageLayout dsv_layout;
    unsigned int fb_width;
    unsigned int fb_height;
    unsigned int fb_layer_count;

    bool xfb_enabled;

    bool predicate_enabled;
    VkDeviceAddress predicate_va;

    /* This is VK_NULL_HANDLE when we are no longer sure which pipeline to bind,
     * if this is NULL, we might need to lookup a pipeline key in order to bind the correct pipeline. */
    VkPipeline current_pipeline;

    /* This is the actual pipeline which is bound to the pipeline. This lets us elide
     * possible calls to vkCmdBindPipeline and avoids invalidating dynamic state. */
    VkPipeline command_buffer_pipeline;

    struct vkd3d_rendering_info rendering_info;
    struct vkd3d_dynamic_state dynamic_state;
    struct vkd3d_pipeline_bindings graphics_bindings;
    struct vkd3d_pipeline_bindings compute_bindings;
    enum vkd3d_pipeline_type active_pipeline_type;

    VkDescriptorSet descriptor_heaps[VKD3D_MAX_BINDLESS_DESCRIPTOR_SETS];

    struct d3d12_pipeline_state *state;
    struct d3d12_state_object *rt_state;

    struct d3d12_command_allocator *allocator;
    struct d3d12_device *device;

    VkBuffer so_counter_buffers[D3D12_SO_BUFFER_SLOT_COUNT];
    VkDeviceSize so_counter_buffer_offsets[D3D12_SO_BUFFER_SLOT_COUNT];

    struct vkd3d_initial_transition *init_transitions;
    size_t init_transitions_size;
    size_t init_transitions_count;

    struct vkd3d_query_range *query_ranges;
    size_t query_ranges_size;
    size_t query_ranges_count;

    struct vkd3d_active_query *active_queries;
    size_t active_queries_size;
    size_t active_queries_count;

    struct vkd3d_active_query *pending_queries;
    size_t pending_queries_size;
    size_t pending_queries_count;

    LONG *outstanding_submissions_count;

    const struct vkd3d_descriptor_metadata_types *cbv_srv_uav_descriptors_types;
    const struct vkd3d_descriptor_metadata_view *cbv_srv_uav_descriptors_view;

    struct d3d12_resource *vrs_image;

    struct d3d12_resource_tracking *dsv_resource_tracking;
    size_t dsv_resource_tracking_count;
    size_t dsv_resource_tracking_size;

    struct d3d12_buffer_copy_tracked_buffer tracked_copy_buffers[VKD3D_BUFFER_COPY_TRACKING_BUFFER_COUNT];
    unsigned int tracked_copy_buffer_count;

    struct d3d12_transfer_batch_state transfer_batch;

    struct vkd3d_private_store private_store;

#ifdef VKD3D_ENABLE_BREADCRUMBS
    unsigned int breadcrumb_context_index;
#endif
};

HRESULT d3d12_command_list_create(struct d3d12_device *device,
        UINT node_mask, D3D12_COMMAND_LIST_TYPE type, struct d3d12_command_list **list);
bool d3d12_command_list_reset_query(struct d3d12_command_list *list,
        VkQueryPool vk_pool, uint32_t index);

static inline struct vkd3d_pipeline_bindings *d3d12_command_list_get_bindings(
        struct d3d12_command_list *list, enum vkd3d_pipeline_type pipeline_type)
{
    switch (pipeline_type)
    {
        case VKD3D_PIPELINE_TYPE_NONE:
            break;

        case VKD3D_PIPELINE_TYPE_GRAPHICS:
        case VKD3D_PIPELINE_TYPE_MESH_GRAPHICS:
            return &list->graphics_bindings;

        case VKD3D_PIPELINE_TYPE_COMPUTE:
        case VKD3D_PIPELINE_TYPE_RAY_TRACING:
            return &list->compute_bindings;
    }

    return NULL;
}

#define VKD3D_BUNDLE_CHUNK_SIZE (256 << 10)
#define VKD3D_BUNDLE_COMMAND_ALIGNMENT (sizeof(UINT64))

struct d3d12_bundle_allocator
{
    ID3D12CommandAllocator ID3D12CommandAllocator_iface;
    LONG refcount;

    void **chunks;
    size_t chunks_size;
    size_t chunks_count;
    size_t chunk_offset;

    struct d3d12_bundle *current_bundle;
    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

HRESULT d3d12_bundle_allocator_create(struct d3d12_device *device,
        struct d3d12_bundle_allocator **allocator);

typedef void (*pfn_d3d12_bundle_command)(d3d12_command_list_iface *command_list, const void *args);

struct d3d12_bundle_command
{
    pfn_d3d12_bundle_command proc;
    struct d3d12_bundle_command *next;
};

struct d3d12_bundle
{
    d3d12_command_list_iface ID3D12GraphicsCommandList_iface;
    LONG refcount;

    bool is_recording;

    struct d3d12_device *device;
    struct d3d12_bundle_allocator *allocator;
    struct d3d12_bundle_command *head;
    struct d3d12_bundle_command *tail;

    struct vkd3d_private_store private_store;
};

HRESULT d3d12_bundle_create(struct d3d12_device *device,
        UINT node_mask, D3D12_COMMAND_LIST_TYPE type, struct d3d12_bundle **bundle);
void d3d12_bundle_execute(struct d3d12_bundle *bundle, d3d12_command_list_iface *list);
struct d3d12_bundle *d3d12_bundle_from_iface(ID3D12GraphicsCommandList *iface);

struct vkd3d_queue
{
    /* Access to VkQueue must be externally synchronized. */
    pthread_mutex_t mutex;

    VkQueue vk_queue;

    VkCommandPool barrier_pool;
    VkCommandBuffer barrier_command_buffer;
    VkSemaphore serializing_binary_semaphore;
    VkSemaphore submission_timeline;
    uint64_t submission_timeline_count;

    uint32_t vk_family_index;
    VkQueueFlags vk_queue_flags;
    uint32_t timestamp_bits;
    uint32_t virtual_queue_count;

    VkSemaphore *wait_semaphores;
    size_t wait_semaphores_size;
    uint64_t *wait_values;
    size_t wait_values_size;
    VkPipelineStageFlags *wait_stages;
    size_t wait_stages_size;
    d3d12_fence_iface **wait_fences;
    size_t wait_fences_size;
    uint32_t wait_count;
};

VkQueue vkd3d_queue_acquire(struct vkd3d_queue *queue);
HRESULT vkd3d_queue_create(struct d3d12_device *device, uint32_t family_index, uint32_t queue_index,
        const VkQueueFamilyProperties *properties, struct vkd3d_queue **queue);
void vkd3d_queue_destroy(struct vkd3d_queue *queue, struct d3d12_device *device);
void vkd3d_queue_release(struct vkd3d_queue *queue);
void vkd3d_queue_add_wait(struct vkd3d_queue *queue, d3d12_fence_iface *waiter, VkSemaphore semaphore, uint64_t value);

enum vkd3d_submission_type
{
    VKD3D_SUBMISSION_WAIT,
    VKD3D_SUBMISSION_SIGNAL,
    VKD3D_SUBMISSION_EXECUTE,
    VKD3D_SUBMISSION_BIND_SPARSE,
    VKD3D_SUBMISSION_STOP,
    VKD3D_SUBMISSION_DRAIN
};

enum vkd3d_sparse_memory_bind_mode
{
    VKD3D_SPARSE_MEMORY_BIND_MODE_UPDATE,
    VKD3D_SPARSE_MEMORY_BIND_MODE_COPY,
};

struct vkd3d_sparse_memory_bind
{
    uint32_t dst_tile;
    uint32_t src_tile;
    VkDeviceMemory vk_memory;
    VkDeviceSize vk_offset;
};

struct vkd3d_sparse_memory_bind_range
{
    uint32_t tile_index;
    uint32_t tile_count;
    VkDeviceMemory vk_memory;
    VkDeviceSize vk_offset;
};

struct d3d12_command_queue_submission_wait
{
    d3d12_fence_iface *fence;
    UINT64 value;
};

struct d3d12_command_queue_submission_signal
{
    d3d12_fence_iface *fence;
    UINT64 value;
};

struct d3d12_command_queue_submission_execute
{
    VkCommandBuffer *cmd;
    LONG **outstanding_submissions_counters;
    UINT cmd_count;
    UINT outstanding_submissions_counter_count;

    struct vkd3d_initial_transition *transitions;
    size_t transition_count;

    bool debug_capture;
};

struct d3d12_command_queue_submission_bind_sparse
{
    enum vkd3d_sparse_memory_bind_mode mode;
    uint32_t bind_count;
    struct vkd3d_sparse_memory_bind *bind_infos;
    struct d3d12_resource *dst_resource;
    struct d3d12_resource *src_resource;
};

struct d3d12_command_queue_submission
{
    enum vkd3d_submission_type type;
    union
    {
        struct d3d12_command_queue_submission_wait wait;
        struct d3d12_command_queue_submission_signal signal;
        struct d3d12_command_queue_submission_execute execute;
        struct d3d12_command_queue_submission_bind_sparse bind_sparse;
    };
};

struct vkd3d_timeline_semaphore
{
    VkSemaphore vk_semaphore;
    uint64_t last_signaled;
};

/* IWineDXGISwapChainFactory */
struct d3d12_swapchain_factory
{
    IWineDXGISwapChainFactory IWineDXGISwapChainFactory_iface;
    struct d3d12_command_queue *queue;
};

HRESULT d3d12_swapchain_factory_init(struct d3d12_command_queue *queue, struct d3d12_swapchain_factory *factory);

/* ID3D12CommandQueue */
struct d3d12_command_queue
{
    ID3D12CommandQueue ID3D12CommandQueue_iface;
    LONG refcount;

    D3D12_COMMAND_QUEUE_DESC desc;

    struct vkd3d_queue *vkd3d_queue;

    struct d3d12_device *device;

    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;
    pthread_t submission_thread;

    struct d3d12_command_queue_submission *submissions;
    size_t submissions_count;
    size_t submissions_size;
    uint64_t drain_count;
    uint64_t queue_drain_count;

    struct vkd3d_fence_worker fence_worker;
    struct vkd3d_private_store private_store;

#ifdef VKD3D_BUILD_STANDALONE_D3D12
    struct d3d12_swapchain_factory swapchain_factory;
#endif
};

HRESULT d3d12_command_queue_create(struct d3d12_device *device,
        const D3D12_COMMAND_QUEUE_DESC *desc, struct d3d12_command_queue **queue);
void d3d12_command_queue_submit_stop(struct d3d12_command_queue *queue);

struct vkd3d_execute_indirect_info
{
    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;
};

enum vkd3d_patch_command_token
{
    VKD3D_PATCH_COMMAND_TOKEN_COPY_CONST_U32 = 0,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_IBO_VA_LO = 1,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_IBO_VA_HI = 2,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_IBO_SIZE = 3,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_INDEX_FORMAT = 4,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_VBO_VA_LO = 5,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_VBO_VA_HI = 6,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_VBO_SIZE = 7,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_VBO_STRIDE = 8,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_ROOT_VA_LO = 9,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_ROOT_VA_HI = 10,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_VERTEX_COUNT = 11,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_INDEX_COUNT = 12,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_INSTANCE_COUNT = 13,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_FIRST_INDEX = 14,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_FIRST_VERTEX = 15,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_FIRST_INSTANCE = 16,
    VKD3D_PATCH_COMMAND_TOKEN_COPY_VERTEX_OFFSET = 17,
    VKD3D_PATCH_COMMAND_INT_MAX = 0x7fffffff
};

/* ID3D12CommandSignature */
struct d3d12_command_signature
{
    ID3D12CommandSignature ID3D12CommandSignature_iface;
    LONG refcount;

    D3D12_COMMAND_SIGNATURE_DESC desc;
    uint32_t argument_buffer_offset;

    /* Complex command signatures require some work to stamp out device generated commands. */
    struct
    {
        VkBuffer buffer;
        VkDeviceAddress buffer_va;
        struct vkd3d_device_memory_allocation memory;
        VkIndirectCommandsLayoutNV layout;
        uint32_t stride;
        struct vkd3d_execute_indirect_info pipeline;
    } state_template;
    bool requires_state_template;

    struct d3d12_device *device;

    struct vkd3d_private_store private_store;
};

HRESULT d3d12_command_signature_create(struct d3d12_device *device, struct d3d12_root_signature *root_signature,
        const D3D12_COMMAND_SIGNATURE_DESC *desc,
        struct d3d12_command_signature **signature);

static inline struct d3d12_command_signature *impl_from_ID3D12CommandSignature(ID3D12CommandSignature *iface)
{
    extern CONST_VTBL struct ID3D12CommandSignatureVtbl d3d12_command_signature_vtbl;
    if (!iface)
        return NULL;
    assert(iface->lpVtbl == &d3d12_command_signature_vtbl);
    return CONTAINING_RECORD(iface, struct d3d12_command_signature, ID3D12CommandSignature_iface);
}

/* Static samplers */
struct vkd3d_sampler_state
{
    pthread_mutex_t mutex;
    struct hash_map map;

    VkDescriptorPool *vk_descriptor_pools;
    size_t vk_descriptor_pools_size;
    size_t vk_descriptor_pool_count;
};

struct vkd3d_shader_debug_ring
{
    VkBuffer host_buffer;
    VkBuffer device_atomic_buffer;

    struct vkd3d_device_memory_allocation host_buffer_memory;
    struct vkd3d_device_memory_allocation device_atomic_buffer_memory;

    uint32_t *mapped_control_block;
    uint32_t *mapped_ring;
    VkDeviceAddress ring_device_address;
    VkDeviceAddress atomic_device_address;
    size_t ring_size;
    size_t control_block_size;

    pthread_t ring_thread;
    pthread_mutex_t ring_lock;
    pthread_cond_t ring_cond;
    bool device_lost;
    bool active;
};

HRESULT vkd3d_sampler_state_init(struct vkd3d_sampler_state *state,
        struct d3d12_device *device);
void vkd3d_sampler_state_cleanup(struct vkd3d_sampler_state *state,
        struct d3d12_device *device);
HRESULT vkd3d_sampler_state_create_static_sampler(struct vkd3d_sampler_state *state,
        struct d3d12_device *device, const D3D12_STATIC_SAMPLER_DESC *desc, VkSampler *vk_sampler);
HRESULT vkd3d_sampler_state_allocate_descriptor_set(struct vkd3d_sampler_state *state,
        struct d3d12_device *device, VkDescriptorSetLayout vk_layout, VkDescriptorSet *vk_set,
        VkDescriptorPool *vk_pool);
void vkd3d_sampler_state_free_descriptor_set(struct vkd3d_sampler_state *state,
        struct d3d12_device *device, VkDescriptorSet vk_set, VkDescriptorPool vk_pool);

HRESULT vkd3d_shader_debug_ring_init(struct vkd3d_shader_debug_ring *state,
        struct d3d12_device *device);
void vkd3d_shader_debug_ring_cleanup(struct vkd3d_shader_debug_ring *state,
        struct d3d12_device *device);
void *vkd3d_shader_debug_ring_thread_main(void *arg);
void vkd3d_shader_debug_ring_init_spec_constant(struct d3d12_device *device,
        struct vkd3d_shader_debug_ring_spec_info *info, vkd3d_shader_hash_t hash);
/* If we assume device lost, try really hard to fish for messages. */
void vkd3d_shader_debug_ring_kick(struct vkd3d_shader_debug_ring *state,
        struct d3d12_device *device, bool device_lost);

enum vkd3d_breadcrumb_command_type
{
    VKD3D_BREADCRUMB_COMMAND_SET_TOP_MARKER,
    VKD3D_BREADCRUMB_COMMAND_SET_BOTTOM_MARKER,
    VKD3D_BREADCRUMB_COMMAND_SET_SHADER_HASH,
    VKD3D_BREADCRUMB_COMMAND_DRAW,
    VKD3D_BREADCRUMB_COMMAND_DRAW_INDEXED,
    VKD3D_BREADCRUMB_COMMAND_DISPATCH,
    VKD3D_BREADCRUMB_COMMAND_EXECUTE_INDIRECT,
    VKD3D_BREADCRUMB_COMMAND_EXECUTE_INDIRECT_TEMPLATE,
    VKD3D_BREADCRUMB_COMMAND_COPY,
    VKD3D_BREADCRUMB_COMMAND_RESOLVE,
    VKD3D_BREADCRUMB_COMMAND_WBI,
    VKD3D_BREADCRUMB_COMMAND_RESOLVE_QUERY,
    VKD3D_BREADCRUMB_COMMAND_GATHER_VIRTUAL_QUERY,
    VKD3D_BREADCRUMB_COMMAND_BUILD_RTAS,
    VKD3D_BREADCRUMB_COMMAND_COPY_RTAS,
    VKD3D_BREADCRUMB_COMMAND_EMIT_RTAS_POSTBUILD,
    VKD3D_BREADCRUMB_COMMAND_TRACE_RAYS,
    VKD3D_BREADCRUMB_COMMAND_BARRIER,
    VKD3D_BREADCRUMB_COMMAND_AUX32, /* Used to report arbitrary 32-bit words as arguments to other commands. */
    VKD3D_BREADCRUMB_COMMAND_AUX64, /* Used to report arbitrary 64-bit words as arguments to other commands. */
    VKD3D_BREADCRUMB_COMMAND_VBO,
    VKD3D_BREADCRUMB_COMMAND_IBO,
    VKD3D_BREADCRUMB_COMMAND_ROOT_DESC,
    VKD3D_BREADCRUMB_COMMAND_ROOT_CONST,
    VKD3D_BREADCRUMB_COMMAND_TAG,
};

#ifdef VKD3D_ENABLE_BREADCRUMBS
struct vkd3d_breadcrumb_counter
{
    uint32_t begin_marker;
    uint32_t end_marker;
};

struct vkd3d_breadcrumb_command
{
    enum vkd3d_breadcrumb_command_type type;
    union
    {
        struct
        {
            vkd3d_shader_hash_t hash;
            VkShaderStageFlagBits stage;
        } shader;

        uint32_t word_32bit;
        uint64_t word_64bit;
        uint32_t count;
        /* Pointer must remain alive. */
        const char *tag;
    };
};

struct vkd3d_breadcrumb_command_list_trace_context
{
    struct vkd3d_breadcrumb_command *commands;
    size_t command_size;
    size_t command_count;
    uint32_t counter;
    uint32_t locked;
};

struct vkd3d_breadcrumb_tracer
{
    /* There is room for N live command lists in this system.
     * We can allocate an index for each command list.
     * For AMD buffer markers, the index refers to the u32 counter in mapped.
     * 0 is inactive (has never been executed),
     * 1 is a command set on command buffer begin,
     * UINT_MAX is set on completion of the command buffer.
     * Concurrent submits is not legal. The counter will go back to 1 again from UINT_MAX
     * for multiple submits. */
    VkBuffer host_buffer;
    struct vkd3d_device_memory_allocation host_buffer_memory;
    struct vkd3d_breadcrumb_counter *mapped;

    struct vkd3d_breadcrumb_command_list_trace_context *trace_contexts;
    size_t trace_context_index;

    pthread_mutex_t lock;
};

HRESULT vkd3d_breadcrumb_tracer_init(struct vkd3d_breadcrumb_tracer *tracer, struct d3d12_device *device);
void vkd3d_breadcrumb_tracer_cleanup(struct vkd3d_breadcrumb_tracer *tracer, struct d3d12_device *device);
unsigned int vkd3d_breadcrumb_tracer_allocate_command_list(struct vkd3d_breadcrumb_tracer *tracer,
        struct d3d12_command_list *list, struct d3d12_command_allocator *allocator);
/* Command allocator keeps a list of allocated breadcrumb command lists. */
void vkd3d_breadcrumb_tracer_release_command_lists(struct vkd3d_breadcrumb_tracer *tracer,
        const unsigned int *indices, size_t indices_count);
void vkd3d_breadcrumb_tracer_report_device_lost(struct vkd3d_breadcrumb_tracer *tracer,
        struct d3d12_device *device);
void vkd3d_breadcrumb_tracer_begin_command_list(struct d3d12_command_list *list);
void vkd3d_breadcrumb_tracer_add_command(struct d3d12_command_list *list,
        const struct vkd3d_breadcrumb_command *command);
void vkd3d_breadcrumb_tracer_signal(struct d3d12_command_list *list);
void vkd3d_breadcrumb_tracer_end_command_list(struct d3d12_command_list *list);

#define VKD3D_BREADCRUMB_COMMAND(cmd_type) do { \
    if (vkd3d_config_flags & VKD3D_CONFIG_FLAG_BREADCRUMBS) { \
        struct vkd3d_breadcrumb_command breadcrumb_cmd; \
        breadcrumb_cmd.type = VKD3D_BREADCRUMB_COMMAND_##cmd_type; \
        vkd3d_breadcrumb_tracer_add_command(list, &breadcrumb_cmd); \
        vkd3d_breadcrumb_tracer_signal(list); \
    } \
} while(0)

/* State commands do no work on their own, should not signal. */
#define VKD3D_BREADCRUMB_COMMAND_STATE(cmd_type) do { \
    if (vkd3d_config_flags & VKD3D_CONFIG_FLAG_BREADCRUMBS) { \
        struct vkd3d_breadcrumb_command breadcrumb_cmd; \
        breadcrumb_cmd.type = VKD3D_BREADCRUMB_COMMAND_##cmd_type; \
        vkd3d_breadcrumb_tracer_add_command(list, &breadcrumb_cmd); \
    } \
} while(0)

#define VKD3D_BREADCRUMB_AUX32(v) do { \
    if (vkd3d_config_flags & VKD3D_CONFIG_FLAG_BREADCRUMBS) { \
        struct vkd3d_breadcrumb_command breadcrumb_cmd; \
        breadcrumb_cmd.type = VKD3D_BREADCRUMB_COMMAND_AUX32; \
        breadcrumb_cmd.word_32bit = v; \
        vkd3d_breadcrumb_tracer_add_command(list, &breadcrumb_cmd); \
    } \
} while(0)

#define VKD3D_BREADCRUMB_AUX64(v) do { \
    if (vkd3d_config_flags & VKD3D_CONFIG_FLAG_BREADCRUMBS) { \
        struct vkd3d_breadcrumb_command breadcrumb_cmd; \
        breadcrumb_cmd.type = VKD3D_BREADCRUMB_COMMAND_AUX64; \
        breadcrumb_cmd.word_64bit = v; \
        vkd3d_breadcrumb_tracer_add_command(list, &breadcrumb_cmd); \
    } \
} while(0)

/* Remember to kick debug ring as well. */
#define VKD3D_DEVICE_REPORT_BREADCRUMB_IF(device, cond) do { \
    if ((vkd3d_config_flags & VKD3D_CONFIG_FLAG_BREADCRUMBS) && (cond)) { \
        vkd3d_breadcrumb_tracer_report_device_lost(&(device)->breadcrumb_tracer, device); \
        vkd3d_shader_debug_ring_kick(&(device)->debug_ring, device, true); \
    } \
} while(0)
#else
#define VKD3D_BREADCRUMB_COMMAND(type) ((void)(VKD3D_BREADCRUMB_COMMAND_##type))
#define VKD3D_BREADCRUMB_COMMAND_STATE(type) ((void)(VKD3D_BREADCRUMB_COMMAND_##type))
#define VKD3D_BREADCRUMB_AUX32(v) ((void)(v))
#define VKD3D_BREADCRUMB_AUX64(v) ((void)(v))
#define VKD3D_DEVICE_REPORT_BREADCRUMB_IF(device, cond) ((void)(device), (void)(cond))
#endif /* VKD3D_ENABLE_BREADCRUMBS */

/* Bindless */
enum vkd3d_bindless_flags
{
    VKD3D_BINDLESS_SAMPLER               = (1u << 0),
    VKD3D_BINDLESS_CBV                   = (1u << 1),
    VKD3D_BINDLESS_SRV                   = (1u << 2),
    VKD3D_BINDLESS_UAV                   = (1u << 3),
    VKD3D_RAW_VA_AUX_BUFFER              = (1u << 4),
    VKD3D_BINDLESS_CBV_AS_SSBO           = (1u << 5),
    VKD3D_BINDLESS_RAW_SSBO              = (1u << 6),
    VKD3D_SSBO_OFFSET_BUFFER             = (1u << 7),
    VKD3D_TYPED_OFFSET_BUFFER            = (1u << 8),
    VKD3D_RAW_VA_ROOT_DESCRIPTOR_CBV     = (1u << 9),
    VKD3D_RAW_VA_ROOT_DESCRIPTOR_SRV_UAV = (1u << 10),
    VKD3D_BINDLESS_MUTABLE_TYPE          = (1u << 11),
    VKD3D_HOIST_STATIC_TABLE_CBV         = (1u << 12),
    VKD3D_BINDLESS_MUTABLE_TYPE_RAW_SSBO = (1u << 13),
};

#define VKD3D_BINDLESS_SET_MAX_EXTRA_BINDINGS 8

enum vkd3d_bindless_set_flag
{
    VKD3D_BINDLESS_SET_SAMPLER    = (1u << 0),
    VKD3D_BINDLESS_SET_CBV        = (1u << 1),
    VKD3D_BINDLESS_SET_SRV        = (1u << 2),
    VKD3D_BINDLESS_SET_UAV        = (1u << 3),
    VKD3D_BINDLESS_SET_IMAGE      = (1u << 4),
    VKD3D_BINDLESS_SET_BUFFER     = (1u << 5),
    VKD3D_BINDLESS_SET_AUX_BUFFER = (1u << 6),
    VKD3D_BINDLESS_SET_RAW_SSBO   = (1u << 7),
    VKD3D_BINDLESS_SET_MUTABLE    = (1u << 8),

    VKD3D_BINDLESS_SET_EXTRA_RAW_VA_AUX_BUFFER           = (1u << 24),
    VKD3D_BINDLESS_SET_EXTRA_OFFSET_BUFFER               = (1u << 25),
    VKD3D_BINDLESS_SET_EXTRA_GLOBAL_HEAP_INFO_BUFFER     = (1u << 26),
    VKD3D_BINDLESS_SET_EXTRA_DESCRIPTOR_HEAP_INFO_BUFFER = (1u << 27),
    VKD3D_BINDLESS_SET_EXTRA_MASK = 0xff000000u
};

struct vkd3d_bindless_set_info
{
    VkDescriptorType vk_descriptor_type;
    D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
    uint32_t flags; /* vkd3d_bindless_set_flag */
    uint32_t set_index;
    uint32_t binding_index;

    /* For VK_VALVE_descriptor_set_host_mapping */
    size_t host_mapping_offset;
    size_t host_mapping_descriptor_size;
    pfn_vkd3d_host_mapping_copy_template host_copy_template;
    pfn_vkd3d_host_mapping_copy_template_single host_copy_template_single;

    VkDescriptorSetLayout vk_set_layout;
    VkDescriptorSetLayout vk_host_set_layout;
};

struct vkd3d_bindless_state
{
    uint32_t flags; /* vkd3d_bindless_flags */

    struct vkd3d_bindless_set_info set_info[VKD3D_MAX_BINDLESS_DESCRIPTOR_SETS];
    unsigned int set_count;
    unsigned int cbv_srv_uav_count;
};

HRESULT vkd3d_bindless_state_init(struct vkd3d_bindless_state *bindless_state,
        struct d3d12_device *device);
void vkd3d_bindless_state_cleanup(struct vkd3d_bindless_state *bindless_state,
        struct d3d12_device *device);
bool vkd3d_bindless_state_find_binding(const struct vkd3d_bindless_state *bindless_state,
        uint32_t flags, struct vkd3d_shader_descriptor_binding *binding);
struct vkd3d_descriptor_binding vkd3d_bindless_state_find_set(const struct vkd3d_bindless_state *bindless_state, uint32_t flags);
uint32_t vkd3d_bindless_state_find_set_info_index(const struct vkd3d_bindless_state *bindless_state,
        uint32_t flags);

static inline struct vkd3d_descriptor_binding vkd3d_bindless_state_binding_from_info_index(
        const struct vkd3d_bindless_state *bindless_state, uint32_t index)
{
    struct vkd3d_descriptor_binding binding;
    binding.binding = bindless_state->set_info[index].binding_index;
    binding.set = bindless_state->set_info[index].set_index;
    return binding;
}

static inline VkDescriptorType vkd3d_bindless_state_get_cbv_descriptor_type(const struct vkd3d_bindless_state *bindless_state)
{
    return bindless_state->flags & VKD3D_BINDLESS_CBV_AS_SSBO
            ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
            : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

struct vkd3d_format_compatibility_list
{
    unsigned int format_count;
    VkFormat vk_formats[VKD3D_MAX_COMPATIBLE_FORMAT_COUNT];
    DXGI_FORMAT uint_format;
};

void vkd3d_format_compatibility_list_add_format(struct vkd3d_format_compatibility_list *list, VkFormat vk_format);

struct vkd3d_memory_info_domain
{
    uint32_t buffer_type_mask;
    uint32_t sampled_type_mask;
    uint32_t rt_ds_type_mask;
};

struct vkd3d_memory_info
{
    uint32_t global_mask;
    /* Includes normal system memory, but also resizable BAR memory.
     * Only types which have HOST_VISIBLE_BIT can be in this domain.
     * For images, we only include memory types which are LINEAR tiled. */
    struct vkd3d_memory_info_domain cpu_accessible_domain;
    /* Also includes fallback memory types when DEVICE_LOCAL is exhausted.
     * It can include HOST_VISIBLE_BIT as well, but when choosing this domain,
     * that's not something we care about.
     * Used when we want to allocate DEFAULT heaps or non-visible CUSTOM heaps.
     * For images, we only include memory types which are OPTIMAL tiled. */
    struct vkd3d_memory_info_domain non_cpu_accessible_domain;

    uint32_t budget_sensitive_mask;
    VkDeviceSize type_budget[VK_MAX_MEMORY_TYPES];
    VkDeviceSize type_current[VK_MAX_MEMORY_TYPES];
    pthread_mutex_t budget_lock;
};

HRESULT vkd3d_memory_info_init(struct vkd3d_memory_info *info,
        struct d3d12_device *device);
void vkd3d_memory_info_cleanup(struct vkd3d_memory_info *info,
        struct d3d12_device *device);

/* meta operations */
struct vkd3d_clear_uav_args
{
    VkClearColorValue clear_color;
    VkOffset2D offset;
    VkExtent2D extent;
};

struct vkd3d_clear_uav_pipelines
{
    VkPipeline buffer;
    VkPipeline buffer_raw;
    VkPipeline image_1d;
    VkPipeline image_2d;
    VkPipeline image_3d;
    VkPipeline image_1d_array;
    VkPipeline image_2d_array;
};

struct vkd3d_clear_uav_ops
{
    VkDescriptorSetLayout vk_set_layout_buffer_raw;
    VkDescriptorSetLayout vk_set_layout_buffer;
    VkDescriptorSetLayout vk_set_layout_image;

    VkPipelineLayout vk_pipeline_layout_buffer_raw;
    VkPipelineLayout vk_pipeline_layout_buffer;
    VkPipelineLayout vk_pipeline_layout_image;

    struct vkd3d_clear_uav_pipelines clear_float;
    struct vkd3d_clear_uav_pipelines clear_uint;
};

struct vkd3d_clear_uav_pipeline
{
    VkDescriptorSetLayout vk_set_layout;
    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;
};

HRESULT vkd3d_clear_uav_ops_init(struct vkd3d_clear_uav_ops *meta_clear_uav_ops,
        struct d3d12_device *device);
void vkd3d_clear_uav_ops_cleanup(struct vkd3d_clear_uav_ops *meta_clear_uav_ops,
        struct d3d12_device *device);

struct vkd3d_copy_image_args
{
    VkOffset2D offset;
};

struct vkd3d_copy_image_info
{
    VkDescriptorSetLayout vk_set_layout;
    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;
};

struct vkd3d_copy_image_pipeline_key
{
    const struct vkd3d_format *format;
    VkImageViewType view_type;
    VkSampleCountFlagBits sample_count;
    VkImageAspectFlags dst_aspect_mask;
};

struct vkd3d_copy_image_pipeline
{
    struct vkd3d_copy_image_pipeline_key key;

    VkPipeline vk_pipeline;
};

struct vkd3d_copy_image_ops
{
    VkDescriptorSetLayout vk_set_layout;
    VkPipelineLayout vk_pipeline_layout;
    VkShaderModule vk_fs_float_module;
    VkShaderModule vk_fs_uint_module;
    VkShaderModule vk_fs_stencil_module;

    pthread_mutex_t mutex;

    struct vkd3d_copy_image_pipeline *pipelines;
    size_t pipelines_size;
    size_t pipeline_count;
};

HRESULT vkd3d_copy_image_ops_init(struct vkd3d_copy_image_ops *meta_copy_image_ops,
        struct d3d12_device *device);
void vkd3d_copy_image_ops_cleanup(struct vkd3d_copy_image_ops *meta_copy_image_ops,
        struct d3d12_device *device);

struct vkd3d_swapchain_pipeline_key
{
    VkPipelineBindPoint bind_point;
    VkFormat format;
    VkFilter filter;
};

struct vkd3d_swapchain_info
{
    VkDescriptorSetLayout vk_set_layout;
    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;
};

struct vkd3d_swapchain_pipeline
{
    VkPipeline vk_pipeline;
    struct vkd3d_swapchain_pipeline_key key;
};

struct vkd3d_swapchain_ops
{
    VkDescriptorSetLayout vk_set_layouts[2];
    VkPipelineLayout vk_pipeline_layouts[2];
    VkShaderModule vk_vs_module;
    VkShaderModule vk_fs_module;
    VkSampler vk_samplers[2];

    pthread_mutex_t mutex;

    struct vkd3d_swapchain_pipeline *pipelines;
    size_t pipelines_size;
    size_t pipeline_count;
};

HRESULT vkd3d_swapchain_ops_init(struct vkd3d_swapchain_ops *meta_swapchain_ops,
        struct d3d12_device *device);
void vkd3d_swapchain_ops_cleanup(struct vkd3d_swapchain_ops *meta_swapchain_ops,
        struct d3d12_device *device);

#define VKD3D_QUERY_OP_WORKGROUP_SIZE (64)

struct vkd3d_query_resolve_args
{
    uint32_t dst_index;
    uint32_t src_index;
    uint32_t query_count;
};

struct vkd3d_query_gather_args
{
    uint32_t query_count;
    uint32_t entry_offset;
};

struct vkd3d_query_gather_info
{
    VkDescriptorSetLayout vk_set_layout;
    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;
};

struct vkd3d_query_ops
{
    VkDescriptorSetLayout vk_gather_set_layout;
    VkPipelineLayout vk_gather_pipeline_layout;
    VkPipeline vk_gather_occlusion_pipeline;
    VkPipeline vk_gather_so_statistics_pipeline;
    VkDescriptorSetLayout vk_resolve_set_layout;
    VkPipelineLayout vk_resolve_pipeline_layout;
    VkPipeline vk_resolve_binary_pipeline;
};

HRESULT vkd3d_query_ops_init(struct vkd3d_query_ops *meta_query_ops,
        struct d3d12_device *device);
void vkd3d_query_ops_cleanup(struct vkd3d_query_ops *meta_query_ops,
        struct d3d12_device *device);

union vkd3d_predicate_command_direct_args
{
    VkDispatchIndirectCommand dispatch;
    VkDrawIndirectCommand draw;
    VkDrawIndexedIndirectCommand draw_indexed;
    uint32_t draw_count;
};

struct vkd3d_predicate_command_args
{
    VkDeviceAddress predicate_va;
    VkDeviceAddress src_arg_va;
    VkDeviceAddress dst_arg_va;
    union vkd3d_predicate_command_direct_args args;
};

enum vkd3d_predicate_command_type
{
    VKD3D_PREDICATE_COMMAND_DRAW,
    VKD3D_PREDICATE_COMMAND_DRAW_INDEXED,
    VKD3D_PREDICATE_COMMAND_DRAW_INDIRECT,
    VKD3D_PREDICATE_COMMAND_DRAW_INDIRECT_COUNT,
    VKD3D_PREDICATE_COMMAND_DISPATCH,
    VKD3D_PREDICATE_COMMAND_DISPATCH_INDIRECT,
    VKD3D_PREDICATE_COMMAND_COUNT
};

struct vkd3d_predicate_command_info
{
    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;
    uint32_t data_size;
};

struct vkd3d_predicate_resolve_args
{
    VkDeviceAddress src_va;
    VkDeviceAddress dst_va;
    VkBool32 invert;
};

struct vkd3d_predicate_ops
{
    VkPipelineLayout vk_command_pipeline_layout;
    VkPipelineLayout vk_resolve_pipeline_layout;
    VkPipeline vk_command_pipelines[VKD3D_PREDICATE_COMMAND_COUNT];
    VkPipeline vk_resolve_pipeline;
    uint32_t data_sizes[VKD3D_PREDICATE_COMMAND_COUNT];
};

HRESULT vkd3d_predicate_ops_init(struct vkd3d_predicate_ops *meta_predicate_ops,
        struct d3d12_device *device);
void vkd3d_predicate_ops_cleanup(struct vkd3d_predicate_ops *meta_predicate_ops,
        struct d3d12_device *device);

struct vkd3d_execute_indirect_args
{
    VkDeviceAddress template_va;
    VkDeviceAddress api_buffer_va;
    VkDeviceAddress device_generated_commands_va;
    VkDeviceAddress indirect_count_va;
    VkDeviceAddress dst_indirect_count_va;
    uint32_t api_buffer_word_stride;
    uint32_t device_generated_commands_word_stride;

    /* Arbitrary tag used for debug version of state patcher. Debug messages from tag 0 are ignored. */
    uint32_t debug_tag;
    uint32_t implicit_instance;
};

struct vkd3d_execute_indirect_pipeline
{
    VkPipeline vk_pipeline;
    uint32_t workgroup_size_x;
};

struct vkd3d_execute_indirect_ops
{
    VkPipelineLayout vk_pipeline_layout;
    struct vkd3d_execute_indirect_pipeline *pipelines;
    size_t pipelines_count;
    size_t pipelines_size;
    pthread_mutex_t mutex;
};

HRESULT vkd3d_execute_indirect_ops_init(struct vkd3d_execute_indirect_ops *meta_indirect_ops,
        struct d3d12_device *device);
void vkd3d_execute_indirect_ops_cleanup(struct vkd3d_execute_indirect_ops *meta_indirect_ops,
        struct d3d12_device *device);

struct vkd3d_meta_ops_common
{
    VkShaderModule vk_module_fullscreen_vs;
    VkShaderModule vk_module_fullscreen_gs;
};

struct vkd3d_meta_ops
{
    struct d3d12_device *device;
    struct vkd3d_meta_ops_common common;
    struct vkd3d_clear_uav_ops clear_uav;
    struct vkd3d_copy_image_ops copy_image;
    struct vkd3d_swapchain_ops swapchain;
    struct vkd3d_query_ops query;
    struct vkd3d_predicate_ops predicate;
    struct vkd3d_execute_indirect_ops execute_indirect;
};

HRESULT vkd3d_meta_ops_init(struct vkd3d_meta_ops *meta_ops, struct d3d12_device *device);
HRESULT vkd3d_meta_ops_cleanup(struct vkd3d_meta_ops *meta_ops, struct d3d12_device *device);

struct vkd3d_clear_uav_pipeline vkd3d_meta_get_clear_buffer_uav_pipeline(struct vkd3d_meta_ops *meta_ops,
        bool as_uint, bool raw);
struct vkd3d_clear_uav_pipeline vkd3d_meta_get_clear_image_uav_pipeline(struct vkd3d_meta_ops *meta_ops,
        VkImageViewType image_view_type, bool as_uint);
VkExtent3D vkd3d_meta_get_clear_image_uav_workgroup_size(VkImageViewType view_type);

static inline VkExtent3D vkd3d_meta_get_clear_buffer_uav_workgroup_size()
{
    VkExtent3D result = { 128, 1, 1 };
    return result;
}

HRESULT vkd3d_meta_get_copy_image_pipeline(struct vkd3d_meta_ops *meta_ops,
        const struct vkd3d_copy_image_pipeline_key *key, struct vkd3d_copy_image_info *info);
VkImageViewType vkd3d_meta_get_copy_image_view_type(D3D12_RESOURCE_DIMENSION dim);
const struct vkd3d_format *vkd3d_meta_get_copy_image_attachment_format(struct vkd3d_meta_ops *meta_ops,
        const struct vkd3d_format *dst_format, const struct vkd3d_format *src_format,
        VkImageAspectFlags dst_aspect, VkImageAspectFlags src_aspect);
HRESULT vkd3d_meta_get_swapchain_pipeline(struct vkd3d_meta_ops *meta_ops,
        const struct vkd3d_swapchain_pipeline_key *key, struct vkd3d_swapchain_info *info);

bool vkd3d_meta_get_query_gather_pipeline(struct vkd3d_meta_ops *meta_ops,
        D3D12_QUERY_HEAP_TYPE heap_type, struct vkd3d_query_gather_info *info);

void vkd3d_meta_get_predicate_pipeline(struct vkd3d_meta_ops *meta_ops,
        enum vkd3d_predicate_command_type command_type, struct vkd3d_predicate_command_info *info);

HRESULT vkd3d_meta_get_execute_indirect_pipeline(struct vkd3d_meta_ops *meta_ops,
        uint32_t patch_command_count, struct vkd3d_execute_indirect_info *info);

enum vkd3d_time_domain_flag
{
    VKD3D_TIME_DOMAIN_DEVICE = 0x00000001u,
    VKD3D_TIME_DOMAIN_QPC    = 0x00000002u,
};

struct vkd3d_physical_device_info
{
    /* properties */
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties;
    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inline_uniform_block_properties;
    VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties;
    VkPhysicalDeviceMaintenance3Properties maintenance3_properties;
    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texel_buffer_alignment_properties;
    VkPhysicalDeviceTransformFeedbackPropertiesEXT xfb_properties;
    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertex_divisor_properties;
    VkPhysicalDeviceSubgroupProperties subgroup_properties;
    VkPhysicalDeviceTimelineSemaphorePropertiesKHR timeline_semaphore_properties;
    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroup_size_control_properties;
    VkPhysicalDeviceCustomBorderColorPropertiesEXT custom_border_color_properties;
    VkPhysicalDeviceShaderCorePropertiesAMD shader_core_properties;
    VkPhysicalDeviceShaderCoreProperties2AMD shader_core_properties2;
    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shader_sm_builtins_properties;
    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT sampler_filter_minmax_properties;
    VkPhysicalDeviceRobustness2PropertiesEXT robustness2_properties;
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT external_memory_host_properties;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties;
    VkPhysicalDeviceFloatControlsPropertiesKHR float_control_properties;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties;
    VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservative_rasterization_properties;
    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR shader_integer_dot_product_properties;
    VkPhysicalDeviceDriverPropertiesKHR driver_properties;
    VkPhysicalDeviceMaintenance4PropertiesKHR maintenance4_properties;
    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV device_generated_commands_properties_nv;
    VkPhysicalDeviceMeshShaderPropertiesEXT mesh_shader_properties;

    VkPhysicalDeviceProperties2KHR properties2;

    /* features */
    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR buffer_device_address_features;
    VkPhysicalDeviceConditionalRenderingFeaturesEXT conditional_rendering_features;
    VkPhysicalDeviceDepthClipEnableFeaturesEXT depth_clip_features;
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features;
    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT demote_features;
    VkPhysicalDeviceInlineUniformBlockFeaturesEXT inline_uniform_block_features;
    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT texel_buffer_alignment_features;
    VkPhysicalDeviceTransformFeedbackFeaturesEXT xfb_features;
    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertex_divisor_features;
    VkPhysicalDeviceCustomBorderColorFeaturesEXT custom_border_color_features;
    VkPhysicalDevice4444FormatsFeaturesEXT ext_4444_formats_features;
    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline_semaphore_features;
    VkPhysicalDeviceFloat16Int8FeaturesKHR float16_int8_features;
    VkPhysicalDevice16BitStorageFeatures storage_16bit_features;
    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR subgroup_extended_types_features;
    VkPhysicalDeviceRobustness2FeaturesEXT robustness2_features;
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extended_dynamic_state_features;
    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extended_dynamic_state2_features;
    VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE mutable_descriptor_features;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fragment_shading_rate_features;
    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features;
    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT subgroup_size_control_features;
    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR separate_depth_stencil_layout_features;
    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR shader_integer_dot_product_features;
    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV barycentric_features_nv;
    VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR barycentric_features_khr;
    VkPhysicalDeviceRayQueryFeaturesKHR ray_query_features;
    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV compute_shader_derivatives_features_nv;
    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR shader_atomic_int64_features;
    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT shader_image_atomic_int64_features;
    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT scalar_block_layout_features;
    VkPhysicalDeviceUniformBufferStandardLayoutFeatures uniform_buffer_standard_layout_features;
    VkPhysicalDeviceImageViewMinLodFeaturesEXT image_view_min_lod_features;
    VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE descriptor_set_host_mapping_features;
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features;
    VkPhysicalDeviceCoherentMemoryFeaturesAMD device_coherent_memory_features_amd;
    VkPhysicalDeviceMaintenance4FeaturesKHR maintenance4_features;
    VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR ray_tracing_maintenance1_features;
    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV device_generated_commands_features_nv;
    VkPhysicalDeviceMeshShaderFeaturesEXT mesh_shader_features;

    VkPhysicalDeviceFeatures2 features2;

    /* others, for extensions that have no feature bits */
    uint32_t time_domains;  /* vkd3d_time_domain_flag */

    bool additional_shading_rates_supported; /* d3d12 additional fragment shading rates cap */
};

struct d3d12_caps
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS options;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1;
    D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2;
    D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3;
    D3D12_FEATURE_DATA_D3D12_OPTIONS4 options4;
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5;
    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6;
    D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7;
    D3D12_FEATURE_DATA_D3D12_OPTIONS8 options8;
    D3D12_FEATURE_DATA_D3D12_OPTIONS9 options9;
    D3D12_FEATURE_DATA_D3D12_OPTIONS10 options10;
    D3D12_FEATURE_DATA_D3D12_OPTIONS11 options11;

    D3D_FEATURE_LEVEL max_feature_level;
    D3D_SHADER_MODEL max_shader_model;
};

enum vkd3d_queue_family
{
    VKD3D_QUEUE_FAMILY_GRAPHICS,
    VKD3D_QUEUE_FAMILY_COMPUTE,
    VKD3D_QUEUE_FAMILY_TRANSFER,
    VKD3D_QUEUE_FAMILY_SPARSE_BINDING,
    /* Keep internal queues at the end */
    VKD3D_QUEUE_FAMILY_INTERNAL_COMPUTE,

    VKD3D_QUEUE_FAMILY_COUNT
};

struct vkd3d_queue_family_info
{
    struct vkd3d_queue **queues;
    uint32_t queue_count;
    uint32_t vk_family_index;
    uint32_t timestamp_bits;
    VkQueueFlags vk_queue_flags;
};

#define VKD3D_CACHED_COMMAND_ALLOCATOR_COUNT 8
struct vkd3d_cached_command_allocator
{
    VkCommandPool vk_command_pool;
    uint32_t vk_family_index;
};

/* ID3D12Device */
typedef ID3D12Device9 d3d12_device_iface;

struct vkd3d_descriptor_qa_global_info;
struct vkd3d_descriptor_qa_heap_buffer_data;

/* ID3D12DeviceExt */
typedef ID3D12DeviceExt d3d12_device_vkd3d_ext_iface;

struct d3d12_device_scratch_pool
{
    struct vkd3d_scratch_buffer scratch_buffers[VKD3D_SCRATCH_BUFFER_COUNT];
    size_t scratch_buffer_count;
};

struct d3d12_device
{
    d3d12_device_iface ID3D12Device_iface;
    d3d12_device_vkd3d_ext_iface ID3D12DeviceExt_iface;
    LONG refcount;

    VkDevice vk_device;
    uint32_t api_version;
    VkPhysicalDevice vk_physical_device;
    struct vkd3d_vk_device_procs vk_procs;
    PFN_vkd3d_signal_event signal_event;

    pthread_mutex_t mutex;

    VkPhysicalDeviceMemoryProperties memory_properties;

    struct vkd3d_vulkan_info vk_info;
    struct vkd3d_physical_device_info device_info;

    struct vkd3d_queue_family_info *queue_families[VKD3D_QUEUE_FAMILY_COUNT];
    uint32_t queue_family_indices[VKD3D_QUEUE_FAMILY_COUNT];
    uint32_t queue_family_count;
    uint32_t unique_queue_mask;

    struct vkd3d_instance *vkd3d_instance;

    IUnknown *parent;
    LUID adapter_luid;

    struct vkd3d_private_store private_store;
    struct d3d12_caps d3d12_caps;

    struct vkd3d_memory_allocator memory_allocator;

    struct d3d12_device_scratch_pool scratch_pools[VKD3D_SCRATCH_POOL_KIND_COUNT];

    struct vkd3d_query_pool query_pools[VKD3D_VIRTUAL_QUERY_POOL_COUNT];
    size_t query_pool_count;

    struct vkd3d_cached_command_allocator cached_command_allocators[VKD3D_CACHED_COMMAND_ALLOCATOR_COUNT];
    size_t cached_command_allocator_count;

    uint32_t *descriptor_heap_gpu_vas;
    size_t descriptor_heap_gpu_va_count;
    size_t descriptor_heap_gpu_va_size;
    uint32_t descriptor_heap_gpu_next;

    HRESULT removed_reason;

    const struct vkd3d_format *formats;
    const struct vkd3d_format *depth_stencil_formats;
    unsigned int format_compatibility_list_count;
    const struct vkd3d_format_compatibility_list *format_compatibility_lists;
    struct vkd3d_bindless_state bindless_state;
    struct vkd3d_memory_info memory_info;
    struct vkd3d_meta_ops meta_ops;
    struct vkd3d_view_map sampler_map;
    struct vkd3d_sampler_state sampler_state;
    struct vkd3d_shader_debug_ring debug_ring;
    struct vkd3d_pipeline_library_disk_cache disk_cache;
#ifdef VKD3D_ENABLE_BREADCRUMBS
    struct vkd3d_breadcrumb_tracer breadcrumb_tracer;
#endif
#ifdef VKD3D_ENABLE_DESCRIPTOR_QA
    struct vkd3d_descriptor_qa_global_info *descriptor_qa_global_info;
#endif
    uint64_t shader_interface_key;
};

HRESULT d3d12_device_create(struct vkd3d_instance *instance,
        const struct vkd3d_device_create_info *create_info, struct d3d12_device **device);
struct vkd3d_queue_family_info *d3d12_device_get_vkd3d_queue_family(struct d3d12_device *device,
        D3D12_COMMAND_LIST_TYPE type);
struct vkd3d_queue *d3d12_device_allocate_vkd3d_queue(struct d3d12_device *device,
        struct vkd3d_queue_family_info *queue_family);
void d3d12_device_unmap_vkd3d_queue(struct d3d12_device *device,
        struct vkd3d_queue *queue);
bool d3d12_device_is_uma(struct d3d12_device *device, bool *coherent);
void d3d12_device_mark_as_removed(struct d3d12_device *device, HRESULT reason,
        const char *message, ...) VKD3D_PRINTF_FUNC(3, 4);

static inline struct d3d12_device *unsafe_impl_from_ID3D12Device(d3d12_device_iface *iface)
{
    return CONTAINING_RECORD(iface, struct d3d12_device, ID3D12Device_iface);
}

static inline struct d3d12_device *impl_from_ID3D12Device(d3d12_device_iface *iface)
{
    extern CONST_VTBL struct ID3D12Device9Vtbl d3d12_device_vtbl;
#ifdef VKD3D_ENABLE_PROFILING
    extern CONST_VTBL struct ID3D12Device9Vtbl d3d12_device_vtbl_profiled;
#endif
    if (!iface)
        return NULL;

#ifdef VKD3D_ENABLE_PROFILING
    assert(iface->lpVtbl == &d3d12_device_vtbl ||
           iface->lpVtbl == &d3d12_device_vtbl_profiled);
#else
    assert(iface->lpVtbl == &d3d12_device_vtbl);
#endif

    return CONTAINING_RECORD(iface, struct d3d12_device, ID3D12Device_iface);
}

bool d3d12_device_validate_shader_meta(struct d3d12_device *device, const struct vkd3d_shader_meta *meta);

HRESULT d3d12_device_get_scratch_buffer(struct d3d12_device *device, enum vkd3d_scratch_pool_kind kind,
        VkDeviceSize min_size, uint32_t memory_types, struct vkd3d_scratch_buffer *scratch);
void d3d12_device_return_scratch_buffer(struct d3d12_device *device, enum vkd3d_scratch_pool_kind kind,
        const struct vkd3d_scratch_buffer *scratch);

HRESULT d3d12_device_get_query_pool(struct d3d12_device *device, uint32_t type_index, struct vkd3d_query_pool *pool);
void d3d12_device_return_query_pool(struct d3d12_device *device, const struct vkd3d_query_pool *pool);

uint64_t d3d12_device_get_descriptor_heap_gpu_va(struct d3d12_device *device);
void d3d12_device_return_descriptor_heap_gpu_va(struct d3d12_device *device, uint64_t va);

static inline bool is_cpu_accessible_heap(const D3D12_HEAP_PROPERTIES *properties)
{
    if (properties->Type == D3D12_HEAP_TYPE_DEFAULT)
        return false;
    if (properties->Type == D3D12_HEAP_TYPE_CUSTOM)
    {
        return properties->CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
                || properties->CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    }
    return true;
}

static inline const struct vkd3d_memory_info_domain *d3d12_device_get_memory_info_domain(
        struct d3d12_device *device,
        const D3D12_HEAP_PROPERTIES *heap_properties)
{
    /* Host visible and non-host visible memory types do not necessarily
     * overlap. Need to select memory types appropriately. */
    if (is_cpu_accessible_heap(heap_properties))
        return &device->memory_info.cpu_accessible_domain;
    else
        return &device->memory_info.non_cpu_accessible_domain;
}

static inline HRESULT d3d12_device_query_interface(struct d3d12_device *device, REFIID iid, void **object)
{
    return ID3D12Device9_QueryInterface(&device->ID3D12Device_iface, iid, object);
}

static inline ULONG d3d12_device_add_ref(struct d3d12_device *device)
{
    return ID3D12Device9_AddRef(&device->ID3D12Device_iface);
}

static inline ULONG d3d12_device_release(struct d3d12_device *device)
{
    return ID3D12Device9_Release(&device->ID3D12Device_iface);
}

static inline unsigned int d3d12_device_get_descriptor_handle_increment_size(
        D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_type)
{
    switch (descriptor_heap_type)
    {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            return VKD3D_RESOURCE_DESC_INCREMENT;

        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
            return sizeof(struct d3d12_rtv_desc);

        default:
            FIXME("Unhandled type %#x.\n", descriptor_heap_type);
            return 0;
    }
}

static inline bool d3d12_device_use_ssbo_raw_buffer(struct d3d12_device *device)
{
    return (device->bindless_state.flags & VKD3D_BINDLESS_RAW_SSBO) != 0;
}

static inline VkDeviceSize d3d12_device_get_ssbo_alignment(struct d3d12_device *device)
{
    return device->device_info.properties2.properties.limits.minStorageBufferOffsetAlignment;
}

static inline bool d3d12_device_use_ssbo_root_descriptors(struct d3d12_device *device)
{
    /* We only know the VA of root SRV/UAVs, so we cannot
     * make any better assumptions about the alignment */
    return d3d12_device_use_ssbo_raw_buffer(device) &&
            d3d12_device_get_ssbo_alignment(device) <= 4;
}

bool d3d12_device_supports_variable_shading_rate_tier_1(struct d3d12_device *device);
bool d3d12_device_supports_variable_shading_rate_tier_2(struct d3d12_device *device);
bool d3d12_device_supports_ray_tracing_tier_1_0(const struct d3d12_device *device);
UINT d3d12_determine_shading_rate_image_tile_size(struct d3d12_device *device);

/* ID3DBlob */
struct d3d_blob
{
    ID3D10Blob ID3DBlob_iface;
    LONG refcount;

    void *buffer;
    SIZE_T size;
};

HRESULT d3d_blob_create(void *buffer, SIZE_T size, struct d3d_blob **blob);

/* ID3D12StateObject */
typedef ID3D12StateObject d3d12_state_object_iface;
typedef ID3D12StateObjectProperties d3d12_state_object_properties_iface;

struct d3d12_state_object_identifier
{
    WCHAR *mangled_export;
    WCHAR *plain_export;
    /* Must be a persistent pointer as long as the StateObject object is live. */
    uint8_t identifier[D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES];

    /* The index into pStages[]. */
    uint32_t general_stage_index;
    uint32_t closest_stage_index;
    uint32_t anyhit_stage_index;
    uint32_t intersection_stage_index;
    VkShaderStageFlagBits general_stage;

    VkDeviceSize stack_size_general;
    VkDeviceSize stack_size_closest;
    VkDeviceSize stack_size_any;
    VkDeviceSize stack_size_intersection;

    /* The index into vkGetShaderStackSize and friends for pGroups[]. */
    uint32_t group_index;

    /* For AddToStateObject(). We need to return the identifier pointer
     * for the parent, not the child. This makes it easy to validate that
     * we observe the same SBT handles as specified by DXR 1.1. */

    /* If -1, ignore, otherwise, redirect. */
    int inherited_collection_index;
    uint32_t inherited_collection_export_index;
};

struct d3d12_state_object_stack_info
{
    uint32_t max_callable;
    uint32_t max_anyhit;
    uint32_t max_miss;
    uint32_t max_raygen;
    uint32_t max_intersect;
    uint32_t max_closest;
};

#ifdef VKD3D_ENABLE_BREADCRUMBS
struct d3d12_state_object_breadcrumb_shader
{
    vkd3d_shader_hash_t hash;
    VkShaderStageFlagBits stage;
    char name[64];
};
#endif

struct d3d12_state_object
{
    d3d12_state_object_iface ID3D12StateObject_iface;
    d3d12_state_object_properties_iface ID3D12StateObjectProperties_iface;
    LONG refcount;
    LONG internal_refcount;
    D3D12_STATE_OBJECT_TYPE type;
    D3D12_STATE_OBJECT_FLAGS flags;
    struct d3d12_device *device;

    /* Could potentially be a hashmap. */
    struct d3d12_state_object_identifier *exports;
    size_t exports_size;
    size_t exports_count;

    struct vkd3d_shader_library_entry_point *entry_points;
    size_t entry_points_count;
    size_t stages_count;
    /* Normally stages_count == entry_points_count, but entry_points is the entry points we
     * export externally, and stages_count matches pStages[] size for purposes of index fixups. */

    /* Can be bound. */
    VkPipeline pipeline;
    /* Can be used as a library. */
    VkPipeline pipeline_library;

    /* Can be inherited by AddToStateObject(). */
    D3D12_RAYTRACING_PIPELINE_CONFIG1 pipeline_config;
    D3D12_RAYTRACING_SHADER_CONFIG shader_config;

    struct
    {
        VkDescriptorSetLayout set_layout;
        VkPipelineLayout pipeline_layout;
        VkDescriptorSet desc_set;
        VkDescriptorPool desc_pool;
        uint32_t set_index;
        uint64_t compatibility_hash;
        bool owned_handles;
    } local_static_sampler;

    UINT64 pipeline_stack_size;
    struct d3d12_state_object_stack_info stack;

    struct d3d12_state_object **collections;
    size_t collections_count;

    struct d3d12_root_signature *global_root_signature;

#ifdef VKD3D_ENABLE_BREADCRUMBS
    /* For breadcrumbs. */
    struct d3d12_state_object_breadcrumb_shader *breadcrumb_shaders;
    size_t breadcrumb_shaders_size;
    size_t breadcrumb_shaders_count;
#endif

    struct vkd3d_private_store private_store;
};

HRESULT d3d12_state_object_create(struct d3d12_device *device, const D3D12_STATE_OBJECT_DESC *desc,
        struct d3d12_state_object *parent,
        struct d3d12_state_object **object);
HRESULT d3d12_state_object_add(struct d3d12_device *device, const D3D12_STATE_OBJECT_DESC *desc,
        struct d3d12_state_object *parent,
        struct d3d12_state_object **object);

static inline struct d3d12_state_object *impl_from_ID3D12StateObject(ID3D12StateObject *iface)
{
    return CONTAINING_RECORD(iface, struct d3d12_state_object, ID3D12StateObject_iface);
}

/* utils */
enum vkd3d_format_type
{
    VKD3D_FORMAT_TYPE_OTHER,
    VKD3D_FORMAT_TYPE_TYPELESS,
    VKD3D_FORMAT_TYPE_SINT,
    VKD3D_FORMAT_TYPE_UINT,
};

struct vkd3d_format_footprint
{
    DXGI_FORMAT dxgi_format;
    uint32_t block_width;
    uint32_t block_height;
    uint32_t block_byte_count;
    uint32_t subsample_x_log2;
    uint32_t subsample_y_log2;
};

struct vkd3d_format
{
    DXGI_FORMAT dxgi_format;
    VkFormat vk_format;
    uint32_t byte_count;
    uint32_t block_width;
    uint32_t block_height;
    uint32_t block_byte_count;
    VkImageAspectFlags vk_aspect_mask;
    unsigned int plane_count;
    enum vkd3d_format_type type;
    bool is_emulated;
    const struct vkd3d_format_footprint *plane_footprints;
};

static inline size_t vkd3d_format_get_data_offset(const struct vkd3d_format *format,
        unsigned int row_pitch, unsigned int slice_pitch,
        unsigned int x, unsigned int y, unsigned int z)
{
    return z * slice_pitch
            + (y / format->block_height) * row_pitch
            + (x / format->block_width) * format->byte_count * format->block_byte_count;
}

static inline bool vkd3d_format_is_compressed(const struct vkd3d_format *format)
{
    return format->block_byte_count != 1;
}

void vkd3d_format_copy_data(const struct vkd3d_format *format, const uint8_t *src,
        unsigned int src_row_pitch, unsigned int src_slice_pitch, uint8_t *dst, unsigned int dst_row_pitch,
        unsigned int dst_slice_pitch, unsigned int w, unsigned int h, unsigned int d);

const struct vkd3d_format *vkd3d_get_format(const struct d3d12_device *device,
        DXGI_FORMAT dxgi_format, bool depth_stencil);
VkFormat vkd3d_internal_get_vk_format(const struct d3d12_device *device, DXGI_FORMAT dxgi_format);
struct vkd3d_format_footprint vkd3d_format_footprint_for_plane(const struct vkd3d_format *format, unsigned int plane_idx);

HRESULT vkd3d_init_format_info(struct d3d12_device *device);
void vkd3d_cleanup_format_info(struct d3d12_device *device);

static inline const struct vkd3d_format *vkd3d_format_from_d3d12_resource_desc(
        const struct d3d12_device *device, const D3D12_RESOURCE_DESC1 *desc, DXGI_FORMAT view_format)
{
    return vkd3d_get_format(device, view_format ? view_format : desc->Format,
            desc->Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
}

static inline VkImageSubresourceRange vk_subresource_range_from_subresource(const VkImageSubresource *subresource)
{
    VkImageSubresourceRange range;
    range.aspectMask = subresource->aspectMask;
    range.baseMipLevel = subresource->mipLevel;
    range.levelCount = 1;
    range.baseArrayLayer = subresource->arrayLayer;
    range.layerCount = 1;
    return range;
}

static inline VkImageSubresourceRange vk_subresource_range_from_layers(const VkImageSubresourceLayers *layers)
{
    VkImageSubresourceRange range;
    range.aspectMask = layers->aspectMask;
    range.baseMipLevel = layers->mipLevel;
    range.levelCount = 1;
    range.baseArrayLayer = layers->baseArrayLayer;
    range.layerCount = layers->layerCount;
    return range;
}

static inline VkImageSubresourceLayers vk_subresource_layers_from_subresource(const VkImageSubresource *subresource)
{
    VkImageSubresourceLayers layers;
    layers.aspectMask = subresource->aspectMask;
    layers.mipLevel = subresource->mipLevel;
    layers.baseArrayLayer = subresource->arrayLayer;
    layers.layerCount = 1;
    return layers;
}

static inline VkImageSubresourceLayers vk_subresource_layers_from_view(const struct vkd3d_view *view)
{
    VkImageSubresourceLayers layers;
    layers.aspectMask = view->format->vk_aspect_mask;
    layers.mipLevel = view->info.texture.miplevel_idx;
    layers.baseArrayLayer = view->info.texture.layer_idx;
    layers.layerCount = view->info.texture.layer_count;
    return layers;
}

static inline VkImageSubresourceRange vk_subresource_range_from_view(const struct vkd3d_view *view)
{
    VkImageSubresourceLayers layers = vk_subresource_layers_from_view(view);
    return vk_subresource_range_from_layers(&layers);
}

static inline bool d3d12_box_is_empty(const D3D12_BOX *box)
{
    return box->right <= box->left || box->bottom <= box->top || box->back <= box->front;
}

static inline unsigned int d3d12_resource_desc_get_width(const D3D12_RESOURCE_DESC1 *desc,
        unsigned int miplevel_idx)
{
    return max(1, desc->Width >> miplevel_idx);
}

static inline unsigned int d3d12_resource_desc_get_height(const D3D12_RESOURCE_DESC1 *desc,
        unsigned int miplevel_idx)
{
    return max(1, desc->Height >> miplevel_idx);
}

static inline unsigned int d3d12_resource_desc_get_depth(const D3D12_RESOURCE_DESC1 *desc,
        unsigned int miplevel_idx)
{
    unsigned int d = desc->Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D ? 1 : desc->DepthOrArraySize;
    return max(1, d >> miplevel_idx);
}

static inline unsigned int d3d12_resource_desc_get_layer_count(const D3D12_RESOURCE_DESC1 *desc)
{
    return desc->Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D ? desc->DepthOrArraySize : 1;
}

static inline unsigned int d3d12_resource_desc_get_sub_resource_count_per_plane(const D3D12_RESOURCE_DESC1 *desc)
{
    return d3d12_resource_desc_get_layer_count(desc) * desc->MipLevels;
}

static inline unsigned int d3d12_resource_desc_get_sub_resource_count(const struct d3d12_device *device,
        const D3D12_RESOURCE_DESC1 *desc)
{
    const struct vkd3d_format *format = vkd3d_get_format(device, desc->Format, true);
    return d3d12_resource_desc_get_sub_resource_count_per_plane(desc) * (format ? format->plane_count : 1);
}

static inline unsigned int d3d12_resource_get_sub_resource_count(const struct d3d12_resource *resource)
{
    return d3d12_resource_desc_get_sub_resource_count_per_plane(&resource->desc) *
            (resource->format ? vkd3d_popcount(resource->format->vk_aspect_mask) : 1);
}

VkDeviceAddress vkd3d_get_buffer_device_address(struct d3d12_device *device, VkBuffer vk_buffer);
VkDeviceAddress vkd3d_get_acceleration_structure_device_address(struct d3d12_device *device,
        VkAccelerationStructureKHR vk_acceleration_structure);

static inline VkDeviceAddress d3d12_resource_get_va(const struct d3d12_resource *resource, VkDeviceSize offset)
{
    return vkd3d_get_buffer_device_address(resource->device, resource->res.vk_buffer) + resource->mem.offset + offset;
}

static inline unsigned int vkd3d_compute_workgroup_count(unsigned int thread_count, unsigned int workgroup_size)
{
    return (thread_count + workgroup_size - 1) / workgroup_size;
}

VkCompareOp vk_compare_op_from_d3d12(D3D12_COMPARISON_FUNC op);
VkSampleCountFlagBits vk_samples_from_dxgi_sample_desc(const DXGI_SAMPLE_DESC *desc);
VkSampleCountFlagBits vk_samples_from_sample_count(unsigned int sample_count);

bool is_valid_feature_level(D3D_FEATURE_LEVEL feature_level);

bool is_valid_resource_state(D3D12_RESOURCE_STATES state);
bool is_write_resource_state(D3D12_RESOURCE_STATES state);

HRESULT return_interface(void *iface, REFIID iface_iid,
        REFIID requested_iid, void **object);

const char *debug_dxgi_format(DXGI_FORMAT format);
const char *debug_d3d12_box(const D3D12_BOX *box);
const char *debug_d3d12_shader_component_mapping(unsigned int mapping);
const char *debug_vk_extent_3d(VkExtent3D extent);

#define VKD3D_DEBUG_FLAGS_BUFFER_SIZE 1024
const char *debug_vk_memory_heap_flags(VkMemoryHeapFlags flags, char buffer[VKD3D_DEBUG_FLAGS_BUFFER_SIZE]);
const char *debug_vk_memory_property_flags(VkMemoryPropertyFlags flags, char buffer[VKD3D_DEBUG_FLAGS_BUFFER_SIZE]);
const char *debug_vk_queue_flags(VkQueueFlags flags, char buffer[VKD3D_DEBUG_FLAGS_BUFFER_SIZE]);

static inline void debug_ignored_node_mask(unsigned int mask)
{
    if (mask && mask != 1)
        FIXME("Ignoring node mask 0x%08x.\n", mask);
}

HRESULT vkd3d_load_vk_global_procs(struct vkd3d_vk_global_procs *procs,
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr);
HRESULT vkd3d_load_vk_instance_procs(struct vkd3d_vk_instance_procs *procs,
        const struct vkd3d_vk_global_procs *global_procs, VkInstance instance);
HRESULT vkd3d_load_vk_device_procs(struct vkd3d_vk_device_procs *procs,
        const struct vkd3d_vk_instance_procs *parent_procs, VkDevice device);

HRESULT vkd3d_set_vk_object_name(struct d3d12_device *device, uint64_t vk_object,
        VkObjectType vk_object_type, const char *name);

enum VkPrimitiveTopology vk_topology_from_d3d12_topology(D3D12_PRIMITIVE_TOPOLOGY topology);

static inline void vk_prepend_struct(void *header, void *structure)
{
    VkBaseOutStructure *vk_header = header, *vk_structure = structure;

    assert(!vk_structure->pNext);
    vk_structure->pNext = vk_header->pNext;
    vk_header->pNext = vk_structure;
}

#define VKD3D_NULL_BUFFER_SIZE 16

struct vkd3d_view_key
{
    enum vkd3d_view_type view_type;
    union
    {
        struct vkd3d_buffer_view_desc buffer;
        struct vkd3d_texture_view_desc texture;
        D3D12_SAMPLER_DESC sampler;
    } u;
};
struct vkd3d_view *vkd3d_view_map_create_view(struct vkd3d_view_map *view_map,
        struct d3d12_device *device, const struct vkd3d_view_key *key);

/* Acceleration structure helpers. */
struct vkd3d_acceleration_structure_build_info
{
    /* This is not a hard limit, just an arbitrary value which lets us avoid allocation for
     * the common case. */
#define VKD3D_BUILD_INFO_STACK_COUNT 16
    const struct VkAccelerationStructureBuildRangeInfoKHR *build_range_ptr_stack[VKD3D_BUILD_INFO_STACK_COUNT];
    VkAccelerationStructureBuildRangeInfoKHR build_range_stack[VKD3D_BUILD_INFO_STACK_COUNT];
    VkAccelerationStructureGeometryKHR geometries_stack[VKD3D_BUILD_INFO_STACK_COUNT];
    const VkAccelerationStructureBuildRangeInfoKHR **build_range_ptrs;
    uint32_t primitive_counts_stack[VKD3D_BUILD_INFO_STACK_COUNT];
    VkAccelerationStructureBuildRangeInfoKHR *build_ranges;
    VkAccelerationStructureBuildGeometryInfoKHR build_info;
    VkAccelerationStructureGeometryKHR *geometries;
    uint32_t *primitive_counts;
};

void vkd3d_acceleration_structure_build_info_cleanup(
        struct vkd3d_acceleration_structure_build_info *info);
bool vkd3d_acceleration_structure_convert_inputs(const struct d3d12_device *device,
        struct vkd3d_acceleration_structure_build_info *info,
        const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS *desc);
void vkd3d_acceleration_structure_emit_postbuild_info(
        struct d3d12_command_list *list,
        const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC *desc,
        uint32_t count, const D3D12_GPU_VIRTUAL_ADDRESS *addresses);
void vkd3d_acceleration_structure_emit_immediate_postbuild_info(
        struct d3d12_command_list *list, uint32_t count,
        const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC *desc,
        VkAccelerationStructureKHR vk_acceleration_structure);
void vkd3d_acceleration_structure_copy(
        struct d3d12_command_list *list,
        D3D12_GPU_VIRTUAL_ADDRESS dst, D3D12_GPU_VIRTUAL_ADDRESS src,
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE mode);

typedef enum D3D11_USAGE
{
    D3D11_USAGE_DEFAULT,
    D3D11_USAGE_IMMUTABLE,
    D3D11_USAGE_DYNAMIC,
    D3D11_USAGE_STAGING,
} D3D11_USAGE;

typedef enum D3D11_BIND_FLAG
{
    D3D11_BIND_VERTEX_BUFFER    = 0x0001,
    D3D11_BIND_INDEX_BUFFER     = 0x0002,
    D3D11_BIND_CONSTANT_BUFFER  = 0x0004,
    D3D11_BIND_SHADER_RESOURCE  = 0x0008,
    D3D11_BIND_STREAM_OUTPUT    = 0x0010,
    D3D11_BIND_RENDER_TARGET    = 0x0020,
    D3D11_BIND_DEPTH_STENCIL    = 0x0040,
    D3D11_BIND_UNORDERED_ACCESS = 0x0080,
    D3D11_BIND_DECODER          = 0x0200,
    D3D11_BIND_VIDEO_ENCODER    = 0x0400
} D3D11_BIND_FLAG;

typedef enum D3D11_TEXTURE_LAYOUT
{
    D3D11_TEXTURE_LAYOUT_UNDEFINED = 0x0,
    D3D11_TEXTURE_LAYOUT_ROW_MAJOR = 0x1,
    D3D11_TEXTURE_LAYOUT_64K_STANDARD_SWIZZLE = 0x2,
} D3D11_TEXTURE_LAYOUT;

typedef enum D3D11_RESOURCE_MISC_FLAG
{
    D3D11_RESOURCE_MISC_GENERATE_MIPS                    = 0x1,
    D3D11_RESOURCE_MISC_SHARED                           = 0x2,
    D3D11_RESOURCE_MISC_TEXTURECUBE                      = 0x4,
    D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS                = 0x10,
    D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS           = 0x20,
    D3D11_RESOURCE_MISC_BUFFER_STRUCTURED                = 0x40,
    D3D11_RESOURCE_MISC_RESOURCE_CLAMP                   = 0x80,
    D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX                = 0x100,
    D3D11_RESOURCE_MISC_GDI_COMPATIBLE                   = 0x200,
    D3D11_RESOURCE_MISC_SHARED_NTHANDLE                  = 0x800,
    D3D11_RESOURCE_MISC_RESTRICTED_CONTENT               = 0x1000,
    D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE         = 0x2000,
    D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER  = 0x4000,
    D3D11_RESOURCE_MISC_GUARDED                          = 0x8000,
    D3D11_RESOURCE_MISC_TILE_POOL                        = 0x20000,
    D3D11_RESOURCE_MISC_TILED                            = 0x40000,
    D3D11_RESOURCE_MISC_HW_PROTECTED                     = 0x80000,
} D3D11_RESOURCE_MISC_FLAG;

struct DxvkSharedTextureMetadata {
    UINT             Width;
    UINT             Height;
    UINT             MipLevels;
    UINT             ArraySize;
    DXGI_FORMAT      Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D11_USAGE      Usage;
    UINT             BindFlags;
    UINT             CPUAccessFlags;
    UINT             MiscFlags;
    D3D11_TEXTURE_LAYOUT TextureLayout;
};

bool vkd3d_set_shared_metadata(HANDLE handle, void *buf, uint32_t buf_size);
bool vkd3d_get_shared_metadata(HANDLE handle, void *buf, uint32_t buf_size, uint32_t *metadata_size);
HANDLE vkd3d_open_kmt_handle(HANDLE kmt_handle);

#define VKD3D_VENDOR_ID_NVIDIA 0x10DE
#define VKD3D_VENDOR_ID_AMD 0x1002
#define VKD3D_VENDOR_ID_INTEL 0x8086

#define VKD3D_DRIVER_VERSION_MAJOR_NV(v) ((v) >> 22)
#define VKD3D_DRIVER_VERSION_MINOR_NV(v) (((v) >> 14) & 0xff)
#define VKD3D_DRIVER_VERSION_PATCH_NV(v) (((v) >>  6) & 0xff)
#define VKD3D_DRIVER_VERSION_MAKE_NV(major, minor, patch) (((major) << 22) | ((minor) << 14) | ((patch) << 6))

#endif  /* __VKD3D_PRIVATE_H */
