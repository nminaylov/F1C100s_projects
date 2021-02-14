#ifndef __F1C100S_DE_H__
#define __F1C100S_DE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
    DE_LCD_R_5BITS = (1 << 2),
    DE_LCD_R_6BITS = (0 << 2),
    DE_LCD_G_5BITS = (1 << 1),
    DE_LCD_G_6BITS = (0 << 1),
    DE_LCD_B_5BITS = (1 << 0),
    DE_LCD_B_6BITS = (0 << 0),
} de_lcd_bus_e;

typedef enum
{
    DEBE_UPDATE_MANUAL = 3,
    DEBE_UPDATE_AUTO = 0,
} debe_reg_update_e;

typedef enum
{
    DEBE_1BPP   = (1 << 8),
    DEBE_2BPP   = (2 << 8),
    DEBE_4BPP   = (4 << 8),
    DEBE_8BPP   = (8 << 8),
    DEBE_16BPP  = (16 << 8),
    DEBE_24BPP  = (24 << 8),
    DEBE_32BPP  = (32 << 8),
} debe_color_mode_bpp_e;

#define DEBE_PALETTE_EN 0x80

typedef enum
{
    DEBE_MODE__1BPP_MONO        = 0 | DEBE_1BPP,
    DEBE_MODE__2BPP_MONO        = 1 | DEBE_2BPP,
    DEBE_MODE__4BPP_MONO        = 2 | DEBE_4BPP,
    DEBE_MODE__8BPP_MONO        = 3 | DEBE_8BPP,
    DEBE_MODE_16BPP_RGB_655     = 4 | DEBE_16BPP,
    DEBE_MODE_16BPP_RGB_565     = 5 | DEBE_16BPP,
    DEBE_MODE_16BPP_RGB_556     = 6 | DEBE_16BPP,
    DEBE_MODE_16BPP_ARGB_1555   = 7 | DEBE_16BPP,
    DEBE_MODE_16BPP_RGBA_5551   = 8 | DEBE_16BPP,
    DEBE_MODE_32BPP_RGB_888     = 9 | DEBE_32BPP,
    DEBE_MODE_32BPP_ARGB_8888   = 10 | DEBE_32BPP,
    DEBE_MODE_24BPP_RGB_888     = 11 | DEBE_24BPP,
    DEBE_MODE__1BPP_PALETTE     = 0 | DEBE_PALETTE_EN | DEBE_1BPP,
    DEBE_MODE__2BPP_PALETTE     = 1 | DEBE_PALETTE_EN | DEBE_2BPP,
    DEBE_MODE__4BPP_PALETTE     = 2 | DEBE_PALETTE_EN | DEBE_4BPP,
    DEBE_MODE__8BPP_PALETTE     = 3 | DEBE_PALETTE_EN | DEBE_8BPP,
} debe_color_mode_e;

typedef struct
{
    int32_t width;
    int32_t height;
    int32_t bus_width;

    int32_t pixel_clock_hz;
    int32_t h_front_porch;
    int32_t h_back_porch;
    int32_t h_sync_len;
    int32_t v_front_porch;
    int32_t v_back_porch;
    int32_t v_sync_len;
    int32_t h_sync_invert;
    int32_t v_sync_invert;
} de_config_t;

void debe_set_bg_color(uint32_t color);
void debe_layer_enable(uint8_t layer);
void debe_layer_disable(uint8_t layer);
void debe_layer_init(uint8_t layer);
void debe_layer_set_pos(uint8_t layer, int16_t x, int16_t y);
void debe_layer_set_size(uint8_t layer, uint16_t w, uint16_t h);
void debe_layer_set_mode(uint8_t layer, debe_color_mode_e mode);
void debe_layer_set_addr(uint8_t layer, void *buf);
void debe_layer_set_alpha(uint8_t layer, uint8_t alpha);
void debe_write_palette(uint32_t *data, uint16_t len);
void debe_load(debe_reg_update_e mode);

void de_init(de_config_t *params);
void de_enable(void);
void de_diable(void);

/************** Display Engine regs ***************/

#define F1C100S_TCON_BASE (0x01C0C000)
#define F1C100S_DEBE_BASE (0x01E60000)
#define F1C100S_DEFE_BASE (0x01E00000)

