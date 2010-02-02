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
* Description:  Phonebook 2 call AIW interest item.
*
*/


// INCLUDE FILES
#include "CPbk2AiwInterestItemCall.h"

// Phonebook 2
#include "CPbk2CallCmd.h"
#include "CPbk2PrependCmd.h"
#include "CPbk2PocCmd.h"
#include <MPbk2Command.h>
#include "MPbk2CommandFactory.h"
#include "CPbk2CallTypeSelector.h"
#include <MPbk2ContactUiControl.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2Commands.rsg>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2AddressSelect.h>
#include <TPbk2AddressSelectParams.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2ApplicationServices.h>
#include "cpbk2storestatechecker.h"
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <VPbkFieldTypeSelectorFactory.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkxSPContacts.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <AiwServiceHandler.h>
#include <eikmenup.h>
#include <eikenv.h>
#include <featmgr.h>
#include <spsettingsvoiputils.h>

// Debugging headers
#include <Pbk2Debug.h>

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
    EPanicLogic_DynInitMenuPaneL = 1,
    EPanicPreCond_ExecuteCommandL,
    EPanicLogic_UnknownState
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2AiwInterestItemCall" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG

/**
 * Copies a link array to another.
 *
 * @param aSourceLinkArray    Link array which is copied
 * @param aTargetLinkArray    Links are copied to this
 */
