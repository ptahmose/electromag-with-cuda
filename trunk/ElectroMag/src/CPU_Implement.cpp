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


// This needs to be visible before any vector templates
#include "SSE math.h"
#include "CPU Implement.h"
#include "X-Compat/HPC Timing.h"
#if !defined(__CYGWIN__) // Don't expect performance if using Cygwin
#include <omp.h>
#else
#pragma message --- Cygwin detected. OpenMP not supported by Cygwin!!! ---
#pragma message --- Expect CPU side performance to suck!!! ---
#endif
#define CoreFunctor electro::PartField
#define CoreFunctorFLOP electroPartFieldFLOP
#define CalcField_CPU_FLOP(n,p) ( n * (p *(CoreFunctorFLOP + 3) + 13) )
#define CalcField_CPU_FLOP_Curvature(n,p) ( n * (p *(CoreFunctorFLOP + 3) + 45) )

using namespace electro;

template<class T>
int CalcField_CPU_T ( Array<Vector3<T> >& fieldLines, Array<pointCharge<T> >& pointCharges,
                      const size_t n, T resolution, perfPacket& perfData )
{
    if ( !n )
        return 1;
    if ( resolution == 0 )
        return 2;
    //get the size of the computation
    size_t p = pointCharges.GetSize();
    size_t totalSteps = ( fieldLines.GetSize() ) /n;

    if ( totalSteps < 2 )
        return 3;

    // Work with data pointers to avoid excessive function calls
    Vector3<T> *lines = fieldLines.GetDataPointer();
    pointCharge<T> *charges = pointCharges.GetDataPointer();

    //Used to mesure execution time
    long long freq, start, end;
    QueryHPCFrequency ( &freq );

    // Start measuring performance
    QueryHPCTimer ( &start );
    /*  Each Field line is independent of the others, so that every field line can be parallelized
        When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
        ideal number of threads based oh the runtime system's cache resources. The code will therefore
        run faster if omp_set_num_threads is not specified.
        The GNU and the Microsoft compiler produce highly suboptimal code
    */
#pragma omp parallel for
    for ( size_t line = 0; line < n; line++ )
    {
        // Intentionally starts from 1, since step 0 is reserved for the starting points
        for ( size_t step = 1; step < totalSteps; step++ )
        {

            // Set temporary cummulative field vector to zero
            Vector3<T> temp = {0,0,0},
                              prevPoint = lines[n* ( step - 1 ) + line];
            for ( size_t point = 0; point < p; point++ )
            {
                // Add partial vectors to the field vector
                temp += CoreFunctor ( charges[point], prevPoint );  // (electroPartFieldFLOP + 3) FLOPs
            }
            // Get the unit vector of the field vector, divide it by the resolution, and add it to the previous point
            lines[step*n + line] = ( prevPoint + vec3SetInvLen ( temp, resolution ) ); // Total: 13 FLOP (Add = 3 FLOP, setLen = 10 FLOP)
        }
    }
    // take ending measurement
    QueryHPCTimer ( &end );
    // Compute performance and time
    perfData.time = ( double ) ( end - start ) / freq;
    perfData.performance = ( n * ( ( totalSteps-1 ) * ( p* ( CoreFunctorFLOP + 3 ) + 13 ) ) / perfData.time ) / 1E9; // Convert from FLOPS to GFLOPS
    return 0;
}

