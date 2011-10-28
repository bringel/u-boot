/*
 * mux.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include <config.h>
#include <asm/io.h>
#include "common_def.h"
#include <asm/arch/hardware.h>

#define MUX_CFG(value, offset)	\
	__raw_writel(value, (CTRL_BASE + offset));

/* PAD Control Fields */
#define SLEWCTRL	(0x1 << 6)
#define	RXACTIVE	(0x1 << 5)
#define	PULLUP_EN	(0x1 << 4) /* Pull UP Selection */
#define PULLUDEN	(0x1 << 3) /* Pull up Disbled */
#define MODE(val)	val

/*
 * PAD CONTROL OFFSETS
 * Field names corresponds to the pad signal name
 */
struct pad_signals {
	int gpmc_ad0;
	int gpmc_ad1;
	int gpmc_ad2;
	int gpmc_ad3;
	int gpmc_ad4;
	int gpmc_ad5;
	int gpmc_ad6;
	int gpmc_ad7;
	int gpmc_ad8;
	int gpmc_ad9;
	int gpmc_ad10;
	int gpmc_ad11;
	int gpmc_ad12;
	int gpmc_ad13;
	int gpmc_ad14;
	int gpmc_ad15;
	int gpmc_a0;
	int gpmc_a1;
	int gpmc_a2;
	int gpmc_a3;
	int gpmc_a4;
	int gpmc_a5;
	int gpmc_a6;
	int gpmc_a7;
	int gpmc_a8;
	int gpmc_a9;
	int gpmc_a10;
	int gpmc_a11;
	int gpmc_wait0;
	int gpmc_wpn;
	int gpmc_be1n;
	int gpmc_csn0;
	int gpmc_csn1;
	int gpmc_csn2;
	int gpmc_csn3;
	int gpmc_clk;
	int gpmc_advn_ale;
	int gpmc_oen_ren;
	int gpmc_wen;
	int gpmc_be0n_cle;
	int lcd_data0;
	int lcd_data1;
	int lcd_data2;
	int lcd_data3;
	int lcd_data4;
	int lcd_data5;
	int lcd_data6;
	int lcd_data7;
	int lcd_data8;
	int lcd_data9;
	int lcd_data10;
	int lcd_data11;
	int lcd_data12;
	int lcd_data13;
	int lcd_data14;
	int lcd_data15;
	int lcd_vsync;
	int lcd_hsync;
	int lcd_pclk;
	int lcd_ac_bias_en;
	int mmc0_dat3;
	int mmc0_dat2;
	int mmc0_dat1;
	int mmc0_dat0;
	int mmc0_clk;
	int mmc0_cmd;
	int mii1_col;
	int mii1_crs;
	int mii1_rxerr;
	int mii1_txen;
	int mii1_rxdv;
	int mii1_txd3;
	int mii1_txd2;
	int mii1_txd1;
	int mii1_txd0;
	int mii1_txclk;
	int mii1_rxclk;
	int mii1_rxd3;
	int mii1_rxd2;
	int mii1_rxd1;
	int mii1_rxd0;
	int rmii1_refclk;
	int mdio_data;
	int mdio_clk;
	int spi0_sclk;
	int spi0_d0;
	int spi0_d1;
	int spi0_cs0;
	int spi0_cs1;
	int ecap0_in_pwm0_out;
	int uart0_ctsn;
	int uart0_rtsn;
	int uart0_rxd;
	int uart0_txd;
	int uart1_ctsn;
	int uart1_rtsn;
	int uart1_rxd;
	int uart1_txd;
	int i2c0_sda;
	int i2c0_scl;
	int mcasp0_aclkx;
	int mcasp0_fsx;
	int mcasp0_axr0;
	int mcasp0_ahclkr;
	int mcasp0_aclkr;
	int mcasp0_fsr;
	int mcasp0_axr1;
	int mcasp0_ahclkx;
	int xdma_event_intr0;
	int xdma_event_intr1;
	int nresetin_out;
	int porz;
	int nnmi;
	int osc0_in;
	int osc0_out;
	int tms;
	int tdi;
	int tdo;
	int tck;
	int ntrst;
	int emu0;
	int emu1;
	int osc1_in;
	int osc1_out;
	int rtc_porz;
	int ext_wakeup;
	int pmic_power_en;
	int enz_kaldo_1p8v;
	int usb0_dm;
	int usb0_dp;
	int usb0_ce;
	int usb0_id;
	int usb0_vbus;
	int usb0_drvvbus;
	int usb1_dm;
	int usb1_dp;
	int usb1_ce;
	int usb1_id;
	int usb1_vbus;
	int usb1_drvvbus;
	int ddr_resetn;
	int ddr_csn0;
	int ddr_cke;
	int ddr_ck;
	int ddr_nck;
	int ddr_casn;
	int ddr_rasn;
	int ddr_wen;
	int ddr_ba0;
	int ddr_ba1;
	int ddr_ba2;
	int ddr_a0;
	int ddr_a1;
	int ddr_a2;
	int ddr_a3;
	int ddr_a4;
	int ddr_a5;
	int ddr_a6;
	int ddr_a7;
	int ddr_a8;
	int ddr_a9;
	int ddr_a10;
	int ddr_a11;
	int ddr_a12;
	int ddr_a13;
	int ddr_a14;
	int ddr_a15;
	int ddr_odt;
	int ddr_d0;
	int ddr_d1;
	int ddr_d2;
	int ddr_d3;
	int ddr_d4;
	int ddr_d5;
	int ddr_d6;
	int ddr_d7;
	int ddr_d8;
	int ddr_d9;
	int ddr_d10;
	int ddr_d11;
	int ddr_d12;
	int ddr_d13;
	int ddr_d14;
	int ddr_d15;
	int ddr_dqm0;
	int ddr_dqm1;
	int ddr_dqs0;
	int ddr_dqsn0;
	int ddr_dqs1;
	int ddr_dqsn1;
	int ddr_vref;
	int ddr_vtp;
	int ddr_strben0;
	int ddr_strben1;
	int ain7;
	int ain6;
	int ain5;
	int ain4;
	int ain3;
	int ain2;
	int ain1;
	int ain0;
	int vrefp;
	int vrefn;
};

