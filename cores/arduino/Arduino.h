/*
 * Arduino Core for Milandr MCUs
 * Copyright (c) 2026 Andrey Osipov
 *
 * This file is part of Arduino_Core_Milandr.
 * Project home: https://github.com/unsi9ned/Arduino_Core_Milandr
 * Author's website: https://hamlab.net
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#ifndef EXTERN_C_BEGIN
	#ifdef __cplusplus
		#define EXTERN_C_BEGIN  extern "C" {
		#define EXTERN_C_END    }
	#else
		#define EXTERN_C_BEGIN
		#define EXTERN_C_END
	#endif
#endif

// Common API from Arduino
#include "api/ArduinoAPI.h"

// Описание платы
#include "variant.h"

// This core drivers
#include "Uart.h"

EXTERN_C_BEGIN

// Sketch functions
void setup(void);
void loop(void);

EXTERN_C_END


