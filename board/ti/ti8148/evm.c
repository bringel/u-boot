/*
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/cache.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <net.h>
#include <miiphy.h>
#include <netdev.h>

#define __raw_readl(a)		(*(volatile unsigned int *)(a))
#define __raw_writel(v, a)	(*(volatile unsigned int *)(a) = (v))
#define __raw_readw(a)		(*(volatile unsigned short *)(a))
#define __raw_writew(v, a)	(*(volatile unsigned short *)(a) = (v))

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SETUP_PLL
static void pll_config(u32, u32, u32, u32, u32);
static void pcie_pll_config(void);
static void sata_pll_config(void);
static void modena_pll_config(void);
static void l3_pll_config(void);
static void ddr_pll_config(void);
static void usb_pll_config(void);
#endif

static void unlock_pll_control_mmr(void);

/*
 * spinning delay to use before udelay works
 */
static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b" : "=r" (loops) : "0"(loops));
}

/*
 * Basic board specific setup
 */
int board_init(void)
{
	u32 regVal;

	/* Get Timer and UART out of reset */

	/* UART softreset */
	regVal = __raw_readl(UART_SYSCFG);
	regVal |= 0x2;
	__raw_writel(regVal, UART_SYSCFG);
	while( (__raw_readl(UART_SYSSTS) & 0x1) != 0x1);

	/* Disable smart idle */
	regVal = __raw_readl(UART_SYSCFG);
	regVal |= (1<<3);
	__raw_writel(regVal, UART_SYSCFG);

	/* mach type passed to kernel */
	gd->bd->bi_arch_number = MACH_TYPE_TI8148EVM;

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_DRAM_1 + 0x100;

	return 0;
}

/*
 * sets uboots idea of sdram size
 */
int dram_init(void)
{
	/* Fill up board info */
	gd->bd->bi_dram[0].start = PHYS_DRAM_1;
	gd->bd->bi_dram[0].size = PHYS_DRAM_1_SIZE;

	gd->bd->bi_dram[1].start = PHYS_DRAM_2;
	gd->bd->bi_dram[1].size = PHYS_DRAM_2_SIZE;

	return 0;
}


/*
 *  tell if GP/HS/EMU/TST
 */
u32 get_device_type(void)
{
	u32 mode;
	mode = __raw_readl(CONTROL_STATUS) & (DEVICE_MASK);
	return(mode >>= 8);
}

/*
 * return SYS_BOOT[4:0]
 */
u32 get_sysboot_value(void)
{
	u32 mode;
	mode = __raw_readl(CONTROL_STATUS) & (SYSBOOT_MASK);
	return mode;
}

int misc_init_r (void)
{
	return 0;
}

#ifdef CONFIG_TI814X_EVM_DDR2
/* assume delay is aprox at least 1us */
void ddr_delay(int d)
{
	 int i;
	/*
	* read a control module register.
	* this is a bit more delay and cannot be optimized by the compiler
	* assuming one read takes 200 cycles and A8 is runing 1 GHz
	* somewhat conservative setting
	*/
	for(i=0; i<50*d; i++)
		RD_MEM_32(CONTROL_STATUS);
}