struct module_pin_mux {
	short reg_offset;
	unsigned char val;
};

struct evm_pin_mux {
	struct module_pin_mux *mod_pin_mux;
	unsigned short profile;
};

#define PAD_CTRL_BASE	0x800
#define OFFSET(x)	(unsigned int) (&((struct pad_signals *) \
				(PAD_CTRL_BASE))->x)

static struct module_pin_mux uart0_pin_mux[] = {
	{OFFSET(uart0_ctsn), (MODE(0) | PULLUDEN | RXACTIVE)},	/* UART0_CTS */
	{OFFSET(uart0_rtsn), (MODE(0) | PULLUDEN)},		/* UART0_RTS */
	{OFFSET(uart0_rxd), (MODE(0) | PULLUDEN | RXACTIVE)},	/* UART0_RXD */
	{OFFSET(uart0_txd), (MODE(0) | PULLUDEN)},		/* UART0_TXD */
	{-1},
};

static struct module_pin_mux emif_pin_mux[] = {
	{OFFSET(ddr_resetn), MODE(0)},			/* RESETN */
	{OFFSET(ddr_csn0), (MODE(0) | PULLUP_EN)},	/* CSNO	*/
	{OFFSET(ddr_cke), MODE(0)},			/* CKE */
	{OFFSET(ddr_ck), (MODE(0) | PULLUP_EN)},	/* CK */
	{OFFSET(ddr_nck), (MODE(0) | PULLUP_EN)},	/* NCK */
	{OFFSET(ddr_casn), (MODE(0) | PULLUP_EN)},	/* CASN */
	{OFFSET(ddr_rasn), (MODE(0) | PULLUP_EN)},	/* RASN */
	{OFFSET(ddr_wen), (MODE(0) | PULLUP_EN)},	/* WEN */
	{OFFSET(ddr_ba0), (MODE(0) | PULLUP_EN)},	/* BA0 */
	{OFFSET(ddr_ba1), (MODE(0) | PULLUP_EN)},	/* BA1 */
	{OFFSET(ddr_ba2), (MODE(0) | PULLUP_EN)},	/* BA2 */
	{OFFSET(ddr_a0), (MODE(0) | PULLUP_EN)},	/* A0 */
	{OFFSET(ddr_a1), (MODE(0) | PULLUP_EN)},	/* A1 */
	{OFFSET(ddr_a2), (MODE(0) | PULLUP_EN)},	/* A2 */
	{OFFSET(ddr_a3), (MODE(0) | PULLUP_EN)},	/* A3 */
	{OFFSET(ddr_a4), (MODE(0) | PULLUP_EN)},	/* A4 */
	{OFFSET(ddr_a5), (MODE(0) | PULLUP_EN)},	/* A5 */
	{OFFSET(ddr_a6), (MODE(0) | PULLUP_EN)},	/* A6 */
	{OFFSET(ddr_a7), (MODE(0) | PULLUP_EN)},	/* A7 */
	{OFFSET(ddr_a8), (MODE(0) | PULLUP_EN)},	/* A8 */
	{OFFSET(ddr_a9), (MODE(0) | PULLUP_EN)},	/* A9 */
	{OFFSET(ddr_a10), (MODE(0) | PULLUP_EN)},	/* A10 */
	{OFFSET(ddr_a11), (MODE(0) | PULLUP_EN)},	/* A11 */
	{OFFSET(ddr_a12), (MODE(0) | PULLUP_EN)},	/* A12 */
	{OFFSET(ddr_a13), (MODE(0) | PULLUP_EN)},	/* A13 */
	{OFFSET(ddr_a14), (MODE(0) | PULLUP_EN)},	/* A14 */
	{OFFSET(ddr_a15), (MODE(0) | PULLUP_EN)},	/* A15 */
	{OFFSET(ddr_odt), (MODE(0) | RXACTIVE)},	/* A16 */
	{OFFSET(ddr_d0), (MODE(0) | RXACTIVE)},		/* ODT */
	{OFFSET(ddr_d1), (MODE(0) | RXACTIVE)},		/* D0 */
	{OFFSET(ddr_d2), (MODE(0) | RXACTIVE)},		/* D1 */
	{OFFSET(ddr_d3), (MODE(0) | RXACTIVE)},		/* D2 */
	{OFFSET(ddr_d4), (MODE(0) | RXACTIVE)},		/* D3 */
	{OFFSET(ddr_d5), (MODE(0) | RXACTIVE)},		/* D4 */
	{OFFSET(ddr_d6), (MODE(0) | RXACTIVE)},		/* D5 */
	{OFFSET(ddr_d7), (MODE(0) | RXACTIVE)},		/* D6 */
	{OFFSET(ddr_d8), (MODE(0) | RXACTIVE)},		/* D7 */
	{OFFSET(ddr_d9), (MODE(0) | RXACTIVE)},		/* D8 */
	{OFFSET(ddr_d10), (MODE(0) | RXACTIVE)},	/* D9 */
	{OFFSET(ddr_d11), (MODE(0) | RXACTIVE)},	/* D10 */
	{OFFSET(ddr_d12), (MODE(0) | RXACTIVE)},	/* D11 */
	{OFFSET(ddr_d13), (MODE(0) | RXACTIVE)},	/* D12 */
	{OFFSET(ddr_d14), (MODE(0) | RXACTIVE)},	/* D13 */
	{OFFSET(ddr_d15), (MODE(0) | RXACTIVE)},	/* D14 */
	{OFFSET(ddr_dqm0), (MODE(0) | PULLUP_EN)},	/* DQM0 */
	{OFFSET(ddr_dqm1), (MODE(0) | PULLUP_EN)},	/* DQM1 */
	{OFFSET(ddr_dqs0), (MODE(0) | RXACTIVE)},	/* DQS0 */
	{OFFSET(ddr_dqsn0), (MODE(0) | PULLUP_EN |
			RXACTIVE)}, /* DQSN0 */
	{OFFSET(ddr_dqs1), (MODE(0) | RXACTIVE)},	 /* DQS1 */
	{OFFSET(ddr_dqsn1), (MODE(0) | PULLUP_EN |
			RXACTIVE)}, /* DQSN1 */
	{OFFSET(ddr_vref), (MODE(0) | RXACTIVE)},	/* VREF */
	{OFFSET(ddr_vtp), (MODE(0) | RXACTIVE)},	/* VTP */
	{-1},
};

