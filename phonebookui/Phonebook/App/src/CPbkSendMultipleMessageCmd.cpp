/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*           Provides phonebook send message command object methods.
*
*/


// INCLUDE FILES
#include "CPbkSendMultipleMessageCmd.h"

#include <CPbkContactEngine.h>
#include <MPbkContactNameFormat.h>
#include <MPbkCommandObserver.h>
#include <CPbkContactItem.h>
#include <CPbkSmsAddressSelect.h>
#include <CPbkEmailOverSmsAddressSelect.h>
#include <CPbkMmsAddressSelect.h>
#include <CPbkEmailAddressSelect.h>
#include "TSingleDesCArray.h"
#include "CPbkAppGlobals.h"
#include <Phonebook.hrh>
#include <phonebook.rsg>
#include <FeatMgr.h>
#include <CPbkAttachmentFile.h>

#include <sendui.h>             // Send UI API
#include <SendUiConsts.h>       // Send UI MTM uid's
#include <CMessageData.h>
#include <StringLoader.h>
#include <aknenv.h>

#include <pbkdebug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KMaxContactIdStringLength = 10;

void WritePostcardAttachmentContentL(
    const CPbkContactItem& aContact,
    CPbkAttachmentFile& aAttachmentFile )
    {
    HBufC* contactId = HBufC::NewLC( KMaxContactIdStringLength );
    contactId->Des().Num( aContact.Id() );
    HBufC8* string8 = HBufC8::NewLC( KMaxContactIdStringLength );
    string8->Des().Append( *contactId );
    aAttachmentFile.File().Write( *string8 );
    CleanupStack::PopAndDestroy( 2, contactId );
    }


// LOCAL DEBUG CODE
#ifdef _DEBUG

enum TPanicCode
    {
    EPanicUnknownMedia = 1,
    };


void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSendMultipleMessageCmd");
    User::Panic(KPanicText,aReason);
    }
#endif


}  // namespace


// ================= MEMBER FUNCTIONS =======================

// Default constructor
CPbkSendMultipleMessageCmd::CPbkSendMultipleMessageCmd
        (
        CPbkContactEngine& aEngine,
        TPbkSendingParams aParams,
        const CContactIdArray& aContacts,
        const TPbkContactItemField* aFocusedField,
        TBool aUseDefaultDirectly) :
        iEngine(aEngine),
        iParams(aParams),
        iContacts(aContacts),
        iFocusedField(aFocusedField),
        iUseDefaultDirectly(aUseDefaultDirectly)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendMultipleMessageCmd::CPbkSendMultipleMessageCmd(0x%x)"), this);
    }

// Second phase constructor
void CPbkSendMultipleMessageCmd::ConstructL()
    {
    }

// Static constructor
CPbkSendMultipleMessageCmd* CPbkSendMultipleMessageCmd::NewL
        (
        CPbkContactEngine& aEngine,
        TPbkSendingParams aParams,
        const CContactIdArray& aContacts,
        const TPbkContactItemField* aFocusedField,
        TBool aUseDefaultDirectly)
    {
    CPbkSendMultipleMessageCmd* self = new(ELeave)
        CPbkSendMultipleMessageCmd( aEngine, aParams, aContacts, aFocusedField,
        aUseDefaultDirectly);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// Destructor
CPbkSendMultipleMessageCmd::~CPbkSendMultipleMessageCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendMultipleMessageCmd::~CPbkSendMultipleMessageCmd(0x%x)"), this);
   delete iAttachmentFile;
   }

void CPbkSendMultipleMessageCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendMultipleMessageCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL(this);

    iMtmUid = ShowWriteQueryL();
    if ( iMtmUid == KNullUid )
        {
        // User pressed cancel
        CleanupStack::PopAndDestroy( this ); // this
        return;
        }
    TUid techTypeUid =
        CPbkAppGlobals::InstanceL()->SendUiL()->TechnologyType( iMtmUid );

    const TInt contactCount( iContacts.Count() );
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    TBool dismissed( EFalse );

    for ( TInt i( 0 ); i < contactCount && !dismissed; ++i )
        {
        TPtrC address( KNullDesC );
        CPbkContactItem* contact = iEngine.ReadContactLC( iContacts[i] );

        // Check specific MTMs first
        if ( iMtmUid ==  KSenduiMtmPostcardUid )
            {
            // Create attachment file object
            delete iAttachmentFile;
            iAttachmentFile = NULL;
            HBufC* attFileName = StringLoader::LoadLC( R_PBK_POSTCARD_WRITE_ATTACHMENT_TAG );
            iAttachmentFile = CPbkAttachmentFile::NewL
                (*attFileName, iEngine.FsSession(), EFileWrite|EFileShareAny);
            CleanupStack::PopAndDestroy( attFileName );
            WritePostcardAttachmentContentL( *contact, *iAttachmentFile );
            messageData->AppendAttachmentL( iAttachmentFile->FileName() );
            iAttachmentFile->Release();
            }
        // Otherwise check by technology type to support 3rd party MTMs
        else if ( iMtmUid == KSenduiMtmSmsUid ||
                  techTypeUid == KSenduiTechnologySmsUid )
            {
            if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                CPbkEmailOverSmsAddressSelect::TParams params( *contact );
                params.SetFocusedField(iFocusedField);
                params.SetUseDefaultDirectly(iUseDefaultDirectly);
                CPbkEmailOverSmsAddressSelect* selectDlg = new(ELeave) CPbkEmailOverSmsAddressSelect;
                if (selectDlg->ExecuteLD(params))
                    {
                    address.Set(params.SelectedField()->Text());
                    }
                }
            else
                {
                CPbkSmsAddressSelect::TParams params( *contact );
                params.SetFocusedField(iFocusedField);
                params.SetUseDefaultDirectly(iUseDefaultDirectly);
                CPbkSmsAddressSelect* selectDlg = new(ELeave) CPbkSmsAddressSelect;
                if (selectDlg->ExecuteLD(params))
                    {
                    address.Set(params.SelectedField()->Text());
                    }
                }
            }
        else if ( iMtmUid == KSenduiMtmMmsUid ||
                  techTypeUid == KSenduiTechnologyMmsUid ||
                  iMtmUid == KSenduiMtmAudioMessageUid )
            {
            CPbkMmsAddressSelect::TParams params( *contact );
            params.SetFocusedField(iFocusedField);
            params.SetUseDefaultDirectly(iUseDefaultDirectly);
            CPbkMmsAddressSelect* selectDlg = new(ELeave) CPbkMmsAddressSelect;
            if (selectDlg->ExecuteLD(params))
                {
                address.Set(params.SelectedField()->Text());
                }
            }
        else if ( iMtmUid == KSenduiMtmSmtpUid ||
                  techTypeUid == KSenduiTechnologyMailUid )
            {
            CPbkEmailAddressSelect::TParams params( *contact );
            params.SetFocusedField(iFocusedField);
            params.SetUseDefaultDirectly(iUseDefaultDirectly);
            CPbkEmailAddressSelect* selectDlg = new(ELeave) CPbkEmailAddressSelect;
            if (selectDlg->ExecuteLD(params))
                {
                address.Set(params.SelectedField()->Text());
                }
            }
        else
            {
            if ( iMtmUid != KNullUid )
                {
                __ASSERT_DEBUG(EFalse, Panic(EPanicUnknownMedia));
                }
            }

        if (address.Length() > 0)
            {
            MPbkContactNameFormat& nameFormatter = iEngine.ContactNameFormat();
            HBufC* nameBuffer = nameFormatter.GetContactTitleOrNullL( *contact );
            if ( nameBuffer )
                {
                CleanupStack::PushL(nameBuffer);
                messageData->AppendToAddressL( address, *nameBuffer );
                CleanupStack::PopAndDestroy( nameBuffer );
                }
            else
                {
                messageData->AppendToAddressL( address );
                }
            }

        if (!dismissed)
            {
            // Abort the whole process if end key was pressed during address
            // selection dialog and Phonebook application was terminated.
            dismissed = (NULL != CAknEnv::AppWithShutterRunning());
            }

        CleanupStack::PopAndDestroy( contact );
        }
    if ( !dismissed )
        {
        // Empty message editor is not displayed if none of selected contacts
        // has a valid address information
        CPbkAppGlobals::InstanceL()->SendUiL()->CreateAndSendMessageL(
            iMtmUid, messageData );
        }

    CleanupStack::PopAndDestroy( messageData );

    if ( iAttachmentFile )
        {
        // no error checking, exiting phonebook would too much for this error
        iEngine.FsSession().Delete( iAttachmentFile->FileName() );
        }

    if ( iObserver )
        {
        iObserver->CommandFinished( *this );
        }
    // Destroy itself as promised
    CleanupStack::PopAndDestroy( this ); // this
    }

void CPbkSendMultipleMessageCmd::ProcessFinished(MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendMultipleMessageCmd::ProcessFinished(0x%x)"), this);
    }

TUid CPbkSendMultipleMessageCmd::ShowWriteQueryL()
    {
    TUid uid = CPbkAppGlobals::InstanceL()->SendUiL()
        ->ShowTypedQueryL( CSendUi::EWriteMenu, NULL, iParams.iCapabilities, iParams.iMtmFilter );
    return uid;
    }

void CPbkSendMultipleMessageCmd::AddObserver( MPbkCommandObserver& aObserver )
    {
    iObserver = &aObserver;
    }


//  End of File
