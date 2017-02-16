/*
 * drivers/devfreq/dramfreq/sunxi-mdfs.c
 *
 * Copyright(c) 2013-2015 Allwinnertech Co., Ltd.
 *
 * Author: Pan Nan <pannan@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/ctype.h>
#include <asm/memory.h>
#include "sunxi-mdfs.h"

#if (0)
#define __MDFS_DEBUG
#else
#undef  __MDFS_DEBUG
#endif

#ifdef __MDFS_DEBUG
#define MDFS_DBG(format,args...)    mdfs_printk("[mdfs] "format,##args)
#else
#define MDFS_DBG(format,args...)
#endif

static unsigned int __sramdata sp_backup;
static unsigned int __sramdata __sram_pllx_para;
static unsigned int __sramdata __sram_div;
static unsigned int __sramdata __sram_jump;
static __dram_para_t __sramdata __sram_dram_para;

#ifdef __MDFS_DEBUG
static void __sram __mdfs_serial_put_char (char c)
{
  while (! (mctl_read_w (SUART_USR) & 2) );
  mctl_write_w (c, SUART_THR);
}
static int __sram __mdfs_serial_puts (const char * string)
{
  while (*string != '\0') {
    if (*string == '\n') {
      __mdfs_serial_put_char ('\r');
    }
    __mdfs_serial_put_char (*string++);
  }
  
  return 0;
}

static unsigned int __sram __mdfs_strlen (const char * s)
{
  const char * sc;
  
  for (sc = s; *sc != '\0'; ++sc)
  {
    /* nothing */
    ;
  }
  return sc - s;
}
static char __sram * __mdfs_strcpy (char * dest, const char * src)
{
  char * tmp = dest;
  
  while ( (*dest++ = *src++) != '\0')
  {
    /* nothing */
    ;
  }
  return tmp;
}
static char __sram * __mdfs_itoa (int value, char * string, int radix)
{
  char stack[16];
  int  negative = 0; 
  int  i;
  int  j;
  char digit_string[] = "0123456789ABCDEF";
  
  if (value == 0)
  {
    string[0] = '0';
    string[1] = '\0';
    return string;
  }
  
  if (value < 0)
  {
    negative = 1;
    value = -value ;
  }
  
  for (i = 0; value > 0; ++i)
  {
    stack[i] = digit_string[value % radix];
    value /= radix;
  }
  
  j = 0;
  if (negative)
  {
    string[j++] = '-';
  }
  for (--i; i >= 0; --i, ++j)
  {
    string[j] = stack[i];
  }
  string[j] = '\0';
  
  return string;
}
static char __sram * __mdfs_utoa (unsigned int value, char * string, int radix)
{
  char stack[16];
  int  i;
  int  j;
  char digit_string[] = "0123456789ABCDEF";
  
  if (value == 0)
  {
    string[0] = '0';
    string[1] = '\0';
    return string;
  }
  
  for (i = 0; value > 0; ++i)
  {
    stack[i] = digit_string[value % radix];
    value /= radix;
  }
  
  for (--i, j = 0; i >= 0; --i, ++j)
  {
    string[j] = stack[i];
  }
  string[j] = '\0';
  
  return string;
}
static char __sram * __mdfs_strncat (char * dest, const char * src, unsigned int count)
{
  char * tmp = dest;
  
  if (count)
  {
    while (*dest)
    {
      dest++;
    }
    while ( (*dest++ = *src++) != 0)
    {
      if (--count == 0)
      {
        *dest = '\0';
        break;
      }
    }
  }
  return tmp;
}
static int __sram __mdfs_print_align (char * string, int len, int align)
{
  char fill_ch[] = "                ";
  if (len < align)
  {
    __mdfs_strncat (string, fill_ch, align - len);
    return align - len;
  }
  return 0;
}
static char __sramdata mdfs_debugger_buffer[256];
static int __sram mdfs_printk (const char * format, ...)
{
  va_list args;
  char string[16];
  char * pdest, *psrc;
  int align, len = 0;
  
  pdest = mdfs_debugger_buffer;
  va_start (args, format);
  while (*format)
  {
    if (*format == '%')
    {
      ++format;
      if ( ('0' < (*format) ) && ( (*format) <= '9') )
      {
        align = *format - '0';
        ++format;
      }
      else
      {
        align = 0;
      }
      switch (*format)
      {
      case 'd':
        {
          __mdfs_itoa (va_arg (args, int), string, 10);
          len = __mdfs_strlen (string);
          len += __mdfs_print_align (string, len, align);
          __mdfs_strcpy (pdest, string);
          pdest += len;
          break;
        }
      case 'x':
      case 'p':
        {
          __mdfs_utoa (va_arg (args, int), string, 16);
          len = __mdfs_strlen (string);
          len += __mdfs_print_align (string, len, align);
          __mdfs_strcpy (pdest, string);
          pdest += len;
          break;
        }
      case 'u':
        {
          __mdfs_utoa (va_arg (args, int), string, 10);
          len = __mdfs_strlen (string);
          len += __mdfs_print_align (string, len, align);
          __mdfs_strcpy (pdest, string);
          pdest += len;
          break;
        }
      case 'c':
        {
          *pdest = (char) va_arg (args, int);
          break;
        }
      case 's':
        {
          psrc = va_arg (args, char *);
          __mdfs_strcpy (pdest, psrc);
          pdest += __mdfs_strlen (psrc);
          break;
        }
      default :
        {
          *pdest++ = '%';
          *pdest++ = *format;
        }
      }
    }
    else
    {
      *pdest++ = *format;
    }
    ++format;
  }
  va_end (args);
  
  *pdest = '\0';
  pdest++;
  __mdfs_serial_puts (mdfs_debugger_buffer);
  
  return (pdest - mdfs_debugger_buffer);
}
#endif /* __MDFS_DEBUG */