static void ddr_init_settings(int emif)
{
	/* DLL Lockdiff */
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x028));
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x05C));
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x090));
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x138));
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x1DC));
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x280));
	__raw_writel(0xF, (DDRPHY_CONFIG_BASE + 0x324));

	/* setup rank delays */
	__raw_writel(0x1, (DDRPHY_CONFIG_BASE + 0x134));
	__raw_writel(0x1, (DDRPHY_CONFIG_BASE + 0x1D8));
	__raw_writel(0x1, (DDRPHY_CONFIG_BASE + 0x27C));
	__raw_writel(0x1, (DDRPHY_CONFIG_BASE + 0x320));


	__raw_writel(INVERT_CLK_OUT, (DDRPHY_CONFIG_BASE + 0x02C));	/* invert_clk_out cmd0 */
	__raw_writel(INVERT_CLK_OUT, (DDRPHY_CONFIG_BASE + 0x060));	/* invert_clk_out cmd0 */
	__raw_writel(INVERT_CLK_OUT, (DDRPHY_CONFIG_BASE + 0x094));	/* invert_clk_out cmd0 */


	__raw_writel(CMD_SLAVE_RATIO, (DDRPHY_CONFIG_BASE + 0x01C));	/* cmd0 slave ratio */
	__raw_writel(CMD_SLAVE_RATIO, (DDRPHY_CONFIG_BASE + 0x050));	/* cmd0 slave ratio */
	__raw_writel(CMD_SLAVE_RATIO, (DDRPHY_CONFIG_BASE + 0x084));	/* cmd0 slave ratio */

	__raw_writel(DQS_GATE_BYTE_LANE0, (DDRPHY_CONFIG_BASE + 0x108));
	__raw_writel(0x00000000, (DDRPHY_CONFIG_BASE + 0x10C));
	__raw_writel(DQS_GATE_BYTE_LANE1, (DDRPHY_CONFIG_BASE + 0x1AC));
	__raw_writel(0x00000000, (DDRPHY_CONFIG_BASE + 0x1B0));
	__raw_writel(DQS_GATE_BYTE_LANE2, (DDRPHY_CONFIG_BASE + 0x250));
	__raw_writel(0x00000000, (DDRPHY_CONFIG_BASE + 0x254));
	__raw_writel(DQS_GATE_BYTE_LANE3, (DDRPHY_CONFIG_BASE + 0x2F4));
	__raw_writel(0x00000000, (DDRPHY_CONFIG_BASE + 0x2F8));

	__raw_writel(WR_DQS_RATIO_BYTE_LANE0, (DDRPHY_CONFIG_BASE + 0x0DC));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x0E0));
	__raw_writel(WR_DQS_RATIO_BYTE_LANE1, (DDRPHY_CONFIG_BASE + 0x180));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x184));
	__raw_writel(WR_DQS_RATIO_BYTE_LANE2, (DDRPHY_CONFIG_BASE + 0x224));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x228));
	__raw_writel(WR_DQS_RATIO_BYTE_LANE3, (DDRPHY_CONFIG_BASE + 0x2C8));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x2CC));

	__raw_writel(WR_DATA_RATIO_BYTE_LANE0, (DDRPHY_CONFIG_BASE + 0x120));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x124));
	__raw_writel(WR_DATA_RATIO_BYTE_LANE1, (DDRPHY_CONFIG_BASE + 0x1C4));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x1C8));
	__raw_writel(WR_DATA_RATIO_BYTE_LANE2, (DDRPHY_CONFIG_BASE + 0x268));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x26C));
	__raw_writel(WR_DATA_RATIO_BYTE_LANE3, (DDRPHY_CONFIG_BASE + 0x30C));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x310));

	__raw_writel(RD_DQS_RATIO, (DDRPHY_CONFIG_BASE + 0x0C8));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x0CC));
	__raw_writel(RD_DQS_RATIO, (DDRPHY_CONFIG_BASE + 0x16C));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x170));
	__raw_writel(RD_DQS_RATIO, (DDRPHY_CONFIG_BASE + 0x210));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x214));
	__raw_writel(RD_DQS_RATIO, (DDRPHY_CONFIG_BASE + 0x2B4));
	__raw_writel(0x0, (DDRPHY_CONFIG_BASE + 0x2B8));

	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x00C));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x010));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x040));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x044));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x074));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x078));

	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x0A8));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x0AC));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x14C));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x150));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x1F0));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x1F4));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x294));
	__raw_writel(0x4, (DDRPHY_CONFIG_BASE + 0x298));

	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x338));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x340));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x348));
	__raw_writel(0x5, (DDRPHY_CONFIG_BASE + 0x350));

}

