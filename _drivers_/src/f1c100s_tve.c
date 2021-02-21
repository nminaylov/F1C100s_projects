/********************************************//**
 * @file f1c100s_tve.c
 * @brief F1C100s TV encoder driver
 * @author n.minaylov
 ***********************************************/

#include <stddef.h>
#include <stdlib.h>
#include "f1c100s_tve.h"
#include "f1c100s_clock.h"
#include "io.h"

static void tve_dac_auto_cali(uint32_t cali);
void tve_init_int(uint32_t *cali);
void tve_dac_enable(void);
void tve_dac_disable(void);
void tve_enable(void);
void tve_disable(void);
void tve_set_tv_mode(uint8_t mode, uint32_t cali);
void tve_dac_autocheck_enable(void);
void tve_dac_autocheck_disable(void);
static void tve_dac_auto_cali(uint32_t cali);
void tve_enhance(uint8_t mode);

void tve_clock_init(void);

static inline void sdelay(int loops)
{
    __asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n" "bne 1b":"=r" (loops):"0"(loops));
}

void tve_init(void)
{
    tve_clock_init();
    tve_init_int(NULL);
    //tve_set_tv_mode(0, 0xFFF);
    //tve_dac_autocheck_enable();
    //tve_enhance(0);
    tve_dac_enable();
    tve_enable();
}

void tve_clock_init(void)
{
    clk_pll_disable(PLL_VIDEO);
    clk_pll_init(PLL_VIDEO, 90, 8); // 24*90/8 = 270MHz // todo проверить 297m
    clk_pll_enable(PLL_VIDEO);
    while (!clk_pll_is_locked(PLL_VIDEO)); // Wait for PLL to lock
    //sdelay(1000);
    clk_enable(CCU_BUS_CLK_GATE1, 10);
    write32(F1C100S_CCU_BASE+CCU_TVE_CLK, 0x80008109);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 10);
    //sdelay(1000);
}

void tve_init_int(uint32_t *cali)
{
    uint32_t val = (*cali) ? (*cali) : 0xFFF;//0x285;

    write32(F1C100S_TVE_BASE+TVE_REG_304, val << 16);
    write32(F1C100S_TVE_BASE+TVE_REG_30C, 0x00101110);
    write32(F1C100S_TVE_BASE+TVE_DAC1, 0x433e12b1);
    write32(F1C100S_TVE_BASE+TVE_ENABLE, (1U << 31));
}

void tve_dac_enable(void)
{
	set32(F1C100S_TVE_BASE+TVE_DAC1, 0x1 << 0);
}

void tve_dac_disable(void)
{
	clear32(F1C100S_TVE_BASE+TVE_DAC1, 0x1 << 0);
}

void tve_enable(void)
{
	clear32(F1C100S_TVE_BASE+TVE_ENABLE, 0x1U << 31);
	set32(F1C100S_TVE_BASE+TVE_ENABLE, 0x1 << 0);
}

void tve_disable(void)
{
	clear32(F1C100S_TVE_BASE+TVE_ENABLE, 0x1 << 0);
	set32(F1C100S_TVE_BASE+TVE_ENABLE, 0x1U << 31);
}

