/***********************************************************************************************
This file is part of ElectroMag.

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

/*////////////////////////////////////////////////////////////////////////////////
compile with:

/O2 /Ob2 /Oi /Ot /GL /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FD
/EHsc /MD /arch:SSE2 /fp:fast /openmp /Yu"stdafx.h" /Fp"Release\ElectroMag.pch" /Fo"Release\\
/Fd"Release\vc80.pdb" /W3 /nologo /c /Wp64 /Zi /TP /errorReport:prompt

for x64, omit /arch:SSE2, as it is the default, and compiler does not recognize the option

/O2 /Ob2 /Oi /Ot options turn on all speed optimizations,
while /arch:SSE2 /fp:fast allow the use of SSE registers
*/////////////////////////////////////////////////////////////////////////////////
#include "CPU Implement.h"
#include "X-Compat/HPC Timing.h"
#if !defined(__CYGWIN__) // Don't expect performance if using Cygwin
#include <omp.h>
#else
#pragma message --- Cygwin detected. OpenMP not supported by Cygwin!!! ---
#pragma message --- Expect CPU side performance to suck!!! ---
#endif
#include <iostream>
#define CoreFunctor electroPartField
#define CoreFunctorFLOP electroPartFieldFLOP
#define CalcField_CPU_FLOP(n,p) ( n * (p *(CoreFunctorFLOP + 3) + 13) )
#define CalcField_CPU_FLOP_Curvature(n,p) ( n * (p *(CoreFunctorFLOP + 3) + 45) )

template<class T>
int CalcField_CPU_T(Array<Vector3<T> >& fieldLines, Array<pointCharge<T> >& pointCharges,
			  const __int64 n, T resolution, perfPacket& perfData)
{
	if(!n)
		return 1;
	if(resolution == 0)
		return 2;
	//get the size of the computation
	__int64 p = pointCharges.GetSize();
	__int64 totalSteps = (fieldLines.GetSize())/n;

	if(totalSteps < 2)
		return 3;
	
	// Work with data pointers to avoid excessive function calls
	Vector3<T> *lines = fieldLines.GetDataPointer();
	pointCharge<T> *charges = pointCharges.GetDataPointer();
	
	//Used to mesure execution time
	__int64 freq, start, end;
	QueryHPCFrequency(&freq);

	// Start measuring performance
	QueryHPCTimer(&start);
	/*	Each Field line is independent of the others, so that every field line can be parallelized
		When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
		ideal number of threads based oh the runtime system's cache resources. The code will therefore
		run faster if omp_set_num_threads is not specified.
		The GNU and the Microsoft compiler produce highly suboptimal code
	*/
	#pragma omp parallel for
	for(__int64 line = 0; line < n; line++)
	{
		// Intentionally starts from 1, since step 0 is reserved for the starting points
		for(__int64 step = 1; step < totalSteps; step++)
		{

			// Set temporary cummulative field vector to zero
			Vector3<T> temp = {0,0,0},
				prevPoint = lines[n*(step - 1) + line];
			for(__int64 point = 0; point < p; point++)
			{
				// Add partial vectors to the field vector
				temp += CoreFunctor(charges[point], prevPoint);	// (electroPartFieldFLOP + 3) FLOPs
			}
			// Get the unit vector of the field vector, divide it by the resolution, and add it to the previous point
			lines[step*n + line] = (prevPoint + vec3SetInvLen(temp, resolution)); // Total: 13 FLOP (Add = 3 FLOP, setLen = 10 FLOP)
		}
	}
	// take ending measurement
	QueryHPCTimer(&end);
	// Compute performance and time
	perfData.time = (double)(end - start) / freq;
	perfData.performance = (n * ( (totalSteps-1)*(p*(CoreFunctorFLOP + 3) + 13) ) / perfData.time)/ 1E9; // Convert from FLOPS to GFLOPS
	return 0;
}

template<class T>
int CalcField_CPU_T_Curvature(Array<Vector3<T> >& fieldLines, Array<pointCharge<T> >& pointCharges,
			  const __int64 n, T resolution, perfPacket& perfData)
{
	if(!n)
		return 1;
	if(resolution == 0)
		return 2;
	//get the size of the computation
	__int64 p = pointCharges.GetSize();
	__int64 totalSteps = (fieldLines.GetSize())/n;
	// since we are multithreading the computation, having
	// perfData.progress = line / n;
	// will not work as intended because different threads will process different ranges of line and the progress
	// indicator will jump herratically.
	// To solve this problem, we compute the percentage that one line represents, and add it to the total progress.
	double perStep = (double)1/n;
	perfData.progress = 0;

	if(totalSteps < 2)
		return 3;
	
	// Work with data pointers to avoid excessive function calls
	Vector3<T> *pLines = fieldLines.GetDataPointer();
	pointCharge<T> *charges = pointCharges.GetDataPointer();
	
	//Used to mesure execution time
	__int64 freq, start, end;
	QueryHPCFrequency(&freq);

	// Start measuring performance
	QueryHPCTimer(&start);
	/*	Each Field line is independent of the others, so that every field line can be parallelized
		When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
		ideal number of threads based oh the runtime system's cache resources. The code will therefore
		run faster if omp_set_num_threads is not specified.
		The GNU and the Microsoft compiler produce highly suboptimal code
	*/
    
	//#pragma unroll_and_jam
	//#pragma omp parallel for
	for(__int64 line = 0; line < n; line++)
	{
		// Intentionally starts from 1, since step 0 is reserved for the starting points
		for(__int64 step = 1; step < totalSteps; step++)
		{

			// Set temporary cummulative field vector to zero
			Vector3<T> temp = {0,0,0}, prevVec,	prevPoint;
			prevVec = prevPoint = pLines[n*(step - 1) + line];// Load prevVec like this to ensure similarity with GPU kernel
			//#pragma unroll(4)
			//#pragma omp parallel for
			for(__int64 point = 0; point < p; point++)
			{
				// Add partial vectors to the field vector
				temp += CoreFunctor(charges[point], prevPoint);	// (electroPartFieldFLOP + 3) FLOPs
			}
			// Calculate curvature
			T k = vec3LenSq(temp);//5 FLOPs
			k = vec3Len( vec3Cross(temp - prevVec, prevVec) )/(k*sqrt(k));// 25FLOPs (3 vec sub + 9 vec cross + 10 setLen + 1 div + 1 mul + 1 sqrt)
			// Finally, add the unit vector of the field divided by the resolution to the previous point to get the next point
			// We increment the curvature by one to prevent a zero curvature from generating #NaN or #Inf, though any positive constant should work
			pLines[step*n + line] = (prevPoint + vec3SetInvLen(temp, (k+1)*resolution)); // Total: 15 FLOP (Add = 3 FLOP, setLen = 10 FLOP, add-mul = 2FLOP)
			prevVec = temp;
		}
        // update progress
		#pragma omp atomic
            perfData.progress += perStep;
	}
	// take ending measurement
	QueryHPCTimer(&end);
	// Compute performance and time
	perfData.time = (double)(end - start) / freq;
	perfData.performance = (n * ( (totalSteps-1)*(p*(CoreFunctorFLOP + 3) + 13) ) / perfData.time)/ 1E9; // Convert from FLOPS to GFLOPS
	return 0;
}

