/*
 * Copyright (c) 2017, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <debug.h>
#include "../qos_common.h"
#include "../qos_reg.h"
#include "qos_init_m3n_v10.h"

#define	RCAR_QOS_VERSION		"rev.0.05"

#define QOSCTRL_EARLYR			(QOS_BASE1 + 0x0060U)

#define REF_ARS_ARBSTOPCYCLE_M3N	(((SL_INIT_SSLOTCLK_M3N) - 5U) << 16U)

#if RCAR_QOS_TYPE  == RCAR_QOS_TYPE_DEFAULT

#if RCAR_REF_INT == RCAR_REF_195
#include "qos_init_m3n_v10_mstat195.h"
#else
#include "qos_init_m3n_v10_mstat390.h"
#endif

#endif

static void dbsc_setting(void)
{
	uint32_t md=0;

	/* Register write enable */
	io_write_32(DBSC_DBSYSCNT0, 0x00001234U);

	/* BUFCAM settings */
	io_write_32(DBSC_DBCAM0CNF1, 0x00043218);	//dbcam0cnf1
	io_write_32(DBSC_DBCAM0CNF2, 0x000000F4);	//dbcam0cnf2
	io_write_32(DBSC_DBSCHCNT0,  0x000F0037);	//dbschcnt0
	io_write_32(DBSC_DBSCHSZ0,   0x00000001);	//dbschsz0
	io_write_32(DBSC_DBSCHRW0,   0x22421111);	//dbschrw0

	md = (*((volatile uint32_t*)RST_MODEMR) & 0x000A0000) >> 17;

	switch (md) {
	case 0x0:
		/* DDR3200 */
		io_write_32(DBSC_SCFCTST2, 0x012F1123);
		break;
	case 0x1:	//MD19=0,MD17=1 : LPDDR4-3000, 4GByte(1GByte x4)
		/* DDR2800 */
		io_write_32(DBSC_SCFCTST2, 0x012F1123);
		break;
	case 0x4:	//MD19=1,MD17=0 : LPDDR4-2400, 4GByte(1GByte x4)
		/* DDR2400 */
		io_write_32(DBSC_SCFCTST2, 0x012F1123);
		break;
	default:	//MD19=1,MD17=1 : LPDDR4-1600, 4GByte(1GByte x4)
		/* DDR1600 */
		io_write_32(DBSC_SCFCTST2, 0x012F1123);
		break;
	}

	/* QoS Settings */
	io_write_32(DBSC_DBSCHQOS00,  0x00000F00);
	io_write_32(DBSC_DBSCHQOS01,  0x00000B00);
	io_write_32(DBSC_DBSCHQOS02,  0x00000000);
	io_write_32(DBSC_DBSCHQOS03,  0x00000000);
	io_write_32(DBSC_DBSCHQOS40,  0x00000300);
	io_write_32(DBSC_DBSCHQOS41,  0x000002F0);
	io_write_32(DBSC_DBSCHQOS42,  0x00000200);
	io_write_32(DBSC_DBSCHQOS43,  0x00000100);
	io_write_32(DBSC_DBSCHQOS90,  0x00000100);
	io_write_32(DBSC_DBSCHQOS91,  0x000000F0);
	io_write_32(DBSC_DBSCHQOS92,  0x000000A0);
	io_write_32(DBSC_DBSCHQOS93,  0x00000040);
	io_write_32(DBSC_DBSCHQOS130, 0x00000100);
	io_write_32(DBSC_DBSCHQOS131, 0x000000F0);
	io_write_32(DBSC_DBSCHQOS132, 0x000000A0);
	io_write_32(DBSC_DBSCHQOS133, 0x00000040);
	io_write_32(DBSC_DBSCHQOS140, 0x000000C0);
	io_write_32(DBSC_DBSCHQOS141, 0x000000B0);
	io_write_32(DBSC_DBSCHQOS142, 0x00000080);
	io_write_32(DBSC_DBSCHQOS143, 0x00000040);
	io_write_32(DBSC_DBSCHQOS150, 0x00000040);
	io_write_32(DBSC_DBSCHQOS151, 0x00000030);
	io_write_32(DBSC_DBSCHQOS152, 0x00000020);
	io_write_32(DBSC_DBSCHQOS153, 0x00000010);

	/* Register write protect */
	io_write_32(DBSC_DBSYSCNT0, 0x00000000U);
}