void CopyContactLinksL( const MVPbkContactLinkArray& aSourceLinkArray,
        CVPbkContactLinkArray& aTargetLinkArray )
    {
    const TInt count = aSourceLinkArray.Count();
    for ( TInt i(0); i < count; ++i )
        {
        const MVPbkContactLink& contactLink = aSourceLinkArray.At(i);
        aTargetLinkArray.AppendL( contactLink.CloneLC() );
        CleanupStack::Pop(); // link
        }
    }


} /// namespace

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::CPbk2AiwInterestItemCall
// --------------------------------------------------------------------------
//
inline CPbk2AiwInterestItemCall::CPbk2AiwInterestItemCall
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) :
            CPbk2AiwInterestItemBase( aInterestId, aServiceHandler )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwInterestItemCall::CPbk2AiwInterestItemCall") );
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::~CPbk2AiwInterestItemCall
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemCall::~CPbk2AiwInterestItemCall()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwInterestItemCall::~CPbk2AiwInterestItemCall") );
    Cancel();
    FeatureManager::UnInitializeLib();
    delete iStoreStateChecker;
    delete iFocusedField;
    delete iContactLink;
    delete iStoreContact;
    delete iSelector;
    delete iRetriever;
    delete ixSPManager;
    delete ixSPContactLinksArray;
    ixSPStoreContactsArray.ResetAndDestroy();
    Release( iAppServices );    
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwInterestItemCall::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    iFieldPropertyArray = &iAppServices->FieldProperties();
    iSelector = CPbk2CallTypeSelector::NewL( iAppServices->ContactManager() );
    iAppUi = Phonebook2::Pbk2AppUi();

    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemCall* CPbk2AiwInterestItemCall::NewL
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2AiwInterestItemCall* self = new ( ELeave ) CPbk2AiwInterestItemCall
        ( aInterestId, aServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemCall::DynInitMenuPaneL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
        const MPbk2ContactUiControl& aControl )
    {
    TBool ret = EFalse;

    // Normal menu
    if ( aResourceId == R_PHONEBOOK2_CALL_MENU || 
        aResourceId == R_PHONEBOOK2_STYLUS_CALL_MENU )
        {
        ret = ETrue;
        FilterCallMenuL( aResourceId, aMenuPane, aControl );
        }

    // Context menu
    else if (aResourceId == R_PHONEBOOK2_CALL_CONTEXT_MENU)
        {
        ret = ETrue;
        FilterContextMenuL( aResourceId, aMenuPane, aControl );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemCall::HandleCommandL
        ( const TInt aMenuCommandId, MPbk2ContactUiControl& aControl,
          TInt aServiceCommandId )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwInterestItemCall::HandleCommandL(0x%x)"), aMenuCommandId );

    // Reset previous request if active
    Cancel();

    TBool ret = EFalse;
    // Only handle this command if service command is call,
    // note that this section is accessed also when the send key
    // is pressed and the initiated command object can then be POC
    // call also
    if ( aServiceCommandId == KAiwCmdCall )
        {
        ret = ETrue;
        iMenuCommandId = aMenuCommandId;
        iControl = &aControl;

        // Retrieve store contact if needed
        MVPbkStoreContact* storeContact = const_cast<MVPbkStoreContact*>
            ( aControl.FocusedStoreContact() );
        if ( !storeContact )
            {
            // No need to check store state, store always available
            // if cmd is invoked from nameslist
            IssueNextAction( ERetrieveStoreContact );
            }
        else
            {
            // Check store availability, callback invokes
            // call if store is available
            CPbk2StoreStateChecker* temp =  CPbk2StoreStateChecker::NewL
                (  storeContact->ParentStore(), *this );
            delete iStoreStateChecker;
            iStoreStateChecker = temp;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
          MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iRetriever )
        {
        if ( iState == ERetrieveStoreContact )
            {
            delete iStoreContact;
            iStoreContact = aContact;
            
            // Start retieving xSP contacts
            TRAPD( err, FetchxSPLinksL() );
            if (err != KErrNone)
                {
                IssueNextAction( EExecuteCall );
                }
            }
        
        // Retrieve one xSP contact at a time
		// No batch implementation for retrieve contacts
        if ( iState == ERetrieveXSPContact )
            {
            // xSP contact retrieval complete. Store it.
            TInt err = ixSPStoreContactsArray.Append( aContact );
            
            // Start retrieving next xSP contact links if any left
            if ( ixSPContactLinksArray->Count() > 0 && err == KErrNone )
                {
                IssueNextAction( ERetrieveXSPContact );
                }
            // If none, or if error execute the call.
            else
                {
                IssueNextAction( EExecuteCall );
                }            
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::VPbkSingleContactOperationFailed
    ( MVPbkContactOperationBase& aOperation, TInt aError )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwInterestItemCall::VPbkSingleContactOperationFailed(%d)"), aError );
    if ( &aOperation == iRetriever )
        {
        HandleError( aError );
        }
    }


// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::StoreState
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::StoreState
        ( MPbk2StoreStateCheckerObserver::TState aState )
    {
    if ( aState == MPbk2StoreStateCheckerObserver::EStoreAvailable )
        {
        IssueNextAction( EExecuteCall );
        }
    else
        {
        IssueNextAction( EShowUnavailableNote );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::DoCancel()
    {
    delete iContactLink;
    iContactLink = NULL;
    delete iRetriever;
    iRetriever = NULL;
	ixSPStoreContactsArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::RunL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::RunL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwInterestItemCall::RunL(%d)"), iState );

    switch ( iState )
        {
        case ERetrieveStoreContact:
            {
            RetrieveContactL();
            break;
            }
        case ERetrieveXSPContact:
            {
            RetrieveContactL();
            break;
            }
        case EExecuteCall:
            {
            MVPbkStoreContact* storeContact = const_cast<MVPbkStoreContact*>
                ( iControl->FocusedStoreContact() );
            if ( !storeContact )
                {
                storeContact = iStoreContact;
                }
            ExecuteCommandL( storeContact );
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
            __ASSERT_DEBUG( EFalse, Panic( EPanicLogic_UnknownState ) );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2AiwInterestItemCall::RunError( TInt aError )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwInterestItemCall::RunError(%d)"), aError );

    HandleError( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::ExecuteCommandL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::ExecuteCommandL
        ( MVPbkStoreContact*& aStoreContact )
    {
    __ASSERT_DEBUG(iControl, Panic( EPanicPreCond_ExecuteCommandL ) );

    MPbk2Command* cmd = NULL;
    TBool launchTelephonyCall = ETrue;

    // Because send key is used also to launch POC calls
    // we have to verify what was the selection and is
    // it ok to launch a telephony call.
    // If command id is EPbk2CmdCall, then the call was launched
    // with the send key.
    if ( iMenuCommandId == EPbk2CmdCall )
        {
        TInt focusedFieldIndex = iControl->FocusedFieldIndex();
        if ( focusedFieldIndex != KErrNotFound )
            {
            MVPbkStoreContactField* focusedField =
                FocusedFieldLC( *aStoreContact, focusedFieldIndex );

            launchTelephonyCall = iSelector->OkToLaunchTelephonyCallL
                ( focusedField );

            CleanupStack::PopAndDestroy(); // focusedField
            }
        }
    else if (iMenuCommandId == EPbk2CmdPrepend)
        {
        cmd = CreatePrependCmdObjectL(
                iMenuCommandId, *iControl, aStoreContact );
        }


    if ( !cmd )
        {
        if ( launchTelephonyCall )
            {
            cmd = CreateCallCmdObjectL
                ( iMenuCommandId, *iControl, aStoreContact );
            }
        else
            {
            cmd = CreatePocCmdObjectL
                ( iMenuCommandId, *iControl, *aStoreContact, NULL );
            }
        }

    if (cmd)
        {
        // Execute the command
        cmd->ExecuteLD();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::CreateCallCmdObjectL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2AiwInterestItemCall::CreateCallCmdObjectL
        ( const TInt aMenuCommandId, MPbk2ContactUiControl& aControl,
        MVPbkStoreContact*& aStoreContact )
    {
    // We need a bunch of pre-initialized variables
    MPbk2Command* cmd = NULL;
    MVPbkStoreContactField* selectedField = NULL;
    TBool okToLaunchTelephonyCall = ETrue;
    TBool selectAccepted = ETrue;
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector action
        ( VPbkFieldTypeSelectorFactory::EEmptySelector );

    AssignFocusedFieldL( aControl, *aStoreContact );

    if ( aMenuCommandId == EPbk2CmdCall )
        {
        // Call launched with send key, an address select needs
        // to be constructed
        TResourceReader resReader;        
        
        TInt resourceId = R_PBK2_SEND_KEY_ADDRESS_SELECT;
        if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        	{
        	resourceId = R_PBK2_SEND_KEY_ADDRESS_SELECT_VOIP;
        	}
        
        CCoeEnv::Static()->CreateResourceReaderLC
            ( resReader, resourceId );

        TPbk2AddressSelectParams params(
            *aStoreContact,
            iAppUi->ApplicationServices().ContactManager(),
            iAppUi->ApplicationServices().NameFormatter(),
            *iFieldPropertyArray,
            resReader,
            R_QTN_PHOB_QTL_CALL_TO_NAME );
        
        RVPbkContactFieldDefaultPriorities defaultPrioritiesArray;
        CVPbkFieldTypeSelector* actionTypeSelector = NULL;
        CSPSettingsVoIPUtils* sPSettings = CSPSettingsVoIPUtils::NewLC();
        
        // IsVoIPSupported() is used for checking whether the platfrom supports VOIP.
        // If the service has been defined, then the specific call should be shown.
        // If the platform supports VOIP and its address has been defined.
        // Then, internet calls and voip calls can be made.
        if ( sPSettings->IsVoIPSupported() && sPSettings->VoIPProfilesExistL() )
            {
            action = VPbkFieldTypeSelectorFactory::EVOIPCallSelector;
            actionTypeSelector = VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    iAppUi->ApplicationServices().ContactManager().FieldTypes());
            
            CleanupStack::PushL( actionTypeSelector );
            iSelector->SetDefaultPrioritiesLC
                        ( defaultPrioritiesArray,
                          *aStoreContact,
                          VPbkFieldTypeSelectorFactory::EVOIPCallSelector );
            }
        // Normal voice calls
		else
            {
            action = VPbkFieldTypeSelectorFactory::EVoiceCallSelector;
                        actionTypeSelector = VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVoiceCallSelector,
                    iAppUi->ApplicationServices().ContactManager().FieldTypes());
            
            CleanupStack::PushL( actionTypeSelector );
            iSelector->SetDefaultPrioritiesLC
                        ( defaultPrioritiesArray,
                          *aStoreContact,
                          VPbkFieldTypeSelectorFactory::EVoiceCallSelector );
            }
        
        params.iCommMethod = action;
        params.SetDefaultPriorities( defaultPrioritiesArray );

        if ( iFocusedField )
            {
            params.SetFocusedField( iFocusedField );
            // If we have field focused we don't launch call directly using
            // default number. The number selection dialog should be shown.
            params.SetUseDefaultDirectly( EFalse );
            }
        else
            {
            // Launch call directly using default values.
            params.SetUseDefaultDirectly( ETrue );
            }
        
        // Run address select
        CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(
                params,
                *actionTypeSelector,
                &ixSPStoreContactsArray.Array(),
                NULL );

        selectedField = addressSelect->ExecuteLD();
        
        CleanupStack::PopAndDestroy( 4 ); // defaultPrioritiesArray, resReader, sPSettings, actionTypeSelector

        // Focused field used already in AddressSelect params and the
        // correct field is in selectedField variable.
        delete iFocusedField;
        iFocusedField = NULL;

        if( selectedField )
            {
            CleanupDeletePushL( selectedField );
            // Because send key is used also to launch POC calls
            // we have to verify what was the selection and is
            // it ok to launch a telephony call
            okToLaunchTelephonyCall = iSelector->OkToLaunchTelephonyCallL
                ( selectedField );
            CleanupStack::Pop( selectedField );
            }
        else
            {
            selectAccepted = EFalse;
            }
        }

    if( selectAccepted )
        {
        if( okToLaunchTelephonyCall )
            {
            if ( iFocusedField && !selectedField )
                {
                // If field is focused it should be delivered to call command.
                // The command object takes ownership of selectedField.
                selectedField = iFocusedField;
                iFocusedField = NULL;
                }

            CleanupDeletePushL( selectedField );
            // Select was either accepted, or we are calling from
            // CallUI menus. In the latter case, the address select
            // will be shown later. Next, create the call command object.
            // Takes ownership of selectedField.
            cmd = CPbk2CallCmd::NewL( aStoreContact, selectedField,
                aControl, aMenuCommandId, iServiceHandler, 
                *iSelector, action );
            CleanupStack::Pop( selectedField );
            }
        else
            {
            // Select was accepted, but the selection indicates
            // we have to create a POC call
            cmd = CreatePocCmdObjectL( aMenuCommandId, aControl,
                *aStoreContact, selectedField );
            }
        }

    return cmd;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::CreatePocCmdObjectL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2AiwInterestItemCall::CreatePocCmdObjectL
        (  const TInt aMenuCommandId,
           MPbk2ContactUiControl& aControl,
           MVPbkStoreContact& aStoreContact,
           MVPbkStoreContactField* aStoreContactField  )
    {
    MPbk2Command* cmd = NULL;
    MVPbkStoreContactField* field = aStoreContactField;

    if ( !field )
        {
        AssignFocusedFieldL( aControl, aStoreContact );
        field = iFocusedField;
        iFocusedField = NULL; // ownership was taken
        }

    CleanupDeletePushL( field );
    cmd = CPbk2PocCmd::NewL( aMenuCommandId, iServiceHandler,
        *iSelector, aControl, aStoreContact, field );
    CleanupStack::Pop( field );

    return cmd;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::CreatePrependCmdObjectL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2AiwInterestItemCall::CreatePrependCmdObjectL
        ( const TInt aMenuCommandId,
          MPbk2ContactUiControl& aControl, MVPbkStoreContact*& aStoreContact )
    {
    return CPbk2PrependCmd::NewL(
        aMenuCommandId,
        aControl,
        iServiceHandler,
        *iSelector,
        aStoreContact,
        *iFieldPropertyArray);
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::RetrieveContactL()
    {
    if ( iState == ERetrieveStoreContact )
        {
        delete iContactLink;
        iContactLink = NULL;
        
        if( !iControl )
            {
            return;
            }
        
        const MVPbkBaseContact* focusedContact = iControl->FocusedContactL();
        
        if( !focusedContact )
            {
            User::Leave( KErrNotFound );
            }
        
        iContactLink = focusedContact->CreateLinkLC();
        CleanupStack::Pop(); // iContactLink
        
        delete iRetriever;
        iRetriever = NULL;
        iRetriever = iAppUi->ApplicationServices().
            ContactManager().RetrieveContactL( *iContactLink, *this );
        }
    
    if ( iState == ERetrieveXSPContact )
        {
        delete iRetriever;
        iRetriever = NULL;
        iRetriever = iAppUi->ApplicationServices().ContactManager().
            RetrieveContactL( ixSPContactLinksArray->At( 0 ), *this );
        ixSPContactLinksArray->Delete( 0 );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::IssueNextAction
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::IssueNextAction( TNextAction aState )
    {
    if ( !IsActive() )
        {
        iState = aState;
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::FocusedFieldLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2AiwInterestItemCall::FocusedFieldLC
        ( MVPbkStoreContact& aStoreContact, TInt aFocusedFieldIndex )
    {
    TInt index( KErrNotFound );
    MVPbkStoreContactField* ret = NULL;

    CPbk2PresentationContact* presentationContact =
        CPbk2PresentationContact::NewL
            ( aStoreContact, *iFieldPropertyArray );
    CleanupStack::PushL( presentationContact );

    index = presentationContact->PresentationFields().StoreIndexOfField
        ( aFocusedFieldIndex );

    CleanupStack::PopAndDestroy( presentationContact );

    if ( index != KErrNotFound )
        {
        // Use FieldAtLC to avoid the unvalidity of the field
        // after new FieldAt call
        ret = aStoreContact.Fields().FieldAtLC( index );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::AssignFocusedFieldL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwInterestItemCall::AssignFocusedFieldL
        ( MPbk2ContactUiControl& aControl,
          MVPbkStoreContact& aStoreContact )
    {
    TInt focusedFieldIndex = aControl.FocusedFieldIndex();

    delete iFocusedField;
    iFocusedField = NULL;
    if ( focusedFieldIndex != KErrNotFound )
        {
        iFocusedField =
            FocusedFieldLC( aStoreContact, focusedFieldIndex );
        CleanupStack::Pop(); // iFocusedField
        }
    }


// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::VideoCallMenuVisibilityL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::VideoCallMenuVisibilityL
        ( CEikMenuPane& aMenuPane ) const
    {
    if ( FeatureManager::FeatureSupported
            ( KFeatureIdVideocallMenuVisibility ) )
        {
        TInt index = KErrNotFound;
        if ( aMenuPane.MenuItemExists( KAiwCmdCall, index ) &&
             index != KErrNotFound )
            {
            CEikMenuPaneItem::SData& itemData =
                aMenuPane.ItemDataByIndexL( index );
            const TInt KDisableCascade = 0;
            itemData.iCascadeId = KDisableCascade;  // disable cascade menu
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::FilterCallMenuL
// Call menu filtering is done in the following situations:
// 1. Contact info view: based on the currently selected (focused) field
// 2. Names list: the list is empty
// 3. Names list: contact(s) selected (marked)
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::FilterCallMenuL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
        const MPbk2ContactUiControl& aControl )
    {
    // Get empty parameter list
    CAiwGenericParamList& paramList =
        iServiceHandler.InParamListL();
    const MVPbkStoreContact* storeContact =
        aControl.FocusedStoreContact();

    // Store contact is not available if we are not in contact info view
    if ( storeContact )
        {
        TPbk2StoreContactAnalyzer analyzer
          ( iAppUi->ApplicationServices().
                ContactManager(), storeContact );

        // Hide voice call options if an SIP field is focused
        const MVPbkBaseContactField* field = aControl.FocusedField();
        if( field && analyzer.IsFieldTypeIncludedL
             ( *field, R_PHONEBOOK2_SIP_SELECTOR ) &&
                 FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            TAiwVariant variant;
            TAiwGenericParam param
                ( EGenericParamSIPAddress, variant );
            paramList.AppendL( param );
            }
        }

    // Check that the list is not empty and does not have marked items
    if ( aControl.NumberOfContacts() > 0 && !aControl.ContactsMarked() )
        {
        // Check item placement
        VideoCallMenuVisibilityL( aMenuPane );

        // Let provider add its menu items to the menu
        iServiceHandler.InitializeMenuPaneL(
            aMenuPane,
            aResourceId,
            EPbk2CmdLast,
            paramList);
        }
    // The list is empty or it has marked items -> hide call option
    else
        {
        aMenuPane.SetItemDimmed( KAiwCmdCall, ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::FilterContextMenuL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::FilterContextMenuL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
        const MPbk2ContactUiControl& aControl )
    {
    // Hide the item by default
    aMenuPane.SetItemDimmed( KAiwCmdCall, ETrue );

    // Get empty parameter list
    CAiwGenericParamList& paramList =
        iServiceHandler.InParamListL();

    if ( aControl.NumberOfContactFields() > 0 )
        {
        // Get the focused field
        const MVPbkBaseContactField* field =
            aControl.FocusedField();
        if ( field )
            {
            TPbk2StoreContactAnalyzer analyzer
                ( iAppUi->ApplicationServices().
                  ContactManager(), NULL );

            if ( analyzer.IsFieldTypeIncludedL
                    ( *field, R_PHONEBOOK2_PHONENUMBER_SELECTOR ) )
                {
                aMenuPane.SetItemDimmed( KAiwCmdCall, EFalse );
                VideoCallMenuVisibilityL( aMenuPane );
                }

            // Next further investigate the field, is it a VoIP field
            if( analyzer.IsFieldTypeIncludedL
                ( *field, R_PHONEBOOK2_SIP_SELECTOR ) &&
                 FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
                {
                aMenuPane.SetItemDimmed( KAiwCmdCall, EFalse );
                VideoCallMenuVisibilityL( aMenuPane );

                // When we are on a Internet telephony field,
                // the Call UI must show only the VoIP option
                TAiwVariant variant;
                TAiwGenericParam param
                    ( EGenericParamSIPAddress, variant );
                paramList.AppendL( param );
                }

            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL
                ( aMenuPane, aResourceId, EPbk2CmdLast, paramList );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::FetchxSPLinksL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::FetchxSPLinksL()
    {
    ixSPManager = CVPbkxSPContacts::NewL( iAppUi->ApplicationServices().ContactManager() );                                                              
    delete iRetriever;
    iRetriever = NULL;
    iRetriever = ixSPManager->GetxSPContactLinksL( *iStoreContact, *this, *this);
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::VPbkOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::VPbkOperationFailed(
                MVPbkContactOperationBase* /*aOperation*/,
                TInt aError )
    {
    // Ignore, xSP info can be omitted. Just run address select dialog
    HandleError( aError );
    IssueNextAction( EExecuteCall );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemCall::VPbkOperationResultCompleted
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemCall::VPbkOperationResultCompleted(
                MVPbkContactOperationBase* /*aOperation*/,
                MVPbkContactLinkArray* aArray )
    {
    // Count is the number of contact links fetched
    TInt count = aArray != NULL ? aArray->Count() : 0;
    if ( count > 0 )
        {
        TRAPD( err,
            // Take an own copy of supplied contact links
            ixSPContactLinksArray = CVPbkContactLinkArray::NewL();
            CopyContactLinksL( *aArray, *ixSPContactLinksArray );
            );
        // If there was an, launch the call
        if ( err != KErrNone )
            {
            HandleError( err );
            IssueNextAction( EExecuteCall );
            }
        else
            {
            // Start fetching xSP contacts
            IssueNextAction( ERetrieveXSPContact );
            }
        }
    // If there are no contact links, launch call
    else
        {
        IssueNextAction( EExecuteCall );
        }
    }

// End of File