// ICC Specific: Inline assembly on x64 with current syntax only works on ICC
// MSVC does not support it for 64-bit, and GCC uses a different (idiotic) syntax
// Until GCC finally decides to accept modern inline assembly syntax, I will
// not support manual vectorization on GCC
// (One may change my mind by first optimizing current code even furter)
// I doubt MSVC will ever decide to to support inline assembly for x64
// compilation, so just switch to ICC, or GCC if you dare >:X
// Really, if enough people use GCC, and they request that I support GCC inline
// assembly, and they are willing to help port the code, I will support it.
#if defined(_M_X64) && defined(__INTEL_COMPILER) && defined(USE_THAT_USELESS_STUFF_I_SPENT_TWO_WEEKS_ON)
#include <xmmintrin.h>

#define 	xAccum	xmm0
#define 	yAccum	xmm1
#define 	zAccum	xmm2
#define 	Vx		xmm3
#define 	Vy		xmm4
#define 	Vz		xmm5
#define 	e_k		xmm6
#define 	rx		xmm7
#define 	ry		xmm8
#define 	rz		xmm9
#define 	Cx		xmm10
#define 	Cy		xmm11
#define 	Cz		xmm12
#define 	Cm		xmm13
#define 	len		xmm14
#define 	extra	xmm15

