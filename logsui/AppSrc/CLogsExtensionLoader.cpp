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
*       Implementation for extension dll loader.
*
*/


// INCLUDE FILES
#include "CLogsExtensionLoader.h"
#include "TLogsDummyExtensionFactory.h"
#include "ExtensionUID.h"
#include <data_caging_path_literals.hrh>  //KDC_APP_BITMAP_DIR and KDC_SHARED_LIB_DIR
#include <featmgr.h>

// MODULE DATA STRUCTURES
typedef MLogsExtensionFactory* (*TExtensionEntryPoint)();


// ================= MEMBER FUNCTIONS =======================
inline CLogsExtensionLoader::CLogsExtensionLoader()
    {
    }

inline void CLogsExtensionLoader::ConstructL()
    {
    //Tries to load the extension dll and initialise iFactory to point to the loaded dll
    TRAPD(err, LoadExtensionDllL());
    
    //If dll is not part of rom image then initialise iFactory to point to dummy implementation instead
    if (err)
        {    
        if ( iFactory )
            {
            iFactory->Release();
            iFactory = NULL;
            }
        iFactory = TLogsDummyExtensionFactory::NewL();
        }
    }

CLogsExtensionLoader* CLogsExtensionLoader::NewL()
    {
    CLogsExtensionLoader* self = new (ELeave) CLogsExtensionLoader;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CLogsExtensionLoader::~CLogsExtensionLoader()
    {      
    if ( iFactory )
        {
        iFactory->Release();
        iFactory = NULL;
        }
    iDll.Close(); // This has to be last 
    }

//Returns refrence to loaded dll's factory functionality or default implementation
MLogsExtensionFactory& CLogsExtensionLoader::ExtensionFactory()
    {    
    return *iFactory;
    }

TUidType CLogsExtensionLoader::UidType()
    {
    return TUidType(KNullUid, TUid::Uid(KExtensionDllUID2), KNullUid);
    }

void CLogsExtensionLoader::LoadExtensionDllL()
    {
    // If voip is not supported then don't try to load the
    // extension dll, leave will cause the dummy extension to be
    // used instead
    if ( !FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        User::Leave(KErrNotFound);
        }        
    else
        {
        // Set up path+filename for extension dll
        TFileName dllFileWithPath;

        dllFileWithPath += KDC_SHARED_LIB_DIR;
        dllFileWithPath += KLogsUiExtensionDllFile;  

        User::LeaveIfError(iDll.Load(dllFileWithPath, UidType()));  
    
        // Get the 1st entry point from the DLL
        TExtensionEntryPoint createExtensionFactoryL =
            reinterpret_cast<TExtensionEntryPoint>(iDll.Lookup(1));
        
        if (!createExtensionFactoryL)
            {
            User::Leave(KErrNotFound);
            }

         // Call the entry point to create the test suite
        iFactory = createExtensionFactoryL();
        if (!iFactory)
            {
            User::Leave(KErrNotFound);
            }
        }
    }

// End of File
