/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
                          sample_defs.h  -  description
                             -------------------
    begin                : sáb nov 30 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SAMPLE_DEFS_H
#define SAMPLE_DEFS_H

#include "typedefs.h"

typedef float sample_float32_t;
typedef int sample_32s_t;
typedef float sample_t; //default sample type
typedef Sint16 sample_16s_t;
typedef Uint8 sample_8u_t;
typedef Sint8 sample_8s_t;
typedef sample_16s_t sample_int_t;
typedef float sample_32f_t;

#define SAMPLE_INT_T_MAX SINT16_MAX

// CONVERT_TO_TYPE can only convert sample_int_t to a narrower type. Ditto for CONVERT_FROM_TYPE.
#define CONVERT_TO_TYPE(type, x) ((x) >> ((sizeof(sample_int_t)-sizeof(type))*BITS_PER_BYTE))
#define CONVERT_FROM_TYPE(type, x) ((x) << ((sizeof(sample_int_t)-sizeof(type))*BITS_PER_BYTE))

#endif
