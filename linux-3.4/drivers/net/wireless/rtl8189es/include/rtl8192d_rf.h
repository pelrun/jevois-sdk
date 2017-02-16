/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
/******************************************************************************
 *
 *
 * Module:  rtl8192d_rf.h ( Header File)
 *
 * Note:  Collect every HAL RF type exter API or constant.
 *
 * Function:
 *
 * Export:
 *
 * Abbrev:
 *
 * History:
 * Data     Who   Remark
 *
 * 09/25/2008 MHC   Create initial version.
 *
 *
******************************************************************************/
#ifndef _RTL8192D_RF_H_
#define _RTL8192D_RF_H_
/* Check to see if the file has been included already.  */


/*--------------------------Define Parameters-------------------------------*/

#define   RF6052_MAX_TX_PWR     0x3F
#define   RF6052_MAX_REG        0x3F
#define   RF6052_MAX_PATH       2
/*--------------------------Define Parameters-------------------------------*/


/*------------------------------Define structure----------------------------*/

/*------------------------------Define structure----------------------------*/


/*------------------------Export global variable----------------------------*/
/*------------------------Export global variable----------------------------*/

/*------------------------Export Marco Definition---------------------------*/

/*------------------------Export Marco Definition---------------------------*/


/*--------------------------Exported Function prototype---------------------*/

void    rtl8192d_RF_ChangeTxPath ( IN  PADAPTER  Adapter,
                                   IN  u16   DataRate);
void    rtl8192d_PHY_RF6052SetBandwidth (
  IN  PADAPTER        Adapter,
  IN  HT_CHANNEL_WIDTH    Bandwidth);
VOID  rtl8192d_PHY_RF6052SetCckTxPower (
  IN  PADAPTER  Adapter,
  IN  u8  * pPowerlevel);
VOID  rtl8192d_PHY_RF6052SetOFDMTxPower (
  IN  PADAPTER  Adapter,
  IN  u8  * pPowerLevel,
  IN  u8    Channel);
int PHY_RF6052_Config8192D ( IN  PADAPTER    Adapter );

BOOLEAN rtl8192d_PHY_EnableAnotherPHY (IN  PADAPTER Adapter, IN  BOOLEAN  bMac0);

void  rtl8192d_PHY_PowerDownAnotherPHY (IN PADAPTER Adapter, IN BOOLEAN bMac0);


/*--------------------------Exported Function prototype---------------------*/


#endif/* End of HalRf.h */
