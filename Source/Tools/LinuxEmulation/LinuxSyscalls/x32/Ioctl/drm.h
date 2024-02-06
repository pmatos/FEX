#pragma once

#include <FEXCore/Utils/CompilerDefs.h>

#include "LinuxSyscalls/x32/Types.h"
#include "LinuxSyscalls/x32/Ioctl/HelperDefines.h"

#include <cstdint>
extern "C" {
// drm headers use a `__user` define that has an address_space attribute. This allows their tooling to see unsafe user-space accesses.
// Define this to nothing so we don't need to modify those headers.
#define __user
#include "fex-drm/drm.h"
#include "fex-drm/drm_mode.h"
#include "fex-drm/i915_drm.h"
#include "fex-drm/amdgpu_drm.h"
#include "fex-drm/lima_drm.h"
#include "fex-drm/panfrost_drm.h"
#include "fex-drm/msm_drm.h"
#include "fex-drm/nouveau_drm.h"
#include "fex-drm/radeon_drm.h"
#include "fex-drm/vc4_drm.h"
#include "fex-drm/v3d_drm.h"
#include "fex-drm/virtgpu_drm.h"
}
#include <sys/ioctl.h>

#define CPYT(x) val.x = x
#define CPYF(x) x = val.x
namespace FEX::HLE::x32 {

  namespace DRM {
    struct FEX_ANNOTATE("alias-x86_32-drm_version") FEX_ANNOTATE("fex-match") fex_drm_version {
      int version_major; /**< Major version */
      int version_minor; /**< Minor version */
      int version_patchlevel; /**< Patch level */
      uint32_t name_len; /**< Length of name buffer */
      compat_ptr<char> name; /**< Name of driver */
      uint32_t date_len; /**< Length of date buffer */
      compat_ptr<char> date; /**< User-space buffer to hold date */
      uint32_t desc_len; /**< Length of desc buffer */
      compat_ptr<char> desc; /**< User-space buffer to hold desc */

      fex_drm_version() = delete;

      operator drm_version() const {
        drm_version val{};
        val.version_major = version_major;
        val.version_minor = version_minor;
        val.version_patchlevel = version_patchlevel;
        val.name_len = name_len;
        val.name = name;
        val.date_len = date_len;
        val.date = date;
        val.desc_len = desc_len;
        val.desc = desc;
        return val;
      }

