/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


// INCLUDE FILES
#include "CLogsViewGlobals.h"
#include "CLogsExtensionLoader.h"

CLogsViewGlobals* CLogsViewGlobals::singleThis;

/// Unnamed namespace for local definitions
namespace {

// ==================== LOCAL FUNCTIONS ====================

/**
 * Returns global instance of CLogsViewGlobals.
 */
inline CLogsViewGlobals* Instance()
    {

    return CLogsViewGlobals::singleThis;

    }

}

// ================= MEMBER FUNCTIONS =======================

inline CLogsViewGlobals::CLogsViewGlobals()
    {
    }

inline CLogsViewGlobals* CLogsViewGlobals::NewLC()
    {
    CLogsViewGlobals* self = new (ELeave) CLogsViewGlobals;
    self->PushL();
    return self;
    }

/**
 * Increments reference count of this instance.
 */
inline void CLogsViewGlobals::IncRef()
    {
    ++iRefCount;
    }

/**
 * Decrements reference count.
 */
inline TInt CLogsViewGlobals::DecRef()
    {
    return --iRefCount;
    }

/**
 * Returns reference count.
 */
inline TInt CLogsViewGlobals::RefCount()
    {
    return iRefCount;
    }
        
CLogsViewGlobals* CLogsViewGlobals::InstanceL()
    {
    CLogsViewGlobals* instance = Instance();
    
    if (!instance)
        {        
        instance = CLogsViewGlobals::NewLC();
        
        CLogsViewGlobals::singleThis = instance;  
        
        CleanupStack::Pop(instance);                
        }

    instance->IncRef();

    return instance;
    }

CLogsViewGlobals::~CLogsViewGlobals()
    {
    delete iLoader;
    }

void CLogsViewGlobals::DoRelease()
    {
    if ( DecRef() == 0 )
        {

        singleThis = 0;

        delete this;
        }
    }

MLogsExtensionFactory& CLogsViewGlobals::ExtensionFactoryL()
    {
    if (!iLoader)
        {
        iLoader = CLogsExtensionLoader::NewL();
        }

    return iLoader->ExtensionFactory();
    }

// End of File