static void emif4p_init(u32 TIM1, u32 TIM2, u32 TIM3, u32 SDREF, u32 SDCFG, u32 RL)
{
	/*Program EMIF0 CFG Registers*/
	__raw_writel(TIM1, EMIF4_0_SDRAM_TIM_1);
	__raw_writel(TIM1, EMIF4_0_SDRAM_TIM_1_SHADOW);
	__raw_writel(TIM2, EMIF4_0_SDRAM_TIM_2);
	__raw_writel(TIM2, EMIF4_0_SDRAM_TIM_2_SHADOW);
	__raw_writel(TIM3, EMIF4_0_SDRAM_TIM_3);
	__raw_writel(TIM3, EMIF4_0_SDRAM_TIM_3_SHADOW);
	__raw_writel(SDCFG, EMIF4_0_SDRAM_CONFIG);
	//__raw_writel(SDREF, EMIF4_0_SDRAM_REF_CTRL);
	//__raw_writel(SDREF, EMIF4_0_SDRAM_REF_CTRL_SHADOW);
	__raw_writel(RL, EMIF4_0_DDR_PHY_CTRL_1);
	__raw_writel(RL, EMIF4_0_DDR_PHY_CTRL_1_SHADOW);

	if (CONFIG_TI816X_TWO_EMIF){
	__raw_writel(TIM1, EMIF4_1_SDRAM_TIM_1);
	__raw_writel(TIM1, EMIF4_1_SDRAM_TIM_1_SHADOW);
	__raw_writel(TIM2, EMIF4_1_SDRAM_TIM_2);
	__raw_writel(TIM2, EMIF4_1_SDRAM_TIM_2_SHADOW);
	__raw_writel(TIM3, EMIF4_1_SDRAM_TIM_3);
	__raw_writel(TIM3, EMIF4_1_SDRAM_TIM_3_SHADOW);
	__raw_writel(SDCFG, EMIF4_1_SDRAM_CONFIG);
	//__raw_writel(SDREF, EMIF4_0_SDRAM_REF_CTRL);
	//__raw_writel(SDREF, EMIF4_0_SDRAM_REF_CTRL_SHADOW);
	__raw_writel(RL, EMIF4_1_DDR_PHY_CTRL_1);
	__raw_writel(RL, EMIF4_1_DDR_PHY_CTRL_1_SHADOW);
	}

}

