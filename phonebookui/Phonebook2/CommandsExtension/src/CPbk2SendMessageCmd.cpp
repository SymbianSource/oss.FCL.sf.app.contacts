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
* Description:  Phonebook 2 send message command.
*
*/


// INCLUDE FILES
#include "CPbk2SendMessageCmd.h"

// Phonebook 2
#include "MPbk2ProcessDecorator.h"
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2AppUiBase.h>
#include <pbk2uicontrols.rsg>
#include <TPbk2AddressSelectParams.h>
#include <CPbk2AddressSelect.h>
#include <pbk2presentation.rsg>
#include <Pbk2DataCaging.hrh>
#include <RPbk2LocalizedResourceFile.h>
#include <CPbk2AttachmentFile.h>
#include <pbk2commands.rsg>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2ApplicationServices.h>
#include <pbk2commonui.rsg>

// Virtual Phonebook
#include <VPbkFieldType.hrh>
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <RVPbkContactFieldDefaultPriorities.h>
#include <MVPbkStreamable.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <StringLoader.h>
#include <sendui.h>
#include <SenduiMtmUids.h>
#include <CMessageData.h>
#include <featmgr.h>
#include <coemain.h>
#include <barsread.h>
#include <akntitle.h>

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
    EPanicLogic,
    EPanicNullPointer
    };

void Panic( TPanicCode aReason )
    {
    _LIT( KPanicText, "CPbk2SendMessageCmd" );
    User::Panic( KPanicText, aReason );
    }
#endif //_DEBUG

}  /// namespace


// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::CPbk2SendMessageCmd
// --------------------------------------------------------------------------
//
CPbk2SendMessageCmd::CPbk2SendMessageCmd(
        MPbk2ContactUiControl& aUiControl ) :
    CActive( EPriorityIdle ),
    iUiControl( &aUiControl ),
    iRecipientCount( 0 ),
    iFocusedFieldIndex( aUiControl.FocusedFieldIndex() )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::~CPbk2SendMessageCmd