static struct module_pin_mux nand_pin_mux[] = {
	{OFFSET(gpmc_ad0), (MODE(0) | RXACTIVE)},	/* NAND AD0 */
	{OFFSET(gpmc_ad1), (MODE(0) | RXACTIVE)},	/* NAND AD1 */
	{OFFSET(gpmc_ad2), (MODE(0) | RXACTIVE)},	/* NAND AD2 */
	{OFFSET(gpmc_ad3), (MODE(0) | RXACTIVE)},	/* NAND AD3 */
	{OFFSET(gpmc_ad4), (MODE(0) | RXACTIVE)},	/* NAND AD4 */
	{OFFSET(gpmc_ad5), (MODE(0) | RXACTIVE)},	/* NAND AD5 */
	{OFFSET(gpmc_ad6), (MODE(0) | RXACTIVE)},	/* NAND AD6 */
	{OFFSET(gpmc_ad7), (MODE(0) | RXACTIVE)},	/* NAND AD7 */
	{OFFSET(gpmc_wait0), (MODE(0) | RXACTIVE | PULLUP_EN)}, /* NAND WAIT */
	{OFFSET(gpmc_wpn), (MODE(0) | PULLUP_EN)},	/* NAND_WPN */
	{OFFSET(gpmc_csn0), (MODE(0) | PULLUP_EN)},	/* NAND_CS0 */
	{OFFSET(gpmc_advn_ale), (MODE(0) | PULLUP_EN)}, /* NAND_ADV_ALE */
	{OFFSET(gpmc_oen_ren), (MODE(0) | PULLUP_EN)},	/* NAND_OE */
	{OFFSET(gpmc_wen), (MODE(0) | PULLUP_EN)},	/* NAND_WEN */
	{OFFSET(gpmc_be0n_cle), (MODE(0) | PULLUP_EN)},	/* NAND_BE_CLE */
	{-1},
};

