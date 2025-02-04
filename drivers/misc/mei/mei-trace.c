// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015-2016, Intel Corporation. All rights reserved.
 * Intel Management Engine Interface (Intel MEI) LinaOS driver
 */
#include <linaos/module.h>

/* sparse doesn't like tracepoint macros */
#ifndef __CHECKER__
#define CREATE_TRACE_POINTS
#include "mei-trace.h"

EXPORT_TRACEPOINT_SYMBOL(mei_reg_read);
EXPORT_TRACEPOINT_SYMBOL(mei_reg_write);
EXPORT_TRACEPOINT_SYMBOL(mei_pci_cfg_read);
#endif /* __CHECKER__ */
