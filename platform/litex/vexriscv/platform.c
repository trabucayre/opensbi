/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Dolu1990 <charles.papon.90@gmail.com>
 *
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_platform.h>
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/uart8250.h>
#include <sbi_utils/sys/clint.h>


/* clang-format off */

#define VEX_HART_COUNT			4
#define VEX_PLATFORM_FEATURES  \
	(SBI_PLATFORM_HAS_TIMER_VALUE | SBI_PLATFORM_HAS_MFAULTS_DELEGATION)

#define VEX_CLINT_ADDR 0xF0010000

#define VEX_HART_STACK_SIZE		8192



/* clang-format on */

static struct clint_data clint = {VEX_CLINT_ADDR, 0, VEX_HART_COUNT, true};

static int vex_final_init(bool cold_boot)
{
	return 0;
}

static u32 vex_pmp_region_count(u32 hartid)
{
	return 0;
}

static int vex_pmp_region_info(u32 hartid, u32 index, ulong *prot, ulong *addr,
				ulong *log2size)
{
	int ret = 0;

	switch (index) {
	default:
		ret = -1;
		break;
	};

	return ret;
}


extern void vex_putc(char ch);
extern int vex_getc(void);

static int vex_console_init(void)
{
	return 0;
}

static int vex_irqchip_init(bool cold_boot)
{
	return 0;
}

static int vex_ipi_init(bool cold_boot)
{
	int rc;

	if (cold_boot) {
		rc = clint_cold_ipi_init(&clint);
		if (rc)
			return rc;
	}

	return clint_warm_ipi_init();
}

static int vex_timer_init(bool cold_boot)
{
	int rc;
	if (cold_boot) {
		rc = clint_cold_timer_init(&clint, NULL); /* Timer has no reference */
		if (rc)
			return rc;
	}

	return clint_warm_timer_init();
}

static int vex_system_reset(u32 type)
{
	/* Tell the "finisher" that the simulation
	 * was successful so that QEMU exits
	 */

	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= vex_pmp_region_count,
	.pmp_region_info	= vex_pmp_region_info,
	.final_init		    = vex_final_init,
	.console_putc		= vex_putc,
	.console_getc		= vex_getc,
	.console_init		= vex_console_init,
	.irqchip_init		= vex_irqchip_init,
	.ipi_send		    = clint_ipi_send,
	.ipi_clear		    = clint_ipi_clear,
	.ipi_init		    = vex_ipi_init,
	.timer_value		= clint_timer_value,
	.timer_event_stop	= clint_timer_event_stop,
	.timer_event_start	= clint_timer_event_start,
	.timer_init			= vex_timer_init,
	.system_reset		= vex_system_reset
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			    = "LiteX/VexRiscv SMP",
	.features		    = VEX_PLATFORM_FEATURES,
	.hart_count		    = VEX_HART_COUNT,
	.hart_stack_size	= VEX_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};


