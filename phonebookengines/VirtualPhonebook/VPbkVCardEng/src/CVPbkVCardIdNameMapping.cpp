/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDES
#include "CVPbkVCardEng.h"
#include "CVPbkVCardIdNameMapping.h"

#include <barsread.h>


CVPbkVCardIdNameMapping* 
CVPbkVCardIdNameMapping::NewLC( TResourceReader& aResReader )
    {
    CVPbkVCardIdNameMapping* self = new (ELeave) CVPbkVCardIdNameMapping( aResReader );
    CleanupStack::PushL( self );
    self->ConstructL( aResReader );
    return self;
    }
        
CVPbkVCardIdNameMapping::~CVPbkVCardIdNameMapping()
    {
    delete iName;
    }

TInt CVPbkVCardIdNameMapping::Id() const
    {
    return iId;
    }

const TDesC8& CVPbkVCardIdNameMapping::Name() const
    {
    if ( iName )
        {
        return *iName;
        }
    return KNullDesC8();
    }

CVPbkVCardIdNameMapping::CVPbkVCardIdNameMapping( TResourceReader& aResReader )
:   iId( aResReader.ReadInt8() )
    {
    }
CVPbkVCardIdNameMapping::CVPbkVCardIdNameMapping()
:   iId( KErrNotFound )
    {
    }

void CVPbkVCardIdNameMapping::ConstructL( TResourceReader& aResReader )
    {
    HBufC* name16 = aResReader.ReadHBufCL();    
    CleanupStack::PushL(name16);
    HBufC8* name = HBufC8::NewLC(name16->Length());
    TPtr8 namePtr = name->Des();
    namePtr.Copy(*name16);        
    iName = name;
    CleanupStack::Pop(); // name
    CleanupStack::PopAndDestroy(); // name16
    }

// End of file


