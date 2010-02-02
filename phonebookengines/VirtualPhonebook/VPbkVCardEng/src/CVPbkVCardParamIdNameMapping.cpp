/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cvpbkvcardeng.h"
#include "cvcardidnamemapping.h"

#include <barsread.h>


CVPbkVCardEng::CVCardIdNameMapping* 
CVPbkVCardEng::CVCardIdNameMapping::NewLC( TResourceReader& aResReader )
    {
    CVCardIdNameMapping* self = new (ELeave) CVCardIdNameMapping( aResReader );
    CleanupStack::PushL( self );
    self->ConstructL( aResReader );
    return self;
    }
        
CVPbkVCardEng::CVCardIdNameMapping::~CVCardIdNameMapping()
    {
    delete iName;
    }

TInt CVPbkVCardEng::CVCardIdNameMapping::Id() const
    {
    return iId;
    }

const TDesC8& CVPbkVCardEng::CVCardIdNameMapping::Name() const
    {
    return *iName;
    }

CVPbkVCardEng::CVCardIdNameMapping::CVCardIdNameMapping( TResourceReader& aResReader )
:   iId( aResReader.ReadInt8() )
    {
    }

void CVPbkVCardEng::CVCardIdNameMapping::ConstructL( TResourceReader& aResReader )
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


