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
* Description:  Phonebook 2 Group UI Extension send message command.
*
*/


// INCLUDE FILES
#include "CPguSendMessageGroupCmd.h"
#include "CPguUIExtensionPlugin.h"

// Phonebook 2
#include "MPbk2ProcessDecorator.h"
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <pbk2uicontrols.rsg>
#include <TPbk2AddressSelectParams.h>
#include <CPbk2AddressSelect.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <VPbkFieldType.hrh>
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <RVPbkContactFieldDefaultPriorities.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactLinkArray.h>

// System includes
#include <featmgr.h>
#include <coemain.h>
#include <barsread.h>
#include <sendui.h>
#include <SenduiMtmUids.h>
#include <CMessageData.h>
#include <eikenv.h>
#include <akntitle.h>
#include <eikspane.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <layoutmetadata.cdl.h>
// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KDefaultTitleFormat = MPbk2ContactNameFormatter::EUseSeparator;
const TInt KGranularity( 4 );

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicUnknownMedia = 1,
    ERunL_InvalidState,
    EPanicLogic
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPguSendMessageGroupCmd");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

}  /// namespace

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::CPguSendMessageGroupCmd
// --------------------------------------------------------------------------
//
CPguSendMessageGroupCmd::CPguSendMessageGroupCmd(
        MPbk2ContactUiControl& aUiControl, 
        CPguUIExtensionPlugin* aPguUIExtensionPlugin ) :
    CActive( EPriorityIdle ),
    iUiControl( &aUiControl ),
    iPguUIExtensionPlugin( aPguUIExtensionPlugin )   
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::~CPguSendMessageGroupCmd
// --------------------------------------------------------------------------
//
CPguSendMessageGroupCmd::~CPguSendMessageGroupCmd()
    {
    Cancel();
    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
		
    delete iMessageData;  
    delete iSelectedContacts;
    delete iMtmFilter;
    delete iRetrieveOperation;
    delete iStoreContact;
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::NewL
// --------------------------------------------------------------------------
//
CPguSendMessageGroupCmd* CPguSendMessageGroupCmd::NewL
        ( MPbk2ContactUiControl& aUiControl,
		      CPguUIExtensionPlugin*  aPguUIExtensionPlugin ) 
    {
    CPguSendMessageGroupCmd* self =
        new ( ELeave ) CPguSendMessageGroupCmd( aUiControl,
            aPguUIExtensionPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::ConstructL()
    {
    
    iUiControl->RegisterCommand( this );
    
    iFieldPropertyArray = &Phonebook2::Pbk2AppUi()->
        ApplicationServices().FieldProperties();

    iMtmFilter = new(ELeave) CArrayFixFlat<TUid>( KGranularity );
    iMtmFilter->AppendL(KSenduiMtmPostcardUid);  // hide postcard

    iSendingCapabilities.iFlags = TSendingCapabilities::ESupportsEditor;
    iSelectedContacts = iUiControl->SelectedContactsOrFocusedContactL();
    iMessageData = CMessageData::NewL();
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::ExecuteLD()
    {
    iCurrentContactLinkIndex = 0;
    iState = EShowWriteQuery;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::DoCancel()
    {
    //Do nothing
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::RunL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::RunL()
    {
    switch ( iState )
        {
        case ERetrieving:
            {
            if ( iSelectedContacts )
                {
                if ( iSelectedContacts->Count() == iCurrentContactLinkIndex )
                    {
                    iState = EStarting;
                    IssueRequest();
                    }
                    else
                    {
                    RetrieveContactL( iSelectedContacts->At(
                                            iCurrentContactLinkIndex ) );
                    iCurrentContactLinkIndex++;
                    }
                }
            else
                {
                iState = EStopping;
                IssueRequest();
                }
            break;
            }
        case EShowWriteQuery:
            {
            iMtmUid = ShowWriteQueryL();
            if ( iMtmUid == KNullUid )
                {
                iState = EStopping;
                // Disconnect the relationship between control and command
                iUiControl->RegisterCommand( NULL );
                iUiControl = NULL;
                }
            else
                {
                iState = ERetrieving;
                }
            IssueRequest();
            break;
            }
        case EResolveGroup:
            {
            ResolveGroupL();
            break;
            }
        case EResolveURL:
            {
            ResolveUrlL();
            break;
            }
        case EStarting:
            {
            DoSendMessageL();
            break;
            }
        case EStopping:
            {
            iSendMessage = EFalse;
            ProcessDismissed( KErrNone );
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(ERunL_InvalidState));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPguSendMessageGroupCmd::RunError( TInt aError )
    {
    return FilterErrors( aError );
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We now have a store contact
        delete iStoreContact;
        iStoreContact = aContact;

        if ( iStoreContact->Group() )
            {
            iState = EResolveGroup;
            IssueRequest();
            }
        else
            {
            iState = EResolveURL;
            IssueRequest();
            }
        }
    else
        {
        IssueStopRequest();
        }
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We cannot get the contact, so we have to
        // fail. We cannot continue, since this operation
        // was executed only in case of one contact.
        ProcessDismissed( aError );
        }
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ShowWriteQueryL
// --------------------------------------------------------------------------
//
TUid CPguSendMessageGroupCmd::ShowWriteQueryL()
    {
    TUid uid = Phonebook2::Pbk2AppUi()->ApplicationServices().SendUiL()->
        ShowTypedQueryL( CSendUi::EWriteMenu, NULL, iSendingCapabilities,
            iMtmFilter );
    return uid;
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::RetrieveContactL(
        const MVPbkContactLink& aContactLink )
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::DoSendMessageL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::DoSendMessageL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguSendMessageGroupCmd::DoSendMessageL() start") );

    if ( iSendMessage )
        {
        // Gets current tile pane and saves it to title
        HBufC* title = NULL;
        CAknTitlePane* titlePane = NULL;
        CEikStatusPane *sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
        titlePane = STATIC_CAST( CAknTitlePane*, 
            sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        title = titlePane->Text()->AllocLC();

        TRAPD( error, Phonebook2::Pbk2AppUi()->ApplicationServices().SendUiL()->
            CreateAndSendMessageL( iMtmUid, iMessageData ) );

        if ( error )
            {
            //Show Appropriate Error Note
        
            }
        if( !Layout_Meta_Data::IsLandscapeOrientation() )
            {
            sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
            sp->MakeVisible( ETrue );
            
            // Sets title pane for tile which was save
            titlePane->SetText( title );
            sp->DrawNow();
            }
        else
            {
            // Sets title pane for tile which was save
            titlePane->SetText( title );
            }
        CleanupStack::Pop();
        }

    iState = EStopping;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ResolveGroupL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::ResolveGroupL()
    {
    MVPbkContactLinkArray* linkArray =
        iStoreContact->Group()->ItemsContainedLC();
    CleanupStack::Pop(); //linkArray

    delete iSelectedContacts;
    iSelectedContacts = linkArray;
    iCurrentContactLinkIndex = 0;

    iState = ERetrieving;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ResolveUrlL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::ResolveUrlL()
    {
    TResourceReader resReader;
    TUid techTypeUid = Phonebook2::Pbk2AppUi()->ApplicationServices().
        SendUiL()->TechnologyType( iMtmUid );

    RVPbkContactFieldDefaultPriorities defaults;
    CleanupClosePushL( defaults );

    TInt resourceId = PrepareAddressSelectL( defaults, techTypeUid );
    CCoeEnv::Static()->CreateResourceReaderLC( resReader, resourceId );

    TPbk2AddressSelectParams params(
        *iStoreContact,
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
        Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter(),
        *iFieldPropertyArray,
        resReader );
    params.SetFocusedField( iFocusedField );
    params.SetUseDefaultDirectly( ETrue );
    params.SetDefaultPriorities( defaults );
    params.SetSuppressWarnings( ETrue );

    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL( params );
    MVPbkStoreContactField* selectedField = addressSelect->ExecuteLD();

    CleanupStack::PopAndDestroy( 2 ); // defaults, resReader

    if ( selectedField )
        {
        CleanupDeletePushL( selectedField );
        SetAddressFromFieldL( *selectedField );
        CleanupStack::PopAndDestroy(); // selectedField
        iState = ERetrieving;
        iSendMessage = ETrue;
        }
    else 
        {
        if ( iPguUIExtensionPlugin->IsEndCallKeyPressed() )
    	       {
    	       iPguUIExtensionPlugin->ClearEndCallKeyPressedFlag();
    	       iState = EStopping;
             }
        else
            {
	          iState = ERetrieving;
            } 
        }          
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::PrepareAddressSelectL
// --------------------------------------------------------------------------
//
inline TInt CPguSendMessageGroupCmd::PrepareAddressSelectL
        ( RVPbkContactFieldDefaultPriorities& aPriorities,
          TUid aTechType )
    {
    // Precondition
    __ASSERT_DEBUG( iMtmUid != KNullUid, Panic( EPanicUnknownMedia) );

    TInt resourceId = KErrNotFound;

    // SMS
    if ( iMtmUid == KSenduiMtmSmsUid ||
         aTechType == KSenduiTechnologySmsUid )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdEmailOverSms ) )
            {
            resourceId = R_PBK2_EMAIL_OVER_SMS_ADDRESS_SELECT;
            User::LeaveIfError( aPriorities.Append
                ( EVPbkDefaultTypeEmailOverSms ) );
            }
        else
            {
            resourceId = R_PBK2_PHONE_NUMBER_SELECT;
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeSms ) );
            }
        }
    // MMS
    else if ( iMtmUid == KSenduiMtmMmsUid ||
              aTechType == KSenduiTechnologyMmsUid )
        {
        resourceId = R_PBK2_MMS_ADDRESS_SELECT;
        User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeMms ) );
        }
    // E-mail
    else if ( iMtmUid ==  KSenduiMtmSmtpUid ||
              aTechType == KSenduiTechnologyMailUid )
        {
        resourceId = R_PBK2_EMAIL_ADDRESS_SELECT;
        User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeEmail ) );
        }
    // Unified message
    else if ( iMtmUid == KSenduiMtmUniMessageUid )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdMMS ) )
            {
            if (Phonebook2::Pbk2AppUi()->ApplicationServices().
                LocallyVariatedFeatureEnabled( EVPbkLVShowEmailInSendMsg ))
            {
                resourceId = R_PBK2_UNIFIED_EDITOR_MMS_ADDRESS_SELECT;
            }
            else
            {
                resourceId = R_PBK2_UNIFIED_EDITOR_PHONE_NUMBER_SELECT;
            }
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeMms ) );
            }

        if ( FeatureManager::FeatureSupported
                ( KFeatureIdEmailOverSms ) )
            {
            if ( resourceId == KErrNotFound )
                {
                resourceId =
                    R_PBK2_UNIFIED_EDITOR_EMAIL_OVER_SMS_ADDRESS_SELECT;
                }
            // Include SMS default always in unified message,
            // in addition it has MMS default included with higher priority
            User::LeaveIfError( aPriorities.Append(
                EVPbkDefaultTypeEmailOverSms ) );
            }
        else
            {
            if ( resourceId == KErrNotFound )
                {
                resourceId = R_PBK2_UNIFIED_EDITOR_PHONE_NUMBER_SELECT;
                }
            // Include SMS default always in unified message,
            // in addition it has MMS default included with higher priority
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeSms ) );
            }
        }

    // Postcondition
    __ASSERT_DEBUG( resourceId != KErrNotFound, Panic( EPanicLogic) );

    return resourceId;
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::SetAddressFromFieldL
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::SetAddressFromFieldL(
        MVPbkStoreContactField& aField )
    {
    HBufC* address = MVPbkContactFieldTextData::Cast( aField.FieldData() ).
        Text().AllocL();
    CleanupStack::PushL( address );

    if ( address && address->Des().Length() > 0 )
        {
        HBufC* nameBuffer =
            Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter().
                GetContactTitleOrNullL(
                    static_cast<const MVPbkBaseContactFieldCollection&>(
                        iStoreContact->Fields() ),
                        KDefaultTitleFormat );
        if ( nameBuffer )
            {
            CleanupStack::PushL( nameBuffer );
            iMessageData->AppendToAddressL( *address, *nameBuffer );
            CleanupStack::PopAndDestroy( nameBuffer );
            }
        else
            {
            iMessageData->AppendToAddressL( *address );
            }
        }
    CleanupStack::PopAndDestroy( address );
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    if (iUiControl)
        {
        iUiControl->UpdateAfterCommandExecution();
        }

    // Notify command owner that the command has finished
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::FilterErrors
// --------------------------------------------------------------------------
//
TInt CPguSendMessageGroupCmd::FilterErrors( TInt aErrorCode )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguSendMessageGroupCmd::FilterErrors(%d) start"), aErrorCode);

    TInt result = aErrorCode;
    switch ( aErrorCode )
        {
        case KErrNotFound:  // FALLTHROUGH
        case KErrInUse:
            {
            // Ignore these errors
            // KErrNotFound means that somebody got the contact first
            // KErrInUse means that the contact is open
            result = KErrNone;
            break;
            }
        default:  // Something more serious happened -> give up
            {
            Cancel();
            ProcessDismissed( aErrorCode );
            break;
            }
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPguSendMessageGroupCmd::FilterErrors(%d) end"), result );

    return result;
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPguSendMessageGroupCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPguSendMessageGroupCmd::IssueStopRequest()
    {
    iState = EStopping;
    if ( !IsActive() )
        {
        IssueRequest();
        }
    }

//  End of File
