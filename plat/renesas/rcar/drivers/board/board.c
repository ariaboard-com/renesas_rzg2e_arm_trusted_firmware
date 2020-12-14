/*
 * Copyright (c) 2015-2018, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <iic_dvfs.h>
#include <mmio.h>
#include "board.h"
#include "../../rcar_def.h"


/************************************************************************
 * Defines
 ************************************************************************/
#ifndef BOARD_DEFAULT
#if (RCAR_LSI == RCAR_E3)
#if RZG_EK874
#define BOARD_DEFAULT		(BOARD_EK874 << BOARD_CODE_SHIFT)
#else
#define BOARD_DEFAULT		(BOARD_EBISU << BOARD_CODE_SHIFT)
#endif
#else  /* (RCAR_LSI == RCAR_E3) */
#if (RZG_HIHOPE_RZG2M) // RZG_HIHOPE_RZG2M
#define BOARD_DEFAULT		(BOARD_HIHOPE_RZG2M << BOARD_CODE_SHIFT)
#elif (RZG_HIHOPE_RZG2N) // RZG_HIHOPE_RZG2N
#define BOARD_DEFAULT		(BOARD_HIHOPE_RZG2N << BOARD_CODE_SHIFT)
#elif (RZG_HIHOPE_RZG2H) // RZG_HIHOPE_RZG2H
#define BOARD_DEFAULT		(BOARD_HIHOPE_RZG2H << BOARD_CODE_SHIFT)
#else
#define BOARD_DEFAULT		(BOARD_SALVATOR_X << BOARD_CODE_SHIFT)
#endif
#endif /* (RCAR_LSI == RCAR_E3) */
#endif

#define SLAVE_ADDR_EEPROM	(0x50U)
#define	REG_ADDR_BOARD_ID	(0x70U)

#define BOARD_CODE_MASK		(0xF8U)
#define BOARD_REV_MASK		(0x07U)
#define BOARD_CODE_SHIFT	(3U)

#define BOARD_ID_UNKNOWN	(0xFFU)

#define	GPIO_INDT5	0xE605500C
#define	GP5_19_BIT	(0x01 << 19)
#define	GP5_21_BIT	(0x01 << 21)
#define	GP5_25_BIT	(0x01 << 25)

/************************************************************************
 * Global variables
 ************************************************************************/
const char *g_board_tbl[] = {
	[BOARD_SALVATOR_X]	= "Salvator-X",
	[BOARD_KRIEK]		= "Kriek",
	[BOARD_STARTER_KIT]	= "Starter Kit",
	[BOARD_SALVATOR_XS]	= "Salvator-XS",
	[BOARD_EBISU]		= "Ebisu",
	[BOARD_STARTER_KIT_PRE]	= "Starter Kit",
	[BOARD_EBISU_4D]	= "Ebisu-4D",
	[BOARD_EK874]		= "EK874 RZ/G2E",
	[BOARD_HIHOPE_RZG2M]	= "HiHope RZ/G2M",
	[BOARD_HIHOPE_RZG2N]	= "HiHope RZ/G2N",
	[BOARD_HIHOPE_RZG2H]	= "HiHope RZ/G2H",
	[BOARD_UNKNOWN]		= "unknown"
};


int32_t get_board_type(uint32_t *type, uint32_t *rev)
{
	int32_t ret = 0;
#if	RZG_HIHOPE_RZG2H
	uint32_t boardInfo;
#else	/* RZG_HIHOPE_RZG2H	*/
	uint8_t read_rev;
#endif	/* RZG_HIHOPE_RZG2H	*/

#if	(RZG_HIHOPE_RZG2N) | (RZG_HIHOPE_RZG2M)
	uint32_t reg, boardInfo;
#endif	/* RZG_HIHOPE_RZG2N | RZG_HIHOPE_RZG2M */

	static uint8_t g_board_id = BOARD_ID_UNKNOWN;
	const uint8_t board_tbl[][8U] = {
		[BOARD_SALVATOR_X]	= {0x10U, 0x11U, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_KRIEK]		= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_STARTER_KIT]	= {0x10U, 0x30U, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_SALVATOR_XS]	= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_EBISU]		= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_STARTER_KIT_PRE]	= {0x10U, 0x10U, 0x20U, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_EBISU_4D]	= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_EK874]		= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_HIHOPE_RZG2M]	= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_HIHOPE_RZG2N]	= {0x20U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
		[BOARD_HIHOPE_RZG2H]	= {0x10U, 0xFFU, 0xFFU, 0xFFU,
					   0xFFU, 0xFFU, 0xFFU, 0xFFU},
	};

	if (BOARD_ID_UNKNOWN == g_board_id) {
#if PMIC_ROHM_BD9571
		/* Board ID detection from EEPROM */
		ret = rcar_iic_dvfs_recieve(SLAVE_ADDR_EEPROM,
			REG_ADDR_BOARD_ID, &g_board_id);
		if (0 != ret) {
			g_board_id = BOARD_ID_UNKNOWN;
		} else if (BOARD_ID_UNKNOWN == g_board_id) {
			/* Can't recognize the board */
			g_board_id = BOARD_DEFAULT;
		} else {
			/* none */
		}
#else	/* PMIC_ROHM_BD9571 */
		g_board_id = BOARD_DEFAULT;
#endif	/* PMIC_ROHM_BD9571 */
	}

	*type = ((uint32_t)g_board_id & BOARD_CODE_MASK) >> BOARD_CODE_SHIFT;
	if (*type < (sizeof(board_tbl) / sizeof(board_tbl[0]))) {
#if (RZG_HIHOPE_RZG2M)
		reg = mmio_read_32(RCAR_PRR);
		if (RCAR_M3_CUT_VER11 == (reg & RCAR_CUT_MASK))
		{
			read_rev = (uint8_t)(g_board_id & BOARD_REV_MASK);
			*rev = board_tbl[*type][read_rev];
		}
		else
		{
			boardInfo = mmio_read_32(GPIO_INDT5) & (GP5_19_BIT |GP5_21_BIT);
			*rev = (((boardInfo & GP5_19_BIT) >> 14) | ((boardInfo & GP5_21_BIT) >> 17)) + 0x30;
		}
#elif (RZG_HIHOPE_RZG2N)
		reg = mmio_read_32(GPIO_INDT5);
		if (reg & GP5_25_BIT)
		{
			read_rev = (uint8_t)(g_board_id & BOARD_REV_MASK);
			*rev = board_tbl[*type][read_rev];
		}
		else
		{
			boardInfo = reg & (GP5_19_BIT |GP5_21_BIT);
			*rev = (((boardInfo & GP5_19_BIT) >> 14) | ((boardInfo & GP5_21_BIT) >> 17)) + 0x30;
		}
#elif (RZG_HIHOPE_RZG2H)
		boardInfo = mmio_read_32(GPIO_INDT5) & (GP5_19_BIT |GP5_21_BIT);
		*rev = (((boardInfo & GP5_19_BIT) >> 14) | ((boardInfo & GP5_21_BIT) >> 17)) + 0x30;
#else
		read_rev = (uint8_t)(g_board_id & BOARD_REV_MASK);
		*rev = board_tbl[*type][read_rev];
#endif
	} else {
		/* If there is no revision information, set Rev0.0. */
		*rev = 0x00U;
	}

	return ret;
}
