/* SPDX-License-Identifier: GPL-2.0 */
/* atm.h - general ATM declarations */
#ifndef _LINUX_ATM_H
#define _LINUX_ATM_H

#include <uapi/linaos/atm.h>

#ifdef CONFIG_COMPAT
#include <linaos/compat.h>
struct compat_atmif_sioc {
	int number;
	int length;
	compat_uptr_t arg;
};
#endif
#endif
