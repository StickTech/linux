/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef __MOCK_REGION_H
#define __MOCK_REGION_H

#include <linaos/types.h>

struct drm_i915_private;
struct intel_memory_region;

struct intel_memory_region *
mock_region_create(struct drm_i915_private *i915,
		   resource_size_t start,
		   resource_size_t size,
		   resource_size_t min_page_size,
		   resource_size_t io_start);

#endif /* !__MOCK_REGION_H */
