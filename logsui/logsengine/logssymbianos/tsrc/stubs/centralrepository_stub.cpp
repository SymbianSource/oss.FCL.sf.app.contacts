/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include <centralrepository.h>
#include "centralrepository_stub_helper.h"

TInt mFailCode = KErrNone;
TInt mCurrentVal = 0;

void CentralRepositoryStubHelper::reset()
{
    mFailCode = KErrNone;
    mCurrentVal = 0;
}
void CentralRepositoryStubHelper::setFailCode(TInt err)
{
    mFailCode = err;
}
void CentralRepositoryStubHelper::setCurrentVal(TInt val)
{
    mCurrentVal = val;
} 
TInt CentralRepositoryStubHelper::currentVal()
{
    return mCurrentVal;
}  
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CRepository* CRepository::NewL( TUid aRepositoryUid )
    {
    CRepository* self = CRepository::NewLC( aRepositoryUid );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CRepository* CRepository::NewLC( TUid aRepositoryUid )
    {
    CRepository* self = new (ELeave) CRepository();
    CleanupStack::PushL( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRepository::~CRepository()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CRepository::Get(TUint32 aKey, TInt& aValue)
    {
    aValue = mCurrentVal;
    return mFailCode;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CRepository::Set(TUint32 aKey, TInt aValue)
    {
    if ( mFailCode == KErrNone )
        {
        mCurrentVal = aValue;
        }
    return mFailCode;
    }


