/* drivers/input/touchscreen/gt9xx.h
 *
 * 2010 - 2013 Goodix Technology.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be a reference
 * to you, when you are integrating the GOODiX's CTP IC into your system,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#ifndef _GOODIX_GT9XX_H_
#define _GOODIX_GT9XX_H_

#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/earlysuspend.h>
#include <mach/sys_config.h>
#include <linux/gpio.h>
#include <linux/init-input.h>

#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinconf-sunxi.h>



#define GTP_CUSTOM_CFG        0
#define GTP_CHANGE_X2Y        0
#define GTP_DRIVER_SEND_CFG   1
#define GTP_HAVE_TOUCH_KEY    0
#define GTP_POWER_CTRL_SLEEP  0
#define GTP_ICS_SLOT_REPORT   0

#define GTP_AUTO_UPDATE       0   
#define GTP_HEADER_FW_UPDATE  0   
#define GTP_AUTO_UPDATE_CFG   0   

#define GTP_COMPATIBLE_MODE   1   

#define GTP_CREATE_WR_NODE    0
#define GTP_ESD_PROTECT       0   
#define GTP_WITH_PEN          0

#define GTP_SLIDE_WAKEUP      0
#define GTP_DBL_CLK_WAKEUP    0   

#define GTP_DEBUG_ON          0
#define GTP_DEBUG_ARRAY_ON    0
#define GTP_DEBUG_FUNC_ON     0
#define GTP_DEBUG_INFO_ON     0

#define GT963_FIRMWARE_CONFIG


#if GTP_COMPATIBLE_MODE
typedef enum
{
  CHIP_TYPE_GT9  = 0,
  CHIP_TYPE_GT9F = 1,
} CHIP_TYPE_T;
#endif

struct goodix_ts_data {
  spinlock_t irq_lock;
  struct i2c_client * client;
  struct input_dev * input_dev;
  struct hrtimer timer;
  struct work_struct  work;
  struct early_suspend early_suspend;
  s32 irq_is_disable;
  s32 use_irq;
  u16 abs_x_max;
  u16 abs_y_max;
  u8  max_touch_num;
  u8  int_trigger_type;
  u8  green_wake_mode;
  u8  enter_update;
  u8  gtp_is_suspend;
  u8  gtp_rawdiff_mode;
  u8  gtp_cfg_len;
  u8  fixed_cfg;
  u8  fw_error;
  u8  pnl_init_error;
  bool is_suspended;
  
  #if GTP_ESD_PROTECT
  spinlock_t esd_lock;
  u8  esd_running;
  s32 clk_tick_cnt;
  #endif
  
  #if GTP_COMPATIBLE_MODE
  u16 bak_ref_len;
  s32 ref_chk_fs_times;
  s32 clk_chk_fs_times;
  CHIP_TYPE_T chip_type;
  u8 rqst_processing;
  u8 is_950;
  #endif
  
};

extern u16 show_len;
extern u16 total_len;


extern void gtp_set_int_value (int status);
extern void gtp_set_io_int (void);
extern void gtp_set_rst_in (void);
extern void gtp_io_init (int ms);
/* sensor_opt1 sensor_opt2 Sensor_ID
    GND         GND         0
    VDDIO       GND         1
    NC          GND         2
    GND         NC/300K     3
    VDDIO       NC/300K     4
    NC          NC/300K     5
*/
#define CTP_CFG_GROUP1 {\
    0x00,0x20,0x03,0xE0,0x01,0x0A,0x0D,0x00,0x01,0x2F,0x14,0x0A,0x41,0x28,0x03,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0x2E,0x0E,0x36,0x34,0x31,0x0D,0x00,0x00,0x03,0x98,0x04,0x1D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,0x10,0x12,0x13,0x14,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x24,0x26,0x28,0x29,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x97,0x01}

#define CTP_CFG_GROUP2 {\
    0x42,0x00,0x03,0x00,0x04,0x05,0x05,0xC0,0x01,0x06,0x1E,0x06,0x50,0x28,0x03,0x05,0x03,0x03,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x00,0x00,0x87,0x0D,0x0E,0x15,0x00,0xD6,0x09,0x00,0x00,0x00,0x99,0x03,0x1D,0x50,0x07,0x00,0x43,0x39,0x05,0x00,0x00,0x00,0x00,0x00,0x0D,0x50,0x94,0xC5,0x02,0x08,0x00,0x00,0x04,0xAB,0x0F,0x00,0x7B,0x16,0x00,0x5C,0x20,0x00,0x49,0x2F,0x00,0x3E,0x43,0x00,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x1C,0x1A,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x24,0x26,0x28,0x0A,0x0C,0x0F,0x10,0x12,0x29,0x1C,0x21,0x20,0x1F,0x1E,0x1D,0x18,0x16,0x14,0x13,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9C,0x01}