struct f1c100s_tcon_reg_t
{
    uint32_t ctrl;                /* 0x00 */
    uint32_t int0;                /* 0x04 */
    uint32_t int1;                /* 0x08 */
    uint8_t res0[0x04];           /* 0x0c */
    uint32_t tcon0_frm_ctrl;      /* 0x10 */
    uint32_t tcon0_frm_seed[6];   /* 0x14 */
    uint32_t tcon0_frm_table[4];  /* 0x2c */
    uint8_t res1[4];              /* 0x3c */
    uint32_t tcon0_ctrl;          /* 0x40 */
    uint32_t tcon0_dclk;          /* 0x44 */
    uint32_t tcon0_timing_active; /* 0x48 */
    uint32_t tcon0_timing_h;      /* 0x4c */
    uint32_t tcon0_timing_v;      /* 0x50 */
    uint32_t tcon0_timing_sync;   /* 0x54 */
    uint32_t tcon0_hv_intf;       /* 0x58 */
    uint8_t res2[0x04];           /* 0x5c */
    uint32_t tcon0_cpu_intf;      /* 0x60 */
    uint32_t tcon0_cpu_wr_dat;    /* 0x64 */
    uint32_t tcon0_cpu_rd_dat0;   /* 0x68 */
    uint32_t tcon0_cpu_rd_dat1;   /* 0x6c */
    uint32_t res3[6];
    uint32_t tcon0_io_polarity;   /* 0x88 */
    uint32_t tcon0_io_tristate;   /* 0x8c */
    uint32_t tcon1_ctrl;          /* 0x90 */
    uint32_t tcon1_timing_source; /* 0x94 */
    uint32_t tcon1_timing_scale;  /* 0x98 */
    uint32_t tcon1_timing_out;    /* 0x9c */
    uint32_t tcon1_timing_h;      /* 0xa0 */
    uint32_t tcon1_timing_v;      /* 0xa4 */
    uint32_t tcon1_timing_sync;   /* 0xa8 */
    uint8_t res4[0x44];           /* 0xac */
    uint32_t tcon1_io_polarity;   /* 0xf0 */
    uint32_t tcon1_io_tristate;   /* 0xf4 */
    uint32_t res5[2];
    uint32_t tcon_debug; /* 0xfC */
};

struct f1c100s_debe_reg_t
{
    uint8_t res0[0x800];            /* 0x000 */
    uint32_t mode;                  /* 0x800 */
    uint32_t backcolor;             /* 0x804 */
    uint32_t disp_size;             /* 0x808 */
    uint8_t res1[0x4];              /* 0x80c */
    uint32_t layer_size[4];         /* 0x810 */
    uint32_t layer_pos[4];          /* 0x820 */
    uint8_t res2[0x10];             /* 0x830 */
    uint32_t layer_stride[4];       /* 0x840 */
    uint32_t layer_addr[4];         /* 0x850 */
    uint32_t res_addr[4];           /* 0x860 */
    uint32_t reg_ctrl;              /* 0x870 */
    uint8_t res3[0xc];              /* 0x874 */
    uint32_t color_key_max;         /* 0x880 */
    uint32_t color_key_min;         /* 0x884 */
    uint32_t color_key_config;      /* 0x888 */
    uint8_t res4[0x4];              /* 0x88c */
    uint32_t layer_attr0_ctrl[4];   /* 0x890 */
    uint32_t layer_attr1_ctrl[4];   /* 0x8a0 */
    uint8_t res5[0x110];            /* 0x8b0 */
    uint32_t output_color_ctrl;     /* 0x9c0 */
    uint8_t res6[0xc];              /* 0x9c4 */
    uint32_t output_color_coef[12]; /* 0x9d0 */
};