#ifndef CONFIG_ARCH_SUN8IW6P1
static unsigned long long __sram __cnt64_read (void)
{
  volatile unsigned int high;
  volatile unsigned int low;
  unsigned long long counter;
  
  low = mctl_read_w (CNT64_CTRL_REG);
  low |= (1 << 1);
  mctl_write_w (low, CNT64_CTRL_REG);
  while (mctl_read_w (CNT64_CTRL_REG) & (1 << 1) );
  
  low  = mctl_read_w (CNT64_LOW_REG);
  high = mctl_read_w (CNT64_HIGH_REG);
  
  counter = high;
  counter = (counter << 32) + low;
  
  return counter;
}

static void __sram mdfs_udelay (unsigned int us)
{
  unsigned long long expire = 0;
  
  if (us == 0)
  { return; }
  
  expire = (24 * us) + __cnt64_read();
  while (expire > __cnt64_read() );
}
#endif

static unsigned int __sram __load_tlb (unsigned int addr, unsigned int len)
{
  unsigned int tmp_addr  = addr;
  unsigned int tmp_value = 0;
  
  while (tmp_addr < (addr + len) )
  {
    tmp_value = * ( (unsigned int *) tmp_addr);
    
    tmp_addr += 4 * 1024;
  }
  return tmp_value;
}

static void __sram mdfs_init_tlb (void)
{
  /* load sram code space to tlb */
  __load_tlb (SRAM_DDRFREQ_OFFSET, SRAM_DDRFREQ_SP_ADDR - SRAM_DDRFREQ_OFFSET);
  
  /* load dram controller to tlb */
  __load_tlb (MCTL_COM_BASE, 0x1000);
  #if defined(CONFIG_ARCH_SUN9IW1P1)
  __load_tlb (MCTL_CTL_BASE, 0x2000);
  __load_tlb (MCTL_PHY_BASE, 0x2000);
  #elif defined(CONFIG_ARCH_SUN8IW5P1) || defined(CONFIG_ARCH_SUN8IW6P1) || defined(CONFIG_ARCH_SUN8IW7P1)
  __load_tlb (MCTL_CTL_BASE, 0x1000);
  #endif
  
  /* load ccm to tlb */
  __load_tlb (CCM_PLL_BASE,  0x1000);
  
  #ifdef __MDFS_DEBUG
  /* load uart0 to tlb */
  __load_tlb (UART_BASE,     0x1000);
  #endif
  
  #if defined(CONFIG_ARCH_SUN8IW6P1) || defined(CONFIG_ARCH_SUN8IW7P1)
  /* load counter-64 to tlb */
  __load_tlb (R_CPU_CFG_REG, 0x1000);
  #else
  /* load sys-power-off-gating and counter-64 to tlb */
  __load_tlb (R_PRCM_BASE,   0x1000);
  #endif
}

static void __sram mdfs_init_para (unsigned int jump, __dram_para_t * dram_para,
                                   unsigned int pll_para_from_dram, unsigned int div)
{
  __sram_dram_para.dram_clk = dram_para->dram_clk;
  __sram_pllx_para = pll_para_from_dram;
  __sram_jump = jump;
  #if defined(CONFIG_ARCH_SUN9IW1P1)
  __sram_dram_para.dram_type = dram_para->dram_type;
  __sram_dram_para.dram_tpr2 = dram_para->dram_tpr2;
  __sram_div = div;
  #elif defined(CONFIG_ARCH_SUN8IW5P1)
  __sram_dram_para.dram_tpr13 = dram_para->dram_tpr13;
  __sram_dram_para.dram_tpr2 = dram_para->dram_tpr2;
  #elif defined(CONFIG_ARCH_SUN8IW6P1)
  __sram_dram_para.dram_odt_en = dram_para->dram_odt_en;
  #endif
}

