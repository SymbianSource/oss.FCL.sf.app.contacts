/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class that reads the icon information of the extension
*
*/



// INCLUDE FILES
#include "CPbk2UIExtensionIconInformation.h"

#include <coemain.h>
#include <barsread.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconInformation::TIconArrayInfo::TIconArrayInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionIconInformation::TIconArrayInfo::TIconArrayInfo(
        TResourceReader& aReader) :   
    iParentArrayId(static_cast<TPbk2IconArrayId>(aReader.ReadInt8())),
    iExtensionIconArrayResId(aReader.ReadInt32())
    {
    }
    
// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconInformation::CPbk2UIExtensionIconInformation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionIconInformation::CPbk2UIExtensionIconInformation()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconInformation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2UIExtensionIconInformation::ConstructL(
        TInt aResourceId)
    {
    // reads the UI extension icon information from the resource structure
    if (aResourceId != 0)
        {
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC(reader, aResourceId);
        // read the icon info array resource id
        iIconInfoArrayResId = reader.ReadInt32();
        
        // read the array of PHONEBOOK2_EXTENSION_ICON_ARRAY structures
        const TInt iconArrayCount = reader.ReadInt16();
        for (TInt i = 0; i < iconArrayCount; ++i)
            {
            iIconArrayExtensions.AppendL(TIconArrayInfo(reader));
            }
        
        CleanupStack::PopAndDestroy(); // reader
        }
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconInformation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionIconInformation* CPbk2UIExtensionIconInformation::NewL(
        TInt aResourceId)
    {
    CPbk2UIExtensionIconInformation* self = 
            new( ELeave ) CPbk2UIExtensionIconInformation();
    CleanupStack::PushL( self );
    self->ConstructL(aResourceId);
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CPbk2UIExtensionIconInformation::~CPbk2UIExtensionIconInformation()
    {
    iIconArrayExtensions.Close();
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconInformation::IconInfoArrayResourceId
// -----------------------------------------------------------------------------
//
TInt CPbk2UIExtensionIconInformation::IconInfoArrayResourceId()
    {
    return iIconInfoArrayResId;
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconInformation::FindIconArrayExtension
// -----------------------------------------------------------------------------
//      
TBool CPbk2UIExtensionIconInformation::FindIconArrayExtension(
        TPbk2IconArrayId aParentArrayId, 
        TInt& aResId)
    {
    const TInt count = iIconArrayExtensions.Count();
    for (TInt i = 0; i < count; ++i)
        { 
        if (iIconArrayExtensions[i].iParentArrayId == aParentArrayId &&
            iIconArrayExtensions[i].iParentArrayId != EPbk2NullIconArrayId)
            {
            aResId = iIconArrayExtensions[i].iExtensionIconArrayResId;
            return ETrue;
            }
        }
    return EFalse;
    }
    
//  End of File  