template<>
int CalcField_CPU_T_Curvature(Array<Vector3<float> >& fieldLines, Array<pointCharge<float> >& pointCharges,
			  const __int64 n, float resolution, perfPacket& perfData)
{
	
	if(!n)
		return 1;
	if(resolution == 0)
		return 2;
	//get the size of the computation
	__int64 p = pointCharges.GetSize();
	__int64 totalSteps = (fieldLines.GetSize())/n;
	// since we are multithreading the computation, having
	// perfData.progress = line / n;
	// will not work as intended because different threads will process different ranges of line and the progress
	// indicator will jump herratically.
	// To solve this problem, we compute the percentage that one line represents, and add it to the total progress.
	double perStep = (double)4/n;
	perfData.progress = 0;

	if(totalSteps < 2)
		return 3;

	// Work with data pointers to avoid excessive function calls
	const Vector3<float> *pLines = fieldLines.GetDataPointer();
	const pointCharge<float> *pCharges = pointCharges.GetDataPointer();

	//Used to mesure execution time
	__int64 freq, start, end;
	QueryHPCFrequency(&freq);

	// Start measuring performance
	QueryHPCTimer(&start);
	/*	Each Field line is independent of the others, so that every field line can be parallelized
		When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
		ideal number of threads based oh the runtime system's cache resources. The code will therefore
		run faster if omp_set_num_threads is not specified.
		Not tested on GNU or the Microsoft compiler
	*/



	#pragma omp parallel for
	for(__int64 line = 0; line < n; line+=4)
	{
		// Some operations are better done here
		// We define exactly 16 packed_single variables
		__m128 elec_k;						// electrostatic constant
		/*
		XMM0:		xAccum
		XMM1:		yAccum
		XMM2:		zAccum
		XMM3:		Vx
		XMM4:		Vy
		XMM5:		Vz
		XMM6:		k_e
		XMM7:		rx
		MM8:		ry
		XMM9:		rz
		XMM10:	Cx
		XMM11:	Cy
		XMM12:	Cz
		XMM13:	Cm
		XMM14:	len
		XMM15:	extra
		*/


		// We can keep these outside of the 16 registers
		// Storage for the previous vector:
		__m128 xPrevAccum, yPrevAccum, zPrevAccum;
		__m128 curvAdjust;// curvature adjusting constant
		__m128 res;

		// We can now load the starting points; we will only need to load them once
		//prevVec = prevPoint = lines[n + line];// Load prevVec like this to ensure similarity with GPU kernel
		xPrevAccum = _mm_set_ps(pLines[line + 3].x, pLines[line + 2].x, pLines[line + 1].x, pLines[line].x);
		yPrevAccum = _mm_set_ps(pLines[line + 3].y, pLines[line + 2].y, pLines[line + 1].y, pLines[line].y);
		zPrevAccum = _mm_set_ps(pLines[line + 3].z, pLines[line + 2].z, pLines[line + 1].z, pLines[line].z);


		////////////////
		//Debugging variables
		// We use these for to extract registers and analyze their values
		////////////////
		//__m128 debugX, debugY, debugZ, debugW, debugHor, _x, _y, _z, _w;
					

		elec_k = _mm_set1_ps((float)electro_k);
		curvAdjust =_mm_set1_ps((float)1);
		res = _mm_set1_ps(resolution);
		//__m128 pchar = _mm_load_ps((float*)&charges[point]);
		//float * chargeBase = (float *) pCharges;
		// We access these variables directly in inline assembly
		const float *chargeBase = (float*)pCharges;
		const float *linesBase = (float*)pLines;
		const __int64 nLines = n;
		// Load the starting pint into registers containing info about the previous position
		__asm
		{
			movaps	Vx, xPrevAccum;
			movaps	Vy, yPrevAccum;
			movaps	Vz, zPrevAccum;
		}



		// Intentionally starts from 1, since step 0 is reserved for the starting points
		for(__int64 step = 1; step < totalSteps; step++)
		{
			// Set temporary cummulative field vector to zero
			//Vector3<float> temp = {0,0,0}, prevVec,	prevPoint;
			__asm
			{
				xorps	xAccum, xAccum;
				xorps	yAccum, yAccum;
				xorps	zAccum, zAccum;
				movaps	e_k, [elec_k];
			}//*/
			//#pragma unroll(4)
			for(__int64 point = 0; point < p; point++)
			{
				// Add partial vectors to the field vector
               /*
                * pointCharge<float> charge = charges[point];
                * Vector3<float> r = vec3(prevPoint, charge.position);		// 3 FLOP
                * float lenSq = vec3LenSq(r);								// 5 FLOP
                * temp += vec3Mul(r, (float)electro_k * charge.magnitude /	// 3 FLOP (vecMul)
                * (lenSq * (float)sqrt(lenSq)) );						// 4 FLOP (1 sqrt + 3 mul-div)
                */

				__asm
				{
					mov	rax, chargeBase;
					mov	rdx, point;
					shl	rdx, 4;
					movaps	Cx, [rax+rdx];
					//prefetcht0	[rax+rdx+16];
					movaps	Cy, Cx;
					movaps	Cz, Cx;
					movaps	Cm, Cx;
					shufps	Cx, Cx, _MM_SHUFFLE(0,0,0,0);
					shufps	Cy, Cy, _MM_SHUFFLE(1,1,1,1);
					shufps	Cz, Cz, _MM_SHUFFLE(2,2,2,2); 
					shufps	Cm, Cm, _MM_SHUFFLE(3,3,3,3);

					subps	Cx, Vx;
					subps	Cy, Vy;
					subps	Cz, Vz;

					movaps	rx, Cx;
					movaps	ry, Cy;
					movaps	rz, Cz;

					mulps	Cx, rx;
					mulps	Cy, ry;
					mulps	Cz, rz;

					addps	Cx, Cy;
					addps	Cx, Cz;

					sqrtps	len, Cx;

					mulps	len, Cx;

					divps	Cm, len;
					mulps	Cm, e_k;

					mulps	rx, Cm;
					mulps	ry, Cm;
					mulps	rz, Cm;

					subps	xAccum, rx;
					subps	yAccum, ry;
					subps	zAccum, rz;
				}//*/
				
	        }// end for loop
			__asm
			{				
				movaps	Cm, xAccum;
				movaps	Cy, yAccum;
				movaps	Cz, zAccum;

				mulps	Cm, xAccum;
				mulps	Cy, yAccum;
				mulps	Cz, zAccum;

				addps	Cm, Cy;
				addps	Cm, Cz;

				movaps	Cx, [xPrevAccum];
				movaps	Cy, [yPrevAccum];
				movaps	Cz, [zPrevAccum];

				movaps	rx, xAccum;
				movaps	ry, yAccum;
				movaps	rz, zAccum;

				subps	rx, Cx;
				subps	ry, Cy;
				subps	rz, Cz;

				movaps	e_k, ry;
				movaps	len, rz;
				movaps	extra, rx;

				mulps	e_k, Cz;
				mulps	len, Cx;
				mulps	extra, Cy;

				mulps	rx, Cz;
				mulps	ry, Cx;
				mulps	rz, Cy;

				subps	e_k, rz;
				subps	len, rx;
				subps	extra, ry;

				mulps	e_k, e_k;
				mulps	len, len;
				mulps	extra, extra;

				addps	len, e_k;
				addps	len, extra;
				sqrtps	len, len;

				sqrtps	extra, Cm;
				mulps	extra, Cm;
				divps	len, extra;
				movaps	[xPrevAccum], xAccum;
				movaps	[yPrevAccum], yAccum;
				movaps	[zPrevAccum], zAccum;
				movaps	extra, [curvAdjust];
				addps	len, extra;
				movaps	extra, [res];
				mulps	len, extra;

				movaps	rx,	xAccum;
				movaps	ry,	yAccum;
				movaps	rz,	zAccum;
				mulps	rx, rx;
				mulps	ry, ry;
				mulps	rz, rz;
				addps	rx, ry;
				addps	rx, rz;
				sqrtps	rx, rx;
				mulps	len, rx;
				divps	xAccum, len;
				divps	yAccum, len;
				divps	zAccum, len;

				addps	Vx, xAccum;
				addps	Vy, yAccum;
				addps	Vz, zAccum;


				// We only need xmm3->xmm5 to be preserved, so we can play around with all other registers
				// We want to get the data from
				//from:
				// | x0, x1, x2, x3 |
				// | y0, y1, y2, y3 |
				// | z0, z1, z2, z3 |
				//to:
				// | x0, y0, z0, x1 |
				// | y1, z1, x2, y2 |
				// | z2, x3, y3, z3 |
				//
				// The latter can easily be copied into memory with three instructions
				// lines[step*n + line] = ...

				// Code of NewRite
				// vector 0: x0, y0, z0, x1
				movaps		Cx, Vx;
				unpcklps	Cx, Vy;
				movaps		extra, Vz;
				unpcklps	extra, Vx;
				shufps		Cx, extra,	_MM_SHUFFLE(3, 0, 1, 0);

				// vector 1: y1, z1, x2, y2
				movaps		Cy, Vz;
				unpcklps	Cy, Vy;
				movaps		extra, Vy;
				unpckhps	extra, Vx;
				shufps		Cy, extra, _MM_SHUFFLE(0, 1, 2, 3);

				// vector 2: z2, x3, y3, z3;
				movaps		Cz, Vx;
				unpckhps	Cz, Vz;
				movaps		extra, Vz;
				unpckhps	extra, Vy;
				shufps		Cz, extra, _MM_SHUFFLE(2, 3, 2, 1);

				
				//*/
				// We have gotten the vectors back to horizontal form (bleah!!)
				// Now we need to use movups to get the horizontals into RAM
				// using a Vector pointer base is lines[nLines * step + line]
				// using a float pointer, base is linesBase[(nLines * step + line)*12]
				mov		rbx, linesBase;
				mov		rax, nLines;
				mov		rcx, line;
				mov		rdx, step;
				imul	rax, rdx;
				add		rax, rcx;
				imul	rax, 12;
				movaps	xmmword ptr [rbx + rax],      Cx;
				movaps	xmmword ptr [rbx + rax + 16], Cy;
				movaps	xmmword ptr [rbx + rax + 32], Cz;
			}

		}
        // update progress
		#pragma omp atomic
            perfData.progress += perStep;
	}
	// take ending measurement
	QueryHPCTimer(&end);
	// Compute performance and time
	perfData.time = (double)(end - start) / freq;
	perfData.performance = (n * ( (totalSteps-1)*(p*(electroPartFieldFLOP + 3) + 13) ) / perfData.time)/ 1E9; // Convert from FLOPS to GFLOPS
	return 0;
}

