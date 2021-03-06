/*
 * Copyright (C) 2010 - Alexandru Gagniuc - <mr.nuke.me@gmail.com>
 * This file is part of ElectroMag.
 *
 * ElectroMag is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ElectroMag is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with ElectroMag.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Abstract_Functor.hpp"
#include <cstdio>
#include <thread>

AbstractFunctor::AbstractFunctor()
{}

AbstractFunctor::~AbstractFunctor()
{}


unsigned long AbstractFunctor::AsyncFunctor(
    AbstractFunctor::AsyncParameters *aParameters)
{
    AbstractFunctor *pObject = aParameters->functorClass;
    std::mutex& hMutex = pObject->hRemapMutex;
    size_t functorID = aParameters->functorIndex;
    size_t deviceID = functorID;
    unsigned long retVal;
    bool fail = true;
    bool reIterate = true;
    while (reIterate)
    {
        // In order for the syncronization mechanism to work, all functorClass
        // in a Run() call must point to the same object
        retVal = pObject->MainFunctor(functorID, deviceID);

        // Check to see whether functor completed without errors
        fail = pObject->FailOnFunctor(functorID);

        hMutex.lock();
        if (fail)
        {
            // The functor has failed; it needs to be remapped to another device
            if (pObject->nIdle)
            {
                // Idle device available; we can remap immediately

                // Get the last idle functor in the list, and remove it from the
                // list
                deviceID = pObject->idleDevices[--pObject->nIdle];
                reIterate = true;
            }
            else
            {
                // No functor is idle, we need to add this to the the failed
                // queue and leave
                pObject->failedFunctors[pObject->nFailed++] = functorID;
                reIterate = false;
            }
        }
        else
        {
            // the functor has succeeded, this device can execute another
            // functor
            if (pObject->nFailed)
            {
                // A Failed functor is available for processing
                functorID = pObject->failedFunctors[--pObject->nFailed];
                reIterate = true;
            }
            else
            {
                // No failed functors, we can add this to the idle queue and
                //leave
                pObject->idleDevices[pObject->nIdle++] = deviceID;
                reIterate = false;
            }
        }
        hMutex.unlock();

    }

    return retVal;
}

unsigned long AbstractFunctor::AsyncAuxFunctor(
    AbstractFunctor::AsyncParameters *aParameters)
{
    return aParameters->functorClass->AuxFunctor();
}

unsigned long AbstractFunctor::Run()
{
    // Allocate needed resources on each device
    AllocateResources();
    if (this->Fail()) return (1<<16);
    
    size_t nFunctors;
    // Create parameters for functors
    GenerateParameterList(&nFunctors);
    if (Fail()) return (2<<16);

    // Alocate resources for calling the async functors
    AbstractFunctor::AsyncParameters *launchParams =
        new AbstractFunctor::AsyncParameters[nFunctors];
    std::thread ** handles = new std::thread*[nFunctors];
    // Allocate and initialize resources that the async functors will use for
    // syncronization
    idleDevices = new size_t[nFunctors];
    failedFunctors = new size_t[nFunctors];
    nFailed = nIdle = 0;

    for (size_t i = 0; i < nFunctors; i++)
    {
        launchParams[i].functorClass = this;
        launchParams[i].functorIndex = i;
        launchParams[i].nFunctors = nFunctors;
        handles[i] =
            new std::thread(AbstractFunctor::AsyncFunctor, &launchParams[i]);

        // Set the name for the thread
        //char threadName[512];
        //snprintf(threadName, 511, "AbstractFunctor Device %lu",
        //    (unsigned long)i);
        //Threads::SetThreadName(threadID, threadName);
    }

    // Create thread for auxiliary functor
    /*AbstractFunctor::AsyncParameters auxParams = {this, 0, 0};
    std::thread *hAuxFunctor
    = new std::thread(AbstractFunctor::AsyncAuxFunctor,&auxParams);*/
    // Set the name for the thread
    //Threads::SetThreadName(threadID, "Aux Functor");

    // Now wait for all functors to complete
    for (size_t i = 0; i < nFunctors; i++)
    {
        handles[i]->join();
    }

    // Release resources used for syncronization
    delete [] this->idleDevices;
    delete [] this->failedFunctors;

    // Now terminate the auxiliary functor
    //hAuxFunctor.cancel();
    //delete hAuxFunctor;

    PostRun();

    return this->nFailed;
}
