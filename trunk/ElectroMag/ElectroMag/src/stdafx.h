/***********************************************************************************************
Copyright (C) 2009 - Alexandru Gagniuc - <http:\\g-tech.homeserver.com\HPC.htm>
 * This file is part of ElectroMag.

    ElectroMag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ElectroMag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ElectroMag.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************************************/

#ifndef _STDAFX_H
#define _STDAFX_H


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#define ENABLE_CUDA_SUPPORT
#define ENABLE_GL_SUPPORT
// TODO: reference additional headers your program requires here
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <exception>
#include <stdlib.h>
#include "X-Compat/Threading.h"
#include "X-Compat/HPC Timing.h"
#include "Data Structures.h"
#include "Newton FEMA.h"
#include "Electrostatics.h"
#include "Electrodynamics.h"
#include "./../../GPGPU_Segment/src/CUDA Interop.h"
#include "CPU Implement.h"
#include "CPUID/CpuID.h"


#endif//_STDAFX_H
