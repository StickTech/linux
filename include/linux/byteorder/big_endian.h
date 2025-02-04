/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_BYTEORDER_BIG_ENDIAN_H
#define _LINUX_BYTEORDER_BIG_ENDIAN_H

#include <uapi/linaos/byteorder/big_endian.h>

#ifndef CONFIG_CPU_BIG_ENDIAN
#warning inconsistent configuration, needs CONFIG_CPU_BIG_ENDIAN
#endif

#include <linaos/byteorder/generic.h>
#endif /* _LINUX_BYTEORDER_BIG_ENDIAN_H */
