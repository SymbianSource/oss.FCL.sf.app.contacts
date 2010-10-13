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
* Description:  Phonebook 2 server app contact fetch phase.
*
*/


#include "CPbk2ContactFetchPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2ServerAppStoreManager.h"
#include <CPbk2FetchDlg.h>
#include <MPbk2ApplicationServices.h>
#include <CPbk2ContactViewBuilder.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement = 0;

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::CPbk2ContactFetchPhase
// --------------------------------------------------------------------------
//
CPbk2ContactFetchPhase::CPbk2ContactFetchPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          CPbk2ServerAppStoreManager& aStoreManager,
          CPbk2FetchDlg::TParams aFetchParams,
          TBool aFetchOkToExit,
          MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection
            aFetchAcceptPolicy,
          CVPbkFieldTypeSelector* aViewFilter,
          TBool aUsingDefaultConfig,
          const CVPbkContactStoreUriArray* aStoreUris ) :
                iObserver( aObserver ),
                iServerAppStoreManager( aStoreManager ),
                iFetchParams( aFetchParams ),
                iFetchOkToExit( aFetchOkToExit ),
                iFetchAcceptPolicy( aFetchAcceptPolicy ),
                iViewFilter( aViewFilter ),
                iUsingDefaultConfig( aUsingDefaultConfig ),
                iStoreUris( aStoreUris )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::~CPbk2ContactFetchPhase