struct f1c100s_defe_reg_t
{
    uint32_t enable;                  /* 0x000 */
    uint32_t frame_ctrl;              /* 0x004 */
    uint32_t bypass;                  /* 0x008 */
    uint32_t algorithm_sel;           /* 0x00c */
    uint32_t line_int_ctrl;           /* 0x010 */
    uint8_t res0[0x0c];               /* 0x014 */
    uint32_t ch0_addr;                /* 0x020 */
    uint32_t ch1_addr;                /* 0x024 */
    uint32_t ch2_addr;                /* 0x028 */
    uint32_t field_sequence;          /* 0x02c */
    uint32_t ch0_offset;              /* 0x030 */
    uint32_t ch1_offset;              /* 0x034 */
    uint32_t ch2_offset;              /* 0x038 */
    uint8_t res1[0x04];               /* 0x03c */
    uint32_t ch0_stride;              /* 0x040 */
    uint32_t ch1_stride;              /* 0x044 */
    uint32_t ch2_stride;              /* 0x048 */
    uint32_t input_fmt;               /* 0x04c */
    uint32_t ch3_addr;                /* 0x050 */
    uint32_t ch4_addr;                /* 0x054 */
    uint32_t ch5_addr;                /* 0x058 */
    uint32_t output_fmt;              /* 0x05c */
    uint32_t int_enable;              /* 0x060 */
    uint32_t int_status;              /* 0x064 */
    uint32_t status;                  /* 0x068 */
    uint8_t res2[0x04];               /* 0x06c */
    uint32_t csc_coef00;              /* 0x070 */
    uint32_t csc_coef01;              /* 0x074 */
    uint32_t csc_coef02;              /* 0x078 */
    uint32_t csc_coef03;              /* 0x07c */
    uint32_t csc_coef10;              /* 0x080 */
    uint32_t csc_coef11;              /* 0x084 */
    uint32_t csc_coef12;              /* 0x088 */
    uint32_t csc_coef13;              /* 0x08c */
    uint32_t csc_coef20;              /* 0x090 */
    uint32_t csc_coef21;              /* 0x094 */
    uint32_t csc_coef22;              /* 0x098 */
    uint32_t csc_coef23;              /* 0x09c */
    uint32_t deinterlace_ctrl;        /* 0x0a0 */
    uint32_t deinterlace_diag;        /* 0x0a4 */
    uint32_t deinterlace_tempdiff;    /* 0x0a8 */
    uint32_t deinterlace_sawtooth;    /* 0x0ac */
    uint32_t deinterlace_spatcomp;    /* 0x0b0 */
    uint32_t deinterlace_burstlen;    /* 0x0b4 */
    uint32_t deinterlace_preluma;     /* 0x0b8 */
    uint32_t deinterlace_tile_addr;   /* 0x0bc */
    uint32_t deinterlace_tile_stride; /* 0x0c0 */
    uint8_t res3[0x0c];               /* 0x0c4 */
    uint32_t wb_stride_enable;        /* 0x0d0 */
    uint32_t ch3_stride;              /* 0x0d4 */
    uint32_t ch4_stride;              /* 0x0d8 */
    uint32_t ch5_stride;              /* 0x0dc */
    uint32_t fe_3d_ctrl;              /* 0x0e0 */
    uint32_t fe_3d_ch0_addr;          /* 0x0e4 */
    uint32_t fe_3d_ch1_addr;          /* 0x0e8 */
    uint32_t fe_3d_ch2_addr;          /* 0x0ec */
    uint32_t fe_3d_ch0_offset;        /* 0x0f0 */
    uint32_t fe_3d_ch1_offset;        /* 0x0f4 */
    uint32_t fe_3d_ch2_offset;        /* 0x0f8 */
    uint8_t res4[0x04];               /* 0x0fc */
    uint32_t ch0_insize;              /* 0x100 */
    uint32_t ch0_outsize;             /* 0x104 */
    uint32_t ch0_horzfact;            /* 0x108 */
    uint32_t ch0_vertfact;            /* 0x10c */
    uint32_t ch0_horzphase;           /* 0x110 */
    uint32_t ch0_vertphase0;          /* 0x114 */
    uint32_t ch0_vertphase1;          /* 0x118 */
    uint8_t res5[0x04];               /* 0x11c */
    uint32_t ch0_horztapoffset0;      /* 0x120 */
    uint32_t ch0_horztapoffset1;      /* 0x124 */
    uint32_t ch0_verttapoffset;       /* 0x128 */
    uint8_t res6[0xd4];               /* 0x12c */
    uint32_t ch1_insize;              /* 0x200 */
    uint32_t ch1_outsize;             /* 0x204 */
    uint32_t ch1_horzfact;            /* 0x208 */
    uint32_t ch1_vertfact;            /* 0x20c */
    uint32_t ch1_horzphase;           /* 0x210 */
    uint32_t ch1_vertphase0;          /* 0x214 */
    uint32_t ch1_vertphase1;          /* 0x218 */
    uint8_t res7[0x04];               /* 0x21c */
    uint32_t ch1_horztapoffset0;      /* 0x220 */
    uint32_t ch1_horztapoffset1;      /* 0x224 */
    uint32_t ch1_verttapoffset;       /* 0x228 */
    uint8_t res8[0x1d4];              /* 0x22c */
    uint32_t ch0_horzcoef0[32];       /* 0x400 */
    uint32_t ch0_horzcoef1[32];       /* 0x480 */
    uint32_t ch0_vertcoef[32];        /* 0x500 */
    uint8_t res9[0x80];               /* 0x580 */
    uint32_t ch1_horzcoef0[32];       /* 0x600 */
    uint32_t ch1_horzcoef1[32];       /* 0x680 */
    uint32_t ch1_vertcoef[32];        /* 0x700 */
    uint8_t res10[0x280];             /* 0x780 */
    uint32_t vpp_enable;              /* 0xa00 */
    uint32_t vpp_dcti;                /* 0xa04 */
    uint32_t vpp_lp1;                 /* 0xa08 */
    uint32_t vpp_lp2;                 /* 0xa0c */
    uint32_t vpp_wle;                 /* 0xa10 */
    uint32_t vpp_ble;                 /* 0xa14 */
};

#ifdef __cplusplus
}
#endif

#endif /* __F1C100S_DE_H__ */