static struct module_pin_mux i2c0_pin_mux[] = {
	{OFFSET(i2c0_sda), (MODE(0) | RXACTIVE | PULLUDEN)},	/* I2C_DATA */
	{OFFSET(i2c0_scl), (MODE(0) | RXACTIVE | PULLUDEN)},	/* I2C_SCLK */
	{-1},
};

static struct module_pin_mux ethernet0_pin_mux[] = {
	{OFFSET(mii1_txen), MODE(2)},			/* RGMII1_TCTL */
	{OFFSET(mii1_rxdv), MODE(2) | RXACTIVE},	/* RGMII1_RCTL */
	{OFFSET(mii1_txd3), MODE(2)},			/* RGMII1_TD3 */
	{OFFSET(mii1_txd2), MODE(2)},			/* RGMII1_TD2 */
	{OFFSET(mii1_txd1), MODE(2)},			/* RGMII1_TD1 */
	{OFFSET(mii1_txd0), MODE(2)},			/* RGMII1_TD0 */
	{OFFSET(mii1_txclk), MODE(2)},			/* RGMII1_TCLK */
	{OFFSET(mii1_rxclk), MODE(2) | RXACTIVE},	/* RGMII1_RCLK */
	{OFFSET(mii1_rxd3), MODE(2) | RXACTIVE},	/* RGMII1_RD3 */
	{OFFSET(mii1_rxd2), MODE(2) | RXACTIVE},	/* RGMII1_RD2 */
	{OFFSET(mii1_rxd1), MODE(2) | RXACTIVE},	/* RGMII1_RD1 */
	{OFFSET(mii1_rxd0), MODE(2) | RXACTIVE},	/* RGMII1_RD0 */
	{OFFSET(rmii1_refclk), MODE(0) | RXACTIVE},	/* RMII1_REFCLK */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN}, /* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},	/* MDIO_CLK */
	{-1},
};

static struct module_pin_mux ethernet1_pin_mux[] = {
	{OFFSET(gpmc_a0), MODE(2)},			/* RGMII2_TCTL */
	{OFFSET(gpmc_a1), MODE(2) | RXACTIVE},		/* RGMII2_RCTL */
	{OFFSET(gpmc_a2), MODE(2)},			/* RGMII2_TD3 */
	{OFFSET(gpmc_a3), MODE(2)},			/* RGMII2_TD2 */
	{OFFSET(gpmc_a4), MODE(2)},			/* RGMII2_TD1 */
	{OFFSET(gpmc_a5), MODE(2)},			/* RGMII2_TD0 */
	{OFFSET(gpmc_a6), MODE(2)},			/* RGMII2_TCLK */
	{OFFSET(gpmc_a7), MODE(2) | RXACTIVE},		/* RGMII2_RCLK */
	{OFFSET(gpmc_a8), MODE(2) | RXACTIVE},		/* RGMII2_RD3 */
	{OFFSET(gpmc_a9), MODE(2) | RXACTIVE},		/* RGMII2_RD2 */
	{OFFSET(gpmc_a10), MODE(2) | RXACTIVE},		/* RGMII2_RD1 */
	{OFFSET(gpmc_a11), MODE(2) | RXACTIVE},		/* RGMII2_RD0 */
	{OFFSET(mii1_col), MODE(1) | RXACTIVE},		/* RMII2_REFCLK */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN}, /* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},	/* MDIO_CLK */
	{-1},
};

