// SPDX-License-Identifier: GPL-2.0-only

#include <linaos/crash_core.h>
#include <linaos/pgtable.h>

#include <asm/setup.h>

void arch_crash_save_vmcoreinfo(void)
{
#ifdef CONFIG_NUMA
	VMCOREINFO_SYMBOL(node_data);
	VMCOREINFO_LENGTH(node_data, MAX_NUMNODES);
#endif
#ifdef CONFIG_X86_PAE
	VMCOREINFO_CONFIG(X86_PAE);
#endif
}