// --------------------------------------------------------------------------
//
CPbk2ContactFetchPhase::~CPbk2ContactFetchPhase()
    {
    if ( iFetchDialogEliminator )
        {
        iFetchDialogEliminator->ForceExit();
        }
    delete iFetchNamesListView;
    delete iFetchGroupsListView;
    delete iFetchResults;
    delete iContactLinks;
    delete iRetrieveOperation;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactFetchPhase* CPbk2ContactFetchPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          CPbk2ServerAppStoreManager& aStoreManager,
          CPbk2FetchDlg::TParams aFetchParams,
          TBool aFetchOkToExit,
          MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection
            aFetchAcceptPolicy,
          CVPbkFieldTypeSelector* aViewFilter,
          TBool aUsingDefaultConfig,
          const CVPbkContactStoreUriArray* aStoreUris )
    {
    CPbk2ContactFetchPhase* self =
        new ( ELeave ) CPbk2ContactFetchPhase
            ( aObserver, aStoreManager, aFetchParams, aFetchOkToExit,
              aFetchAcceptPolicy, aViewFilter, aUsingDefaultConfig,
              aStoreUris );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::LaunchServicePhaseL()
    {
    CPbk2ServerAppAppUi* appUi = static_cast<CPbk2ServerAppAppUi*>
            ( CEikonEnv::Static()->EikAppUi() );

    TUint32 flags = EVPbkContactViewFlagsNone;
    // Because the filter needs to have the contact store open,
    // we can't build the view earlier
    delete iFetchNamesListView;
    iFetchNamesListView = NULL;
    iFetchNamesListView = iServerAppStoreManager.BuildFetchViewL
        ( EVPbkContactsView, *iStoreUris, iViewFilter,
          appUi->ApplicationServices().StoreProperties(),
          appUi->ApplicationServices().SortOrderManager(),
          flags );

    flags = EVPbkExcludeEmptyGroups;
    delete iFetchGroupsListView;
    iFetchGroupsListView = NULL;
    iFetchGroupsListView = iServerAppStoreManager.BuildFetchViewL
        ( EVPbkGroupsView, *iStoreUris, iViewFilter,
          appUi->ApplicationServices().StoreProperties(),
          appUi->ApplicationServices().SortOrderManager(),
          flags );

    // Launch the fetch dialog
    iFetchParams.iExitCallback = this;
    iFetchParams.iNamesListView = iFetchNamesListView;
    iFetchParams.iGroupsListView = iFetchGroupsListView;
    iFetchDlg = CPbk2FetchDlg::NewL( iFetchParams, *this );
    iFetchDialogEliminator = iFetchDlg;
    iFetchDialogEliminator->ResetWhenDestroyed( &iFetchDialogEliminator );
    iFetchDlg->ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::CancelServicePhase()
    {
    if ( iFetchDialogEliminator )
        {
        iFetchDialogEliminator->ForceExit();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::RequestCancelL( TInt aExitCommandId )
    {
    if ( iFetchDialogEliminator )
        {
        iFetchDialogEliminator->RequestExitL( aExitCommandId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::AcceptDelayedL
        ( const TDesC8& aContactLinkBuffer )
    {
    if ( iFetchDlg )
        {
        iFetchDlg->AcceptDelayedFetchL( aContactLinkBuffer );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2ContactFetchPhase::Results() const
    {
    return iFetchResults;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2ContactFetchPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2ContactFetchPhase::TakeStoreContact()
    {
    // No store contact concept in fetch
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2ContactFetchPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::Pbk2AcceptFetchSelectionL
// --------------------------------------------------------------------------
//
MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection
    CPbk2ContactFetchPhase::AcceptFetchSelectionL
        ( TInt aNumMarkedEntries, MVPbkContactLink& aLastSelection )
    {
    MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection result =
        iFetchAcceptPolicy;

    if ( result == MPbk2FetchDlgObserver::EFetchDelayed )
        {
        HBufC8* linkData = aLastSelection.PackLC();
        TPckg<TInt> buffer(aNumMarkedEntries);
        iObserver.PhaseAccept( *this, buffer, *linkData );
        CleanupStack::PopAndDestroy(); // linkData
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::FetchCompletedL
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::FetchCompletedL
        ( MVPbkContactLinkArray* aMarkedEntries )
    {
    delete iFetchResults;
    iFetchResults = NULL;
    iFetchResults = CVPbkContactLinkArray::NewL();

    CVPbkContactLinkArray* linkArray = iFetchResults;

    // View filter can not guarentee that returned contacts match the filter.
    // If user selects a group all the contacts of the group are returned
    // by fetch and we must manually filter them here.
    if ( iViewFilter )
        {
        delete iContactLinks;
        iContactLinks = NULL;
        iContactLinks = CVPbkContactLinkArray::NewL();
        linkArray = iContactLinks;
        }

    const TInt count = aMarkedEntries->Count();
    for (TInt i = 0; i < count; ++i)
        {
        linkArray->AppendL( aMarkedEntries->At(i).CloneLC() );
        CleanupStack::Pop(); // aMarkedEntries->At(i).CloneLC()
        }

    if ( iViewFilter )
        {
        // Go through contacts one by one
        RetrieveContactL();
        }
    else
        {
        // Finished
        iObserver.NextPhase( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::FetchCanceled
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::FetchCanceled()
    {
    delete iFetchResults;
    iFetchResults = NULL;
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::FetchAborted
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::FetchAborted()
    {
    delete iFetchResults;
    iFetchResults = NULL;

    iObserver.PhaseAborted( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::FetchOkToExit
// --------------------------------------------------------------------------
//
TInt CPbk2ContactFetchPhase::FetchOkToExit()
    {
    return iFetchOkToExit;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactFetchPhase::OkToExitL( TInt aCommandId )
    {
    return iObserver.PhaseOkToExit( *this, aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    TRAPD( err, HandleContactOperationCompleteL( aContact ) );
    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::HandleContactOperationCompleteL
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::HandleContactOperationCompleteL
        ( MVPbkStoreContact* aContact )
    {
    // Verify current contact
    VerifyContactL( aContact );
    
    // Retrieve next contact
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi* appUi = static_cast<CPbk2ServerAppAppUi*>
            ( CEikonEnv::Static()->EikAppUi() );

    // Fetch one contact at a time if service cancellation is not
    // commanded.
    if ( iContactLinks && iContactLinks->Count() > 0 )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        iRetrieveOperation = appUi->ApplicationServices().ContactManager().
            RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
        iContactLinks->Delete( KFirstElement );
        }
    else
        {
        iObserver.NextPhase( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactFetchPhase::VerifyContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactFetchPhase::VerifyContactL( MVPbkStoreContact* aContact )
    {
    CleanupDeletePushL( aContact );

    MVPbkStoreContactFieldCollection& fields = aContact->Fields();
    TInt fieldCount = fields.FieldCount();

    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const MVPbkStoreContactField& field = fields.FieldAt( i );
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && iViewFilter->IsFieldTypeIncluded( *fieldType ) )
            {
            MVPbkContactLink* link = aContact->CreateLinkLC();
            CleanupStack::Pop(); // link
            iFetchResults->AppendL( link );
            break;
            }
        }

    CleanupStack::PopAndDestroy(); // aContact
    }

// End of File
