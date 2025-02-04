// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * arch/powerpc/platforms/83xx/mpc834x_itx.c
 *
 * MPC834x ITX board specific routines
 *
 * Maintainer: Kumar Gala <galak@kernel.crashing.org>
 */

#include <linaos/stddef.h>
#include <linaos/kernel.h>
#include <linaos/init.h>
#include <linaos/errno.h>
#include <linaos/reboot.h>
#include <linaos/pci.h>
#include <linaos/kdev_t.h>
#include <linaos/major.h>
#include <linaos/console.h>
#include <linaos/delay.h>
#include <linaos/seq_file.h>
#include <linaos/root_dev.h>
#include <linaos/of_platform.h>

#include <linaos/atomic.h>
#include <asm/time.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/ipic.h>
#include <asm/irq.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>

#include "mpc83xx.h"

static const struct of_device_id mpc834x_itx_ids[] __initconst = {
	{ .compatible = "fsl,pq2pro-localbus", },
	{},
};

static int __init mpc834x_itx_declare_of_platform_devices(void)
{
	mpc83xx_declare_of_platform_devices();
	return of_platform_bus_probe(NULL, mpc834x_itx_ids, NULL);
}
machine_device_initcall(mpc834x_itx, mpc834x_itx_declare_of_platform_devices);

/* ************************************************************************
 *
 * Setup the architecture
 *
 */
static void __init mpc834x_itx_setup_arch(void)
{
	mpc83xx_setup_arch();

	mpc834x_usb_cfg();
}

/*
 * Called very early, MMU is off, device-tree isn't unflattened
 */
static int __init mpc834x_itx_probe(void)
{
	return of_machine_is_compatible("MPC834xMITX");
}

define_machine(mpc834x_itx) {
	.name			= "MPC834x ITX",
	.probe			= mpc834x_itx_probe,
	.setup_arch		= mpc834x_itx_setup_arch,
	.discover_phbs  	= mpc83xx_setup_pci,
	.init_IRQ		= mpc83xx_ipic_init_IRQ,
	.get_irq		= ipic_get_irq,
	.restart		= mpc83xx_restart,
	.time_init		= mpc83xx_time_init,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};