#include <emmintrin.h>

template <>
int CalcField_CPU_T_Curvature(Array<Vector3<double> >& fieldLines, Array<pointCharge<double> >& pointCharges,
			  const __int64 n, double resolution, perfPacket& perfData)
{
	
	if(!n)
		return 1;
	if(resolution == 0)
		return 2;
	//get the size of the computation
	__int64 p = pointCharges.GetSize();
	__int64 totalSteps = (fieldLines.GetSize())/n;
	// since we are multithreading the computation, having
	// perfData.progress = line / n;
	// will not work as intended because different threads will process different ranges of line and the progress
	// indicator will jump herratically.
	// To solve this problem, we compute the percentage that one line represents, and add it to the total progress.
	double perStep = (double)2/n;
	perfData.progress = 0;

	if(totalSteps < 2)
		return 3;

	// Work with data pointers to avoid excessive function calls
	const Vector3<double> *pLines = fieldLines.GetDataPointer();
	const pointCharge<double> *pCharges = pointCharges.GetDataPointer();

	//Used to mesure execution time
	__int64 freq, start, end;
	QueryHPCFrequency(&freq);

	// Start measuring performance
	QueryHPCTimer(&start);
	/*	Each Field line is independent of the others, so that every field line can be parallelized
		When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
		ideal number of threads based oh the runtime system's cache resources. The code will therefore
		run faster if omp_set_num_threads is not specified.
		Not tested on GNU or the Microsoft compiler
	*/



	#pragma omp parallel for
	for(__int64 line = 0; line < n; line+=2)
	{
		// Some operations are better done here
		// We define exactly 16 packed_single variables
		__m128d elec_k;						// electrostatic constant
		/*
		XMM0:		xAccum
		XMM1:		yAccum
		XMM2:		zAccum
		XMM3:		Vx
		XMM4:		Vy
		XMM5:		Vz
		XMM6:		k_e
		XMM7:		rx
		MM8:		ry
		XMM9:		rz
		XMM10:	Cx
		XMM11:	Cy
		XMM12:	Cz
		XMM13:	Cm
		XMM14:	len
		XMM15:	extra
		*/


		// We can keep these outside of the 16 registers
		// Storage for the previous vector:
		__m128d xPrevAccum, yPrevAccum, zPrevAccum;
		__m128d curvAdjust;// curvature adjusting constant
		__m128d res;

		// We can now load the starting points; we will only need to load them once
		//prevVec = prevPoint = lines[n + line];// Load prevVec like this to ensure similarity with GPU kernel
		xPrevAccum = _mm_set_pd(pLines[line + 1].x, pLines[line].x);
		yPrevAccum = _mm_set_pd(pLines[line + 1].y, pLines[line].y);
		zPrevAccum = _mm_set_pd(pLines[line + 1].z, pLines[line].z);


		////////////////
		//Debugging variables
		// We use these for to extract registers and analyze their values
		////////////////
		__m128d debugX, debugY, debugZ, debugW, debugHor, _x, _y, _z, _w;
					

		elec_k = _mm_set1_pd((double)electro_k);
		curvAdjust =_mm_set1_pd((double)1);
		res = _mm_set1_pd(resolution);
		//__m128 pchar = _mm_load_ps((float*)&charges[point]);
		//float * chargeBase = (float *) pCharges;
		// We access these variables directly in inline assembly
		const double *chargeBase = (double*)pCharges;
		const double *linesBase = (double*)pLines;
		const __int64 nLines = n;
		// Load the starting pint into registers containing info about the previous position
		__asm
		{
			movaps	Vx, xPrevAccum;
			movaps	Vy, yPrevAccum;
			movaps	Vz, zPrevAccum;
		}



		// Intentionally starts from 1, since step 0 is reserved for the starting points
		for(__int64 step = 1; step < totalSteps; step++)
		{
			// Set temporary cummulative field vector to zero
			//Vector3<float> temp = {0,0,0}, prevVec,	prevPoint;
			__asm
			{
				xorps	xAccum, xAccum;
				xorps	yAccum, yAccum;
				xorps	zAccum, zAccum;
				movaps	e_k, [elec_k];
			}//*/
			//#pragma unroll(4)
			for(__int64 point = 0; point < p; point++)
			{
				// Add partial vectors to the field vector
               /*
                * pointCharge<float> charge = charges[point];
                * Vector3<float> r = vec3(prevPoint, charge.position);		// 3 FLOP
                * float lenSq = vec3LenSq(r);								// 5 FLOP
                * temp += vec3Mul(r, (float)electro_k * charge.magnitude /	// 3 FLOP (vecMul)
                * (lenSq * (float)sqrt(lenSq)) );						// 4 FLOP (1 sqrt + 3 mul-div)
                */

				__asm
				{
					mov	rax, chargeBase;
					mov	rdx, point;
					shl	rdx, 5;
					//movapd	Cx, [rax+rdx];
					//movapd	Cz,	[rax+rdx + 16]
					movapd	Cy, Cx;
					movapd	Cm, Cz;
					shufpd	Cx, Cx, _MM_SHUFFLE2(0,0);
					shufpd	Cy, Cy, _MM_SHUFFLE2(1,1);
					shufpd	Cz, Cz, _MM_SHUFFLE2(0,0); 
					shufpd	Cm, Cm, _MM_SHUFFLE2(1,1);

					subpd	Cx, Vx;
					subpd	Cy, Vy;
					subpd	Cz, Vz;

					movapd	rx, Cx;
					movapd	ry, Cy;
					movapd	rz, Cz;

					mulpd	Cx, rx;
					mulpd	Cy, ry;
					mulpd	Cz, rz;

					addpd	Cx, Cy;
					addpd	Cx, Cz;

					sqrtpd	len, Cx;

					mulpd	len, Cx;

					divpd	Cm, len;
					mulpd	Cm, e_k;

					mulpd	rx, Cm;
					mulpd	ry, Cm;
					mulpd	rz, Cm;

					subpd	xAccum, rx;
					subpd	yAccum, ry;
					subpd	zAccum, rz;
				}//*/
				
	        }// end for loop
			__asm
			{				
				movapd	Cm, xAccum;
				movapd	Cy, yAccum;
				movapd	Cz, zAccum;

				mulpd	Cm, xAccum;
				mulpd	Cy, yAccum;
				mulpd	Cz, zAccum;

				addpd	Cm, Cy;
				addpd	Cm, Cz;

				movapd	Cx, [xPrevAccum];
				movapd	Cy, [yPrevAccum];
				movapd	Cz, [zPrevAccum];

				movapd	rx, xAccum;
				movapd	ry, yAccum;
				movapd	rz, zAccum;

				subpd	rx, Cx;
				subpd	ry, Cy;
				subpd	rz, Cz;

				movapd	e_k, ry;
				movapd	len, rz;
				movapd	extra, rx;

				mulpd	e_k, Cz;
				mulpd	len, Cx;
				mulpd	extra, Cy;

				mulpd	rx, Cz;
				mulpd	ry, Cx;
				mulpd	rz, Cy;

				subpd	e_k, rz;
				subpd	len, rx;
				subpd	extra, ry;

				mulpd	e_k, e_k;
				mulpd	len, len;
				mulpd	extra, extra;

				addpd	len, e_k;
				addpd	len, extra;
				sqrtpd	len, len;

				sqrtpd	extra, Cm;
				mulpd	extra, Cm;
				divpd	len, extra;
				movapd	[xPrevAccum], xAccum;
				movapd	[yPrevAccum], yAccum;
				movapd	[zPrevAccum], zAccum;
				movapd	extra, [curvAdjust];
				addpd	len, extra;
				movapd	extra, [res];
				mulpd	len, extra;

				movapd	rx,	xAccum;
				movapd	ry,	yAccum;
				movapd	rz,	zAccum;
				mulpd	rx, rx;
				mulpd	ry, ry;
				mulpd	rz, rz;
				addpd	rx, ry;
				addpd	rx, rz;
				sqrtpd	rx, rx;
				mulpd	len, rx;
				divpd	xAccum, len;
				divpd	yAccum, len;
				divpd	zAccum, len;

				addpd	Vx, xAccum;
				addpd	Vy, yAccum;
				addpd	Vz, zAccum;

				// We only need xmm3->xmm5 to be preserved, so we can play around with all other registers
				// We want to get the data from
				//from:
				// | x0, x1, x2, x3 |
				// | y0, y1, y2, y3 |
				// | z0, z1, z2, z3 |
				//to:
				// | x0, y0, z0, x1 |
				// | y1, z1, x2, y2 |
				// | z2, x3, y3, z3 |
				//
				// The latter can easily be copied into memory with three instructions
				// lines[step*n + line] = ...

				// Code of NewRite
				// vector 0: x0, y0
				movapd		Cx, Vx;
				unpcklpd	Cx, Vy;

				// vector 1: z0, x1
				movapd		Cy, Vz;
				shufpd		Cy, Vx, _MM_SHUFFLE2(1,0)

				// vector 2: y1, z1
				movapd		Cz, Vy;
				unpckhpd	Cz, Vz;
				
				//*/
				// We have gotten the vectors back to horizontal form (bleah!!)
				// Now we need to use movups to get the horizontals into RAM
				// using a Vector pointer base is lines[nLines * step + line]
				// using a float pointer, base is linesBase[(nLines * step + line)*24]
				mov		rbx, linesBase;
				mov		rax, nLines;
				mov		rcx, line;
				mov		rdx, step;
				imul	rax, rdx;
				add		rax, rcx;
				imul	rax, 24;
				movapd	xmmword ptr [rbx + rax],      Cx;
				movapd	xmmword ptr [rbx + rax + 16], Cy;
				movapd	xmmword ptr [rbx + rax + 32], Cz;
			}

		}
        // update progress
		#pragma omp atomic
            perfData.progress += perStep;
	}
	// take ending measurement
	QueryHPCTimer(&end);
	// Compute performance and time
	perfData.time = (double)(end - start) / freq;
	perfData.performance = (n * ( (totalSteps-1)*(p*(electroPartFieldFLOP + 3) + 13) ) / perfData.time)/ 1E9; // Convert from FLOPS to GFLOPS
	return 0;
}
#elif (defined(__GNUC__) && defined(__SSE__)) || defined (_MSC_VER)
// I think optimizations should also be available for GNU. We include MSVC as
// well because it basically suports the same intrinsics
#include <xmmintrin.h>

