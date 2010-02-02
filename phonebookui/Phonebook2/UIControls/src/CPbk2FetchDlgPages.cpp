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
* Description:  Phonebook 2 fetch dialog pages.
*
*/


// INCLUDE FILES
#include "CPbk2FetchDlgPages.h"

// Phonebook 2
#include "MPbk2FetchDlgPage.h"
#include <CPbk2ViewState.h>
#include <MPbk2ContactUiControl.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EInvalidPageCount = 1,
    ECurrentPageNULL,
    EPageNotFound,
    EPageIndexOOB
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPBk2FetchDlgPage" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::CPbk2FetchDlgPages
// --------------------------------------------------------------------------
//
CPbk2FetchDlgPages::CPbk2FetchDlgPages()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::~CPbk2FetchDlgPages
// --------------------------------------------------------------------------
//
CPbk2FetchDlgPages::~CPbk2FetchDlgPages()
    {
    for (TInt i=iPages.Count()-1; i>=0; --i)
        {
        delete iPages[i];
        }
    iPages.Close();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::AddL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPages::AddL( MPbk2FetchDlgPage* aPage )
    {
    User::LeaveIfError( iPages.Append( aPage ) );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::DlgPageCount
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlgPages::DlgPageCount() const
    {
    return iPages.Count();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::DlgPageAt
// --------------------------------------------------------------------------
//
MPbk2FetchDlgPage& CPbk2FetchDlgPages::DlgPageAt( TInt aIndex ) const
    {
    __ASSERT_DEBUG( DlgPageCount() > aIndex, Panic( EPageIndexOOB ) );
    return *const_cast<MPbk2FetchDlgPage*>(iPages[aIndex]);
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::DlgPageWithId
// --------------------------------------------------------------------------
//
MPbk2FetchDlgPage* CPbk2FetchDlgPages::DlgPageWithId( TInt aPageId ) const
    {
    MPbk2FetchDlgPage* ret = NULL;

    const TInt count = iPages.Count();
    for ( TInt i=0; i < count; ++i )
        {
        const MPbk2FetchDlgPage* page = iPages[i];
        if ( page->FetchDlgPageId() == aPageId )
            {
            ret = const_cast<MPbk2FetchDlgPage*>( page );
            break;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::HandlePageChangedL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPages::HandlePageChangedL( TInt aPageId )
    {
    MPbk2FetchDlgPage* newPage = DlgPageWithId( aPageId );

    if ( newPage )
        {
        if ( iCurrentPage )
            {
            iCurrentPage->DeactivateFetchDlgPage();
            }
        newPage->ActivateFetchDlgPageL();
        iCurrentPage = newPage;
        }

    __ASSERT_DEBUG( newPage, Panic( EPageNotFound ) );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::AllPagesReady
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgPages::AllPagesReady()
    {
    const TInt count = DlgPageCount();
    __ASSERT_DEBUG( count > 0, Panic( EInvalidPageCount ) );

    TBool ret = ETrue;

    for ( TInt i = 0; i < count; ++i )
        {
        if ( !DlgPageAt( i ).DlgPageReady() )
            {
            ret = EFalse;
            break;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::IsActivePageEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgPages::IsActivePageEmpty()
    {
    return iCurrentPage->DlgPageEmpty();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::ActiveFirstPageL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPages::ActiveFirstPageL()
    {
    __ASSERT_DEBUG(DlgPageCount() > 0, Panic(EInvalidPageCount));
    iCurrentPage = &DlgPageAt(0);
    iCurrentPage->ActivateFetchDlgPageL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2FetchDlgPages::FocusedContactL() const
    {
    __ASSERT_DEBUG( iCurrentPage, Panic( ECurrentPageNULL ) );
    return iCurrentPage->FocusedContactL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::CurrentPage
// --------------------------------------------------------------------------
//
MPbk2FetchDlgPage& CPbk2FetchDlgPages::CurrentPage() const
    {
    return *iCurrentPage;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPages::SelectContactL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPages::SelectContactL
        ( const MVPbkContactLink& aContactLink, TBool aSelect )
    {
    const TInt count = DlgPageCount();
    __ASSERT_DEBUG( count > 0, Panic( EInvalidPageCount ) );

    for ( TInt i = 0; i < count; ++i )
        {
        DlgPageAt( i ).SelectContactL( aContactLink, aSelect );
        }
    }

// End of File
