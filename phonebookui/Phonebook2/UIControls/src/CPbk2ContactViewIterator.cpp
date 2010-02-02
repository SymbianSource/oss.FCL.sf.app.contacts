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
* Description:  Phonebook 2 contact link iterator.
*
*/


#include "CPbk2ContactViewIterator.h"

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>

/// Unnamed namespace for local definitions
namespace {
    const TInt KAtFirstElement( 0 );

} /// namespace
   
// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::CPbk2ContactViewIterator
// --------------------------------------------------------------------------
//    
inline CPbk2ContactViewIterator::CPbk2ContactViewIterator
        ( MVPbkContactViewBase& aView, TArray<TInt> aSelectedIndexes )  :
            iView( aView ), 
            iSelectedIndexes( aSelectedIndexes ), 
            iCursor( KAtFirstElement )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::~CPbk2ContactViewIterator
// --------------------------------------------------------------------------
//     
CPbk2ContactViewIterator::~CPbk2ContactViewIterator()    
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::NewLC
// --------------------------------------------------------------------------
//     
CPbk2ContactViewIterator* CPbk2ContactViewIterator::NewLC
        ( MVPbkContactViewBase& aView,
          TArray<TInt> aSelectedIndexes )
    {
    CPbk2ContactViewIterator* self = 
        new (ELeave) CPbk2ContactViewIterator( aView, aSelectedIndexes );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::HasNext
// --------------------------------------------------------------------------
//     
TBool CPbk2ContactViewIterator::HasNext() const
    {
    TBool ret = EFalse;

    if ( iCursor < iSelectedIndexes.Count() )
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::HasPrevious
// --------------------------------------------------------------------------
//     
TBool CPbk2ContactViewIterator::HasPrevious() const
    {
    TBool ret = EFalse;
    if ( iCursor > KAtFirstElement )
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::NextL
// --------------------------------------------------------------------------
// 
MVPbkContactLink* CPbk2ContactViewIterator::NextL()
    {
    MVPbkContactLink* link = NULL;
    if ( HasNext() )
        {
        link = iView.ContactAtL
            ( iSelectedIndexes[ iCursor ] ).CreateLinkLC();
        CleanupStack::Pop(); // link
        ++iCursor;
        }
    return link;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::PreviousL
// --------------------------------------------------------------------------
//     
MVPbkContactLink* CPbk2ContactViewIterator::PreviousL()
    {
    MVPbkContactLink* link = NULL;
    if ( HasPrevious() )
        {
        --iCursor;
        link = iView.ContactAtL
            ( iSelectedIndexes[ iCursor ] ).CreateLinkLC();
        CleanupStack::Pop(); // link    
        }
    return link;    
    }
    
// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::SetToFirst
// --------------------------------------------------------------------------
//     
void CPbk2ContactViewIterator::SetToFirst()
    {
    iCursor = KAtFirstElement;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewIterator::SetToLast
// --------------------------------------------------------------------------
//     
void CPbk2ContactViewIterator::SetToLast()
    {
    iCursor = iSelectedIndexes.Count();
    }    
    
// End of File