// --------------------------------------------------------------------------
//
CPbk2SendMessageCmd::~CPbk2SendMessageCmd()
    {
    Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    delete iAttachmentFile;
    delete iMessageData;
    delete iSelectedContacts;

    delete iMtmFilter;
    delete iRetrieveOperation;
    delete iStoreContact;
    delete iSendUi;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2SendMessageCmd* CPbk2SendMessageCmd::NewL(
        MPbk2ContactUiControl& aUiControl )
    {
    CPbk2SendMessageCmd* self =
        new ( ELeave ) CPbk2SendMessageCmd( aUiControl );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); // self
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::ConstructL()
    {
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );

    iSendUi = CSendUi::NewL();
    iAppServices = CPbk2ApplicationServices::InstanceL();

    iFieldPropertyArray = &(iAppServices->FieldProperties());

    iMtmFilter = new ( ELeave ) CArrayFixFlat<TUid>( KGranularity );
    iSendingCapabilities.iFlags = TSendingCapabilities::ESupportsEditor;
    iSelectedContacts = iUiControl->SelectedContactsOrFocusedContactL();
    __ASSERT_DEBUG(iSelectedContacts, Panic(EPanicNullPointer));
    iMessageData = CMessageData::NewL();
    iUiControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::ExecuteLD()
    {
    iCurrentContactLinkIndex = 0;
    iState = EShowWriteQuery;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::AddObserver( MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::SetMtmUid
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::SetMtmUid( TUid aUid )
    {
    iMtmUid = aUid;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::RunL()
    {
    switch (iState)
        {
        case ERetrieving:
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
            break;
            }

        case EShowWriteQuery:
            {
            // No need to query message type if it's already known
            if( iMtmUid == KNullUid )
                {
                iMtmUid = ShowWriteQueryL();
                }

            if ( iMtmUid == KNullUid )
                {
                IssueStopRequest( KErrCancel );
                }
            else
                {
                iState = ERetrieving;
                IssueRequest();
                }
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
            ProcessDismissed( KErrNone );
            break;
            }

        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( ERunL_InvalidState ) );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SendMessageCmd::RunError( TInt aError )
    {
    return FilterErrors( aError );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    // We now have a store contact
    delete iStoreContact;
    iStoreContact = aContact;

    iState = EResolveURL;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    // We cannot get the contact, so we have to
    // fail. We cannot continue, since this operation
    // was executed only in case of one contact.
    ProcessDismissed( aError );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ShowWriteQueryL
// --------------------------------------------------------------------------
//
TUid CPbk2SendMessageCmd::ShowWriteQueryL()
    {
    if ( iSelectedContacts->Count() > 1 )
        {
        iMtmFilter->AppendL(KSenduiMtmPostcardUid);  // hide postcard
        }
    TUid uid = iSendUi->ShowTypedQueryL( CSendUi::EWriteMenu, NULL,
        iSendingCapabilities, iMtmFilter );
    return uid;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::RetrieveContactL
        ( const MVPbkContactLink& aContactLink )
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = iAppServices->
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::DoSendMessageL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::DoSendMessageL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2SendMessageCmd::DoSendMessageL() start") );

    iSendUi->CreateAndSendMessageL( iMtmUid, iMessageData );

    iState = EStopping;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ResolveUrlL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::ResolveUrlL()
    {
    if ( iMtmUid == KSenduiMtmPostcardUid )
        {
        // Send post card
        SendPostCardL();
        iState = EStarting;
        IssueRequest();
        }
    else
        {
        TUid techTypeUid = iSendUi->TechnologyType( iMtmUid );

        RVPbkContactFieldDefaultPriorities defaults;
        CleanupClosePushL( defaults );

        // Prepare address select
        TInt resourceId = PrepareAddressSelectL( defaults, techTypeUid );

        if ( resourceId > KErrNotFound )
            {
            MVPbkStoreContactField* selectedField =
                SelectFieldL( resourceId, defaults );

            if ( !selectedField )
                {
                CPbk2AppUiBase<CAknViewAppUi>& appUi = static_cast<CPbk2AppUiBase<CAknViewAppUi>&>
                    ( *CEikonEnv::Static()->EikAppUi() );

                if ( appUi.IsForeground() )
                    {
                    if ( iSelectedContacts->Count() == iCurrentContactLinkIndex
                             && iRecipientCount == 0 )
                        {
                        // This is the last contact and there are
                        // no recipients, stop
                        IssueStopRequest( KErrNone );
                        }
                    else
                        {
                        // If this is not the last contact or if there are
                        // recipients, go on
                        iState = ERetrieving;
                        IssueRequest();
                        }
                    }
                else
                    {
                    // If appUi is not in the foreground, 
                    // It means Phonebook2 has hidden itself in background. 
                    // All active commands should be stopped so that names list view is to be activated 
                    // and no other views will be shown when re-entering Phonebook2
                    IssueStopRequest( KErrNone );
                    }               	
                }
            else
                {
                CleanupDeletePushL( selectedField );
                SetAddressFromFieldL( *selectedField );
                CleanupStack::PopAndDestroy(); // selectedField

                iState = ERetrieving;
                IssueRequest();
                }
            }

        CleanupStack::PopAndDestroy(); // defaults
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::PrepareAddressSelectL
// --------------------------------------------------------------------------
//
inline TInt CPbk2SendMessageCmd::PrepareAddressSelectL
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
    // MMS & Audio message
    // Audio Messaging uses same address select as MMS for now
    else if ( iMtmUid == KSenduiMtmMmsUid ||
              iMtmUid == KSenduiMtmAudioMessageUid ||
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
            if (iAppServices->LocallyVariatedFeatureEnabled(
                EVPbkLVShowEmailInSendMsg ))
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
            // in addition unified message has MMS default included
            // with higher priority
            User::LeaveIfError( aPriorities.Append
                ( EVPbkDefaultTypeEmailOverSms ) );
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
// CPbk2SendMessageCmd::SetAddressFromFieldL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::SetAddressFromFieldL
        ( MVPbkStoreContactField& aField )
    {
    HBufC* address = MVPbkContactFieldTextData::Cast( aField.FieldData()).
        Text().AllocL();
    CleanupStack::PushL( address );

    if ( address && address->Des().Length() > 0 )
        {
        HBufC* nameBuffer = iAppServices->NameFormatter().
            GetContactTitleL(
                static_cast<const MVPbkBaseContactFieldCollection&>(
                    iStoreContact->Fields() ),
                KDefaultTitleFormat);
        CleanupStack::PushL( nameBuffer );
        const TDesC& unnamed = iAppServices->NameFormatter().UnnamedText();
        if ( unnamed == ( *nameBuffer ) )
            {
            iMessageData->AppendToAddressL( *address );
            }
        else
            {
            iMessageData->AppendToAddressL( *address, *nameBuffer );
            }
        ++iRecipientCount;
        CleanupStack::PopAndDestroy(); // nameBuffer
        }
    CleanupStack::PopAndDestroy( address );
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::ProcessDismissed( TInt aCancelCode )
    {
    if ( iAttachmentFile )
        {
        // No error checking, exiting Phonebook would
        // be too much for this error
        CCoeEnv::Static()->FsSession().Delete( iAttachmentFile->FileName() );
        }

    if ( aCancelCode != KErrCancel  && iUiControl)
        {
        TRAPD( err,
            iUiControl->ResetFindL();
            // Keep focus in contact
            if ( iStoreContact )
                {
                if ( iFocusedFieldIndex != KErrNotFound )
                    {
                    iUiControl->SetFocusedFieldIndex( iFocusedFieldIndex );
                    }
                iUiControl->SetFocusedContactL( *iStoreContact );
                }
                TUid titlePaneUid;
                titlePaneUid.iUid = EEikStatusPaneUidTitle;
                CEikStatusPane *statusPane = ((CAknAppUi*)CEikonEnv::Static()->EikAppUi())->StatusPane();
                CEikStatusPaneBase::TPaneCapabilities subPane =
                            statusPane->PaneCapabilities(titlePaneUid);

                if (subPane.IsPresent()&&subPane.IsAppOwned())
                    {
                    CAknTitlePane* titlePane =
                                (CAknTitlePane *)statusPane->ControlL(titlePaneUid);
                    HBufC* titlePaneText = StringLoader::LoadLC(R_QTN_PHOB_TITLE);
                    titlePane->SetTextL(*titlePaneText);

                    CleanupStack::PopAndDestroy(titlePaneText);
                    }
            ); // TRAPD
        iUiControl->UpdateAfterCommandExecution();
        if (err != KErrNone)
            {
            CCoeEnv::Static()->HandleError(err);
            }
        }

    // Notify command owner that the command has finished
    if (iCommandObserver)
        {
        iCommandObserver->CommandFinished( *this );
        }

    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::FilterErrors
// --------------------------------------------------------------------------
//
TInt CPbk2SendMessageCmd::FilterErrors( TInt aErrorCode )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2SendMessageCmd::FilterErrors(%d) start" ), aErrorCode );

    TInt result = aErrorCode;
    switch (aErrorCode)
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

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2SendMessageCmd::FilterErrors(%d) end" ), result );

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::IssueStopRequest( TInt aErrorCode )
    {
    iState = EStopping;

    if ( aErrorCode != KErrNone )
        {
        ProcessDismissed( aErrorCode );
        }

    if (!IsActive())
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::ToStoreFieldIndexL
// --------------------------------------------------------------------------
//
TInt CPbk2SendMessageCmd::ToStoreFieldIndexL( TInt aPresIndex )
    {
    TInt index( KErrNotFound );
    CPbk2PresentationContact* presentationContact =
        CPbk2PresentationContact::NewL(
            *iStoreContact, iAppServices->FieldProperties() );
    CleanupStack::PushL( presentationContact );

    index = presentationContact->PresentationFields().StoreIndexOfField
        ( aPresIndex );

    CleanupStack::PopAndDestroy( presentationContact );

    return index;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::SelectFieldL
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2SendMessageCmd::SelectFieldL(
        TUint aResourceId,
        RVPbkContactFieldDefaultPriorities& aPriorities )
    {
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC( resReader, aResourceId );

    TPbk2AddressSelectParams params
        ( *iStoreContact,
            iAppServices->ContactManager(),
            iAppServices->NameFormatter(),
            *iFieldPropertyArray, resReader );

    // Set focused field if we are in contact info
    TInt fieldIndex = KErrNotFound;
    if (iUiControl)
        {
        fieldIndex = iUiControl->FocusedFieldIndex();
        }
    MVPbkStoreContactField* focusedField = NULL;
    if ( fieldIndex != KErrNotFound )
        {
        focusedField = iStoreContact->Fields().FieldAtLC
                ( ToStoreFieldIndexL( fieldIndex ) );
        }
    else
        {
        // We are in names list and we should use the default value
        // directly if available
        params.SetUseDefaultDirectly( ETrue );
        }

    params.SetFocusedField( focusedField );
    params.SetDefaultPriorities( aPriorities );

    params.iCommMethod = VPbkFieldTypeSelectorFactory::EUniEditorSelector;
	CVPbkFieldTypeSelector* actionTypeSelector = 
        VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                VPbkFieldTypeSelectorFactory::EUniEditorSelector,
                Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().FieldTypes());
        
    CleanupStack::PushL( actionTypeSelector );
    
    // Run address select
    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(
            params,
            *actionTypeSelector,
            NULL,
            NULL );
    
    MVPbkStoreContactField* selectedField = addressSelect->ExecuteLD();

    if ( focusedField )
        {
        CleanupStack::PopAndDestroy(); // focusedField
        }
    CleanupStack::PopAndDestroy( 2 ); // actionTypeSelector, resReader 

    return selectedField;
    }

// --------------------------------------------------------------------------
// CPbk2SendMessageCmd::SendPostCardL
// --------------------------------------------------------------------------
//
void CPbk2SendMessageCmd::SendPostCardL()
    {
    // Create attachment file object
    delete iAttachmentFile;
    iAttachmentFile = NULL;
    HBufC* attFileName = StringLoader::LoadLC
        ( R_PBK2_POSTCARD_WRITE_ATTACHMENT_TAG );
    iAttachmentFile = CPbk2AttachmentFile::NewL
        ( *attFileName, CCoeEnv::Static()->FsSession(),
          EFileWrite|EFileShareAny );
    CleanupStack::PopAndDestroy( attFileName );

    HBufC8* buf = iSelectedContacts->PackLC();
    iAttachmentFile->File().Write( *buf );
    CleanupStack::PopAndDestroy( buf );

    iMessageData->AppendAttachmentL( iAttachmentFile->FileName() );
    iAttachmentFile->Release();
    }

//  End of File