      fex_drm_version(struct drm_version val) : name{val.name}, date{val.date}, desc{val.desc} {
        version_major = val.version_major;
        version_minor = val.version_minor;
        version_patchlevel = val.version_patchlevel;
        name_len = val.name_len;
        name = val.name;
        date_len = val.date_len;
        date = val.date;
        desc_len = val.desc_len;
        desc = val.desc;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_unique") FEX_ANNOTATE("fex-match") fex_drm_unique {
      compat_size_t unique_len;
      compat_ptr<char> unique;

      fex_drm_unique() = delete;

      operator drm_unique() const {
        drm_unique val{};
        val.unique_len = unique_len;
        val.unique = unique;
        return val;
      }

      fex_drm_unique(struct drm_unique val) : unique{val.unique} { unique_len = val.unique_len; }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_map") FEX_ANNOTATE("fex-match") fex_drm_map {
      uint32_t offset;
      uint32_t size;
      enum drm_map_type type;
      enum drm_map_flags flags;
      compat_ptr<void> handle;
      int32_t mtrr;

      fex_drm_map() = delete;

      operator drm_map() const {
        drm_map val{};
        CPYT(offset);
        CPYT(size);
        CPYT(type);
        CPYT(flags);
        CPYT(handle);
        CPYT(mtrr);
        return val;
      }

      fex_drm_map(struct drm_map val) : handle{val.handle} {
        CPYT(offset);
        CPYT(size);
        CPYT(type);
        CPYT(flags);
        CPYT(mtrr);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_client") FEX_ANNOTATE("fex-match") fex_drm_client {
      int32_t idx;
      int32_t auth;
      uint32_t pid;
      uint32_t uid;
      uint32_t magic;
      uint32_t iocs;

      fex_drm_client() = delete;

      operator drm_client() const {
        drm_client val{};
        CPYT(idx);
        CPYT(auth);
        CPYT(pid);
        CPYT(uid);
        CPYT(magic);
        CPYT(iocs);
        return val;
      }

      fex_drm_client(struct drm_client val) {
        CPYF(idx);
        CPYF(auth);
        CPYF(pid);
        CPYF(uid);
        CPYF(magic);
        CPYF(iocs);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_stats") FEX_ANNOTATE("fex-match") fex_drm_stats {
      uint32_t count;
      struct {
        uint32_t value;
        enum drm_stat_type type;
      } data[15];

      fex_drm_stats() = delete;

      operator drm_stats() const {
        drm_stats val{};
        CPYT(count);
        for (size_t i = 0; i < 15; ++i) {
          CPYT(data[i].value);
          CPYT(data[i].type);
        }
        return val;
      }

      fex_drm_stats(struct drm_stats val) {
        CPYF(count);
        for (size_t i = 0; i < 15; ++i) {
          CPYF(data[i].value);
          CPYF(data[i].type);
        }
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_buf_desc") FEX_ANNOTATE("fex-match") fex_drm_buf_desc {
      int32_t count;
      int32_t size;
      int32_t low_mark;
      int32_t high_mark;
      enum {
        _DRM_PAGE_ALIGN = 0x01,
        _DRM_AGP_BUFFER = 0x02,
        _DRM_SG_BUFFER = 0x04,
        _DRM_FB_BUFFER = 0x08,
        _DRM_PCI_BUFFER_RO = 0x10
      } flags;
      uint32_t agp_start;

      fex_drm_buf_desc() = delete;

      operator drm_buf_desc() const {
        drm_buf_desc val{};
        CPYT(count);
        CPYT(size);
        CPYT(low_mark);
        CPYT(high_mark);
        CPYT(agp_start);
        return val;
      }

      fex_drm_buf_desc(struct drm_buf_desc val) {
        CPYF(count);
        CPYF(size);
        CPYF(low_mark);
        CPYF(high_mark);
        CPYF(agp_start);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_buf_info") FEX_ANNOTATE("fex-match") fex_drm_buf_info {
      int32_t count;
      compat_ptr<struct drm_buf_desc> list;

      fex_drm_buf_info() = delete;

      operator drm_buf_info() const {
        drm_buf_info val{};
        CPYT(count);
        CPYT(list);
        return val;
      }

      fex_drm_buf_info(struct drm_buf_info val) : list{val.list} { CPYF(count); }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_buf_pub") FEX_ANNOTATE("fex-match") fex_drm_buf_pub {
      int32_t idx;
      int32_t total;
      int32_t used;
      compat_ptr<void> address;

      fex_drm_buf_pub() = delete;

      operator drm_buf_pub() const {
        drm_buf_pub val{};
        CPYT(idx);
        CPYT(total);
        CPYT(used);
        CPYT(address);
        return val;
      }

      fex_drm_buf_pub(struct drm_buf_pub val) : address{val.address} {
        CPYF(idx);
        CPYF(total);
        CPYF(used);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_buf_map") FEX_ANNOTATE("fex-match") fex_drm_buf_map {
      int32_t count;
#ifdef __cplusplus
      compat_ptr<void> virt;
#else
      compat_ptr<void> virtual;
#endif
      compat_ptr<drm_buf_pub> list;

      fex_drm_buf_map() = delete;

      operator drm_buf_map() const {
        drm_buf_map val{};
        CPYT(count);
#ifdef __cplusplus
        CPYT(virt);
#else
        CPYT(virtual);
#endif
        CPYT(list);
        return val;
      }

      fex_drm_buf_map(struct drm_buf_map val)
#ifdef __cplusplus
        : virt{val.virt}
#else
        : virtual {val.virtual}
#endif
          ,
          list{val.list} {
        CPYF(count);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_buf_free") FEX_ANNOTATE("fex-match") fex_drm_buf_free {
      int32_t count;
      compat_ptr<int> list;

      fex_drm_buf_free() = delete;

      operator drm_buf_free() const {
        drm_buf_free val{};
        CPYT(count);
        CPYT(list);
        return val;
      }

      fex_drm_buf_free(struct drm_buf_free val) : list{val.list} { CPYF(count); }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_ctx_priv_map") FEX_ANNOTATE("fex-match") fex_drm_ctx_priv_map {
      uint32_t ctx_id;
      compat_ptr<void> handle;

      fex_drm_ctx_priv_map() = delete;

      operator drm_ctx_priv_map() const {
        drm_ctx_priv_map val{};
        CPYT(ctx_id);
        CPYT(handle);
        return val;
      }

      fex_drm_ctx_priv_map(struct drm_ctx_priv_map val) : handle{val.handle} { CPYF(ctx_id); }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_ctx_res") FEX_ANNOTATE("fex-match") fex_drm_ctx_res {
      int32_t count;
      compat_ptr<struct drm_ctx> contexts;

      fex_drm_ctx_res() = delete;

      operator drm_ctx_res() const {
        drm_ctx_res val{};
        CPYT(count);
        return val;
      }

      fex_drm_ctx_res(struct drm_ctx_res val) : contexts{val.contexts} { CPYF(count); }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_dma") FEX_ANNOTATE("fex-match") fex_drm_dma {
      int32_t context;
      int32_t send_count;
      compat_ptr<int32_t> send_indices;
      compat_ptr<int32_t> send_sizes;
      enum drm_dma_flags flags;
      int32_t request_count;
      int32_t request_size;
      compat_ptr<int32_t> request_indices;
      compat_ptr<int32_t> request_sizes;
      int32_t granted_count;

      fex_drm_dma() = delete;

      operator drm_dma() const {
        drm_dma val{};
        CPYT(context);
        CPYT(send_count);
        CPYT(send_indices);
        CPYT(send_sizes);
        CPYT(flags);
        CPYT(request_count);
        CPYT(request_size);
        CPYT(request_indices);
        CPYT(request_sizes);
        CPYT(granted_count);
        return val;
      }

      fex_drm_dma(struct drm_dma val)
        : send_indices{val.send_indices},
          send_sizes{val.send_sizes},
          request_indices{val.request_indices},
          request_sizes{val.request_sizes} {
        CPYF(context);
        CPYF(send_count);
        CPYF(flags);
        CPYF(request_count);
        CPYF(request_size);
        CPYF(granted_count);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_scatter_gather") FEX_ANNOTATE("fex-match") fex_drm_scatter_gather {
      uint32_t size;
      uint32_t handle;

      fex_drm_scatter_gather() = delete;

      operator drm_scatter_gather() const {
        drm_scatter_gather val{};
        CPYT(size);
        CPYT(handle);
        return val;
      }

      fex_drm_scatter_gather(struct drm_scatter_gather val) {
        CPYF(size);
        CPYF(handle);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_wait_vblank_request") FEX_ANNOTATE("fex-match") fex_drm_wait_vblank_request {
      enum drm_vblank_seq_type type;
      uint32_t sequence;
      uint32_t signal;

      fex_drm_wait_vblank_request() = delete;

      operator drm_wait_vblank_request() const {
        drm_wait_vblank_request val{};
        CPYT(type);
        CPYT(sequence);
        CPYT(signal);
        return val;
      }

      fex_drm_wait_vblank_request(struct drm_wait_vblank_request val) {
        CPYF(type);
        CPYF(sequence);
        CPYF(signal);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_wait_vblank_reply") FEX_ANNOTATE("fex-match") fex_drm_wait_vblank_reply {
      enum drm_vblank_seq_type type;
      uint32_t sequence;
      int32_t tval_sec;
      int32_t tval_usec;

      fex_drm_wait_vblank_reply() = delete;

      operator drm_wait_vblank_reply() const {
        drm_wait_vblank_reply val{};
        CPYT(type);
        CPYT(sequence);
        CPYT(tval_sec);
        CPYT(tval_usec);
        return val;
      }

      fex_drm_wait_vblank_reply(struct drm_wait_vblank_reply val) {
        CPYF(type);
        CPYF(sequence);
        CPYF(tval_sec);
        CPYF(tval_usec);
      }
    };

    union FEX_ANNOTATE("alias-x86_32-drm_wait_vblank") FEX_ANNOTATE("fex-match") fex_drm_wait_vblank {
      fex_drm_wait_vblank_request request;
      fex_drm_wait_vblank_reply reply;

      fex_drm_wait_vblank() = delete;
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_update_draw") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_update_draw {
      drm_drawable_t handle;
      uint32_t type;
      uint32_t num;
      compat_uint64_t data;

      fex_drm_update_draw() = delete;

      operator drm_update_draw() const {
        drm_update_draw val{};
        CPYT(handle);
        CPYT(type);
        CPYT(num);
        CPYT(data);
        return val;
      }

      fex_drm_update_draw(struct drm_update_draw val) {
        CPYF(handle);
        CPYF(type);
        CPYF(num);
        CPYF(data);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_mode_get_plane_res") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_mode_get_plane_res {
      compat_uint64_t plane_id_ptr;
      uint32_t count_planes;
      fex_drm_mode_get_plane_res() = delete;

      operator drm_mode_get_plane_res() const {
        drm_mode_get_plane_res val{};
        CPYT(plane_id_ptr);
        CPYT(count_planes);
        return val;
      }

      fex_drm_mode_get_plane_res(struct drm_mode_get_plane_res val) {
        CPYF(plane_id_ptr);
        CPYF(count_planes);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_mode_fb_cmd2") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_mode_fb_cmd2 {
      uint32_t fb_id;
      uint32_t width;
      uint32_t height;
      uint32_t pixel_format;
      uint32_t flags;

      uint32_t handles[4];
      uint32_t pitches[4];
      uint32_t offsets[4];
      compat_uint64_t modifier[4];
      fex_drm_mode_fb_cmd2() = delete;

      operator drm_mode_fb_cmd2() const {
        drm_mode_fb_cmd2 val{};
        CPYT(fb_id);
        CPYT(width);
        CPYT(height);
        CPYT(pixel_format);
        CPYT(flags);
        for (int i = 0; i < 4; ++i) {
          CPYT(handles[i]);
          CPYT(pitches[i]);
          CPYT(offsets[i]);
          CPYT(modifier[i]);
        }
        return val;
      }

      fex_drm_mode_fb_cmd2(struct drm_mode_fb_cmd2 val) {
        CPYF(fb_id);
        CPYF(width);
        CPYF(height);
        CPYF(pixel_format);
        CPYF(flags);
        for (int i = 0; i < 4; ++i) {
          CPYF(handles[i]);
          CPYF(pitches[i]);
          CPYF(offsets[i]);
          CPYF(modifier[i]);
        }
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_mode_obj_get_properties") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_mode_obj_get_properties {
      compat_uint64_t props_ptr;
      compat_uint64_t prop_values_ptr;
      uint32_t count_props;
      uint32_t obj_id;
      uint32_t obj_type;

      fex_drm_mode_obj_get_properties() = delete;

      operator drm_mode_obj_get_properties() const {
        drm_mode_obj_get_properties val{};
        val.props_ptr = props_ptr;
        val.prop_values_ptr = prop_values_ptr;
        val.count_props = count_props;
        val.obj_id = obj_id;
        val.obj_type = obj_type;
        return val;
      }

      fex_drm_mode_obj_get_properties(struct drm_mode_obj_get_properties val) {
        props_ptr = val.props_ptr;
        prop_values_ptr = val.prop_values_ptr;
        count_props = val.count_props;
        obj_type = val.obj_type;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_mode_obj_set_property") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_mode_obj_set_property {
      compat_uint64_t value;
      uint32_t prop_id;
      uint32_t obj_id;
      uint32_t obj_type;

      fex_drm_mode_obj_set_property() = delete;

      operator drm_mode_obj_set_property() const {
        drm_mode_obj_set_property val{};
        val.value = value;
        val.prop_id = prop_id;
        val.obj_id = obj_id;
        val.obj_type = obj_type;
        return val;
      }

      fex_drm_mode_obj_set_property(struct drm_mode_obj_set_property val) {
        value = val.value;
        prop_id = val.prop_id;
        obj_id = val.obj_id;
        obj_type = val.obj_type;
      }
    };

  }

  namespace AMDGPU {
    struct FEX_ANNOTATE("alias-x86_32-drm_amdgpu_gem_metadata") FEX_ANNOTATE("fex-match") fex_drm_amdgpu_gem_metadata {
      __u32 handle;
      __u32 op;
      struct {
        compat_uint64_t flags;
        compat_uint64_t tiling_info;
        __u32 data_size_bytes;
        __u32 data[64];
      } data;

      fex_drm_amdgpu_gem_metadata() = delete;
      operator drm_amdgpu_gem_metadata() const {
        drm_amdgpu_gem_metadata val{};
        val.handle = handle;
        val.op = op;
        val.data.flags = data.flags;
        val.data.tiling_info = data.tiling_info;
        val.data.data_size_bytes = data.data_size_bytes;
        memcpy(val.data.data, data.data, sizeof(data.data));
        return val;
      }

      fex_drm_amdgpu_gem_metadata(struct drm_amdgpu_gem_metadata val) {
        handle = val.handle;
        op = val.op;
        data.flags = val.data.flags;
        data.tiling_info = val.data.tiling_info;
        data.data_size_bytes = val.data.data_size_bytes;
        memcpy(data.data, val.data.data, sizeof(data.data));
      }
    };
  }

  namespace RADEON {
    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_gem_create") FEX_ANNOTATE("fex-match") fex_drm_radeon_gem_create {
      compat_uint64_t size;
      compat_uint64_t alignment;
      __u32 handle;
      __u32 initial_domain;
      __u32 flags;

      fex_drm_radeon_gem_create() = delete;

      operator drm_radeon_gem_create() const {
        drm_radeon_gem_create val{};
        val.size = size;
        val.alignment = alignment;
        val.handle = handle;
        val.initial_domain = initial_domain;
        val.flags = flags;
        return val;
      }

      fex_drm_radeon_gem_create(struct drm_radeon_gem_create val) {
        size = val.size;
        alignment = val.alignment;
        handle = val.handle;
        initial_domain = val.initial_domain;
        flags = val.flags;
      }
    };

    struct FEX_PACKED FEX_ANNOTATE("alias-x86_32-drm_radeon_init") FEX_ANNOTATE("fex-match") fex_drm_radeon_init_t {
      enum {} func;

      compat_ulong_t sarea_priv_offset;
      int32_t is_pci;
      int32_t cp_mode;
      int32_t gart_size;
      int32_t ring_size;
      int32_t usec_timeout;

      uint32_t fb_bpp;
      uint32_t front_offset, front_pitch;
      uint32_t back_offset, back_pitch;
      uint32_t depth_bpp;
      uint32_t depth_offset, depth_pitch;

      compat_ulong_t fb_offset;
      compat_ulong_t mmio_offset;
      compat_ulong_t ring_offset;
      compat_ulong_t ring_rptr_offset;
      compat_ulong_t buffers_offset;
      compat_ulong_t gart_textures_offset;

      fex_drm_radeon_init_t() = delete;

      operator drm_radeon_init_t() const {
        drm_radeon_init_t val{};
        val.sarea_priv_offset = sarea_priv_offset;
        val.is_pci = is_pci;
        val.cp_mode = cp_mode;
        val.gart_size = gart_size;
        val.ring_size = ring_size;
        val.usec_timeout = usec_timeout;
        val.fb_bpp = fb_bpp;
        val.front_offset = front_offset;
        val.front_pitch = front_pitch;
        val.back_offset = back_offset;
        val.back_pitch = back_pitch;
        val.depth_bpp = depth_bpp;
        val.depth_offset = depth_offset;
        val.depth_pitch = depth_pitch;
        val.fb_offset = fb_offset;
        val.mmio_offset = mmio_offset;
        val.ring_offset = ring_offset;
        val.ring_rptr_offset = ring_rptr_offset;
        val.buffers_offset = buffers_offset;
        val.gart_textures_offset = gart_textures_offset;
        return val;
      }

      fex_drm_radeon_init_t(drm_radeon_init_t val) {
        sarea_priv_offset = val.sarea_priv_offset;
        is_pci = val.is_pci;
        cp_mode = val.cp_mode;
        gart_size = val.gart_size;
        ring_size = val.ring_size;
        usec_timeout = val.usec_timeout;
        fb_bpp = val.fb_bpp;
        front_offset = val.front_offset;
        front_pitch = val.front_pitch;
        back_offset = val.back_offset;
        back_pitch = val.back_pitch;
        depth_bpp = val.depth_bpp;
        depth_offset = val.depth_offset;
        depth_pitch = val.depth_pitch;
        fb_offset = val.fb_offset;
        mmio_offset = val.mmio_offset;
        ring_offset = val.ring_offset;
        ring_rptr_offset = val.ring_rptr_offset;
        buffers_offset = val.buffers_offset;
        gart_textures_offset = val.gart_textures_offset;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_clear") FEX_ANNOTATE("fex-match") fex_drm_radeon_clear_t {
      uint32_t flags;
      uint32_t clear_color;
      uint32_t clear_depth;
      uint32_t color_mask;
      uint32_t depth_mask;
      compat_ptr<drm_radeon_clear_rect_t> depth_boxes;

      fex_drm_radeon_clear_t() = delete;

      operator drm_radeon_clear_t() const {
        drm_radeon_clear_t val{};
        val.flags = flags;
        val.clear_color = clear_color;
        val.clear_depth = clear_depth;
        val.color_mask = color_mask;
        val.depth_mask = depth_mask;
        val.depth_boxes = depth_boxes;
        return val;
      }

      fex_drm_radeon_clear_t(drm_radeon_clear_t val) : depth_boxes{val.depth_boxes} {
        flags = val.flags;
        clear_color = val.clear_color;
        clear_depth = val.clear_depth;
        color_mask = val.color_mask;
        depth_mask = val.depth_mask;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_stipple") FEX_ANNOTATE("fex-match") fex_drm_radeon_stipple_t {
      compat_ptr<uint32_t> mask;

      fex_drm_radeon_stipple_t() = delete;

      operator drm_radeon_stipple_t() const {
        drm_radeon_stipple_t val{};
        val.mask = mask;
        return val;
      }

      fex_drm_radeon_stipple_t(drm_radeon_stipple_t val) : mask{val.mask} {}
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_texture") FEX_ANNOTATE("fex-match") fex_drm_radeon_texture_t {
      uint32_t offset;
      int32_t pitch;
      int32_t format;
      int32_t width;
      int32_t height;
      compat_ptr<drm_radeon_tex_image_t> image;

      fex_drm_radeon_texture_t() = delete;

      operator drm_radeon_texture_t() const {
        drm_radeon_texture_t val{};
        val.offset = offset;
        val.pitch = pitch;
        val.format = format;
        val.width = width;
        val.height = height;
        val.image = image;
        return val;
      }

      fex_drm_radeon_texture_t(drm_radeon_texture_t val) : image{val.image} {
        offset = val.offset;
        pitch = val.pitch;
        format = val.format;
        width = val.width;
        height = val.height;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_vertex2") FEX_ANNOTATE("fex-match") fex_drm_radeon_vertex2_t {
      int32_t idx;
      int32_t discard;
      int32_t nr_states;
      compat_ptr<drm_radeon_state_t> state;
      int32_t nr_prims;
      compat_ptr<drm_radeon_prim_t> prim;

      fex_drm_radeon_vertex2_t() = delete;

      operator drm_radeon_vertex2_t() const {
        drm_radeon_vertex2_t val;
        val.idx = idx;
        val.discard = discard;
        val.nr_states = nr_states;
        val.state = state;
        val.nr_prims = nr_prims;
        val.prim = prim;
        return val;
      }

      fex_drm_radeon_vertex2_t(drm_radeon_vertex2_t val) : state{val.state}, prim{val.prim} {
        idx = val.idx;
        discard = val.discard;
        nr_states = val.nr_states;
        nr_prims = val.nr_prims;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_cmd_buffer") FEX_ANNOTATE("fex-match") fex_drm_radeon_cmd_buffer_t {
      int32_t bufsz;
      compat_ptr<char> buf;
      int32_t nbox;
      compat_ptr<drm_clip_rect> boxes;

      fex_drm_radeon_cmd_buffer_t() = delete;

      operator drm_radeon_cmd_buffer_t() const {
        drm_radeon_cmd_buffer_t val;
        val.bufsz = bufsz;
        val.buf = buf;
        val.nbox = nbox;
        val.boxes = boxes;
        return val;
      }

      fex_drm_radeon_cmd_buffer_t(drm_radeon_cmd_buffer_t val) : buf{val.buf}, boxes{val.boxes} {
        val.bufsz = bufsz;
        val.nbox = nbox;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_getparam") FEX_ANNOTATE("fex-match") fex_drm_radeon_getparam_t {
      int32_t param;
      compat_ptr<void> value;

      fex_drm_radeon_getparam_t() = delete;

      operator drm_radeon_getparam_t() const {
        drm_radeon_getparam_t val;
        val.param = param;
        val.value = value;
        return val;
      }

      fex_drm_radeon_getparam_t(drm_radeon_getparam_t val) : value{val.value} { val.param = param; }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_mem_alloc") FEX_ANNOTATE("fex-match") fex_drm_radeon_mem_alloc_t {
      int32_t region;
      int32_t alignment;
      int32_t size;
      compat_ptr<int32_t> region_offset;

      fex_drm_radeon_mem_alloc_t() = delete;

      operator drm_radeon_mem_alloc_t() const {
        drm_radeon_mem_alloc_t val;
        val.region = region;
        val.alignment = alignment;
        val.size = size;
        val.region_offset = region_offset;
        return val;
      }

      fex_drm_radeon_mem_alloc_t(drm_radeon_mem_alloc_t val) : region_offset{val.region_offset} {
        val.region = region;
        val.alignment = alignment;
        val.size = size;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_irq_emit") FEX_ANNOTATE("fex-match") fex_drm_radeon_irq_emit_t {
      compat_ptr<int32_t> irq_seq;

      fex_drm_radeon_irq_emit_t() = delete;

      operator drm_radeon_irq_emit_t() const {
        drm_radeon_irq_emit_t val;
        val.irq_seq = irq_seq;
        return val;
      }

      fex_drm_radeon_irq_emit_t(drm_radeon_irq_emit_t val) : irq_seq{val.irq_seq} {}
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_radeon_setparam") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_radeon_setparam_t {
      uint32_t param;
      compat_int64_t value;

      fex_drm_radeon_setparam_t() = delete;

      operator drm_radeon_setparam_t() const {
        drm_radeon_setparam_t val;
        val.param = param;
        val.value = value;
        return val;
      }

      fex_drm_radeon_setparam_t(drm_radeon_setparam_t val) {
        param = val.param;
        value = val.value;
      }
    };

  }

  namespace MSM {
    struct FEX_ANNOTATE("alias-x86_32-drm_msm_timespec") FEX_ANNOTATE("fex-match") fex_drm_msm_timespec {
      compat_int64_t tv_sec;
      compat_int64_t tv_nsec;

      fex_drm_msm_timespec() = delete;
      operator drm_msm_timespec() const {
        drm_msm_timespec val{};
        val.tv_sec = tv_sec;
        val.tv_nsec = tv_nsec;
        return val;
      }

      fex_drm_msm_timespec(struct drm_msm_timespec val) {
        tv_sec = val.tv_sec;
        tv_nsec = val.tv_nsec;
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_msm_wait_fence") FEX_ANNOTATE("fex-match") FEX_PACKED fex_drm_msm_wait_fence {
      uint32_t fence;
      uint32_t flags;
      struct fex_drm_msm_timespec timeout;
      uint32_t queueid;

      fex_drm_msm_wait_fence() = delete;

      operator drm_msm_wait_fence() const {
        drm_msm_wait_fence val{};
        val.fence = fence;
        val.flags = flags;
        val.timeout = timeout;
        val.queueid = queueid;
        return val;
      }

      fex_drm_msm_wait_fence(struct drm_msm_wait_fence val) : timeout{val.timeout} {
        fence = val.fence;
        flags = val.flags;
        queueid = val.queueid;
      }
    };

  }

  namespace I915 {

    struct FEX_ANNOTATE("alias-x86_32-drm_i915_batchbuffer") FEX_ANNOTATE("fex-match") fex_drm_i915_batchbuffer_t {
      int32_t start;
      int32_t used;
      int32_t DR1;
      int32_t DR4;
      int32_t num_cliprects;
      compat_ptr<struct drm_clip_rect> cliprects;

      fex_drm_i915_batchbuffer_t() = delete;

      operator drm_i915_batchbuffer_t() const {
        drm_i915_batchbuffer_t val{};
        CPYT(start);
        CPYT(used);
        CPYT(DR1);
        CPYT(DR4);
        CPYT(num_cliprects);
        CPYT(cliprects);
        return val;
      }

      fex_drm_i915_batchbuffer_t(drm_i915_batchbuffer_t val) : cliprects{val.cliprects} {
        CPYF(start);
        CPYF(used);
        CPYF(DR1);
        CPYF(DR4);
        CPYF(num_cliprects);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_i915_irq_emit") FEX_ANNOTATE("fex-match") fex_drm_i915_irq_emit_t {
      compat_ptr<int> irq_seq;

      fex_drm_i915_irq_emit_t() = delete;

      operator drm_i915_irq_emit_t() const {
        drm_i915_irq_emit_t val{};
        CPYT(irq_seq);
        return val;
      }

      fex_drm_i915_irq_emit_t(drm_i915_irq_emit_t val) : irq_seq{val.irq_seq} {}
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_i915_getparam") FEX_ANNOTATE("fex-match") fex_drm_i915_getparam_t {
      int32_t param;
      compat_ptr<int> value;
      fex_drm_i915_getparam_t() = delete;

      operator drm_i915_getparam_t() const {
        drm_i915_getparam_t val{};
        CPYT(param);
        CPYT(value);
        return val;
      }

      fex_drm_i915_getparam_t(drm_i915_getparam_t val) : value{val.value} { CPYF(param); }
    };

    struct FEX_ANNOTATE("alias-x86_32-drm_i915_mem_alloc") FEX_ANNOTATE("fex-match") fex_drm_i915_mem_alloc_t {
      int32_t region;
      int32_t alignment;
      int32_t size;
      compat_ptr<int> region_offset;
      fex_drm_i915_mem_alloc_t() = delete;

      operator drm_i915_mem_alloc_t() const {
        drm_i915_mem_alloc_t val{};
        CPYT(region);
        CPYT(alignment);
        CPYT(size);
        CPYT(region_offset);
        return val;
      }

      fex_drm_i915_mem_alloc_t(drm_i915_mem_alloc_t val) : region_offset{val.region_offset} {
        CPYT(region);
        CPYT(alignment);
        CPYT(size);
      }
    };

    struct FEX_ANNOTATE("alias-x86_32-_drm_i915_cmdbuffer") FEX_ANNOTATE("fex-match") fex_drm_i915_cmdbuffer_t {
      compat_ptr<char> buf;
      int32_t sz;
      int32_t DR1;
      int32_t DR4;
      int32_t num_cliprects;
      compat_ptr<struct drm_clip_rect> cliprects;

      fex_drm_i915_cmdbuffer_t() = delete;

      operator drm_i915_cmdbuffer_t() const {
        drm_i915_cmdbuffer_t val{};
        CPYT(buf);
        CPYT(sz);
        CPYT(DR1);
        CPYT(DR4);
        CPYT(num_cliprects);
        CPYT(cliprects);
        return val;
      }

      fex_drm_i915_cmdbuffer_t(drm_i915_cmdbuffer_t val) : buf{val.buf}, cliprects{val.cliprects} {
        CPYT(sz);
        CPYT(DR1);
        CPYT(DR4);
        CPYT(num_cliprects);
      }
    };

// I915 defines if they don't exist
// Older DRM doesn't have this
#ifndef DRM_IOCTL_I915_GEM_MMAP_OFFSET
    struct drm_i915_gem_mmap_offset {
      uint32_t handle;
      uint32_t pad;
      compat_uint64_t offset;
      compat_uint64_t flags;
      compat_uint64_t extensions;
    };

#define DRM_IOCTL_I915_GEM_MMAP_OFFSET DRM_IOWR(DRM_COMMAND_BASE + DRM_I915_GEM_MMAP_GTT, FEX::HLE::x32::I915::drm_i915_gem_mmap_offset)
#endif
  }

  namespace VC4 {
    struct FEX_ANNOTATE("alias-x86_32-drm_vc4_perfmon_get_values") FEX_ANNOTATE("fex-match") fex_drm_vc4_perfmon_get_values {
      uint32_t id;
      compat_uint64_t values_ptr;

      fex_drm_vc4_perfmon_get_values() = delete;

      operator drm_vc4_perfmon_get_values() const {
        drm_vc4_perfmon_get_values val{};
        val.id = id;
        val.values_ptr = values_ptr;
        return val;
      }
      fex_drm_vc4_perfmon_get_values(drm_vc4_perfmon_get_values val) {
        id = val.id;
        values_ptr = val.values_ptr;
      }
    };

  }

  namespace V3D {
    struct FEX_ANNOTATE("alias-x86_32-drm_v3d_submit_csd") FEX_ANNOTATE("fex-match") fex_drm_v3d_submit_csd {
      uint32_t cfg[7];
      uint32_t coef[4];

      compat_uint64_t bo_handles;

      uint32_t bo_handle_count;

      uint32_t in_sync;

      uint32_t out_sync;

      /**
   * @name This member were added in Linux 5.15
   * Commit: 26a4dc29b74a137f45665089f6d3d633fcc9b662
   *
   * As far as I can tell this is an ABI break, Probably safe since this likely would have been padded to 8 bytes.
   * Still pretty sketchy.
   * @{ */

      uint32_t perfmon_id;
      /**  @} */

      /**
   * @name These members were added in Linux 5.17
   * Commit: bb3425efdcd99f2b4e608e850226f7107b2f993e
   * This added additional members to `drm_v3d_submit_cl` and `drm_v3d_submit_tfu` as well.
   *
   * As far as I can tell this is an ABI break for the `submit_tfu` and `submit_csd` structs.
   * `submit_cl` is safe because it it already had a flags member.
   *
   * We just need to eat the fact that if the userspace isn't compiled against Linux 5.17 headers
   * that copying this member may cause faults that we can't capture currently.
   * @{ */

      compat_uint64_t extensions;

      uint32_t flags;

      uint32_t pad;
      /**  @} */

      fex_drm_v3d_submit_csd() = default;

      operator drm_v3d_submit_csd() const {
        drm_v3d_submit_csd val{};
        memcpy(val.cfg, cfg, sizeof(cfg));
        memcpy(val.coef, coef, sizeof(coef));
        val.bo_handles = bo_handles;
        val.bo_handle_count = bo_handle_count;
        val.in_sync = in_sync;
        val.out_sync = out_sync;
        val.perfmon_id = perfmon_id;
        val.extensions = extensions;
        val.flags = flags;
        val.pad = pad;
        return val;
      }

      static void SafeConvertToGuest(fex_drm_v3d_submit_csd *Result, drm_v3d_submit_csd Src, size_t IoctlSize) {
        // We need to be more careful since this API changes over time
        fex_drm_v3d_submit_csd Tmp = Src;
        memcpy(Result, &Tmp, IoctlSize);
      }

      static drm_v3d_submit_csd SafeConvertToHost(fex_drm_v3d_submit_csd *Src, size_t IoctlSize) {
        // We need to be more careful since this API changes over time
        drm_v3d_submit_csd Result{};

        // Copy the incoming variable over with memcpy
        // This way if it is smaller than expected we will zero the remaining struct
        fex_drm_v3d_submit_csd Tmp{};
        memcpy(&Tmp, Src, std::min(IoctlSize, sizeof(fex_drm_v3d_submit_csd)));

        memcpy(Result.cfg, Tmp.cfg, sizeof(cfg));
        memcpy(Result.coef, Tmp.coef, sizeof(coef));
        Result.bo_handles = Tmp.bo_handles;
        Result.bo_handle_count = Tmp.bo_handle_count;
        Result.in_sync = Tmp.in_sync;
        Result.out_sync = Tmp.out_sync;
        Result.perfmon_id = Tmp.perfmon_id;
        Result.extensions = Tmp.extensions;
        Result.flags = Tmp.flags;
        Result.pad = Tmp.pad;

        return Result;
      }

      fex_drm_v3d_submit_csd(drm_v3d_submit_csd val) {
        memcpy(cfg, val.cfg, sizeof(cfg));
        memcpy(coef, val.coef, sizeof(coef));
        bo_handles = val.bo_handles;
        bo_handle_count = val.bo_handle_count;
        in_sync = val.in_sync;
        out_sync = val.out_sync;
        perfmon_id = val.perfmon_id;
        extensions = val.extensions;
        flags = val.flags;
        pad = val.pad;
      }
    };

  }

#include "LinuxSyscalls/x32/Ioctl/drm.inl"
#include "LinuxSyscalls/x32/Ioctl/amdgpu_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/msm_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/i915_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/lima_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/panfrost_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/nouveau_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/radeon_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/vc4_drm.inl"
#include "LinuxSyscalls/x32/Ioctl/v3d_drm.inl"

}
#undef CPYT
#undef CPYF