static void ddrsetup(void)
{
	/* setup a small control period */
	__raw_writel(0x0000613B, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(0x1000613B, EMIF4_0_SDRAM_REF_CTRL);
	__raw_writel(0x10000C30, EMIF4_0_SDRAM_REF_CTRL);

	__raw_writel(EMIF_PHYCFG, EMIF4_0_DDR_PHY_CTRL_1);
	__raw_writel(EMIF_PHYCFG, EMIF4_0_DDR_PHY_CTRL_1_SHADOW);

	if (CONFIG_TI816X_TWO_EMIF){
	/* setup a small control period */
	__raw_writel(0x0000613B, EMIF4_1_SDRAM_REF_CTRL);
	__raw_writel(0x1000613B, EMIF4_1_SDRAM_REF_CTRL);
	__raw_writel(0x10000C30, EMIF4_1_SDRAM_REF_CTRL);

	__raw_writel(EMIF_PHYCFG, EMIF4_1_DDR_PHY_CTRL_1);
	__raw_writel(EMIF_PHYCFG, EMIF4_1_DDR_PHY_CTRL_1_SHADOW);
	}

}

static void update_dqs(int emif)
{
}

static void ddr_pll_config()
{
	pll_config(DDR_PLL_BASE,
			DDR_N, DDR_M,
			DDR_M2, DDR_CLKCTRL);
}

/*
 * configure individual ADPLLJ
 */
static void pll_config(u32 base, u32 n, u32 m, u32 m2, u32 clkctrl_val)
{
	__raw_writel(0x2, CM_DEFAULT_L3_FAST_CLKSTCTRL);			/*Enable the Power Domain Transition of L3 Fast Domain Peripheral*/
	__raw_writel(0x2, CM_DEFAULT_EMIF_0_CLKCTRL);				/*Enable EMIF0 Clock*/
	__raw_writel(0x2, CM_DEFAULT_EMIF_1_CLKCTRL); 				/*Enable EMIF1 Clock*/
	while((__raw_readl(CM_DEFAULT_L3_FAST_CLKSTCTRL) & 0x300) != 0x300);	/*Poll for L3_FAST_GCLK  & DDR_GCLK  are active*/
	while((__raw_readl(CM_DEFAULT_EMIF_0_CLKCTRL)) != 0x2);	/*Poll for Module is functional*/
	while((__raw_readl(CM_DEFAULT_EMIF_1_CLKCTRL)) != 0x2);	/*Poll for Module is functional*/

	ddr_init_settings(0);

	if (CONFIG_TI816X_TWO_EMIF){
		ddr_init_settings(1);
	}

	__raw_writel(0x2, CM_DEFAULT_DMM_CLKCTRL); 				/*Enable EMIF1 Clock*/
	while((__raw_readl(CM_DEFAULT_DMM_CLKCTRL)) != 0x2);		/*Poll for Module is functional*/

	/*Program the DMM to Access EMIF0*/
	__raw_writel(0x80640300, DMM_LISA_MAP__0);
	__raw_writel(0xC0640320, DMM_LISA_MAP__1);

	/*Program the DMM to Access EMIF1*/
	__raw_writel(0x80640300, DMM_LISA_MAP__2);
	__raw_writel(0xC0640320, DMM_LISA_MAP__3);

	/*Enable Tiled Access*/
	__raw_writel(0x80000000, DMM_PAT_BASE_ADDR);

	emif4p_init(EMIF_TIM1, EMIF_TIM2, EMIF_TIM3, EMIF_SDREF & 0xFFFFFFFF, EMIF_SDCFG, 0x10B);
	ddrsetup();
	update_dqs(0);
	update_dqs(1);
}
#endif

#ifdef CONFIG_SETUP_PLL
static void pcie_pll_config()
{
	/* Powerdown both reclkp/n single ended receiver */
	__raw_writel(0x00000000, SERDES_REFCLK_CTRL);

	__raw_writel(0x00000000, PCIE_PLLCFG0);

	/* PCIe(2.5GHz) mode, 100MHz refclk, MDIVINT = 25, disable (50,100,125M) clks */
	__raw_writel(0x00640000, PCIE_PLLCFG1);

	/* SSC Mantissa and exponent = 0 */
	__raw_writel(0x00000000, PCIE_PLLCFG2);

	/* TBD */
	__raw_writel(0x004008E0, PCIE_PLLCFG3);

	/* TBD */
	__raw_writel(0x0000609C, PCIE_PLLCFG4);

	/* pcie_serdes_cfg_misc */
	/* TODO: verify the address over here (CTRL_BASE + 0x6FC = 0x481406FC ???)*/
	__raw_writel(0x00000E7B, 0x48141318);
	delay(3);

	/* Enable PLL LDO */
	__raw_writel(0x00000004, PCIE_PLLCFG0);
	delay(3);

	/* Enable DIG LDO, PLL LD0 */
	__raw_writel(0x00000014, PCIE_PLLCFG0);
	delay(3);

	/* Enable DIG LDO, ENBGSC_REF, PLL LDO */
	__raw_writel(0x00000016, PCIE_PLLCFG0);
	delay(3);
	__raw_writel(0x30000016, PCIE_PLLCFG0);
	delay(3);
	__raw_writel(0x70000016, PCIE_PLLCFG0);
	delay(3);

	/* Enable DIG LDO, SELSC, ENBGSC_REF, PLL LDO */
	__raw_writel(0x70000017, PCIE_PLLCFG0);
	delay(3);

	/* wait for ADPLL lock */
	while(__raw_readl(PCIE_PLLSTATUS) != 0x1);

}

static void sata_pll_config()
{
	__raw_writel(0xC12C003C, SATA_PLLCFG1);
	__raw_writel(0x004008E0, SATA_PLLCFG3);
	delay(35);

	/* Enable PLL LDO */
	__raw_writel(0x00000004, SATA_PLLCFG0);
	delay(75);

	/* Enable DIG LDO, ENBGSC_REF, PLL LDO */
	__raw_writel(0xC0000016, SATA_PLLCFG0);
	delay(60);

	/* Enable DIG LDO, SELSC, ENBGSC_REF, PLL LDO */
	__raw_writel(0xC0000017, SATA_PLLCFG0);

	/* wait for ADPLL lock */
	while(__raw_readl(SATA_PLLSTATUS) != 0x1);
}

static void usb_pll_config()
{
	pll_config(USB_PLL_BASE,
			USB_N, USB_M,
			USB_M2, USB_CLKCTRL);
}

static void modena_pll_config()
{
	pll_config(MODENA_PLL_BASE,
			MODENA_N, MODENA_M,
			MODENA_M2, MODENA_CLKCTRL);
}

static void l3_pll_config()
{
	pll_config(L3_PLL_BASE,
			L3_N, L3_M,
			L3_M2, L3_CLKCTRL);
}

static void ddr_pll_config()
{
	pll_config(DDR_PLL_BASE,
			DDR_N, DDR_M,
			DDR_M2, DDR_CLKCTRL);
}
#endif
/*
 * configure individual ADPLLJ
 */
static void pll_config(u32 base, u32 n, u32 m, u32 m2, u32 clkctrl_val)
{
	u32 m2nval, mn2val, read_clkctrl = 0;
	m2nval = (m2 << 16) | n;
	mn2val = m;

	/*
	 * ref_clk = 20/(n + 1);
	 * clkout_dco = ref_clk * m;
	 * clk_out = clkout_dco/m2;
	*/

	__raw_writel(m2nval, (base + ADPLLJ_M2NDIV));
	__raw_writel(mn2val, (base + ADPLLJ_MN2DIV));

	/* Load M2, N2 dividers of ADPLL */
	__raw_writel(0x1, (base + ADPLLJ_TENABLEDIV));
	__raw_writel(0x0, (base + ADPLLJ_TENABLEDIV));

	/* Loda M, N dividers of ADPLL */
	__raw_writel(0x1, (base + ADPLLJ_TENABLE));
	__raw_writel(0x0, (base + ADPLLJ_TENABLE));

	read_clkctrl = __raw_readl(base + ADPLLJ_CLKCTRL);
	__raw_writel((read_clkctrl & 0xff7e3fff) | clkctrl_val,
			base + ADPLLJ_CLKCTRL);

	/* Wait for phase and freq lock */
	while((__raw_readl(base + ADPLLJ_STATUS) & 0x00000600) != 0x00000600);

}

/*
 * Enable the clks & power for perifs (TIMER1, UART0,...)
 */
void per_clocks_enable(void)
{
	u32 temp;

	__raw_writel(0x2, CM_ALWON_L3_SLOW_CLKSTCTRL);

	/* TODO: No module level enable as in ti8148 ??? */
#if 0
	/* TIMER 1 */
	__raw_writel(0x2, CM_ALWON_TIMER_1_CLKCTRL);
#endif
	/* Selects OSC0 (20MHz) for DMTIMER1 */
	temp = __raw_readl(DMTIMER_CLKSRC);
	temp &= (0x7 << 3);
	temp |= (0x4 << 3);
	__raw_writel(temp, DMTIMER_CLKSRC);

#if 0
	while(((__raw_readl(CM_ALWON_L3_SLOW_CLKSTCTRL) & (0x80000<<1)) >> (19+1)) != 1);
	while(((__raw_readl(CM_ALWON_TIMER_1_CLKCTRL) & 0x30000)>>16) !=0);
#endif
	__raw_writel(0x2,(DM_TIMER1_BASE + 0x54));
	while(__raw_readl(DM_TIMER1_BASE + 0x10) & 1);

	__raw_writel(0x1,(DM_TIMER1_BASE + 0x38));

	/* UARTs */
	__raw_writel(0x2, CM_ALWON_UART_0_CLKCTRL);
	while(__raw_readl(CM_ALWON_UART_0_CLKCTRL) != 0x2);

	__raw_writel(0x2, CM_ALWON_UART_1_CLKCTRL);
	while(__raw_readl(CM_ALWON_UART_1_CLKCTRL) != 0x2);

	__raw_writel(0x2, CM_ALWON_UART_2_CLKCTRL);
	while(__raw_readl(CM_ALWON_UART_2_CLKCTRL) != 0x2);

	while((__raw_readl(CM_ALWON_L3_SLOW_CLKSTCTRL) & 0x2100) != 0x2100);

	/* SPI */
	__raw_writel(0x2, CM_ALWON_SPI_CLKCTRL);
	while(__raw_readl(CM_ALWON_SPI_CLKCTRL) != 0x2);

	/* I2C0 and I2C2 */
	__raw_writel(0x2, CM_ALWON_I2C_0_CLKCTRL);
	while(__raw_readl(CM_ALWON_I2C_0_CLKCTRL) != 0x2);

	/* Ethernet */
	__raw_writel(0x2, CM_ETHERNET_CLKSTCTRL);
	__raw_writel(0x2, CM_ALWON_ETHERNET_0_CLKCTRL);

}

/*
 * inits clocks for PRCM as defined in clocks.h
 */
void prcm_init(u32 in_ddr)
{
	/* Enable the control module */
	__raw_writel(0x2, CM_ALWON_CONTROL_CLKCTRL);

#ifdef CONFIG_SETUP_PLL
	/* Setup the various plls */
	pcie_pll_config();
	sata_pll_config();
	modena_pll_config();
	l3_pll_config();
	ddr_pll_config();

	usb_pll_config();
	/*  With clk freqs setup to desired values, enable the required peripherals */
	per_clocks_enable();
#endif
}

static void unlock_pll_control_mmr(void)
{
	/* ??? */
	__raw_writel(0x1EDA4C3D, 0x481C5040);
	__raw_writel(0x2FF1AC2B, 0x48140060);
	__raw_writel(0xF757FDC0, 0x48140064);
	__raw_writel(0xE2BC3A6D, 0x48140068);
	__raw_writel(0x1EBF131D, 0x4814006c);
	__raw_writel(0x6F361E05, 0x48140070);

}


void cpsw_pad_config(u32 instance)
{
#define PADCTRL_BASE 0x48140000
#define PAD232_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0B9C))
#define PAD233_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BA0))
#define PAD234_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BA4))
#define PAD235_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BA8))
#define PAD236_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BAC))
#define PAD237_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BB0))
#define PAD238_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BB4))
#define PAD239_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BB8))
#define PAD240_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BBC))
#define PAD241_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BC0))
#define PAD242_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BC4))
#define PAD243_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BC8))
#define PAD244_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BCC))
#define PAD245_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BD0))
#define PAD246_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BD4))
#define PAD247_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BD8))
#define PAD248_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BDC))
#define PAD249_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BE0))
#define PAD250_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BE4))
#define PAD251_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BE8))
#define PAD252_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BEC))
#define PAD253_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BF0))
#define PAD254_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BF4))
#define PAD255_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BF8))
#define PAD256_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0BFC))
#define PAD257_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0C00))
#define PAD258_CNTRL  (*(volatile unsigned int *)(PADCTRL_BASE + 0x0C04))

	if (instance == 0) {
		volatile u32 val = 0;
		val = PAD232_CNTRL;
		PAD232_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD233_CNTRL;
		PAD233_CNTRL = (volatile unsigned int) (BIT(19) | BIT(17) | BIT(0));
		val = PAD234_CNTRL;
		PAD234_CNTRL = (volatile unsigned int) (BIT(19) | BIT(18) | BIT(17) | BIT(0));
		val = PAD235_CNTRL;
		PAD235_CNTRL = (volatile unsigned int) (BIT(19) | BIT(18) | BIT(0));
		val = PAD236_CNTRL;
		PAD236_CNTRL = (volatile unsigned int) (BIT(19) | BIT(18) | BIT(0));
		val = PAD237_CNTRL;
		PAD237_CNTRL = (volatile unsigned int) (BIT(19) | BIT(18) | BIT(0));
		val = PAD238_CNTRL;
		PAD238_CNTRL = (volatile unsigned int) (BIT(19) | BIT(18) | BIT(0));
		val = PAD239_CNTRL;
		PAD239_CNTRL = (volatile unsigned int) (BIT(19) | BIT(18) | BIT(0));
		val = PAD240_CNTRL;
		PAD240_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD241_CNTRL;
		PAD241_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD242_CNTRL;
		PAD242_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD243_CNTRL;
		PAD243_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD244_CNTRL;
		PAD244_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD245_CNTRL;
		PAD245_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD246_CNTRL;
		PAD246_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD247_CNTRL;
		PAD247_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD248_CNTRL;
		PAD248_CNTRL = (volatile unsigned int) (BIT(18) | BIT(0));
		val = PAD249_CNTRL;
		PAD249_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD250_CNTRL;
		PAD250_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD251_CNTRL;
		PAD251_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD252_CNTRL;
		PAD252_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD253_CNTRL;
		PAD253_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD254_CNTRL;
		PAD254_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD255_CNTRL;
		PAD255_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD256_CNTRL;
		PAD256_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD257_CNTRL;
		PAD257_CNTRL = (volatile unsigned int) (BIT(0));
		val = PAD258_CNTRL;
		PAD258_CNTRL = (volatile unsigned int) (BIT(0));
	}
}


/*
 * baord specific muxing of pins
 */
void set_muxconf_regs(void)
{
	u32 i, add, val;
	u32 pad_conf[] = {
#include "mux.h"
	};

	for (i = 0; i<N_PINS; i++)
	{
		add = PIN_CTRL_BASE + (i*4);
		val = __raw_readl(add);
		val |= pad_conf[i];
		__raw_writel(val, add);
	}
}

/*
 * early system init of muxing and clocks.
 */
void s_init(u32 in_ddr)
{
	l2_cache_enable();		/* Can be removed as A8 comes up with L2 enabled */
	prcm_init(in_ddr);		/* Setup the PLLs and the clocks for the peripherals */
#ifdef CONFIG_TI814X_CONFIG_DDR
	if (!in_ddr)
		config_ti814x_sdram_ddr();	/* Do DDR settings */
#endif
	set_muxconf_regs();

}

/*
 * Reset the board
 */
void reset_cpu (ulong addr)
{
	addr = __raw_readl(PRM_DEVICE_RSTCTRL);
	addr &= ~BIT(1);
	addr |= BIT(1);
	__raw_writel(addr, PRM_DEVICE_RSTCTRL);
}

#ifdef CONFIG_DRIVER_TI_CPSW

#define PHY_CONF_REG           22
#define PHY_CONF_TXCLKEN       (1 << 5)

/* TODO : Check for the board specific PHY */
static void phy_init(char *name, int addr)
{
	unsigned short val;
	unsigned int   cntr = 0;

	/* Enable PHY to clock out TX_CLK */
	miiphy_read(name, addr, PHY_CONF_REG, &val);
	val |= PHY_CONF_TXCLKEN;
	miiphy_write(name, addr, PHY_CONF_REG, val);
	miiphy_read(name, addr, PHY_CONF_REG, &val);

	/* Enable Autonegotiation */
	if (miiphy_read(name, addr, PHY_BMCR, &val) != 0) {
		printf("failed to read bmcr\n");
		return;
	}
	val |= PHY_BMCR_DPLX | PHY_BMCR_AUTON | PHY_BMCR_100_MBPS;
	if (miiphy_write(name, addr, PHY_BMCR, val) != 0) {
		printf("failed to write bmcr\n");
		return;
	}
	miiphy_read(name, addr, PHY_BMCR, &val);

	/* Setup GIG advertisement */
	miiphy_read(name, addr, PHY_1000BTCR, &val);
	val |= PHY_1000BTCR_1000FD;
	val &= ~PHY_1000BTCR_1000HD;
	miiphy_write(name, addr, PHY_1000BTCR, val);
	miiphy_read(name, addr, PHY_1000BTCR, &val);

	/* Setup general advertisement */
	if (miiphy_read(name, addr, PHY_ANAR, &val) != 0) {
		printf("failed to read anar\n");
		return;
	}
	val |= (PHY_ANLPAR_10 | PHY_ANLPAR_10FD | PHY_ANLPAR_TX | PHY_ANLPAR_TXFD);
	if (miiphy_write(name, addr, PHY_ANAR, val) != 0) {
		printf("failed to write anar\n");
		return;
	}
	miiphy_read(name, addr, PHY_ANAR, &val);

	/* Restart auto negotiation*/
	miiphy_read(name, addr, PHY_BMCR, &val);
	val |= PHY_BMCR_RST_NEG;
	miiphy_write(name, addr, PHY_BMCR, val);

       /*check AutoNegotiate complete - it can take upto 3 secs*/
       do{
               udelay(40000);
               cntr++;

               if (!miiphy_read(name, addr, PHY_BMSR, &val)){
                       if(val & PHY_BMSR_AUTN_COMP)
                               break;
               }

       }while(cntr < 250);
       if (!miiphy_read(name, addr, PHY_BMSR, &val)) {
               if (!(val & PHY_BMSR_AUTN_COMP))
                       printf("phy_init: auto negotitation failed!\n");
       }

       return;
}

static void cpsw_control(int enabled)
{
       /* nothing for now */
       /* TODO : VTP was here before */
       return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
       {
               .slave_reg_ofs  = 0x50,
               .sliver_reg_ofs = 0x700,
               .phy_id         = 1,
       },
       {
               .slave_reg_ofs  = 0x90,
               .sliver_reg_ofs = 0x740,
               .phy_id         = 0,
       },
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base              = TI814X_CPSW_MDIO_BASE,
	.cpsw_base              = TI814X_CPSW_BASE,
	.mdio_div               = 0xff,
	.channels               = 8,
	.cpdma_reg_ofs          = 0x100,
	.slaves                 = 2,
	.slave_data             = cpsw_slaves,
	.ale_reg_ofs            = 0x600,
	.ale_entries            = 1024,
	.host_port_reg_ofs      = 0x28,
	.hw_stats_reg_ofs       = 0x400,
	.mac_control            = /*(1 << 18)   |*/ /* IFCTLA   */
				(1 << 15)     | /* EXTEN      */
				(1 << 5)      | /* MIIEN      */
				(1 << 4)      | /* TXFLOWEN   */
				(1 << 3),       /* RXFLOWEN   */
	.control                = cpsw_control,
	.phy_init               = phy_init,
	.host_port_num		= 0,
};

int board_eth_init(bd_t *bis)
{
       return cpsw_register(&cpsw_data);
}
#endif

