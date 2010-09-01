/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Scanner that finds extensions.
*
*/


// INCLUDE FILES
#include "CxSPScanner.h"
#include <ecom/ecom.h>
#include "CxSPLoader.h"
#include "ExtensionManagerUID.H"


// Unnamed namespace for local definitions
namespace {

_LIT8(KPbkxSPExtension, KPBKXSPDEFAULTDATASTRING);

void CleanupResetAndDestroy(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }
} // namespace

// ==================== MEMBER FUNCTIONS ====================
CxSPScanner::CxSPScanner(CxSPArray& aExtensions) :
    iExtensions(aExtensions)
    {
	}

CxSPScanner::~CxSPScanner()
    {
    }


CxSPScanner* CxSPScanner::NewL(CxSPArray& aExtensions)
    {
    CxSPScanner* self =
        new (ELeave) CxSPScanner(aExtensions);
    return self;
    }

void CxSPScanner::ScanL()
    {
    RImplInfoPtrArray implementations;
    TEComResolverParams resolverParams;

    resolverParams.SetWildcardMatch(ETrue);
    resolverParams.SetDataType(KPbkxSPExtension);

    REComSession::ListImplementationsL
        (TUid::Uid(KPBKXSPINTERFACEDEFINITIONUID),
         resolverParams,
         implementations);

    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy,
                                     &implementations));

    const TInt count = implementations.Count();
    iExtensions.SetReserveL(iExtensions.Count() + count);
    for (TInt i = count-1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);

        CxSPLoader* extension = NULL;
        TRAPD(err,
              extension = CxSPLoader::NewL(implInfo->ImplementationUid()));
        if (err == KErrNone)
            {
            CleanupStack::PushL(extension);
            iExtensions.AppendL(extension);
            CleanupStack::Pop(extension);
            }

        CleanupStack::PopAndDestroy(implInfo);
        }
    CleanupStack::PopAndDestroy(); // implementations     
    }
    
// End of File