struct /*__declspec(align(__alignof(__m128)))*/__mVector3_ps
{
    __m128 x, y, z;
};
// Quick SSE C++ style operators
/*
inline __m128 operator + (const __m128 A, const __m128 B)
{
    return _mm_add_ps(A,B);
}
inline __m128 operator - (const __m128 A, const __m128 B)
{
    return _mm_sub_ps(A,B);
}
inline __m128 operator * (const __m128 A, const __m128 B)
{
    return _mm_mul_ps(A,B);
}
inline __m128 operator / (const __m128 A, const __m128 B)
{
    return _mm_div_ps(A,B);
}//*/
// Quick SSE Vector library
inline __mVector3_ps _mm_vec3_ps(__mVector3_ps head, __mVector3_ps tail)
{
    __mVector3_ps result;
    result.x = _mm_sub_ps(head.x, tail.x);
    result.y = _mm_sub_ps(head.y, tail.y);
    result.z = _mm_sub_ps(head.z, tail.z);
    return result;
}
inline void operator+=(__mVector3_ps &rhs, const __mVector3_ps B)
{
    rhs.x = _mm_add_ps(rhs.x, B.x);
    rhs.y = _mm_add_ps(rhs.y, B.y);
    rhs.z = _mm_add_ps(rhs.z, B.z);
}
inline void operator -= (__mVector3_ps &rhs, const __mVector3_ps B)
{
    rhs.x = _mm_sub_ps(rhs.x, B.x);
    rhs.y = _mm_sub_ps(rhs.y, B.y);
    rhs.z = _mm_sub_ps(rhs.z, B.z);
}
inline __mVector3_ps operator + (__mVector3_ps A, __mVector3_ps B)
{
    __mVector3_ps result;
    result.x = _mm_add_ps(A.x, B.x);
    result.y = _mm_add_ps(A.y, B.y);
    result.z = _mm_add_ps(A.z, B.z);
    return result;
}
inline __mVector3_ps operator - (__mVector3_ps A, __mVector3_ps B)
{
    __mVector3_ps result;
    result.x = _mm_sub_ps(A.x, B.x);
    result.y = _mm_sub_ps(A.y, B.y);
    result.z = _mm_sub_ps(A.z, B.z);
    return result;
}
inline __mVector3_ps operator * (__mVector3_ps vec, __m128 val)
{
    __mVector3_ps result;
    result.x = _mm_mul_ps(vec.x, val);
    result.y = _mm_mul_ps(vec.y, val);
    result.z = _mm_mul_ps(vec.z, val);
    return result;
}
inline __mVector3_ps operator / (__mVector3_ps vec, __m128 val)
{
    __mVector3_ps result;
    result.x = _mm_div_ps(vec.x, val);
    result.y = _mm_div_ps(vec.y, val);
    result.z = _mm_div_ps(vec.z, val);
    return result;
}
inline __m128 _mm_vec3LenSq_ps(const __m128 x, const __m128 y, const __m128 z)
{
    return _mm_add_ps(_mm_add_ps(_mm_mul_ps(x,x),_mm_mul_ps(y,y)),_mm_mul_ps(z,z));
}

