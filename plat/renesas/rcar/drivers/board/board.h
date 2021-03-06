/*
 * Copyright (c) 2015-2018, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BOARD_H_
#define BOARD_H_


/************************************************************************
 * Board type
 ************************************************************************/
#define BOARD_SALVATOR_X		(0x00U)
#define BOARD_KRIEK			(0x01U)
#define BOARD_STARTER_KIT		(0x02U)
#define BOARD_SALVATOR_XS		(0x04U)
#define BOARD_EBISU			(0x08U)
#define BOARD_STARTER_KIT_PRE		(0x0BU)
#define BOARD_EBISU_4D			(0x0DU)
#define BOARD_EK874			(0x0EU)
#define BOARD_HIHOPE_RZG2M		(0x0FU)
#define BOARD_HIHOPE_RZG2N		(0x10U)
#define BOARD_HIHOPE_RZG2H		(0x11U)

#define BOARD_UNKNOWN			(BOARD_HIHOPE_RZG2H+1U)

#define BOARD_REV_UNKNOWN		(0xFFU)

/************************************************************************
 * Board name
 ************************************************************************/
extern const char *g_board_tbl[];

/************************************************************************
 * Revisions are expressed in 8 bits.
 *  The upper 4 bits are major version.
 *  The lower 4 bits are minor version.
 ************************************************************************/
#define GET_BOARD_MAJOR(a)	((uint32_t)(a) >> 4U)
#define GET_BOARD_MINOR(a)	((uint32_t)(a) & 0xFU)

#define GET_BOARD_NAME(a)	(g_board_tbl[(a)])

int32_t get_board_type(uint32_t *type, uint32_t *rev);

#endif /* BOARD_H_ */