#if defined(CONFIG_ARCH_SUN9IW1P1)
static void __sram mdfs_start (int type, int freq_jump, __dram_para_t * para,
                               unsigned int pll6_para, unsigned int div)
{
  unsigned int reg_val = 0;
  unsigned int ch_num = 0 ;
  unsigned int rank_num = 0 ;
  unsigned int dev_clk = para->dram_clk;
  unsigned int pll_ddr = pll6_para;
  unsigned int ret_val = 0;
  unsigned int dram_div = div;
  unsigned int i = 0, j = 0;
  unsigned int rank0_lcdlr2 = 0;
  unsigned int rank1_lcdlr2 = 0;
  unsigned int rank0_gtr = 0;
  unsigned int rank1_gtr = 0;
  unsigned int tprd = 0;
  unsigned int zq0cr = 0;
  
  reg_val = mctl_read_w (MC_CR);
  if ( (reg_val >> 19) & 0x1)
  { ch_num = 1; }    
  if (reg_val & 0x1)
  { rank_num = 1; }  
  
  if (ch_num) {
    mctl_write_w (0x1, M0_DCMDAPC);         
    mctl_write_w (0x1, M0_DCMDAPC + 0x1000);
    mdfs_udelay (20);
    
    mctl_write_w (0x9, M0_DRAMTMG8);
    mctl_write_w (0x9, M0_DRAMTMG8 + 0x1000);
    
    zq0cr = mctl_read_w (P0_ZQ0CR);
    reg_val = (zq0cr & (~ (0x7 << 11) ) );
    mctl_write_w (reg_val, P0_ZQ0CR);
    mctl_write_w (reg_val, P0_ZQ0CR + 0x1000);
    
    mctl_write_w (0x01, M0_RFSHCTL3);
    mctl_write_w (0x01, M0_RFSHCTL3 + 0x1000);
    
    reg_val = mctl_read_w (M0_RFSHTMG);
    reg_val &= ~ (0xfff0000);
    if (dev_clk < 300)
    { reg_val |= ( ( (para->dram_tpr2) & 0xffc) << 14); }
    else
      if (dev_clk < 672)
      { reg_val |= ( ( (para->dram_tpr2) & 0xffe) << 15); }
      else
      { reg_val |= ( ( (para->dram_tpr2) & 0xfff) << 16); }
    mctl_write_w (reg_val, M0_RFSHTMG);
    mctl_write_w (reg_val, M0_RFSHTMG + 0x1000);
    
    mctl_write_w (1, M0_PWRCTL);
    mctl_write_w (1, M0_PWRCTL + 0x1000);
    mdfs_udelay (10);
    
    while ( ( (mctl_read_w (M0_STATR) & 0x7) != 0x3) );
    while ( ( (mctl_read_w (M0_STATR + 0x1000) & 0x7) != 0x3) );
    
    if (dram_div != 0) {
      reg_val = mctl_read_w (CCM_DRAMCLK_CFG_REG);
      reg_val &= ~ (0xf << 8);
      reg_val |= (0x3 << 12);
      reg_val |= ( (dram_div - 1) << 8);
      mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
      reg_val |= (0x1 << 16);
      mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
      mdfs_udelay (1);
    }
    else {
      if ( (dev_clk == 480) || (dev_clk == 240) ) {
        reg_val = mctl_read_w (CCM_DRAMCLK_CFG_REG);
        reg_val &= ~ (0xf << 8);
        reg_val &= ~ (0x3 << 12);
        if (dev_clk == 240)
        { reg_val |= (0x1 << 8); }
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        reg_val |= (0x1 << 16);
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        mdfs_udelay (1);
      }
      else {
        mctl_write_w (0x50, CCM_PLL_BASE + 0x090);
        
        reg_val = mctl_read_w (CCM_DRAMCLK_CFG_REG);
        reg_val &= ~ (0xf << 8);
        reg_val |= (0x3 << 12);
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        reg_val |= (0x1 << 16);
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        mdfs_udelay (1);
        
        mctl_write_w (pll_ddr, CCM_PLL6_DDR_REG);
        pll_ddr |= (1U << 30);
        mctl_write_w (pll_ddr, CCM_PLL6_DDR_REG);
        
        mdfs_udelay (1);
        
        do {
          reg_val = (mctl_read_w (CCM_PLL_BASE + 0x09c) >> 5) & 0x1;
        }
        while (reg_val == 0);
      }
    }
    
    mctl_write_w (0x25800708, P0_PTR1);
    mctl_write_w (0x25800708, P0_PTR1 + 0x1000);
    if (dev_clk <= 800)        
    { ret_val = 0x40020061; }
    else
    { ret_val = 0x40000071; }  
    mctl_write_w (ret_val, P0_PIR);
    mctl_write_w (ret_val, P0_PIR + 0x1000);
    mdfs_udelay (1);
    while ( (mctl_read_w (P0_PGSR0) & 0x1) != 0x1);
    while ( (mctl_read_w (P0_PGSR0 + 0x1000) & 0x1) != 0x1);
    
    mctl_write_w (0, M0_PWRCTL);
    mctl_write_w (0, M0_PWRCTL + 0x1000);
    
    mdfs_udelay (200);
    
    while ( ( (mctl_read_w (M0_STATR) & 0x7) != 0x1) );
    while ( ( (mctl_read_w (M0_STATR + 0x1000) & 0x7) != 0x1) );
    
    reg_val = mctl_read_w (P0_DSGCR);
    reg_val &= (~ (0x3 << 6) );
    mctl_write_w (reg_val , P0_DSGCR );
    
    mctl_write_w (reg_val , P0_DSGCR + 0x1000 );
    
    if (dev_clk <= 800) {
      if (para->dram_type == 3)
      { ret_val = 0x40020c01; }
      else
      { ret_val = 0x40020401; }//ret_val = 0x40020c01;
    }
    else {
      if (para->dram_type == 3)
      { ret_val = 0x40000c01; }
      else
      { ret_val = 0x40000401; }//ret_val = 0x40000c01;
    }
    mctl_write_w (ret_val, P0_PIR);
    mctl_write_w (ret_val, P0_PIR + 0x1000);
    mdfs_udelay (1);
    while ( (mctl_read_w (P0_PGSR0) & 0x1) != 0x1);        
    while ( (mctl_read_w (P0_PGSR0 + 0x1000) & 0x1) != 0x1);
    
    if (para->dram_type == 3) {
      if (dev_clk < 400) {
        mctl_write_w (0x0, M0_ODTMAP);
        mctl_write_w (0x0, M0_ODTMAP + 0x1000);
      }
      else {
        mctl_write_w (0x00002211, M0_ODTMAP);
        mctl_write_w (0x00002211, M0_ODTMAP + 0x1000);
      }
    }
    
    if (rank_num) {
      for (i = 0; i < 4; i++) {
        reg_val = mctl_read_w (P0_DX0LCDLR2 + i * 0x80);
        rank0_lcdlr2 = (reg_val & 0xff);
        rank1_lcdlr2 = ( (reg_val >> 8) & 0xff);
        
        reg_val = mctl_read_w (P0_DX0GTR + i * 0x80);
        rank0_gtr = (reg_val & 0x7);
        rank1_gtr = ( (reg_val >> 3) & 0x7);
        
        reg_val = mctl_read_w (P0_DX0MDLR + i * 0x80);
        tprd = ( (reg_val >> 8) & 0xff);
        
        reg_val = (rank0_lcdlr2 + rank1_lcdlr2 + ( (rank0_gtr + rank1_gtr) * tprd) );
        reg_val >>= 1;
        
        for (j = 0; tprd <= reg_val; j++)
        { reg_val -= tprd; }
        rank0_lcdlr2 = reg_val;
        rank1_lcdlr2 = reg_val;
        rank0_gtr = j;
        rank1_gtr = j;
        mctl_write_w ( (rank0_lcdlr2 | (rank1_lcdlr2 << 8) ), (P0_DX0LCDLR2 + i * 0x80) );
        
        reg_val = mctl_read_w (P0_DX0GTR + i * 0x80);
        reg_val &= ~ (0x3f);
        reg_val |= ( (rank1_gtr << 3) | rank0_gtr);
        mctl_write_w (reg_val, (P0_DX0GTR + i * 0x80) );
      }
      
      for (i = 0; i < 4; i++) {
        reg_val = mctl_read_w (P0_DX0LCDLR2 + 0x1000 + i * 0x80);
        rank0_lcdlr2 = (reg_val & 0xff);
        rank1_lcdlr2 = ( (reg_val >> 8) & 0xff);
        
        reg_val = mctl_read_w (P0_DX0GTR + 0x1000 + i * 0x80);
        rank0_gtr = (reg_val & 0x7);
        rank1_gtr = ( (reg_val >> 3) & 0x7);
        
        reg_val = mctl_read_w (P0_DX0MDLR + 0x1000 + i * 0x80);
        tprd = ( (reg_val >> 8) & 0xff);
        
        reg_val = (rank0_lcdlr2 + rank1_lcdlr2 + ( (rank0_gtr + rank1_gtr) * tprd) );
        reg_val >>= 1;
        
        for (j = 0; tprd <= reg_val; j++)
        { reg_val -= tprd; }
        rank0_lcdlr2 = reg_val;
        rank1_lcdlr2 = reg_val;
        rank0_gtr = j;
        rank1_gtr = j;
        mctl_write_w ( (rank0_lcdlr2 | (rank1_lcdlr2 << 8) ), (P0_DX0LCDLR2 + 0x1000 + i * 0x80) );
        
        reg_val = mctl_read_w (P0_DX0GTR + 0x1000 + i * 0x80);
        reg_val &= ~ (0x3f);
        reg_val |= ( (rank1_gtr << 3) | rank0_gtr);
        mctl_write_w (reg_val, (P0_DX0GTR + 0x1000 + i * 0x80) );
      }
    }
    
    if ( (para->dram_type) == 6 || (para->dram_type) == 7) {
      reg_val = mctl_read_w (P0_DSGCR);
      reg_val &= (~ (0x3 << 6) );
      reg_val |= (0x1 << 6);     
      mctl_write_w (reg_val , P0_DSGCR);
      
      mctl_write_w (reg_val , P0_DSGCR + 0x1000);
    }
    
    mctl_write_w (0x0, M0_RFSHCTL3);
    mctl_write_w (0x0, M0_RFSHCTL3 + 0x1000);
    
    mctl_write_w (0x0, M0_DCMDAPC);        
    mctl_write_w (0x0, M0_DCMDAPC + 0x1000);
    
    mctl_write_w (zq0cr, P0_ZQ0CR);
    mctl_write_w (zq0cr, P0_ZQ0CR + 0x1000);
  }
  else {
    mctl_write_w (0x1, M0_DCMDAPC);        
    mdfs_udelay (20);
    
    mctl_write_w (0x9, M0_DRAMTMG8);
    
    zq0cr = mctl_read_w (P0_ZQ0CR);
    reg_val = (zq0cr & (~ (0x7 << 11) ) );
    mctl_write_w (reg_val, P0_ZQ0CR);
    
    mctl_write_w (0x01, M0_RFSHCTL3);
    
    reg_val = mctl_read_w (M0_RFSHTMG);
    reg_val &= ~ (0xfff0000);
    if (dev_clk < 300)
    { reg_val |= ( ( (para->dram_tpr2) & 0xffc) << 14); }
    else
      if (dev_clk < 672)
      { reg_val |= ( ( (para->dram_tpr2) & 0xffe) << 15); }
      else
      { reg_val |= ( ( (para->dram_tpr2) & 0xfff) << 16); }
    mctl_write_w (reg_val, M0_RFSHTMG);
    
    mctl_write_w (1, M0_PWRCTL);
    
    while ( ( (mctl_read_w (M0_STATR) & 0x7) != 0x3) );
    
    if (dram_div != 0) {
      reg_val = mctl_read_w (CCM_DRAMCLK_CFG_REG);
      reg_val &= ~ (0xf << 8);
      reg_val |= (0x3 << 12);
      reg_val |= ( (dram_div - 1) << 8);
      mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
      reg_val |= (0x1 << 16);
      mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
      mdfs_udelay (1);
    }
    else {
      if ( (dev_clk == 480) || (dev_clk == 240) ) {
        reg_val = mctl_read_w (CCM_DRAMCLK_CFG_REG);
        reg_val &= ~ (0xf << 8);
        reg_val &= ~ (0x3 << 12);
        if (dev_clk == 240)
        { reg_val |= (0x1 << 8); }
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        reg_val |= (0x1 << 16);
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        mdfs_udelay (1);
      }
      else {
        mctl_write_w (0x50, CCM_PLL_BASE + 0x090);
        reg_val = mctl_read_w (CCM_DRAMCLK_CFG_REG);
        reg_val &= ~ (0xf << 8);
        reg_val |= (0x3 << 12);
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        reg_val |= (0x1 << 16);
        mctl_write_w (reg_val, CCM_DRAMCLK_CFG_REG);
        mdfs_udelay (1);
        
        mctl_write_w (pll_ddr, CCM_PLL6_DDR_REG);
        pll_ddr |= (1U << 30);
        mctl_write_w (pll_ddr, CCM_PLL6_DDR_REG);
        
        do {
          reg_val = (mctl_read_w (CCM_PLL_BASE + 0x09c) >> 5) & 0x1;
        }
        while (reg_val == 0);
      }
    }
    
    mctl_write_w (0x25800708, P0_PTR1);
    if (dev_clk <= 800)        
    { ret_val = 0x40020061; }
    else
    { ret_val = 0x40000071; }  
    mctl_write_w (ret_val, P0_PIR);
    mdfs_udelay (1);
    while ( (mctl_read_w (P0_PGSR0) & 0x1) != 0x1);
    
    mctl_write_w (0, M0_PWRCTL);
    
    mdfs_udelay (200);
    
    while ( ( (mctl_read_w (M0_STATR) & 0x7) != 0x1) );
    
    reg_val = mctl_read_w (P0_DSGCR);
    reg_val &= (~ (0x3 << 6) );
    mctl_write_w (reg_val , P0_DSGCR );
    
    if (dev_clk <= 800) {
      if (para->dram_type == 3)
      { ret_val = 0x40020c01; }
      else
      { ret_val = 0x40020401; }//ret_val = 0x40020c01;
    }
    else {
      if (para->dram_type == 3)
      { ret_val = 0x40000c01; }
      else
      { ret_val = 0x40000401; }//ret_val = 0x40000c01;
    }
    mctl_write_w (ret_val , P0_PIR);
    mdfs_udelay (1);
    while ( (mctl_read_w (P0_PGSR0) & 0x1) != 0x1);
    
    if (para->dram_type == 3) {
      if (dev_clk < 400)
      { mctl_write_w (0x0, M0_ODTMAP); }
      else
      { mctl_write_w (0x00002211, M0_ODTMAP); }
    }
    
    if (rank_num) {
      for (i = 0; i < 4; i++) {
        reg_val = mctl_read_w (P0_DX0LCDLR2 + i * 0x80);
        rank0_lcdlr2 = (reg_val & 0xff);
        rank1_lcdlr2 = ( (reg_val >> 8) & 0xff);
        
        reg_val = mctl_read_w (P0_DX0GTR + i * 0x80);
        rank0_gtr = (reg_val & 0x7);
        rank1_gtr = ( (reg_val >> 3) & 0x7);
        
        reg_val = mctl_read_w (P0_DX0MDLR + i * 0x80);
        tprd = ( (reg_val >> 8) & 0xff);
        
        reg_val = (rank0_lcdlr2 + rank1_lcdlr2 + ( (rank0_gtr + rank1_gtr) * tprd) );
        reg_val >>= 1;
        
        for (j = 0; tprd <= reg_val; j++)
        { reg_val -= tprd; }
        rank0_lcdlr2 = reg_val;
        rank1_lcdlr2 = reg_val;
        rank0_gtr = j;
        rank1_gtr = j;
        mctl_write_w ( (rank0_lcdlr2 | (rank1_lcdlr2 << 8) ), (P0_DX0LCDLR2 + i * 0x80) );
        
        reg_val = mctl_read_w (P0_DX0GTR + i * 0x80);
        reg_val &= ~ (0x3f);
        reg_val |= ( (rank1_gtr << 3) | rank0_gtr);
        mctl_write_w (reg_val, (P0_DX0GTR + i * 0x80) );
      }
    }
    
    if ( (para->dram_type) == 6 || (para->dram_type) == 7) {
      reg_val = mctl_read_w (P0_DSGCR);
      reg_val &= (~ (0x3 << 6) );
      reg_val |= (0x1 << 6);     
      mctl_write_w (reg_val, P0_DSGCR);
    }
    
    mctl_write_w (0x0, M0_RFSHCTL3);
    
    mctl_write_w (0x0, M0_DCMDAPC);
    
    mctl_write_w (zq0cr, P0_ZQ0CR);
  }
  
  #if 0
  mctl_write_w (0x8, M0_DRAMTMG8);
  mctl_write_w (0x8, M0_DRAMTMG8 + 0x1000);
  
  if (dev_clk < 200) {
    reg_val = mctl_read_w (M0_PWRCTL);
    reg_val |= ( (0x1 << 0) | (0x1 << 3) );
    mctl_write_w (reg_val, M0_PWRCTL);
    reg_val = mctl_read_w (M0_PWRCTL + 0x1000);
    reg_val |= ( (0x1 << 0) | (0x1 << 3) );
    mctl_write_w (reg_val, M0_PWRCTL + 0x1000);
  }
  else {
    reg_val = mctl_read_w (M0_PWRCTL);
    reg_val &= ~ ( (0x1 << 0) | (0x1 << 3) );
    mctl_write_w (reg_val, M0_PWRCTL);
    reg_val = mctl_read_w (M0_PWRCTL + 0x1000);
    reg_val &= ~ ( (0x1 << 0) | (0x1 << 3) );
    mctl_write_w (reg_val, M0_PWRCTL + 0x1000);
  }
  #endif
}