static struct module_pin_mux nor_pin_mux[] = {
	{OFFSET(lcd_data0), MODE(1) | PULLUDEN},	/* NOR_A0 */
	{OFFSET(lcd_data1), MODE(1) | PULLUDEN},	/* NOR_A1 */
	{OFFSET(lcd_data2), MODE(1) | PULLUDEN},	/* NOR_A2 */
	{OFFSET(lcd_data3), MODE(1) | PULLUDEN},	/* NOR_A3 */
	{OFFSET(lcd_data4), MODE(1) | PULLUDEN},	/* NOR_A4 */
	{OFFSET(lcd_data5), MODE(1) | PULLUDEN},	/* NOR_A5 */
	{OFFSET(lcd_data6), MODE(1) | PULLUDEN},	/* NOR_A6 */
	{OFFSET(lcd_data7), MODE(1) | PULLUDEN},	/* NOR_A7 */
	{OFFSET(gpmc_a8), MODE(0)},			/* NOR_A8 */
	{OFFSET(gpmc_a9), MODE(0)},			/* NOR_A9 */
	{OFFSET(gpmc_a10), MODE(0)},			/* NOR_A10 */
	{OFFSET(gpmc_a11), MODE(0)},			/* NOR_A11 */
	{OFFSET(lcd_data8), MODE(1) | PULLUDEN},	/* NOR_A12 */
	{OFFSET(lcd_data9), MODE(1) | PULLUDEN},	/* NOR_A13 */
	{OFFSET(lcd_data10), MODE(1) | PULLUDEN},	/* NOR_A14 */
	{OFFSET(lcd_data11), MODE(1) | PULLUDEN},	/* NOR_A15 */
	{OFFSET(lcd_data12), MODE(1) | PULLUDEN},	/* NOR_A16 */
	{OFFSET(lcd_data13), MODE(1) | PULLUDEN},	/* NOR_A17 */
	{OFFSET(lcd_data14), MODE(1) | PULLUDEN},	/* NOR_A18 */
	{OFFSET(lcd_data15), MODE(1) | PULLUDEN},	/* NOR_A19 */
	{OFFSET(gpmc_a4), MODE(4)},			/* NOR_A20 */
	{OFFSET(gpmc_a5), MODE(4)},			/* NOR_A21 */
	{OFFSET(gpmc_a6), MODE(4)},			/* NOR_A22 */
	{OFFSET(gpmc_ad0), MODE(0) | RXACTIVE},		/* NOR_AD0 */
	{OFFSET(gpmc_ad1), MODE(0) | RXACTIVE},		/* NOR_AD1 */
	{OFFSET(gpmc_ad2), MODE(0) | RXACTIVE},		/* NOR_AD2 */
	{OFFSET(gpmc_ad3), MODE(0) | RXACTIVE},		/* NOR_AD3 */
	{OFFSET(gpmc_ad4), MODE(0) | RXACTIVE},		/* NOR_AD4 */
	{OFFSET(gpmc_ad5), MODE(0) | RXACTIVE},		/* NOR_AD5 */
	{OFFSET(gpmc_ad6), MODE(0) | RXACTIVE},		/* NOR_AD6 */
	{OFFSET(gpmc_ad7), MODE(0) | RXACTIVE},		/* NOR_AD7 */
	{OFFSET(gpmc_ad8), MODE(0) | RXACTIVE},		/* NOR_AD8 */
	{OFFSET(gpmc_ad9), MODE(0) | RXACTIVE},		/* NOR_AD9 */
	{OFFSET(gpmc_ad10), MODE(0) | RXACTIVE},	/* NOR_AD10 */
	{OFFSET(gpmc_ad11), MODE(0) | RXACTIVE},	/* NOR_AD11 */
	{OFFSET(gpmc_ad12), MODE(0) | RXACTIVE},	/* NOR_AD12 */
	{OFFSET(gpmc_ad13), MODE(0) | RXACTIVE},	/* NOR_AD13 */
	{OFFSET(gpmc_ad14), MODE(0) | RXACTIVE},	/* NOR_AD14 */
	{OFFSET(gpmc_ad15), MODE(0) | RXACTIVE},	/* NOR_AD15 */
	{OFFSET(gpmc_csn0), (MODE(0) | PULLUP_EN)},	/* NOR_CE */
	{OFFSET(gpmc_oen_ren), (MODE(0) | PULLUP_EN)},	/* NOR_OE */
	{OFFSET(gpmc_wen), (MODE(0) | PULLUP_EN)},	/* NOR_WEN */
	{OFFSET(gpmc_wait0), (MODE(0) | RXACTIVE | PULLUP_EN)}, /* NOR WAIT */
	{OFFSET(lcd_ac_bias_en), MODE(7) | RXACTIVE | PULLUDEN}, /* NOR RESET */
	{-1},
};