void qos_init_m3n_v10(void)
{
	dbsc_setting();

	/* DRAM Split Address mapping */
#if RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_4CH
 #if RCAR_LSI == RCAR_M3N
  #error "Don't set DRAM Split 4ch(M3N)"
 #else
	ERROR("DRAM Split 4ch not supported.(M3N)");
	panic();
 #endif
#elif (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_2CH)
 #if RCAR_LSI == RCAR_M3N
  #error "Don't set DRAM Split 2ch(M3N)"
 #else
	ERROR("DRAM Split 2ch not supported.(M3N)");
	panic();
 #endif
#else
	NOTICE("BL2: DRAM Split is OFF\n");
#endif

#if !(RCAR_QOS_TYPE == RCAR_QOS_NONE)
#if RCAR_QOS_TYPE  == RCAR_QOS_TYPE_DEFAULT
	NOTICE("BL2: QoS is default setting(%s)\n", RCAR_QOS_VERSION);
#endif

#if RCAR_REF_INT == RCAR_REF_195
	NOTICE("BL2: DRAM refresh interval 1.95 usec\n");
#else
	NOTICE("BL2: DRAM refresh interval 3.9 usec\n");
#endif

	io_write_32(QOSCTRL_RAS,   0x00000028U);
	io_write_64(QOSCTRL_DANN,  0x0402000002020201UL);
	io_write_32(QOSCTRL_DANT,  0x00100804U);
	io_write_32(QOSCTRL_INSFC, 0x06330001U);
	io_write_32(QOSCTRL_EARLYR, 0x00000001U);
	io_write_32(QOSCTRL_RACNT0, 0x00010003U);


	io_write_32(QOSCTRL_SL_INIT, SL_INIT_REFFSSLOT | SL_INIT_SLOTSSLOT | SL_INIT_SSLOTCLK_M3N);
	io_write_32(QOSCTRL_REF_ARS, REF_ARS_ARBSTOPCYCLE_M3N);

	{
	uint32_t i;

	for (i = 0U; i < ARRAY_SIZE(mstat_fix); i++) {
		io_write_64(QOSBW_FIX_QOS_BANK0 + i*8,
				mstat_fix[i]);
		io_write_64(QOSBW_FIX_QOS_BANK1 + i*8,
				mstat_fix[i]);
	}
	for (i = 0U; i < ARRAY_SIZE(mstat_be); i++) {
		io_write_64(QOSBW_BE_QOS_BANK0 + i*8,
				mstat_be[i]);
		io_write_64(QOSBW_BE_QOS_BANK1 + i*8,
				mstat_be[i]);
	}
	}

	/* AXI-IF arbitration setting */
	//delete

	/* 3DG bus Leaf setting */
	io_write_32(GPU_ACT_GRD, 0x00001234U);
	io_write_32(GPU_ACT0, 0x00000000U);
	io_write_32(GPU_ACT1, 0x00000000U);
	io_write_32(GPU_ACT2, 0x00000000U);
	io_write_32(GPU_ACT3, 0x00000000U);
	io_write_32(GPU_ACT_GRD, 0x00000000U);

	/* RT bus Leaf setting */
	io_write_32(RT_ACT0, 0x00000000U);
	io_write_32(RT_ACT1, 0x00000000U);

	/* CCI bus Leaf setting */
	io_write_32(CPU_ACT0, 0x00000003U);
	io_write_32(CPU_ACT1, 0x00000003U);

	io_write_32(QOSCTRL_RAEN,  0x00000001U);

	io_write_32(QOSCTRL_STATQC, 0x00000001U);
#else
	NOTICE("BL2: QoS is None\n");

	io_write_32(QOSCTRL_RAEN,  0x00000001U);
#endif /* !(RCAR_QOS_TYPE == RCAR_QOS_NONE) */
}
