/* SPDX-License-Identifier: GPL-2.0 */
#include <linaos/reboot.h>
#include <linaos/amba/serial.h>
extern struct amba_pl010_data ap_uart_data;
void integrator_init_early(void);
int integrator_init(bool is_cp);
void integrator_reserve(void);
