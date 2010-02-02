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
* Description:  Phonebook 2 Store Property Content Store Name.
*
*/


#include "CPbk2StorePropertyContentStoreName.h"

#include "CPbk2LocalizedText.h"

// --------------------------------------------------------------------------
// CPbk2StorePropertyContentStoreName::CPbk2StorePropertyContentStoreName
// --------------------------------------------------------------------------
//
CPbk2StorePropertyContentStoreName::CPbk2StorePropertyContentStoreName(
        const TArray<const CPbk2LocalizedText*> aTexts,
        TPbk2ContentId aContentId ) :
    iTexts( aTexts ), iContentId( aContentId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyContentStoreName::~CPbk2StorePropertyContentStoreName
// --------------------------------------------------------------------------
//
CPbk2StorePropertyContentStoreName::~CPbk2StorePropertyContentStoreName()
    {
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyContentStoreName::Uid
// --------------------------------------------------------------------------
//
TInt CPbk2StorePropertyContentStoreName::Uid() const
    {    
    return iContentId;
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyContentStoreName::StoreName
// --------------------------------------------------------------------------
//    
const TDesC& CPbk2StorePropertyContentStoreName::StoreName() const
    {
    for ( TInt i = 0; i < iTexts.Count(); ++i )
        {
        if ( iTexts[ i ]->ContextUid() == iContentId )
            {
            return iTexts[ i ]->Text();
            }
        }
        
    return KNullDesC;
    }
    
// End of file