#define CTP_CFG_GROUP3 {\
  }

#define CTP_CFG_GROUP4 {\
  }

#define CTP_CFG_GROUP5 {\
  }

#define CTP_CFG_GROUP6 {\
  }

#define GTP_RST_PORT    config_info.wakeup_gpio.gpio
#define GTP_INT_PORT    config_info.irq_gpio.gpio

#define GTP_GPIO_AS_INPUT(pin)          do{\
    gtp_set_rst_in();\
  }while(0)

#define GTP_GPIO_AS_INT(pin)            do{\
    gtp_set_io_int();\
  }while(0)
#define GTP_GPIO_OUTPUT(pin,level)      do{\
    if(pin == GTP_INT_PORT)\
    {   gtp_set_int_value(level);\
    }\
    else \
    {ctp_wakeup(level,0);\
    }\
  }while(0)

#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#define GTP_IRQ_TAB                     {IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH}

#if GTP_CUSTOM_CFG
#define GTP_MAX_HEIGHT   480
#define GTP_MAX_WIDTH    800
#define GTP_INT_TRIGGER  1           
#else
#define GTP_MAX_HEIGHT   4096
#define GTP_MAX_WIDTH    4096
#define GTP_INT_TRIGGER  1
#endif
#define GTP_MAX_TOUCH         5

#if GTP_HAVE_TOUCH_KEY
#define GTP_KEY_TAB  {KEY_MENU, KEY_HOME, KEY_BACK}
#endif

#define GTP_DRIVER_VERSION    "V2.0<2013/08/28>"
#define GTP_I2C_NAME          "gt9xxf_ts"
#define GTP_POLL_TIME         10
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MIN_LENGTH 186
#define GTP_CONFIG_MAX_LENGTH 240
#define FAIL                  0
#define SUCCESS               1
#define SWITCH_OFF            0
#define SWITCH_ON             1

#define GTP_REG_BAK_REF                 0x99D0
#define GTP_REG_MAIN_CLK                0x8020
#define GTP_REG_CHIP_TYPE               0x8000
#define GTP_REG_HAVE_KEY                0x804E
#define GTP_REG_MATRIX_DRVNUM           0x8069
#define GTP_REG_MATRIX_SENNUM           0x806A

#define GTP_FL_FW_BURN              0x00
#define GTP_FL_ESD_RECOVERY         0x01
#define GTP_FL_READ_REPAIR          0x02

#define GTP_BAK_REF_SEND                0
#define GTP_BAK_REF_STORE               1
#define CFG_LOC_DRVA_NUM                29
#define CFG_LOC_DRVB_NUM                30
#define CFG_LOC_SENS_NUM                31

#define GTP_CHK_FW_MAX                  40
#define GTP_CHK_FS_MNT_MAX              300
#define GTP_BAK_REF_PATH                "/data/gtp_ref.bin"
#define GTP_MAIN_CLK_PATH               "/data/gtp_clk.bin"
#define GTP_RQST_CONFIG                 0x01
#define GTP_RQST_BAK_REF                0x02
#define GTP_RQST_RESET                  0x03
#define GTP_RQST_MAIN_CLOCK             0x04
#define GTP_RQST_RESPONDED              0x00
#define GTP_RQST_IDLE                   0xFF

#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8140

#define RESOLUTION_LOC        3
#define TRIGGER_LOC           8

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))
#define GTP_INFO(fmt,arg...)           do{\
    if(GTP_DEBUG_INFO_ON)\
      printk("<<-GTP-INFO->> [%d]"fmt"\n",__LINE__, ##arg);\
  }while(0)
#define GTP_ERROR(fmt,arg...)          printk("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)          do{\
    if(GTP_DEBUG_ON)\
      printk("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
  }while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{\
    s32 i;\
    u8* a = array;\
    if(GTP_DEBUG_ARRAY_ON)\
    {\
      printk("<<-GTP-DEBUG-ARRAY->>\n");\
      for (i = 0; i < (num); i++)\
      {\
        printk("%02x   ", (a)[i]);\
        if ((i + 1 ) %10 == 0)\
        {\
          printk("\n");\
        }\
      }\
      printk("\n");\
    }\
  }while(0)
#define GTP_DEBUG_FUNC()               do{\
    if(GTP_DEBUG_FUNC_ON)\
      printk("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
  }while(0)
#define GTP_SWAP(x, y)                 do{\
    typeof(x) z = x;\
    x = y;\
    y = z;\
  }while (0)

extern struct ctp_config_info config_info;

#endif /* _GOODIX_GT9XX_H_ */