inline __m128 _mm_vec3LenSq_ps(const __mVector3_ps vec)
{
    return _mm_vec3LenSq_ps(vec.x, vec.y, vec.z);
}

inline __m128 _mm_vec3Len_ps(const __m128 x, const __m128 y, const __m128 z)
{
    return _mm_sqrt_ps(_mm_vec3LenSq_ps(x, y, z));
}

inline __m128 _mm_vec3Len_ps(const __mVector3_ps vec)
{
    return _mm_vec3Len_ps(vec.x, vec.y, vec.z);
}


inline __mVector3_ps _mm_vec3Cross_ps(__m128 xIndex, __m128 yIndex, __m128 zIndex,
                                    __m128 xMiddle, __m128 yMiddle, __m128 zMiddle
                                    )
{
    __mVector3_ps result;
	//result.x = index.y * middle.z - index.z * middle.y;		// 3 FLOPs
    result.x = _mm_sub_ps(_mm_mul_ps(yIndex,zMiddle),_mm_mul_ps(zIndex,yMiddle));
	//result.y = index.z * middle.x - index.x * middle.z;		// 3 FLOPs
    result.y = _mm_sub_ps(_mm_mul_ps(zIndex,xMiddle),_mm_mul_ps(xIndex,zMiddle));
	//result.z = index.x * middle.y - index.y * middle.x;		// 3 FLOPs
    result.z = _mm_sub_ps(_mm_mul_ps(xIndex,yMiddle),_mm_mul_ps(yIndex,xMiddle));
    return result;
}

inline __mVector3_ps _mm_vec3Cross_ps(const __mVector3_ps index, const __mVector3_ps middle)
{
    __mVector3_ps result;
	//result.x = index.y * middle.z - index.z * middle.y;		// 3 FLOPs
    result.x = _mm_sub_ps(_mm_mul_ps(index.y,middle.z),_mm_mul_ps(index.z,middle.y));
	//result.y = index.z * middle.x - index.x * middle.z;		// 3 FLOPs
    result.y = _mm_sub_ps(_mm_mul_ps(index.z,middle.x),_mm_mul_ps(index.x,middle.z));
	//result.z = index.x * middle.y - index.y * middle.x;		// 3 FLOPs
    result.z = _mm_sub_ps(_mm_mul_ps(index.x,middle.y),_mm_mul_ps(index.y,middle.x));
    return result;
}

inline __mVector3_ps _mm_vec3SetInvLen_ps(__mVector3_ps vec, __m128 scalarInvLen)
{
	__m128 len = _mm_vec3Len_ps(vec);
	scalarInvLen =_mm_mul_ps(scalarInvLen, len);
	return vec / scalarInvLen;
}

