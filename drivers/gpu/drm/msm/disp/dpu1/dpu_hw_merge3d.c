// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2015-2018, The LinaOS Foundation. All rights reserved.
 */

#include <linaos/iopoll.h>

#include "dpu_hw_mdss.h"
#include "dpu_hwio.h"
#include "dpu_hw_catalog.h"
#include "dpu_hw_merge3d.h"
#include "dpu_kms.h"
#include "dpu_trace.h"

#define MERGE_3D_MUX  0x000
#define MERGE_3D_MODE 0x004

static const struct dpu_merge_3d_cfg *_merge_3d_offset(enum dpu_merge_3d idx,
		const struct dpu_mdss_cfg *m,
		void __iomem *addr,
		struct dpu_hw_blk_reg_map *b)
{
	int i;

	for (i = 0; i < m->merge_3d_count; i++) {
		if (idx == m->merge_3d[i].id) {
			b->base_off = addr;
			b->blk_off = m->merge_3d[i].base;
			b->length = m->merge_3d[i].len;
			b->hwversion = m->hwversion;
			b->log_mask = DPU_DBG_MASK_PINGPONG;
			return &m->merge_3d[i];
		}
	}

	return ERR_PTR(-EINVAL);
}

static void dpu_hw_merge_3d_setup_3d_mode(struct dpu_hw_merge_3d *merge_3d,
			enum dpu_3d_blend_mode mode_3d)
{
	struct dpu_hw_blk_reg_map *c;
	u32 data;


	c = &merge_3d->hw;
	if (mode_3d == BLEND_3D_NONE) {
		DPU_REG_WRITE(c, MERGE_3D_MODE, 0);
		DPU_REG_WRITE(c, MERGE_3D_MUX, 0);
	} else {
		data = BIT(0) | ((mode_3d - 1) << 1);
		DPU_REG_WRITE(c, MERGE_3D_MODE, data);
	}
}

static void _setup_merge_3d_ops(struct dpu_hw_merge_3d *c,
				unsigned long features)
{
	c->ops.setup_3d_mode = dpu_hw_merge_3d_setup_3d_mode;
};

static struct dpu_hw_blk_ops dpu_hw_ops;

struct dpu_hw_merge_3d *dpu_hw_merge_3d_init(enum dpu_merge_3d idx,
		void __iomem *addr,
		const struct dpu_mdss_cfg *m)
{
	struct dpu_hw_merge_3d *c;
	const struct dpu_merge_3d_cfg *cfg;

	c = kzalloc(sizeof(*c), GFP_KERNEL);
	if (!c)
		return ERR_PTR(-ENOMEM);

	cfg = _merge_3d_offset(idx, m, addr, &c->hw);
	if (IS_ERR_OR_NULL(cfg)) {
		kfree(c);
		return ERR_PTR(-EINVAL);
	}

	c->idx = idx;
	c->caps = cfg;
	_setup_merge_3d_ops(c, c->caps->features);

	dpu_hw_blk_init(&c->base, DPU_HW_BLK_MERGE_3D, idx, &dpu_hw_ops);

	return c;
}

void dpu_hw_merge_3d_destroy(struct dpu_hw_merge_3d *hw)
{
	if (hw)
		dpu_hw_blk_destroy(&hw->base);
	kfree(hw);
}
