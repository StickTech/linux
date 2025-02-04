/**************************************************************************
 *
 * Copyright (c) 2006-2007 Tungsten Graphics, Inc., Cedar Park, TX., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/
/*
 * Authors: Thomas Hellström <thomas-at-tungstengraphics-dot-com>
 */

#include <linaos/export.h>
#include <linaos/highmem.h>
#include <linaos/mem_encrypt.h>
#include <xen/xen.h>

#include <drm/drm_cache.h>

#if defined(CONFIG_X86)
#include <asm/smp.h>

/*
 * clflushopt is an unordered instruction which needs fencing with mfence or
 * sfence to avoid ordering issues.  For drm_clflush_page this fencing happens
 * in the caller.
 */
static void
drm_clflush_page(struct page *page)
{
	uint8_t *page_virtual;
	unsigned int i;
	const int size = boot_cpu_data.x86_clflush_size;

	if (unlikely(page == NULL))
		return;

	page_virtual = kmap_atomic(page);
	for (i = 0; i < PAGE_SIZE; i += size)
		clflushopt(page_virtual + i);
	kunmap_atomic(page_virtual);
}

static void drm_cache_flush_clflush(struct page *pages[],
				    unsigned long num_pages)
{
	unsigned long i;

	mb(); /*Full memory barrier used before so that CLFLUSH is ordered*/
	for (i = 0; i < num_pages; i++)
		drm_clflush_page(*pages++);
	mb(); /*Also used after CLFLUSH so that all cache is flushed*/
}
#endif

/**
 * drm_clflush_pages - Flush dcache lines of a set of pages.
 * @pages: List of pages to be flushed.
 * @num_pages: Number of pages in the array.
 *
 * Flush every data cache line entry that points to an address belonging
 * to a page in the array.
 */
void
drm_clflush_pages(struct page *pages[], unsigned long num_pages)
{

#if defined(CONFIG_X86)
	if (static_cpu_has(X86_FEATURE_CLFLUSH)) {
		drm_cache_flush_clflush(pages, num_pages);
		return;
	}

	if (wbinvd_on_all_cpus())
		pr_err("Timed out waiting for cache flush\n");

#elif defined(__powerpc__)
	unsigned long i;

	for (i = 0; i < num_pages; i++) {
		struct page *page = pages[i];
		void *page_virtual;

		if (unlikely(page == NULL))
			continue;

		page_virtual = kmap_atomic(page);
		flush_dcache_range((unsigned long)page_virtual,
				   (unsigned long)page_virtual + PAGE_SIZE);
		kunmap_atomic(page_virtual);
	}
#else
	pr_err("Architecture has no drm_cache.c support\n");
	WARN_ON_ONCE(1);
#endif
}
EXPORT_SYMBOL(drm_clflush_pages);

/**
 * drm_clflush_sg - Flush dcache lines pointing to a scather-gather.
 * @st: struct sg_table.
 *
 * Flush every data cache line entry that points to an address in the
 * sg.
 */
void
drm_clflush_sg(struct sg_table *st)
{
#if defined(CONFIG_X86)
	if (static_cpu_has(X86_FEATURE_CLFLUSH)) {
		struct sg_page_iter sg_iter;

		mb(); /*CLFLUSH is ordered only by using memory barriers*/
		for_each_sgtable_page(st, &sg_iter, 0)
			drm_clflush_page(sg_page_iter_page(&sg_iter));
		mb(); /*Make sure that all cache line entry is flushed*/

		return;
	}

	if (wbinvd_on_all_cpus())
		pr_err("Timed out waiting for cache flush\n");
#else
	pr_err("Architecture has no drm_cache.c support\n");
	WARN_ON_ONCE(1);
#endif
}
EXPORT_SYMBOL(drm_clflush_sg);

/**
 * drm_clflush_virt_range - Flush dcache lines of a region
 * @addr: Initial kernel memory address.
 * @length: Region size.
 *
 * Flush every data cache line entry that points to an address in the
 * region requested.
 */
void
drm_clflush_virt_range(void *addr, unsigned long length)
{
#if defined(CONFIG_X86)
	if (static_cpu_has(X86_FEATURE_CLFLUSH)) {
		const int size = boot_cpu_data.x86_clflush_size;
		void *end = addr + length;

		addr = (void *)(((unsigned long)addr) & -size);
		mb(); /*CLFLUSH is only ordered with a full memory barrier*/
		for (; addr < end; addr += size)
			clflushopt(addr);
		clflushopt(end - 1); /* force serialisation */
		mb(); /*Ensure that evry data cache line entry is flushed*/
		return;
	}

	if (wbinvd_on_all_cpus())
		pr_err("Timed out waiting for cache flush\n");
#else
	pr_err("Architecture has no drm_cache.c support\n");
	WARN_ON_ONCE(1);
#endif
}
EXPORT_SYMBOL(drm_clflush_virt_range);

bool drm_need_swiotlb(int dma_bits)
{
	struct resource *tmp;
	resource_size_t max_iomem = 0;

	/*
	 * Xen paravirtual hosts require swiotlb regardless of requested dma
	 * transfer size.
	 *
	 * NOTE: Really, what it requires is use of the dma_alloc_coherent
	 *       allocator used in ttm_dma_populate() instead of
	 *       ttm_populate_and_map_pages(), which bounce buffers so much in
	 *       Xen it leads to swiotlb buffer exhaustion.
	 */
	if (xen_pv_domain())
		return true;

	/*
	 * Enforce dma_alloc_coherent when memory encryption is active as well
	 * for the same reasons as for Xen paravirtual hosts.
	 */
	if (mem_encrypt_active())
		return true;

	for (tmp = iomem_resource.child; tmp; tmp = tmp->sibling)
		max_iomem = max(max_iomem,  tmp->end);

	return max_iomem > ((u64)1 << dma_bits);
}
EXPORT_SYMBOL(drm_need_swiotlb);