static struct module_pin_mux mmc0_pin_mux[] = {
	{OFFSET(mmc0_dat3), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT3 */
	{OFFSET(mmc0_dat2), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT2 */
	{OFFSET(mmc0_dat1), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT1 */
	{OFFSET(mmc0_dat0), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT0 */
	{OFFSET(mmc0_clk), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CMD */
	{OFFSET(mcasp0_aclkr), (MODE(4) | RXACTIVE)},		/* MMC0_WP */
	{OFFSET(spi0_cs1), (MODE(5) | RXACTIVE | PULLUP_EN)},	/* MMC0_CD */
	{-1},
};

static struct module_pin_mux mmc1_pin_mux[] = {
	{OFFSET(gpmc_ad3), (MODE(1) | RXACTIVE)},	/* MMC1_DAT3 */
	{OFFSET(gpmc_ad2), (MODE(1) | RXACTIVE)},	/* MMC1_DAT2 */
	{OFFSET(gpmc_ad1), (MODE(1) | RXACTIVE)},	/* MMC1_DAT1 */
	{OFFSET(gpmc_ad0), (MODE(1) | RXACTIVE)},	/* MMC1_DAT0 */
	{OFFSET(gpmc_csn1), (MODE(2) | RXACTIVE | PULLUP_EN)},	/* MMC1_CLK */
	{OFFSET(gpmc_csn2), (MODE(2) | RXACTIVE | PULLUP_EN)},	/* MMC1_CMD */
	{OFFSET(uart1_rxd), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_WP */
	{OFFSET(mcasp0_fsx), (MODE(4) | RXACTIVE)},	/* MMC1_CD */
	{-1},
};

static struct module_pin_mux spi0_pin_mux[] = {
	{OFFSET(spi0_sclk), MODE(0) | RXACTIVE},  /*SPI0_SCLK */
	{OFFSET(spi0_d0), MODE(0) | RXACTIVE},    /*SPI0_D0 */
	{OFFSET(spi0_d1), MODE(0) | RXACTIVE},    /*SPI0_D1 */
	{OFFSET(spi0_cs0), MODE(0) | RXACTIVE},   /*SPI0_CS0 */
	{-1},
};

static struct module_pin_mux spi1_pin_mux[] = {
	{OFFSET(mcasp0_aclkx), MODE(3) | RXACTIVE | PULLUDEN},  /*SPI1_SCLK */
	{OFFSET(mcasp0_fsx), MODE(3) | RXACTIVE | PULLUDEN},    /*SPI1_D0 */
	{OFFSET(mcasp0_axr0), MODE(3) | RXACTIVE | PULLUDEN},   /*SPI1_D1 */
	{OFFSET(mcasp0_ahclkr), MODE(3) | RXACTIVE | PULLUDEN}, /*SPI1_CS0 */
	{-1},
};

#if defined(CONFIG_AM335X_MIN_CONFIG) && defined(CONFIG_SPI)
/*
 * initialization of SPI pinmuxing in min U-boot
 */
static struct evm_pin_mux spi_pin_mux[] = {
	{spi0_pin_mux, PROFILE_2},
	{spi1_pin_mux, PROFILE_ALL},
	{0},
};
#endif

/*
 * Update the structure with the modules present in the general purpose
 * board and the profiles in which the modules are present.
 * If the module is physically present but if it is not available
 * in any of the profile, then do not update it.
 * For eg, nand is avialable only in the profiles 0 and 1, whereas
 * UART0  is available in all the profiles.
 */
static struct evm_pin_mux general_purpose_evm_pin_mux[] = {
	{emif_pin_mux, PROFILE_ALL},
	{uart0_pin_mux, PROFILE_ALL},
	{nand_pin_mux, PROFILE_ALL & ~PROFILE_2 & ~PROFILE_3},
	{ethernet0_pin_mux, PROFILE_ALL},
	{ethernet1_pin_mux, PROFILE_1 | PROFILE_2 | PROFILE_4 | PROFILE_6},
	{nor_pin_mux, PROFILE_3},
	{mmc0_pin_mux, PROFILE_ALL},
	{mmc1_pin_mux, PROFILE_2},
	{spi0_pin_mux, PROFILE_2},
	{0},
};

/*
 * Update the structure with the modules present in the ia daughter board and
 * the profiles in which the modules are present. If the module is physically
 * present but if it is not available in any of the profile, then do not update
 */
static struct evm_pin_mux ia_motor_control_evm_pin_mux[] = {
	{emif_pin_mux, PROFILE_ALL},
	{nand_pin_mux, PROFILE_ALL},
	{mmc0_pin_mux, PROFILE_ALL},
	{spi1_pin_mux, PROFILE_ALL},
	{0},
};

/* IP Phone EVM has single profile */
static struct evm_pin_mux ip_phone_evm_pin_mux[] = {
	{nand_pin_mux, PROFILE_0},
	{emif_pin_mux, PROFILE_0},
	{ethernet0_pin_mux, PROFILE_0},
	{ethernet1_pin_mux, PROFILE_0},
	{mmc0_pin_mux, PROFILE_0},
	{0},
};

/* Base board has single profile */
static struct evm_pin_mux low_cost_evm_pin_mux[] = {
	{emif_pin_mux, PROFILE_NONE},
	{uart0_pin_mux,	PROFILE_NONE},
	{nand_pin_mux, PROFILE_NONE},
	{ethernet0_pin_mux, PROFILE_NONE},
	{mmc0_pin_mux, PROFILE_NONE},
	{0},
};

static struct evm_pin_mux *am335x_evm_pin_mux[] = {
	general_purpose_evm_pin_mux,
	ia_motor_control_evm_pin_mux,
	ip_phone_evm_pin_mux,
	low_cost_evm_pin_mux,
};

/*
 * Configure the pin mux for the module
 */
static void configure_module_pin_mux(struct module_pin_mux *mod_pin_mux)
{
	int i;

	if (!mod_pin_mux)
		return;

	for (i = 0; mod_pin_mux[i].reg_offset != -1; i++)
		MUX_CFG(mod_pin_mux[i].val, mod_pin_mux[i].reg_offset);
}

/*
 * Check each module in the daughter board(first argument) whether it is
 * available in the selected profile(second argument). If the module is not
 * available in the selected profile, skip the corresponding configuration.
 */
static void set_evm_pin_mux(struct evm_pin_mux *am335x_evm_pin_mux,
			char profile)
{
	int i;

	if (!am335x_evm_pin_mux)
		return;

	for (i = 0; am335x_evm_pin_mux[i].mod_pin_mux != 0; i++)
		if ((am335x_evm_pin_mux[i].profile & profile) ||
				(profile == PROFILE_NONE))
			configure_module_pin_mux(am335x_evm_pin_mux[i].
					mod_pin_mux);
}

void configure_evm_pin_mux(unsigned char daughter_board_id, unsigned short
		profile)
{
	if (daughter_board_id > BASE_BOARD_ONLY)
		return;

	set_evm_pin_mux(am335x_evm_pin_mux[daughter_board_id], profile);
}

void enable_i2c0_pin_mux(void)
{
	configure_module_pin_mux(i2c0_pin_mux);
}

void enable_uart0_pin_mux(void)
{
	configure_module_pin_mux(uart0_pin_mux);
}

#if defined(CONFIG_AM335X_MIN_CONFIG) && defined(CONFIG_SPI)
void enable_spi_pinmux(unsigned char board_id, unsigned short profile)
{
	if (spi_pin_mux[board_id].profile == profile)
		configure_module_pin_mux(spi_pin_mux[board_id].mod_pin_mux);
}
#endif
