/*
 * Copyright (c) 2015-2017, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <debug.h>
#include <mmio.h>
#include "rcar_def.h"
#include "pfc_init.h"
#if RCAR_LSI == RCAR_AUTO
  #include "H3/pfc_init_h3_v1.h"
  #include "H3/pfc_init_h3_v2.h"
  #include "M3/pfc_init_m3.h"
  #include "M3N/pfc_init_m3n.h"
#endif
#if RCAR_LSI == RCAR_H3	/* H3 */
  #include "H3/pfc_init_h3_v1.h"
  #include "H3/pfc_init_h3_v2.h"
#endif
#if RCAR_LSI == RCAR_M3	/* M3 */
  #include "M3/pfc_init_m3.h"
#endif
#if RCAR_LSI == RCAR_M3N	/* M3N */
  #include "M3N/pfc_init_m3n.h"
#endif

 /* Product Register */
#define PRR			(0xFFF00044U)
#define PRR_PRODUCT_MASK	(0x00007F00U)
#define PRR_CUT_MASK		(0x000000FFU)
#define PRR_PRODUCT_H3		(0x00004F00U)           /* R-Car H3 */
#define PRR_PRODUCT_M3		(0x00005200U)           /* R-Car M3 */
#define PRR_PRODUCT_M3N		(0x00005500U)           /* R-Car M3N */
#define PRR_PRODUCT_10		(0x00U)
#define PRR_PRODUCT_11		(0x01U)
#define PRR_PRODUCT_20		(0x10U)

#define PRR_PRODUCT_ERR(reg)	do{\
				ERROR("LSI Product ID(PRR=0x%x) PFC "\
				"initialize not supported.\n",reg);\
				panic();\
				}while(0)
#define PRR_CUT_ERR(reg)	do{\
				ERROR("LSI Cut ID(PRR=0x%x) PFC "\
				"initialize not supported.\n",reg);\
				panic();\
				}while(0)


void pfc_init(void)
{
	uint32_t reg;

	reg = mmio_read_32(RCAR_PRR);
#if RCAR_LSI == RCAR_AUTO
	switch (reg & RCAR_PRODUCT_MASK) {
	case RCAR_PRODUCT_H3:
		switch (reg & PRR_CUT_MASK) {
		case PRR_PRODUCT_10:
			pfc_init_h3_v1();
			break;
		case PRR_PRODUCT_11:
			pfc_init_h3_v1();
			break;
		case PRR_PRODUCT_20:
			pfc_init_h3_v2();
			break;
		default:
			PRR_CUT_ERR(reg);
			break;
		}
		break;
	case RCAR_PRODUCT_M3:
		pfc_init_m3();
		break;
	case RCAR_PRODUCT_M3N:
		pfc_init_m3n();
		break;
	default:
		PRR_PRODUCT_ERR(reg);
		break;
	}

#elif RCAR_LSI_CUT_COMPAT
	switch (reg & PRR_PRODUCT_MASK) {
	case PRR_PRODUCT_H3:
#if RCAR_LSI != RCAR_H3
		PRR_PRODUCT_ERR(reg);
#else
		switch (reg & PRR_CUT_MASK) {
		case PRR_PRODUCT_10:
			pfc_init_h3_v1();
			break;
		case PRR_PRODUCT_11:
			pfc_init_h3_v1();
			break;
		case PRR_PRODUCT_20:
			pfc_init_h3_v2();
			break;
		default:
			PRR_CUT_ERR(reg);
			break;
		}
#endif
		break;
	case PRR_PRODUCT_M3:
#if RCAR_LSI != RCAR_M3
		PRR_PRODUCT_ERR(reg);
#else
		pfc_init_m3();
#endif
		break;
	case PRR_PRODUCT_M3N:
#if RCAR_LSI != RCAR_M3N
		PRR_PRODUCT_ERR(reg);
#else
		pfc_init_m3n();
#endif
		break;
	default:
		PRR_PRODUCT_ERR(reg);
		break;
	}

#else
 #if RCAR_LSI == RCAR_H3	/* H3 */
  #if RCAR_LSI_CUT == RCAR_CUT_10
	/* H3 Cut 10 */
	if ((PRR_PRODUCT_H3 | PRR_PRODUCT_10)
			!= (reg & (PRR_PRODUCT_MASK | PRR_CUT_MASK))) {
		PRR_PRODUCT_ERR(reg);
	}
	pfc_init_h3_v1();
  #elif RCAR_LSI_CUT == RCAR_CUT_11
	/* H3 Cut 11 */
	if ((PRR_PRODUCT_H3 | PRR_PRODUCT_11)
			!= (reg & (PRR_PRODUCT_MASK | PRR_CUT_MASK))) {
		PRR_PRODUCT_ERR(reg);
	}
	pfc_init_h3_v1();
  #elif RCAR_LSI_CUT == RCAR_CUT_20
	/* H3 Cut 20 */
	if ((PRR_PRODUCT_H3 | PRR_PRODUCT_20)
			!= (reg & (PRR_PRODUCT_MASK | PRR_CUT_MASK))) {
		PRR_PRODUCT_ERR(reg);
	}
	pfc_init_h3_v2();
  #else
   #error "Don't have PFC initialize routine(H3)."
  #endif
 #elif RCAR_LSI == RCAR_M3	/* M3 */
	if ((PRR_PRODUCT_M3) != (reg & PRR_PRODUCT_MASK)) {
		PRR_PRODUCT_ERR(reg);
	}
	pfc_init_m3();
 #elif RCAR_LSI == RCAR_M3N	/* M3N */
	if ((PRR_PRODUCT_M3N) != (reg & PRR_PRODUCT_MASK)) {
		PRR_PRODUCT_ERR(reg);
	}
	pfc_init_m3n();
 #else
  #error "Don't have PFC initialize routine(unknown)."
 #endif
#endif
}
