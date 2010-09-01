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
* Description:  Phonebook 2 fetch dialog group page selection marker.
*
*/


#include "CPbk2FetchDlgGroupMarker.h"

// Phonebook 2
#include <MPbk2FetchDlg.h>
#include <CPbk2NamesListControl.h>

// Virtual Phonebook
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactGroup.h>

// System includes

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    ELogic = 1
    };

void Panic( TInt aReason )
    {
    _LIT( KPanicText, "CPbk2FetchDlgGroupMarker" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

}

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::CPbk2FetchDlgGroupMarker
// --------------------------------------------------------------------------
//
CPbk2FetchDlgGroupMarker::CPbk2FetchDlgGroupMarker
        ( CVPbkContactManager& aContactManager,
          MVPbkContactLinkArray& aSelectedContacts,
          MVPbkContactViewBase& aGroupsView,
          CPbk2NamesListControl& aControl ):
            CActive( EPriorityIdle ),
            iContactManager( aContactManager ),
            iSelectedContacts( aSelectedContacts ),
            iGroupsView( aGroupsView ),
            iControl( aControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::~CPbk2FetchDlgGroupMarker
// --------------------------------------------------------------------------
//
CPbk2FetchDlgGroupMarker::~CPbk2FetchDlgGroupMarker()
    {
    Cancel();
    delete iRetrieveOperation;
    delete iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FetchDlgGroupMarker::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::NewL
// --------------------------------------------------------------------------
//
CPbk2FetchDlgGroupMarker* CPbk2FetchDlgGroupMarker::NewL
        ( CVPbkContactManager& aContactManager,
          MVPbkContactLinkArray& aSelectedContacts,
          MVPbkContactViewBase& aGroupsView,
          CPbk2NamesListControl& aControl )
    {
    CPbk2FetchDlgGroupMarker* self = new ( ELeave ) CPbk2FetchDlgGroupMarker
        ( aContactManager, aSelectedContacts, aGroupsView, aControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::MarkSelectedGroupsL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::MarkSelectedGroupsL()
    {
    iCursor = 0;
    iState = ERetrieving;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::RunL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::RunL()
    {
    switch ( iState )
        {
        case ERetrieving:
            {
            RetrieveNextGroupL();
            break;
            }
        case EInspecting:
            {
            SelectGroupL();
            break;
            }
        case EStopping:     // FALLTHROUGH
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::DoCancel()
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlgGroupMarker::RunError( TInt /*aError*/ )
    {
    // Stop on errors
    Cancel();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    delete iStoreContact;
    iStoreContact = aContact;

    iState = EInspecting;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    // Stop on errors
    iState = EStopping;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::RetrieveNextGroupL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::RetrieveNextGroupL()
    {
    TInt count = iGroupsView.ContactCountL();

    if ( count > iCursor )
        {
        MVPbkContactLink* link = iGroupsView.CreateLinkLC( iCursor );

        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        iRetrieveOperation =  iContactManager.RetrieveContactL
            ( *link, *this );    
    
        CleanupStack::PopAndDestroy(); // link
        }
    else
        {
        // All groups are inspected
        iState = EStopping;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::SelectGroupL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::SelectGroupL()
    {
    MVPbkContactGroup* group = iStoreContact->Group();
    __ASSERT_DEBUG( group, Panic( ELogic ) );

    MVPbkContactLinkArray* members = group->ItemsContainedLC();
    
    if ( SelectionContainsAllMembersL( *members ) )
        {
        iControl.SetSelectedContactL( iCursor, ETrue );
        }
    else
        {
        iControl.SetSelectedContactL( iCursor, EFalse );
        }

    CleanupStack::PopAndDestroy(); // members
    
    ++iCursor;

    iState = ERetrieving;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::SelectionContainsAllMembersL
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgGroupMarker::SelectionContainsAllMembersL
        ( MVPbkContactLinkArray& aMembers )
    {
    TBool ret = ETrue;

    if ( iSelectedContacts.Count() < aMembers.Count() )
        {
        // The selection is smaller than member count
        ret = EFalse;
        }

    for ( TInt i = 0; ret && i < aMembers.Count(); ++i )
        {
        MVPbkContactLink* compareLink = aMembers.At( i ).CloneLC();
        if ( iSelectedContacts.Find( *compareLink ) < KErrNone )
            {
            // Group member was not found from selected contacts
            ret = EFalse;
            }
        CleanupStack::PopAndDestroy(); // compareLink
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupMarker::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupMarker::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