void tve_set_tv_mode(uint8_t mode, uint32_t cali)
{
	tve_dac_auto_cali(cali);
	if (mode) // NTSC
	{
		write32(F1C100S_TVE_BASE+TVE_ENABLE, 0x00000300);
		write32(F1C100S_TVE_BASE+TVE_CFG1, 0x07070000);
		write32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x30001400);
		write32(F1C100S_TVE_BASE+TVE_CHROMA_FREQ, 0x21F07C1F);
		write32(F1C100S_TVE_BASE+TVE_FB_PORCH, 0x00760020);
		write32(F1C100S_TVE_BASE+TVE_HD_VS, 0x00000016);
		write32(F1C100S_TVE_BASE+TVE_LINE_NUM, 0x0016020D);
		write32(F1C100S_TVE_BASE+TVE_LEVEL, 0x00F0011A);
		write32(F1C100S_TVE_BASE+TVE_CB_RESET, 0x00000001);
		write32(F1C100S_TVE_BASE+TVE_VS_NUM, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_FILTER, 0x00000002);
		write32(F1C100S_TVE_BASE+TVE_CBCR_LEVEL, 0x0000004F);
		write32(F1C100S_TVE_BASE+TVE_TINT_PHASE, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_B_WIDTH, 0x0016447E);
		write32(F1C100S_TVE_BASE+TVE_CBCR_GAIN, 0x0000A0A0);
		write32(F1C100S_TVE_BASE+TVE_SYNC_LEVEL, 0x001000F0);
		write32(F1C100S_TVE_BASE+TVE_WHITE_LEVEL, 0x01E80320);
		write32(F1C100S_TVE_BASE+TVE_ACT_LINE, 0x000005A0);
		write32(F1C100S_TVE_BASE+TVE_CHROMA_BW, 0x00010000);
		write32(F1C100S_TVE_BASE+TVE_CFG2, 0x00000101);
		write32(F1C100S_TVE_BASE+TVE_RESYNC, 0x20050368);
		write32(F1C100S_TVE_BASE+TVE_SLAVE, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_CFG3, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_CFG4, 0x00000000);
	} 
	else // PAL
	{
		write32(F1C100S_TVE_BASE+TVE_ENABLE, 0x00000300);
		write32(F1C100S_TVE_BASE+TVE_CFG1, 0x07070001);
		write32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x30001400);
		write32(F1C100S_TVE_BASE+TVE_CHROMA_FREQ, 0x2A098ACB);
		write32(F1C100S_TVE_BASE+TVE_FB_PORCH, 0x008A0018);
		write32(F1C100S_TVE_BASE+TVE_HD_VS, 0x00000016);
		write32(F1C100S_TVE_BASE+TVE_LINE_NUM, 0x00160271);
		write32(F1C100S_TVE_BASE+TVE_LEVEL, 0x00FC00FC);
		write32(F1C100S_TVE_BASE+TVE_CB_RESET, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_VS_NUM, 0x00000001);
		write32(F1C100S_TVE_BASE+TVE_FILTER, 0x00000005);
		write32(F1C100S_TVE_BASE+TVE_CBCR_LEVEL, 0x00002929);
		write32(F1C100S_TVE_BASE+TVE_TINT_PHASE, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_B_WIDTH, 0x0016447E);
		write32(F1C100S_TVE_BASE+TVE_CBCR_GAIN, 0x0000A8A8);
		write32(F1C100S_TVE_BASE+TVE_SYNC_LEVEL, 0x001000FC);
		write32(F1C100S_TVE_BASE+TVE_WHITE_LEVEL, 0x01E80320);
		write32(F1C100S_TVE_BASE+TVE_ACT_LINE, 0x000005A0);
		write32(F1C100S_TVE_BASE+TVE_CHROMA_BW, 0x00010000);
		write32(F1C100S_TVE_BASE+TVE_CFG2, 0x00000101);
		write32(F1C100S_TVE_BASE+TVE_RESYNC, 0x2005000A);
		write32(F1C100S_TVE_BASE+TVE_SLAVE, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_CFG3, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_CFG4, 0x00000000);
		write32(F1C100S_TVE_BASE+TVE_REG_3A0, 0x00030001);
	}
}

void tve_dac_autocheck_enable(void)
{
	write32(F1C100S_TVE_BASE+TVE_REG_0F8, 0x00000280);
	/* 20ms x 10 */
	write32(F1C100S_TVE_BASE+TVE_REG_0FC, 0x028F00FF);
	/* 1.0v refer for 0.71v/1.43v detect */
	write32(F1C100S_TVE_BASE+TVE_AUTO_DEB, 0x00000009);
	/* detect enable */
	write32(F1C100S_TVE_BASE+TVE_AUTO_EN, 0x00000001);
}

void tve_dac_autocheck_disable(void)
{
	write32(F1C100S_TVE_BASE+TVE_AUTO_EN, 0x0);
	write32(F1C100S_TVE_BASE+TVE_REG_0F8, 0x0);
}