template<class T>
int CalcField_CPU_T_Curvature ( Array<Vector3<T> >& fieldLines, Array<pointCharge<T> >& pointCharges,
                                const size_t n, T resolution, perfPacket& perfData )
{
    if ( !n )
        return 1;
    if ( resolution == 0 )
        return 2;
    //get the size of the computation
    size_t p = pointCharges.GetSize();
    size_t totalSteps = ( fieldLines.GetSize() ) /n;
    // since we are multithreading the computation, having
    // long lo.progress = line / n;
    // will not work as intended because different threads will process different ranges of line and the progress
    // indicator will jump herratically.
    // To solve this problem, we compute the percentage that one line represents, and add it to the total progress.
    double perStep = ( double ) 1/n;
    perfData.progress = 0;

    if ( totalSteps < 2 )
        return 3;

    // Work with data pointers to avoid excessive function calls
    Vector3<T> *pLines = fieldLines.GetDataPointer();
    pointCharge<T> *charges = pointCharges.GetDataPointer();

    //Used to mesure execution time
    long long freq, start, end;
    QueryHPCFrequency ( &freq );

    // Start measuring performance
    QueryHPCTimer ( &start );
    /*  Each Field line is independent of the others, so that every field line can be parallelized
        When compiling with the Intel C++ compiler, the OpenMP runtime will automatically choose  the
        ideal number of threads based oh the runtime system's cache resources. The code will therefore
        run faster if omp_set_num_threads is not specified.
        The GNU and the Microsoft compiler produce highly suboptimal code
    */

    //#pragma unroll_and_jam
#pragma omp parallel for
    for ( size_t line = 0; line < n; line++ )
    {
        // Intentionally starts from 1, since step 0 is reserved for the starting points
        for ( size_t step = 1; step < totalSteps; step++ )
        {

            // Set temporary cummulative field vector to zero
            Vector3<T> temp = {0,0,0}, prevVec, prevPoint;
            prevVec = prevPoint = pLines[n* ( step - 1 ) + line];// Load prevVec like this to ensure similarity with GPU kernel
            //#pragma unroll(4)
            //#pragma omp parallel for
            for ( size_t point = 0; point < p; point++ )
            {
                // Add partial vectors to the field vector
                temp += CoreFunctor ( charges[point], prevPoint );  // (electroPartFieldFLOP + 3) FLOPs
            }
            // Calculate curvature
            T k = vec3LenSq ( temp );//5 FLOPs
            k = vec3Len ( vec3Cross ( temp - prevVec, prevVec ) ) / ( k*sqrt ( k ) );// 25FLOPs (3 vec sub + 9 vec cross + 10 setLen + 1 div + 1 mul + 1 sqrt)
            // Finally, add the unit vector of the field divided by the resolution to the previous point to get the next point
            // We increment the curvature by one to prevent a zero curvature from generating #NaN or #Inf, though any positive constant should work
            pLines[step*n + line] = ( prevPoint + vec3SetInvLen ( temp, ( k+1 ) *resolution ) ); // Total: 15 FLOP (Add = 3 FLOP, setLen = 10 FLOP, add-mul = 2FLOP)
            prevVec = temp;
        }
        // update progress
#pragma omp atomic
        perfData.progress += perStep;
    }
    // take ending measurement
    QueryHPCTimer ( &end );
    // Compute performance and time
    perfData.time = ( double ) ( end - start ) / freq;
    perfData.performance = ( n * ( ( totalSteps-1 ) * ( p* ( CoreFunctorFLOP + 3 ) + 13 ) ) / perfData.time ) / 1E9; // Convert from FLOPS to GFLOPS
    return 0;
}

template<>
int CalcField_CPU<float> ( Array<Vector3<float> >& fieldLines, Array<pointCharge<float> >& pointCharges,
                           const size_t n, float resolution, perfPacket& perfData, bool useCurvature )
{
    if ( useCurvature ) return CalcField_CPU_T_Curvature<float> ( fieldLines, pointCharges, n, resolution, perfData );
    else return CalcField_CPU_T<float> ( fieldLines, pointCharges, n, resolution, perfData );
}

template<>
int CalcField_CPU<double> ( Array<Vector3<double> >& fieldLines, Array<pointCharge<double> >& pointCharges,
                            const size_t n, double resolution, perfPacket& perfData, bool useCurvature )
{
    if ( useCurvature ) return CalcField_CPU_T_Curvature<double> ( fieldLines, pointCharges, n, resolution, perfData );
    else return CalcField_CPU_T<double> ( fieldLines, pointCharges, n, resolution, perfData );
}