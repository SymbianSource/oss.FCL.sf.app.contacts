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
* Description:  Phonebook 2 PoC AIW interest item.
*
*/


#include "CPbk2AiwInterestItemPoc.h"

// Phonebook 2
#include "CPbk2CallTypeSelector.h"
#include "CPbk2PocCmd.h"
#include <TPbk2StoreContactAnalyzer.h>
#include <MPbk2Command.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2Commands.rsg>
#include <MPbk2ContactUiControl.h>
#include <Pbk2UID.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include "cpbk2storestatechecker.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <AiwServiceHandler.h>
#include <eikmenup.h>
#include <eikenv.h>
#include <AiwPoCParameters.h>
#include <AiwPoCParameters.hrh>

/// Unnamed namespace for local definitions
namespace {

/**
 * Standard error handling.
 *
 * @param aError    Error code.
 */
void HandleError( const TInt aError )
    {
    if (aError != KErrNone)
        {
        // Get UIKON environment
        CEikonEnv* eikEnv = CEikonEnv::Static();
        if (eikEnv)
            {
            // Display standard error message
            eikEnv->HandleError(aError);
            }
        }
    }

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ExecuteCommandL = 1,
    EPanicPreCond_ReadContactL,
    EPanicLogic_UnknownState
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2AiwInterestItemPoc" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG


} /// namespace


// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::CPbk2AiwInterestItemPoc
// --------------------------------------------------------------------------
//
inline CPbk2AiwInterestItemPoc::CPbk2AiwInterestItemPoc
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) :
            CPbk2AiwInterestItemBase( aInterestId, aServiceHandler )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::~CPbk2AiwInterestItemPoc
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemPoc::~CPbk2AiwInterestItemPoc()
    {
    Cancel();
    delete iStoreStateChecker;
    delete iContactLink;
    delete iStoreContact;
    delete iSelector;
    delete iRetriever;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwInterestItemPoc::ConstructL()
    {
    iFieldPropertyArray =
        &Phonebook2::Pbk2AppUi()->ApplicationServices().FieldProperties();
    iSelector = CPbk2CallTypeSelector::NewL
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemPoc* CPbk2AiwInterestItemPoc::NewL
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2AiwInterestItemPoc* self =
        new (ELeave) CPbk2AiwInterestItemPoc( aInterestId, aServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemPoc::DynInitMenuPaneL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
          const MPbk2ContactUiControl& aControl )
    {
    TBool ret = EFalse;

    switch ( aResourceId )
        {
        case R_PHONEBOOK2_GROUPVIEWS_POC_MENU:
            {
            ret = ETrue;

            if ( aControl.NumberOfContacts() > 0 )
                {
                DoInitMenuPaneL( aResourceId, aMenuPane, aControl,
                    EAiwPoCCmdTalkGroup );
                }
            else
                {
                aMenuPane.SetItemDimmed( KAiwCmdPoC, ETrue );
                }
            break;
            }
        case R_PHONEBOOK2_POC_MENU:
            {
            ret = ETrue;
            TBool noPTTField = EFalse;

            // In contact info view, the PoC menu must be filtered
            // away if there are no PoC fields
            const MVPbkStoreContact* storeContact =
                aControl.FocusedStoreContact();

            // Store contact is not available if we are not in contact
            // info view
            if ( storeContact )
                {
                TPbk2StoreContactAnalyzer analyzer
                    ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                      ContactManager(), storeContact );

                if ( analyzer.HasFieldL
                        ( R_PHONEBOOK2_SIP_MSISDN_SELECTOR ) < 0 )
                    {
                    noPTTField = ETrue;
                    }
                }

            // If the list is empty hide PoC option.
            // In contact info view, the contact must also have
            // a PTT field available.
            if ( aControl.NumberOfContacts() > 0 && !noPTTField )
                {
                DoInitMenuPaneL( aResourceId, aMenuPane, aControl );
                }
            else
                {
                aMenuPane.SetItemDimmed( KAiwCmdPoC, ETrue );
                }
            break;
            }
        case R_PHONEBOOK2_POC_CONTEXT_MENU:
            {
            ret = ETrue;

            // Hide the item by default
            aMenuPane.SetItemDimmed( KAiwCmdPoC, ETrue );

            // Get the focused field
            const MVPbkBaseContactField* field =
                aControl.FocusedField();

            if ( field )
                {
                TInt tmp( KErrNotFound );

                TPbk2StoreContactAnalyzer analyzer
                    ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                      ContactManager(), NULL );

                // Pager field is part of SIP_MSISDN selector
                // so exclude it from the menu here first
                if ( analyzer.IsFieldTypeIncludedL(
                        *field, R_PHONEBOOK2_PAGER_SELECTOR ) &&
                        aMenuPane.MenuItemExists( KAiwCmdPoC, tmp ) )
                    {
                    // If we are in pager field let the item be dimmed
                    }

                // Check are we dealing with a PTT field
                else if ( analyzer.IsFieldTypeIncludedL(
                        *field, R_PHONEBOOK2_SIP_MSISDN_SELECTOR ) )
                    {
                    aMenuPane.SetItemDimmed( KAiwCmdPoC, EFalse );
                    DoInitMenuPaneL( aResourceId, aMenuPane, aControl );
                    }
                }
            break;
            }

        default:
            {
            // Do nothing
            break;
            }
        };
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemPoc::HandleCommandL
        ( const TInt aMenuCommandId, MPbk2ContactUiControl& aControl,
          TInt aServiceCommandId )
    {
    TBool ret = EFalse;
    // Only handle this command if service command is talk
    if ( aServiceCommandId == KAiwCmdPoC )
        {
        ret = ETrue;
        iMenuCommandId = aMenuCommandId;
        iControl = &aControl;

        // Retrieve store contact if needed
        MVPbkStoreContact* storeContact = const_cast<MVPbkStoreContact*>(
            aControl.FocusedStoreContact() );
        if ( !storeContact )
            {
            // No need to check store state, store always available
            // if cmd is invoked from nameslist
            IssueNextAction( ERetrieveContact );
            }
        else
            {
            // Check store availability
            CPbk2StoreStateChecker* temp =
                CPbk2StoreStateChecker::NewL(
                    storeContact->ParentStore(), *this );
            delete iStoreStateChecker;
            iStoreStateChecker = temp;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iRetriever )
        {
        delete iStoreContact;
        iStoreContact = aContact;

        IssueNextAction( EExecutePoc );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    if ( &aOperation == iRetriever )
        {
        HandleError( aError );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::StoreState
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::StoreState
        ( MPbk2StoreStateCheckerObserver::TState aState )
    {
    if ( aState == MPbk2StoreStateCheckerObserver::EStoreAvailable )
        {
        IssueNextAction( EExecutePoc );
        }
    else
        {
        IssueNextAction( EShowUnavailableNote );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::RunL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::RunL()
    {
    switch ( iState )
        {
        case ERetrieveContact:
            {
            RetrieveContactL();
            break;
            }
        case EExecutePoc:
            {
            MVPbkStoreContact* storeContact = const_cast<MVPbkStoreContact*>(
                iControl->FocusedStoreContact() );
            if ( !storeContact )
                {
                storeContact = iStoreContact;
                }
            ExecuteCommandL( *storeContact );
            break;
            }
        case EShowUnavailableNote:
            {
            if ( iStoreStateChecker )
                {
                iStoreStateChecker->ShowUnavailableNoteL();
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG(iControl, Panic( EPanicLogic_UnknownState ) );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::ExecuteCommandL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::ExecuteCommandL
        ( MVPbkStoreContact& aStoreContact )
    {
    __ASSERT_DEBUG(iControl, Panic( EPanicPreCond_ExecuteCommandL ) );

    // Execute the command
    MPbk2Command* cmd = CreatePocCmdObjectL
        ( iMenuCommandId, *iControl, aStoreContact );
    cmd->ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::RetrieveContactL()
    {
    delete iContactLink;
    iContactLink = NULL;
    iContactLink = iControl->FocusedContactL()->CreateLinkLC();
    CleanupStack::Pop(); // iContactLink

    delete iRetriever;
    iRetriever = NULL;
    iRetriever = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL( *iContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::IssueRetrieveRequest
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::IssueNextAction( TNextAction aState )
    {
    if ( !IsActive() )
        {
        iState = aState;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::DoInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemPoc::DoInitMenuPaneL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
          const MPbk2ContactUiControl& aControl,
          TUint aPreferredMenu /*= 0*/ ) const
    {
    CAiwGenericParamList& params = iServiceHandler.InParamListL();
    TAiwPocParameterData pocParameter;
    pocParameter.iConsumerAppUid = TUid::Uid( KPbk2UID3 );
    pocParameter.iConsumerWindowGroup =
        CCoeEnv::Static()->RootWin().Identifier();
    pocParameter.iCommandId = 0; // not used in menu construct

    if ( aPreferredMenu != 0 )
        {
        // Select POC call type
        pocParameter.iPreferredMenu = EAiwPoCCmdTalkGroup;
        }
    else
        {
        pocParameter.iPreferredMenu =
            iSelector->SelectPocCallType( aControl );
        }

    // PoC parameter data always included
    TAiwGenericParam param = TAiwGenericParam( EGenericParamPoCData,
        TAiwVariant( TAiwPocParameterDataPckg( pocParameter ) ) );
    params.AppendL( param );

    // Let provider add its menu items to the menu
    iServiceHandler.InitializeMenuPaneL( aMenuPane, aResourceId,
        KAiwCmdPoC, params);
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemPoc::CreatePocCmdObjectL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2AiwInterestItemPoc::CreatePocCmdObjectL( 
        const TInt aMenuCommandId, 
        MPbk2ContactUiControl& aControl,
        MVPbkStoreContact& aStoreContact )
    {
    MPbk2Command* cmd = NULL;

    cmd = CPbk2PocCmd::NewL( aMenuCommandId, iServiceHandler,
        *iSelector, aControl, aStoreContact, NULL );

    return cmd;
    }

// End of File