static void tve_dac_auto_cali(uint32_t cali)
{
    uint32_t dac_err_reg[10];
    int32_t dac_err_val[10];
	uint32_t cali_err_1[10];
	uint32_t cali_err_2[10];
	int32_t dac_err_auto;
	int32_t dac_err_efuse;
	int32_t dac_err_opti;
	uint32_t dac_err_set;
	uint32_t i;

	write32(F1C100S_TVE_BASE+TVE_ENABLE, 0x80000300); /* tvclk enable */
	write32(F1C100S_TVE_BASE+TVE_AUTO_EN, 0x00000000); /* auto detect disable */
	write32(F1C100S_TVE_BASE+TVE_DAC1, 0x433e12b1); /* dac setting */

	write32(F1C100S_TVE_BASE+TVE_REG_304, cali << 16 | 0xc << 8);
	set32(F1C100S_TVE_BASE+TVE_REG_300, 0x1);	/* force DAC for cali */
	for (i = 0; i < 3; i++)
	{
		set32(F1C100S_TVE_BASE+TVE_REG_304, 0x1 << 4);
		sdelay(10000);
		set32(F1C100S_TVE_BASE+TVE_REG_304, 0x1 << 0);
		sdelay(10000);
		dac_err_reg[i] = (read32(F1C100S_TVE_BASE+TVE_REG_308) >> 16) & 0x3ff;
		clear32(F1C100S_TVE_BASE+TVE_REG_304, 0x1 << 0);
		clear32(F1C100S_TVE_BASE+TVE_REG_304, 0x1 << 4);
		sdelay(10000);
		if (dac_err_reg[i] & (1 << 9))
			dac_err_val[i] = 0 + (dac_err_reg[i] & 0x1ff);
		else
			dac_err_val[i] = 0 - (dac_err_reg[i] & 0x1ff);
	}

	cali_err_1[0] = abs(dac_err_val[1] - dac_err_val[0]);
	cali_err_1[1] = abs(dac_err_val[2] - dac_err_val[1]);
	cali_err_1[2] = abs(dac_err_val[0] - dac_err_val[2]);

	cali_err_2[0] = cali_err_1[2] + cali_err_1[0];
	cali_err_2[1] = cali_err_1[0] + cali_err_1[1];
	cali_err_2[2] = cali_err_1[1] + cali_err_1[2];

	if (cali_err_2[0] < cali_err_2[1] && cali_err_2[0] < cali_err_2[2])
		dac_err_auto = dac_err_val[0];
	else if (cali_err_2[1] < cali_err_2[0] && cali_err_2[1] < cali_err_2[2])
		dac_err_auto = dac_err_val[1];
	else
		dac_err_auto = dac_err_val[2];

	if (cali & (1 << 9))
		dac_err_efuse = 0 + (cali & 0x1ff);
	else
		dac_err_efuse = 0 - (cali & 0x1ff);

	if (abs(dac_err_auto - dac_err_efuse) < 100)
		dac_err_opti = dac_err_auto;
	else
		dac_err_opti = dac_err_efuse;

	if (dac_err_opti >= 0)
		dac_err_set = (1 << 9) | dac_err_opti;
	else
		dac_err_set = 0 - dac_err_opti;

	write32(F1C100S_TVE_BASE+TVE_REG_304, dac_err_set << 16 | 0xc << 8);
	clear32(F1C100S_TVE_BASE+TVE_REG_300, 0x1);
	write32(F1C100S_TVE_BASE+TVE_AUTO_EN, 0x1);	/* enable plug detect */
}

void tve_enhance(uint8_t mode)
{
	if (0 == mode) 
	{
		clear32(F1C100S_TVE_BASE+TVE_ENABLE, 0xf << 10);	/* deflick off */
		set32(F1C100S_TVE_BASE+TVE_ENABLE, 0x5 << 10);	/* deflick is 5 */
		set32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x1U << 31);	/* lti on */
		set32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x1U << 16);	/* notch off */
	} 
	else if (1 == mode) 
	{
		clear32(F1C100S_TVE_BASE+TVE_ENABLE, 0xf << 10);	/* deflick off */
		set32(F1C100S_TVE_BASE+TVE_ENABLE, 0x5 << 10);	/* deflick is 5 */
		set32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x1U << 31);	/* lti on */
		clear32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x1U << 16);	/* notch on */
	} 
	else if (2 == mode) 
	{
		clear32(F1C100S_TVE_BASE+TVE_ENABLE, 0xf << 10);	/* deflick off */
		clear32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x1U << 31);	/* lti off */
		set32(F1C100S_TVE_BASE+TVE_NOTCH_DELAY, 0x1U << 16);	/* notch off */
	}
}
