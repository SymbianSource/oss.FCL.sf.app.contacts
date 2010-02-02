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
* Description:  Phonebook 2 Localized text.
*
*/


#include "CPbk2LocalizedText.h"

/// SYSTEM INCLUDES
#include <barsread.h> // TResourceReader

namespace{
    
#ifdef _DEBUG
namespace {

    enum TPanic
        {
        EPanic_PreCond_Text = 1
        };
        
    void Panic( TPanic aPanicCode )
        {
        _LIT( KPanicCat, "CPbk2LocalizedText" );
        User::Panic( KPanicCat, aPanicCode );
        }
}
#endif // _DEBUG    

} // namespace


// --------------------------------------------------------------------------
// CPbk2LocalizedText::CPbk2LocalizedText
// --------------------------------------------------------------------------
//
CPbk2LocalizedText::CPbk2LocalizedText()
    {
    // Do nothing    
    }
    
// --------------------------------------------------------------------------
// CPbk2LocalizedText::CPbk2LocalizedText
// --------------------------------------------------------------------------
//
CPbk2LocalizedText::CPbk2LocalizedText( 
        TInt aUid, 
        HBufC* aText ) : 
    iContextUid( aUid ), 
    iText( aText )
    {
    // Do nothing    
    }    
    
// --------------------------------------------------------------------------
// CPbk2LocalizedText::~CPbk2LocalizedText
// --------------------------------------------------------------------------
//
CPbk2LocalizedText::~CPbk2LocalizedText()
    {
    delete iText;
    }

// --------------------------------------------------------------------------
// CPbk2LocalizedText::NewL
// --------------------------------------------------------------------------
//
CPbk2LocalizedText* CPbk2LocalizedText::NewL( TResourceReader& aReader )
    {
    CPbk2LocalizedText* self = new ( ELeave ) CPbk2LocalizedText();
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    CleanupStack::Pop( self );
    return self; 
    }
    
// --------------------------------------------------------------------------
// CPbk2LocalizedText::NewL
// --------------------------------------------------------------------------
//
CPbk2LocalizedText* CPbk2LocalizedText::NewL(
        TInt aUid, 
        HBufC* aText )
    {
    CPbk2LocalizedText* self = 
        new ( ELeave ) CPbk2LocalizedText( aUid, aText );
    return self; 
    }    

// --------------------------------------------------------------------------
// CPbk2LocalizedText::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2LocalizedText::ConstructL( TResourceReader& aReader )
    {
    iContextUid = aReader.ReadInt16();
    iText = aReader.ReadHBufCL();
    }

// --------------------------------------------------------------------------
// CPbk2LocalizedText::ContextId
// --------------------------------------------------------------------------
//
TInt CPbk2LocalizedText::ContextUid() const
    {
    return iContextUid;
    }

// --------------------------------------------------------------------------
// CPbk2LocalizedText::Text
// --------------------------------------------------------------------------
//
const TDesC& CPbk2LocalizedText::Text() const
    {
    __ASSERT_DEBUG( iText, Panic( EPanic_PreCond_Text ));
    return *iText;
    }

// End of file