#elif defined(CONFIG_ARCH_SUN8IW5P1)
static void __sram mdfs_start (int type, int freq_jump, __dram_para_t * para,
                               unsigned int pllx_para, unsigned int div)
{
  unsigned int pll_source = 0;
  unsigned int reg_val = 0;
  unsigned int m_div = 0;
  unsigned int rank_num = 0;
  unsigned int trefi = 0;
  unsigned int trfc = 0;

  if (para->dram_clk <= 192) {
    trefi = 0x16 ;
    trfc = 0x21;
  }
  else {
    trefi = (para->dram_tpr2 >> 0) & 0xfff;
    trfc = (para->dram_tpr2 >> 12) & 0xfff;
  }

  pll_source = (mctl_read_w (_CCM_PLL_DDR_CFG_REG) >> 16) & 0x1;
  mctl_write_w (0x0, MC_MAER);

  reg_val = mctl_read_w (PWRCTL);
  reg_val |= 0x1 << 0;
  reg_val |= 0x1 << 8;
  mctl_write_w (reg_val, PWRCTL);
  mdfs_udelay (100);

  reg_val = 0;
  reg_val = (trefi << 16) | (trfc << 0);
  mctl_write_w (reg_val, RFSHTMG);

  while ( ( (mctl_read_w (STATR) & 0x7) != 0x3) );

  reg_val = mctl_read_w (VDD_SYS_POFF_GATING);
  reg_val |= 0x3 << 0;
  mctl_write_w (reg_val, VDD_SYS_POFF_GATING);
  mdfs_udelay (10);

  mctl_write_w (0x0, MC_CLKEN);
  reg_val = mctl_read_w (_CCM_PLL_DDR0_REG);
  reg_val &= ~ ( (1U << 31) | (0x1 << 24) );
  mctl_write_w (reg_val, _CCM_PLL_DDR0_REG);
  reg_val = mctl_read_w (_CCM_PLL_DDR1_REG);
  reg_val &= ~ ( (1U << 31) | (0x1 << 24) );
  mctl_write_w (reg_val, _CCM_PLL_DDR1_REG);
  mdfs_udelay (100);

  if (para->dram_clk <= 192) {
    if (pll_source) {
      mctl_write_w (0x0, _CCM_PLL_DDR1_PATTERN_REG);
      mctl_write_w (0x80003700, _CCM_PLL_DDR1_REG);
      mctl_write_w (0xc0003700, _CCM_PLL_DDR1_REG);
      mdfs_udelay (1000);
      while (mctl_read_w (_CCM_PLL_DDR1_REG) & (0x1 << 30) ) {}
    }
    else {
      mctl_write_w (0x0, _CCM_PLL_DDR0_PATTERN_REG);
      mctl_write_w (0x80001b10, _CCM_PLL_DDR0_REG);
      mctl_write_w (0x80101b10, _CCM_PLL_DDR0_REG);
      mdfs_udelay (1000);
      while (mctl_read_w (_CCM_PLL_DDR0_REG) & (0x1 << 20) ) {}
    }

    reg_val = mctl_read_w (CCM_DRAM_CFG_REG);
    reg_val &= ~ (0xf << 0);
    reg_val |= (0x1 << 0);
    mctl_write_w (reg_val, CCM_DRAM_CFG_REG);
    reg_val |= (0x1 << 16);       
    mctl_write_w (reg_val, CCM_DRAM_CFG_REG);
    while (mctl_read_w (CCM_DRAM_CFG_REG) & (0x1 << 16) );
    mdfs_udelay (100);
    mctl_write_w (0xc10f, MC_CLKEN);
    mdfs_udelay (100);

    reg_val = mctl_read_w (PLLGCR);
    reg_val |= ( (0x1U << 29) | (0x1U << 31) );
    mctl_write_w (reg_val, PLLGCR);

    reg_val = mctl_read_w (PLLGCR);
    reg_val |= (0x1U << 30);
    mctl_write_w (reg_val, PLLGCR);
    mdfs_udelay (100);
    reg_val = mctl_read_w (PLLGCR);
    reg_val &= ~ (0x1U << 30);
    mctl_write_w (reg_val, PLLGCR);
    mdfs_udelay (100);
  }
  else {  
    if (pll_source) {
      mctl_write_w (pllx_para, _CCM_PLL_DDR1_REG);
      mctl_write_w ( (pllx_para | (0x1U << 30) ), _CCM_PLL_DDR1_REG);
      mdfs_udelay (1000);
      m_div = 4;
    }
    else {
      mctl_write_w (pllx_para, _CCM_PLL_DDR0_REG);
      mctl_write_w ( (pllx_para | (0x1U << 20) ), _CCM_PLL_DDR0_REG);
      mdfs_udelay (1000);
      m_div = 2;
    }

    if ( (para->dram_tpr13 >> 16) & 0x3f) {
      if ( (para->dram_tpr13 >> 8) & 0x1) {
        if (para->dram_tpr13 & (0x1 << 16) )
        { mctl_write_w ( ( (0x3U << 17) | (0x158U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR1_PATTERN_REG); }

        if (para->dram_tpr13 & (0x1 << 17) )
        { mctl_write_w ( (0x1999U | (0x3U << 17) | (0x135U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR1_PATTERN_REG); }

        if (para->dram_tpr13 & (0x1 << 18) )
        { mctl_write_w ( (0x3333U | (0x3U << 17) | (0x120U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR1_PATTERN_REG); }
        else
          if (para->dram_tpr13 & (0x1 << 19) )
          { mctl_write_w ( (0x6666U | (0x3U << 17) | (0xD8U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR1_PATTERN_REG); }
          else
            if (para->dram_tpr13 & (0x1 << 20) )
            { mctl_write_w ( (0x9999U | (0x3U << 17) | (0x90U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR1_PATTERN_REG); }
            else
              if (para->dram_tpr13 & (0x1 << 21) )
              { mctl_write_w ( (0xccccU | (0x3U << 17) | (0x48U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR1_PATTERN_REG); }

        reg_val = mctl_read_w (_CCM_PLL_DDR1_REG);
        reg_val |= ( (0x1U << 24) | (0x1U << 30) );
        mctl_write_w (reg_val, _CCM_PLL_DDR1_REG);
        while (mctl_read_w (_CCM_PLL_DDR1_REG) & (0x1 << 30) );
      }
      else {
        if (para->dram_tpr13 & (0x1 << 18) )
        { mctl_write_w ( (0x3333U | (0x3U << 17) | (0x120U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR0_PATTERN_REG); }
        else
          if (para->dram_tpr13 & (0x1 << 19) )
          { mctl_write_w ( (0x6666U | (0x3U << 17) | (0xD8U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR0_PATTERN_REG); }
          else
            if (para->dram_tpr13 & (0x1 << 20) )
            { mctl_write_w ( (0x9999U | (0x3U << 17) | (0x90U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR0_PATTERN_REG); }
            else
              if (para->dram_tpr13 & (0x1 << 21) )
              { mctl_write_w ( (0xccccU | (0x3U << 17) | (0x48U << 20) | (0x3U << 29) | (0x1U << 31) ), _CCM_PLL_DDR0_PATTERN_REG); }

        reg_val = mctl_read_w (_CCM_PLL_DDR0_REG);
        reg_val |= ( (0x1U << 24) | (0x1U << 20) );
        mctl_write_w (reg_val, _CCM_PLL_DDR0_REG);
        while (mctl_read_w (_CCM_PLL_DDR0_REG) & (0x1 << 20) );
      }
    }

    reg_val = mctl_read_w (CCM_DRAM_CFG_REG);
    reg_val &= ~ (0xf << 0);
    reg_val |= ( (m_div - 1) << 0);
    mctl_write_w (reg_val, CCM_DRAM_CFG_REG);
    reg_val |= (0x1 << 16);      
    mctl_write_w (reg_val, CCM_DRAM_CFG_REG);
    while (mctl_read_w (CCM_DRAM_CFG_REG) & (0x1 << 16) );
    mdfs_udelay (100);
    mctl_write_w (0x400f, MC_CLKEN);
    mdfs_udelay (100);

    reg_val = mctl_read_w (PLLGCR);
    reg_val &= ~ ( (0x1U << 29) | (0x1U << 31) );
    mctl_write_w (reg_val, PLLGCR);
  }

  if (para->dram_clk <= 192)     
  { reg_val = 0x40020061; }
  else
  { reg_val = 0x40000071; }      
  mctl_write_w (reg_val, PIR);
  while ( (mctl_read_w (PGSR0) & 0x1) != 0x1);

  reg_val = mctl_read_w (VDD_SYS_POFF_GATING);
  reg_val &= ~ (0x3 << 0);
  mctl_write_w (reg_val, VDD_SYS_POFF_GATING);
  mdfs_udelay (10);

  mctl_write_w (0x0, PWRCTL);
  while ( (mctl_read_w (STATR) & 0x7) != 0x1);

  if (para->dram_clk <= 192) {
    reg_val = mctl_read_w (DXnGCR0 (0) );
    reg_val &= ~ (0x3 << 9);
    mctl_write_w (reg_val, DXnGCR0 (0) );

    reg_val = mctl_read_w (DXnGCR0 (1) );
    reg_val &= ~ (0x3 << 9);
    mctl_write_w (reg_val, DXnGCR0 (1) );

    mctl_write_w (0x0, ODTMAP);

    if ( ( (para->dram_tpr13 >> 12) & 0x1) == 0) {
      mctl_write_w (0x9, PWRCTL);
    }
  }
  else {
    reg_val = mctl_read_w (DXnGCR0 (0) );
    reg_val |= (0x3 << 9);
    mctl_write_w (reg_val, DXnGCR0 (0) );
    reg_val = mctl_read_w (DXnGCR0 (1) );
    reg_val |= (0x3 << 9);
    mctl_write_w (reg_val, DXnGCR0 (1) );

    rank_num = mctl_read_w (MC_WORK_MODE) & 0x1;
    if (rank_num)
    { mctl_write_w (0x00000303, ODTMAP); }
    else
    { mctl_write_w (0x00000201, ODTMAP); }
  }

  mctl_write_w (0xffff, MC_MAER);
}

#elif defined(CONFIG_ARCH_SUN8IW6P1)
static void __sram mdfs_start (int type, int freq_jump, __dram_para_t * para,
                               unsigned int pllx_para, unsigned int div)
{
  unsigned int reg_val = 0;
  unsigned int i = 0;
  unsigned int odt_type = 0;

  reg_val = (mctl_read_w (MC_MDFSCR) & 0x1);
  if (!reg_val) {
    reg_val = mctl_read_w (PWRCTL);
    reg_val |= (0x1 << 8);
    mctl_write_w (reg_val, PWRCTL);

    if (para->dram_clk < 200) {
      for (i = 0; i < 8; i++) {
        reg_val = mctl_read_w (DATX0IOCR (i) );
        reg_val &= ~ (0x3U << 24);
        reg_val |= (0x2 << 24);
        reg_val &= ~ (0x1f << 0);
        mctl_write_w (reg_val, DATX0IOCR (i) );
        mctl_write_w (reg_val, DATX1IOCR (i) );
        mctl_write_w (reg_val, DATX2IOCR (i) );
        mctl_write_w (reg_val, DATX3IOCR (i) );
      }
    }
    else {
      odt_type = (para->dram_odt_en & 0x2) >> 1;
      for (i = 0; i < 8; i++) {
        reg_val = mctl_read_w (DATX0IOCR (i) );
        reg_val &= ~ (0x3U << 24);
        reg_val |= (odt_type << 24);
        reg_val |= (0x8 << 0);
        mctl_write_w (reg_val, DATX0IOCR (i) );
        mctl_write_w (reg_val, DATX1IOCR (i) );
        mctl_write_w (reg_val, DATX2IOCR (i) );
        mctl_write_w (reg_val, DATX3IOCR (i) );
      }
    }

    reg_val = mctl_read_w (PWRCTL);
    reg_val &= ~ (0x1 << 8);
    mctl_write_w (reg_val, PWRCTL);
  }
}

#elif defined(CONFIG_ARCH_SUN8IW7P1)
static void __sram mdfs_start (int type, int freq_jump, __dram_para_t * para,
                               unsigned int pll_ddr_para, unsigned int div)
{
  unsigned int reg_val = 0;
  unsigned int pll_ddr = pll_ddr_para;
  unsigned int rank_num = 0 ;
  unsigned int i = 0 ;

  rank_num = mctl_read_w (MC_WORK_MODE) & 0x1;

  /* 1. enter self-refresh and disable all master access */
  reg_val = mctl_read_w (PWRCTL);
  reg_val |= (0x1 << 0);
  reg_val |= (0x1 << 8);
  mctl_write_w (reg_val, PWRCTL);
  mdfs_udelay (1);

  while ( (mctl_read_w (STATR) & 0x7) != 0x3);

  /* 2.Update PLL setting and wait 1ms */
  mctl_write_w (pll_ddr, _CCM_PLL_DDR_REG);
  pll_ddr |= (1U << 20);
  mctl_write_w (pll_ddr, _CCM_PLL_DDR_REG);
  mdfs_udelay (1000);

  /*3.set PIR register issue phy reset and DDL calibration */
  if (rank_num) {
    reg_val = mctl_read_w (DTCR);
    reg_val &= ~ (0x3 << 24);
    reg_val |= (0x3 << 24);
    mctl_write_w (reg_val, DTCR);
  }
  else {
    reg_val = mctl_read_w (DTCR);
    reg_val &= ~ (0x3 << 24);
    reg_val |= (0x1 << 24);
    mctl_write_w (reg_val, DTCR);
  }

  mctl_write_w (0x40000061, PIR);
  mdfs_udelay (1);

  while ( (mctl_read_w (PGSR0) & 0x1) != 0x1);

  /*4.setting ODT configure */
  if (para->dram_clk <= 408) {
    for (i = 0; i < 4; i++) {
      reg_val = mctl_read_w (DXnGCR0 (i) );
      reg_val &= ~ (0x3U << 4);
      reg_val |= (0x2 << 4);
      mctl_write_w (reg_val, DXnGCR0 (i) );
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      reg_val = mctl_read_w (DXnGCR0 (i) );
      reg_val &= ~ (0x3U << 4);
      mctl_write_w (reg_val, DXnGCR0 (i) );
    }
  }

  /*5.exit self-refresh and enable all master access */
  reg_val = mctl_read_w (PWRCTL);
  reg_val &= ~ (0x1 << 0);
  reg_val &= ~ (0x1 << 8);
  mctl_write_w (reg_val, PWRCTL);
  mdfs_udelay (1);

  while ( (mctl_read_w (STATR) & 0x7) != 0x1);
}
#endif

int __sram mdfs_main (unsigned int jump, __dram_para_t * dram_para,
                      unsigned int pll_para_from_dram, unsigned int div)
{
  MDFS_DBG ("enter\n");
  DDR_SAVE_SP (sp_backup);
  
  #ifdef CONFIG_ARCH_SUN9IW1P1
  prefetch_and_prediction_disable();
  #endif
  
  mdfs_init_tlb();
  mdfs_init_para (jump, dram_para, pll_para_from_dram, div);
  
  asm ("dsb");
  asm ("isb");
  mdfs_start (0, __sram_jump, &__sram_dram_para, __sram_pllx_para, __sram_div);
  
  #ifdef CONFIG_ARCH_SUN9IW1P1
  prefetch_and_prediction_enable();
  #endif
  
  DDR_RESTORE_SP (sp_backup);
  MDFS_DBG ("done\n");
  
  return 0;
}