template<>
int CalcField_CPU_T_Curvature<float>(Array<Vector3<float> >& fieldLines, Array<pointCharge<float> >& pointCharges,
			  const __int64 n, float resolution, perfPacket& perfData)
{
	if(!n)
		return 1;
	if(resolution == 0)
		return 2;
	//get the size of the computation
	__int64 p = pointCharges.GetSize();
	__int64 totalSteps = (fieldLines.GetSize())/n;
	
    #define LINES_PARRALELISM 4
    #define SIMD_WIDTH 4	// Represents how many floats can be packed into an SSE Register; Must ALWAYS be 4
    #define LINES_WIDTH (LINES_PARRALELISM * SIMD_WIDTH)
    #define ALIGNMENT_MASK (LINES_WIDTH * sizeof(float) - 1)

    if(n & ALIGNMENT_MASK)
        return 5;

	// since we are multithreading the computation, having
	// perfData.progress = line / n;
	// will not work as intended because different threads will process different ranges of line and the progress
	// indicator will jump herratically.
	// To solve this problem, we compute the percentage that one line represents, and add it to the total progress
	double perStep = (double)LINES_WIDTH/n;
	perfData.progress = 0;

    if(totalSteps < 2)
		return 3;

    //Used to mesure execution time
	__int64 freq, start, end;
	QueryHPCFrequency(&freq);

	// Start measuring performance
	QueryHPCTimer(&start);
	/*	Each Field line is independent of the others, so that every field line can be parallelized
		When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
		ideal number of threads based oh the runtime system's cache resources. The code will therefore
		run faster if omp_set_num_threads is not specified.
		The GNU and the Microsoft compiler produce highly suboptimal code
	*/

	//#pragma unroll_and_jam
	#pragma omp parallel for
	for(__int64 line = 0; line < n; line+=LINES_WIDTH)
	{
		// Work with data pointers to avoid excessive calls to Array<T>::operator[]
		const Vector3<float> *pLines = fieldLines.GetDataPointer();
		const pointCharge<float> *pCharges = pointCharges.GetDataPointer();

        // We can keep these outside of the 16 registers
        __m128 Cx, Cy, Cz, Cm;
        __mVector3_ps prevPoint[LINES_PARRALELISM];
        __mVector3_ps Accum[LINES_PARRALELISM], prevAccum[LINES_PARRALELISM];

		// We can now load the starting points; we will only need to load them once
		//prevVec = prevPoint = lines[n + line];// Load prevVec like this to ensure similarity with GPU kernel
        for(size_t i = 0; i < LINES_PARRALELISM; i++)
        {
            prevAccum[i].x = prevPoint[i].x =_mm_set_ps(
                    pLines[line + (i*SIMD_WIDTH) + 3].x, pLines[line + (i*SIMD_WIDTH) + 2].x,
                    pLines[line + (i*SIMD_WIDTH) + 1].x, pLines[line + (i*SIMD_WIDTH)].x);
            prevAccum[i].y = prevPoint[i].y =_mm_set_ps(
                    pLines[line + (i*SIMD_WIDTH) + 3].y, pLines[line + (i*SIMD_WIDTH) + 2].y,
                    pLines[line + (i*SIMD_WIDTH) + 1].y, pLines[line + (i*SIMD_WIDTH)].y);
            prevAccum[i].z = prevPoint[i].z =_mm_set_ps(
                    pLines[line + (i*SIMD_WIDTH) + 3].z, pLines[line + (i*SIMD_WIDTH) + 2].z,
                    pLines[line + (i*SIMD_WIDTH) + 1].z, pLines[line + (i*SIMD_WIDTH)].z);
        }


        const __m128 zero = _mm_set1_ps((float)0);
		const __m128 elec_k = _mm_set1_ps((float)electro_k);
		const __m128 curvAdjust =_mm_set1_ps((float)1); // curvature adjusting constant
		const __m128 res = _mm_set1_ps(resolution);
        // TODO:Remnants from old code, should we remove them?
		const float *linesBase = (float*)pLines;
		const __int64 nLines = n;

		// Intentionally starts from 1, since step 0 is reserved for the starting points
		for(__int64 step = 1; step < totalSteps; step++)
		{
#			pragma unroll
			for(size_t i = 0; i < LINES_PARRALELISM; i++)
				Accum[i].x = Accum[i].y = Accum[i].z = zero;

			for(__int64 point = 0; point < p; point++)
			{
				// Add partial vectors to the field vector
				// temp += CoreFunctor(charges[point], prevPoint);	// (electroPartFieldFLOP + 3) FLOPs
				Cm = _mm_load_ps((float*)&pCharges[point]);
                __mVector3_ps Cpos;
                Cpos.x = _mm_shuffle_ps(Cm, Cm, _MM_SHUFFLE(0,0,0,0));
                Cpos.y = _mm_shuffle_ps(Cm, Cm, _MM_SHUFFLE(1,1,1,1));
                Cpos.z = _mm_shuffle_ps(Cm, Cm, _MM_SHUFFLE(2,2,2,2));
                Cm = _mm_shuffle_ps(Cm, Cm, _MM_SHUFFLE(3,3,3,3));

                /* temp += electroPartFieldVec(charges[point], prevPoint);
                 *
                 * Vector3<double> electroPartFieldVec(pointCharge<double> charge, Vector3<double> point)
                 * {
				 *		Vector3<T> r = vec3(point, charge.position);		// 3 FLOP
                 *  	T lenSq = vec3LenSq(r);								// 5 FLOP
                 *      return vec3Mul(r, (T)electro_k * charge.magnitude /	// 3 FLOP (vecMul)
                 *          (lenSq * (T)sqrt(lenSq)) );						// 4 FLOP (1 sqrt + 3 mul,div)
                 * }
                 */
                // Vector3<T> r = vec3(point, charge.position);
                __mVector3_ps r = _mm_vec3_ps(prevPoint[0], Cpos);
                // T lenSq = vec3LenSq(r);
                __m128 lenSq = _mm_vec3LenSq_ps(r);
                //return vec3Mul(r, (T)electro_k * charge.magnitude /
                //      (lenSq * (T)sqrt(lenSq)) );
                Accum[0] += r * _mm_div_ps( _mm_mul_ps(elec_k, Cm),_mm_mul_ps( lenSq, _mm_sqrt_ps(lenSq) ) );

#				if (LINES_PARRALELISM > 1)
                r = _mm_vec3_ps(prevPoint[1], Cpos);
                lenSq = _mm_vec3LenSq_ps(r);
                Accum[1] += r * _mm_div_ps( _mm_mul_ps(elec_k, Cm),_mm_mul_ps( lenSq, _mm_sqrt_ps(lenSq) ) );
#				endif
#				if (LINES_PARRALELISM == 3)
#				error LINES_PARRALELISM Should not be set to 3, as the alignment mask may fail to function properly
#				endif
#				if (LINES_PARRALELISM > 3)
                r = _mm_vec3_ps(prevPoint[3], Cpos);
                lenSq = _mm_vec3LenSq_ps(r);
                Accum[3] += r * _mm_div_ps( _mm_mul_ps(elec_k, Cm),_mm_mul_ps( lenSq, _mm_sqrt_ps(lenSq) ) );
#				endif
#				if (LINES_PARRALELISM > 4)
#				error Too many lines per iteration
#				endif
				
			}

            /*
             * T k = vec3LenSq(temp);//5 FLOPs
             * k = vec3Len( vec3Cross(temp - prevVec, prevVec) )/(k*sqrt(k));// 25FLOPs (3 vec sub + 9 vec cross + 10 setLen + 1 div + 1 mul + 1 sqrt)
             * pLines[step*n + line] = (prevPoint + vec3SetInvLen(temp, (k+1)*resolution)); // Total: 15 FLOP (Add = 3 FLOP, setLen = 10 FLOP, add-mul = 2FLOP)
             * prevVec = temp;
             */
#			pragma unroll
            for(size_t i = 0; i < LINES_PARRALELISM; i++)
            {
            // T k = vec3LenSq(temp);
            __m128 k = _mm_vec3LenSq_ps(Accum[i]);
            // k = vec3Len( vec3Cross(temp - prevVec, prevVec) ) / (k*sqrt(k));
            k = _mm_div_ps(
                    (_mm_vec3Len_ps( _mm_vec3Cross_ps(Accum[i] - prevAccum[i], prevAccum[i]) ) ),
                    (_mm_mul_ps(k, _mm_sqrt_ps(k)) ) );
            // (prevPoint += vec3SetInvLen(temp, (k+1)*resolution))
            prevPoint[i] += _mm_vec3SetInvLen_ps(Accum[i],
                            _mm_mul_ps(res, _mm_add_ps(k, curvAdjust)));

			// We only need xmm3->xmm5 to be preserved, so we can play around with all other registers
			// We want to get the data from
			//from:
			// | x0, x1, x2, x3 |
			// | y0, y1, y2, y3 |
			// | z0, z1, z2, z3 |
			//to:
			// | x0, y0, z0, x1 |
			// | y1, z1, x2, y2 |
			// | z2, x3, y3, z3 |
			//
			// The latter can easily be copied into memory with three movaps instructions
			// lines[step*n + line] = ...

			// Code of NewRite
			// vector 0: x0, y0, z0, x1
			//	movaps		Cx, Vx;
			//	unpcklps	Cx, Vy;
			//	movaps		extra, Vz;
			//	unpcklps	extra, Vx;
			//	shufps		Cx, extra,	_MM_SHUFFLE(3, 0, 1, 0);
            Cx = _mm_shuffle_ps(_mm_unpacklo_ps(prevPoint[i].x,prevPoint[i].y),
                            _mm_unpacklo_ps(prevPoint[i].z,prevPoint[i].x),
                            _MM_SHUFFLE(3,0,1,0));

			// vector 1: y1, z1, x2, y2
			//	movaps		Cy, Vz;
			//	unpcklps	Cy, Vy;
			//	movaps		extra, Vy;
			//	unpckhps	extra, Vx;
			//	shufps		Cy, extra, _MM_SHUFFLE(0, 1, 2, 3);
            Cy = _mm_shuffle_ps(_mm_unpacklo_ps(prevPoint[i].z,prevPoint[i].y),
                            _mm_unpackhi_ps(prevPoint[i].y,prevPoint[i].x),
                            _MM_SHUFFLE(0,1,2,3));

			// vector 2: z2, x3, y3, z3;
			//	movaps		Cz, Vx;
			//	unpckhps	Cz, Vz;
			//	movaps		extra, Vz;
			//	unpckhps	extra, Vy;
			//	shufps		Cz, extra, _MM_SHUFFLE(2, 3, 2, 1);
            Cz = _mm_shuffle_ps(_mm_unpackhi_ps(prevPoint[i].x,prevPoint[i].z),
                            _mm_unpackhi_ps(prevPoint[i].z,prevPoint[i].y),
                            _MM_SHUFFLE(2,3,2,1));


			//*/
			// We have gotten the vectors back to horizontal form (bleah!!)
			// Now we need to use movups to get the horizontals into RAM
			// using a Vector pointer base is lines[nLines * step + line]
			// using a float pointer, base is linesBase[(nLines * step + line)*3]
            // using a void pointer, base is linesBase[(nLines * step + line)*12]
            _mm_store_ps((float*)&linesBase[(nLines * step + (i*SIMD_WIDTH) + line)*3],Cx);
			_mm_store_ps((float*)&linesBase[(nLines * step + (i*SIMD_WIDTH) + line)*3 + 4],Cy);
            _mm_store_ps((float*)&linesBase[(nLines * step + (i*SIMD_WIDTH) + line)*3 + 8],Cz);
            }
		}
        // update progress
		#pragma omp atomic
            perfData.progress += perStep;
	}
	// take ending measurement
	QueryHPCTimer(&end);
	// Compute performance and time
	perfData.time = (double)(end - start) / freq;
	perfData.performance = (n * ( (totalSteps-1)*(p*(CoreFunctorFLOP + 3) + 13) ) / perfData.time)/ 1E9; // Convert from FLOPS to GFLOPS
	return 0;
}

#endif//USE_AUTO_VECTOR

template<>
int CalcField_CPU<float>(Array<Vector3<float> >& fieldLines, Array<pointCharge<float> >& pointCharges,
			  const __int64 n, float resolution, perfPacket& perfData, bool useCurvature)
{
	if(useCurvature) return CalcField_CPU_T_Curvature<float>(fieldLines, pointCharges, n, resolution, perfData);
	else return CalcField_CPU_T<float>(fieldLines, pointCharges, n, resolution, perfData);
}

template<>
int CalcField_CPU<double>(Array<Vector3<double> >& fieldLines, Array<pointCharge<double> >& pointCharges,
			  const __int64 n, double resolution, perfPacket& perfData, bool useCurvature)
{
	if(useCurvature) return CalcField_CPU_T_Curvature<double>(fieldLines, pointCharges, n, resolution, perfData);
    else return CalcField_CPU_T<double>(fieldLines, pointCharges, n, resolution, perfData);
}
