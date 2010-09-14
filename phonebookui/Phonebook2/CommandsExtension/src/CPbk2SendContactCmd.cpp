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
* Description:  Phonebook 2 send contact command.
*
*/


#include "CPbk2SendContactCmd.h"

// Phonebook 2
#include "Pbk2SendCmdUtils.h"
#include "CPbk2vCardConverter.h"
#include <MPbk2CommandObserver.h>
#include <pbk2commands.rsg>
#include <pbk2cmdextres.rsg>
#include <Pbk2ProcessDecoratorFactory.h>
#include <MPbk2ProcessDecorator.h>
#include <MPbk2ContactLinkIterator.h>
#include <CPbk2AttachmentFile.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkVCardEng.h>
#include <MVPbkContactOperationBase.h>
#include <MPbk2ContactUiControl.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldTypeMapping.h>
#include <vpbkeng.rsg>

// System includes
#include <txtrich.h>
#include <barsread.h>
#include <AknWaitDialog.h>
#include <sendui.h>
#include <TSendingCapabilities.h>
#include <MsgBioUids.h>
#include <SendUiConsts.h>
#include <eikenv.h>
#include <CMessageData.h>
#include <eikclb.h>
#include <aknPopup.h>
#include <featmgr.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TUint KNoMenu = 0;
const TInt KGranularity( 4 );

struct TPbk2SelectionMapping
    {
    /// Maps to first selection
    TPbk2SendvCardChoiceItemEnumerations iStSelection;
    /// Maps to second selection
    TPbk2SendvCardChoiceItemEnumerations iNdSelection;
    /// Maps to third selection
    TPbk2SendvCardChoiceItemEnumerations iRdSelection;
    //// Resource id
    TInt iResId;
    };

enum TSelection
    {
    ESelectionLast = -1
    };

/// Maps listbox index selections to 'sending selected detail'
/// menu selections If command not supported selection is ECancel.
static const TPbk2SelectionMapping SelectionMapping[] =
    {
    {ESendCurrentItem, ESendAllData, ECancel,
        R_PBK2_CONTACTINFO_SEND_OPTIONS},
    {ESendCurrentItem, ESendAllDataWithoutPicture, ESendAllData,
        R_PBK2_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL},
    {ESendAllDataWithoutPicture, ESendAllData, ECancel,
        R_PBK2_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL_NO_FIELD},
    {ESendAllDataWithoutPicture, ESendAllData, ECancel,
        R_PHONEBOOK2_SEND_OPTIONS},
    {ECancel, ECancel, ECancel, ESelectionLast}
    };


/// Represents the different listbox index selections.
enum TPbk2ListBoxSelections
    {
    EFirstSelection = 0,
    ESecondSelection,
    EThirdSelection
    };

#if 0 // Not used in ECE
/**
    Function checks if the focused field is one of these fields.

    - Prefix
    - Suffix
    - Firstname
    JAPANESE.PRONOUNCIATION{
    - FirstnameReading
    }JAPANESE.PRONOUNCIATION
    - Middle name
    - Lastname
    JAPANESE.PRONOUNCIATION{
    - Last name Reading
    }JAPANESE.PRONOUNCIATION
    - Ringing tone
    CALL.OBJECT.DISPLAY{
    - Image
    }CALL.OBJECT.DISPLAY
    CALL.OBJECT.DISPLAY{
    - Text for call
    }CALL.OBJECT.DISPLAY
    - Synchronization
*/
TBool IsNameOrPropertyField( const MVPbkBaseContactField& aField )
    {
    TBool res ( EFalse );
    const MVPbkFieldType* fieldType = aField.BestMatchingFieldType();
    if ( fieldType )
        {
        TInt resId( fieldType->FieldTypeResId() );
        if ( resId == R_VPBK_FIELD_TYPE_PREFIX ||
            resId == R_VPBK_FIELD_TYPE_SUFFIX ||
            resId == R_VPBK_FIELD_TYPE_FIRSTNAME ||
            resId == R_VPBK_FIELD_TYPE_FIRSTNAMEREADING ||
            resId == R_VPBK_FIELD_TYPE_MIDDLENAME ||
            resId == R_VPBK_FIELD_TYPE_LASTNAME ||
            resId == R_VPBK_FIELD_TYPE_LASTNAMEREADING ||
            resId == R_VPBK_FIELD_TYPE_RINGTONE ||
            resId == R_VPBK_FIELD_TYPE_CALLEROBJIMG ||
            resId == R_VPBK_FIELD_TYPE_CALLEROBJTEXT ||
            resId == R_VPBK_FIELD_TYPE_SYNCCLASS )
            {
            res = ETrue;
            }
        }
    return res;
    }
#endif // 0

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_SendvCardsL = 1,
    EPanicLogic_CmdSendContactDataL,
    EPanicLogic_MapSelection,
    EPanicLogic_ConversionDone,
    EPanicLogic_ReadFileTypeL,
    ERunL_OOB,
    EPanicPreCond_ErrArgument,
    EResetFindResult
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2SendContactCmd");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG


/**
 * Helper class for sending the vCard(s) in async callback.
 */
NONSHARABLE_CLASS(CVCardSender) : public CIdle
    {
    public: // Constructor

        /**
         * Creates a new instance of this object.
         *
         * @param aPriority     Desired priority.
         */
        static CVCardSender* NewL(
                TInt aPriority );

    private: // From CIdle
        void RunL();
        TInt RunError(
                TInt aError );

    private: // Implementation
        CVCardSender(
                TInt aPriority );
    };

// --------------------------------------------------------------------------
// CVCardSender::CVCardSender
// --------------------------------------------------------------------------
//
inline CVCardSender::CVCardSender( TInt aPriority ):
        CIdle( aPriority )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CVCardSender::NewL
// --------------------------------------------------------------------------
//
CVCardSender* CVCardSender::NewL( TInt aPriority )
    {
    return new ( ELeave ) CVCardSender( aPriority );
    }

// --------------------------------------------------------------------------
// CVCardSender::RunL
// --------------------------------------------------------------------------
//
void CVCardSender::RunL()
    {
    CIdle::RunL();
    // Destroy self.
    // If RunL (the callback) leaves RunError will handle the deletion.
    delete this;
    }

// --------------------------------------------------------------------------
// CVCardSender::RunError
// --------------------------------------------------------------------------
//
TInt CVCardSender::RunError( TInt aError)
    {
    delete this;
    // Forward all errors to the active scheduler
    return aError;
    }

/**
 * Creates a rich text object and packages contents of a file to it.
 * The file's data is not converted in any way except that characters
 * are widened to 16 bits.
 *
 * @param aEikEnv       EIKON environment.
 * @param aFileName     Name of the file to convert.
 * @return  A new rich text object with file's contents.
 *          The returned object is also left on top of the cleanup stack.
 */
CRichText* CreateRichTextFromFileLC
        (CEikonEnv& aEikEnv, const TDesC& aFileName)
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CreateRichTextFromFileLC (%S)"), &aFileName);
    // Common allocation granularity and buffer size for rich text and
    // file reading
    const TInt KBufSize = CEditableText::EDefaultTextGranularity;

    // Create a rich text object with default formatting
    CRichText* richText = CRichText::NewL(
        aEikEnv.SystemParaFormatLayerL(),
        aEikEnv.SystemCharFormatLayerL(),
        CEditableText::ESegmentedStorage,
        KBufSize  // Allocation granularity
        );
    CleanupStack::PushL(richText);

    // Open the file for reading
    RFile file;
    User::LeaveIfError(file.Open
        (aEikEnv.FsSession(), aFileName,
        EFileRead|EFileStream|EFileShareReadersOnly));
    CleanupClosePushL(file);

    // Create two buffers: 8-bit for reading from file and 16-bit for
    // converting to 16-bit format
    HBufC8* buf8 = HBufC8::NewLC(KBufSize);
    TPtr8 ptr8 = buf8->Des();
    HBufC16* buf16 = HBufC16::NewLC(ptr8.MaxLength());
    TPtr16 ptr16 = buf16->Des();

    // Read, convert and append to rich text until the file ends
    for (TInt err = file.Read(ptr8);
        ptr8.Length() > 0;
        err = file.Read(ptr8))
        {
        User::LeaveIfError(err);
        ptr16.Copy(ptr8);
        richText->InsertL(richText->DocumentLength(), ptr16);
        }

    // Cleanup and return
    CleanupStack::PopAndDestroy(3);  // buf16, buf8, file
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CreateRichTextFromFileLC, end"));
    return richText;
    }

}  /// namespace

/**
 * Sending parameters.
 */
class TPbk2SendingParams
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aMtmFilter    MTM filter
         * @param aCapabilities Sending capabilities.
         */
        TPbk2SendingParams(
                CArrayFixFlat<TUid>* aMtmFilter,
                TSendingCapabilities aCapabilities );

    public: // Data
        /// Ref: MTM filter
        CArrayFixFlat<TUid>* iMtmFilter;
        /// Own: Sending capabilities
        TSendingCapabilities iCapabilities;
    };

// --------------------------------------------------------------------------
// TPbk2SendingParams::TPbk2SendingParams
// --------------------------------------------------------------------------
//
inline TPbk2SendingParams::TPbk2SendingParams
            ( CArrayFixFlat<TUid>* aMtmFilter,
              TSendingCapabilities aCapabilities ) :
                iMtmFilter( aMtmFilter ), iCapabilities( aCapabilities )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::CPbk2SendContactCmd
// --------------------------------------------------------------------------
//
inline CPbk2SendContactCmd::CPbk2SendContactCmd(
        MPbk2ContactUiControl& aUiControl ) :
    CActive(EPriorityIdle),
    iUiControl(&aUiControl)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::~CPbk2SendContactCmd
// --------------------------------------------------------------------------
//
CPbk2SendContactCmd::~CPbk2SendContactCmd()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd, destructor (0x%x)"), this);
    Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    delete iWaiter;
    delete iSelectedContacts;
    delete iConverter;
    delete iVCardSender;
    delete iVCardEngine;
    iStoreContacts.ResetAndDestroy();
    iStoreContacts.Close();
    delete iRetrieveOperation;
    delete iDecorator;
    delete iCntIterator;
    delete iSendUi;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2SendContactCmd* CPbk2SendContactCmd::NewL
    (MPbk2ContactUiControl& aUiControl)
    {
    CPbk2SendContactCmd* self = new (ELeave)CPbk2SendContactCmd(aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ConstructL()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd::ConstructL(0x%x)"), this);
    CActiveScheduler::Add(this);
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );
    iEikEnv = CEikonEnv::Static();
    iAppServices = CPbk2ApplicationServices::InstanceL();
    iVCardEngine = CVPbkVCardEng::NewL( iAppServices->ContactManager() );
    iConverter = CPbk2vCardConverter::NewL(
        iEikEnv->FsSession(),
        iAppServices->ContactManager(),
        *iVCardEngine,
        iAppServices->NameFormatter() );
    iVCardSender = CVCardSender::NewL(CActive::EPriorityIdle);
    iSendUi = CSendUi::NewL();

    if( iUiControl )
        {
        // Use iterator to make sure that OOM situation does not occur
        // if multiple contacts selected.
        iCntIterator = iUiControl->SelectedContactsIteratorL();
        if ( !iCntIterator )
            {
            // If no iterator, there is no selected contacts.
            // Only one contact is returned.
            iSelectedContacts = iUiControl->SelectedContactsOrFocusedContactL();
            }
        iFocusedField = iUiControl->FocusedField();
        iUiControl->RegisterCommand( this );
        }
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd::ConstructL, end"));
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL(this);

    iCurrentContactLinkIndex = 0;
    iState = EShowingSendQuery;
    IssueRequest();

    CleanupStack::Pop(this);
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd::ExecuteLD, end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == & aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::RunL()
    {
    switch (iState)
        {
        case EShowingSendQuery:
            {
            iMtmUid = ShowSendQueryL();
            if (iMtmUid == KNullUid)
                {
                iState = ECancelling;
                }
            else
                {
                iState = ERetrievingContact;
                }
            IssueRequest();
            break;
            }

        case ERetrievingContact:
            {
            if ( iSelectedContacts )
                {
                __ASSERT_DEBUG( iSelectedContacts->Count() >
                        iCurrentContactLinkIndex, Panic( ERunL_OOB ) );
                RetrieveContactL(iSelectedContacts->At
                    (iCurrentContactLinkIndex));
                }
            else
                {
                iState = ESelectingSentData;
                IssueRequest();
                }
            break;
            }

        case ESelectingSentData:
            {
            TInt selection(ESendAllData);
            if ( iSelectedContacts )
                {
                // If there is only one contact or launched in info view
                // do the data selection
                selection = SelectSentDataL();
                }
            ConvertContactL(selection);
            break;
            }

        case EStopping:
            {
            ProcessDone(KErrNone);
            break;
            }

        case ECancelling:
            {
            ProcessDone( KErrCancel );
            break;
            }
        case EReleaseResource:
            {
            CPbk2AttachmentFileArray& fileArray = iConverter->AttachmentFileArray();
            TInt count = fileArray.Count();
            TInt deleteIndex = count -1;
            CPbk2AttachmentFile * file = fileArray.At(deleteIndex);
            fileArray.Delete(deleteIndex);
            delete file;
            
            if ( count > 1) 
            {
              IssueRequest();
            }
            else
            {
              iWaiter->AsyncStop();
            }
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SendContactCmd::RunError(TInt aError)
    {
    if( iWaiter && iWaiter->IsStarted() )
        iWaiter->AsyncStop();
    
    return FilterErrors(aError);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::DoCancel()
    {
    if( iWaiter && iWaiter->IsStarted() )
        iWaiter->AsyncStop();
    
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ConversionDone
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ConversionDone( TInt PBK2_DEBUG_ONLY( aCount ) )
    {
    PBK2_DEBUG_PRINT
        ( PBK2_DEBUG_STRING( "CPbk2SendContactCmd::ConversionDone(0x%x)" ),
        this );

    __ASSERT_DEBUG( iStoreContacts.Count() == aCount,
        Panic( EPanicLogic_ConversionDone ) );

    // Decorator will call processdismissed which will continue
    // contact send
    iDecorator->ProcessStopped();

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2SendContactCmd::ConversionDone(0x%x), end" ), this );

    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ConversionError
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ConversionError(TInt aError)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::ConversionError(%d)"), aError);

    // Report the failure to the user
    CCoeEnv::Static()->HandleError( aError );

    // iDecorator calls processdismissed
    iDecorator->ProcessStopped();
    iState = EStopping;

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::ConversionError(0x%x), end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation, MVPbkStoreContact* aContact)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We now have a store contact
        TInt error = iStoreContacts.Append(aContact);
        if ( error == KErrNone )
            {
            ++iCurrentContactLinkIndex;
            if (iSelectedContacts->Count() == iCurrentContactLinkIndex)
                {
                iCurrentContactLinkIndex = 0;
                iState = ESelectingSentData;
                }
            else
                {
                iState = ERetrievingContact;
                }

            IssueRequest();
            }
        else
            {
            ProcessDone( error );
            }
        }
    else
        {
        IssueStopRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, TInt aError)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
      ("CPbk2SendContactCmd::VPbkSingleContactOperationFailed (%d)"), aError);
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We cannot get the contact, so we have to
        // fail. We cannot continue, since this operation
        // was executed only in case of one contact.
        ProcessDone(aError);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::SendvCardsLD
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::SendvCardsLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd::SendvCardsLD(0x%x)"), this);
    // Relinquish ownership, iVCardSender takes care of it self
    iVCardSender = NULL;

    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );

    if (iConverter->FileNames().MdcaCount() > 0 &&
        !iUnderDestruction)
        {
        TSendingCapabilities caps;
        User::LeaveIfError(
                iSendUi->ServiceCapabilitiesL( iMtmUid, caps ) );
        if (caps.iFlags & TSendingCapabilities::ESupportsAttachments)
            {
            // Not sending through SMS, just pass the attachments
            __ASSERT_DEBUG(iConverter->FileNames().MdcaCount()>=1,
                Panic(EPanicPreCond_SendvCardsL));

            //Fill message data
            const TInt count( iConverter->FileNames().MdcaCount());
            for( TInt i( 0 ); i < count; ++i )
                {
                messageData->AppendAttachmentHandleL(
                        iConverter->AttachmentFileArray().FileHandleAt( i ) );
                }
            SendMsgUsingSendUI(messageData);
            }
        else if (caps.iFlags & TSendingCapabilities::ESupportsBodyText)
            {
            PBK2_DEBUG_PRINT
                (PBK2_DEBUG_STRING("SendvCardsLD, KSenduiMtmSmsUid"));
            // Sending through SMS -> there should be only one vCard
            // attachment. Package the attachment to a rich text object and
            // send it as the message body.
            __ASSERT_DEBUG(iConverter->FileNames().MdcaCount()==1,
                Panic(EPanicPreCond_SendvCardsL));

            // Copy the one and only attachment into a rich text object
            CRichText* msgBody = CreateRichTextFromFileLC
                (*iEikEnv, iConverter->FileNames().MdcaPoint(0));

            messageData->SetBodyTextL( msgBody );
            SendMsgUsingSendUI(messageData);

            CleanupStack::PopAndDestroy(msgBody);
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }

    CleanupStack::PopAndDestroy(); //messageData

    ProcessDone(KErrNone);
        PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SendContactCmd::SendvCardsLD, end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::SendvCardsLD
// --------------------------------------------------------------------------
//
TInt CPbk2SendContactCmd::SendvCardsLD(TAny* aThis)
    {
    __ASSERT_DEBUG( aThis, Panic(EPanicPreCond_ErrArgument) );

    CPbk2SendContactCmd* self = static_cast<CPbk2SendContactCmd*>(aThis);
    self->SendvCardsLD();

    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::AnyThumbnailsL
// --------------------------------------------------------------------------
//
TBool CPbk2SendContactCmd::AnyThumbnailsL() const
    {
    TInt cntCount(iStoreContacts.Count());

    for (TInt i=0; i < cntCount; ++i)
        {
        MVPbkStoreContact* contact = (iStoreContacts)[i];
        const MVPbkBaseContactField* field =
            Pbk2SendCmdUtils::FindFieldL(
                *contact,
                R_THUMBNAIL_FIELD_TYPE,
                iAppServices->ContactManager().FieldTypes());

        if (field)
            {
            // We can exit the loop as soon as a thumbnail
            // is found
            return ETrue;
            }
        }
    return EFalse;
    }

#if 0
// --------------------------------------------------------------------------
// CPbk2SendContactCmd::SelectionListL
// --------------------------------------------------------------------------
//
TInt CPbk2SendContactCmd::SelectionListL() const
    {
    TInt ret( KNoMenu );
    TBool supportsFieldType( ETrue );
    TBool nameOrPropertyField( EFalse );

    // Check is the sending media SMS and does the contact
    // have a thumbnail
    TBool smsAndThumbnail( EFalse );
    if ( ( iMtmUid == KSenduiMtmSmsUid ) && AnyThumbnailsL() )
        {
        smsAndThumbnail = ETrue;
        }

    // If focused field is supplied, the command object was
    // launched from contact info view and that requires
    // we have to check is the field supported by vCard spec
    if ( iFocusedField )
        {
        if ( iAppServices && ( !iVCardEngine->SupportsFieldType(
                *Pbk2SendCmdUtils::FieldType(
                    *iFocusedField,
                        iAppServices->ContactManager().FieldTypes() ) ) ) )
            {
            supportsFieldType = EFalse;
            }
        }

    // Now check the cases when the menu needs to be shown
    // There are two main branches depending on which view
    // this command object was launched from
    if ( iFocusedField )
        {
        // Command object was launched from contact info view.

        if ( !smsAndThumbnail )
            {
            // When there is no thumbnail involved the selection
            // menu is shown only if we are over vCard supported
            // field and the focus is not in the name or property field.
            nameOrPropertyField = IsNameOrPropertyField( *iFocusedField );
            if ( supportsFieldType && !nameOrPropertyField )
                {
                ret = R_PBK2_CONTACTINFO_SEND_OPTIONS;
                }
            }
        else
            {
            // Thumbnail is involved, the selection menu depends
            // on whether we are over vCard supported field or not
            if ( supportsFieldType )
                {
                ret = R_PBK2_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL;
                }
            else
                {
                ret = R_PBK2_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL_NO_FIELD;
                }
            }
        }
    else
        {
        // Command object was launched from contact list view,
        // the menu is shown only in case there is thumbnail
        // involded and the sending media is SMS
        if ( smsAndThumbnail )
            {
            ret = R_PHONEBOOK2_SEND_OPTIONS;
            }
        }

    return ret;
    }
#endif

#if 1
// --------------------------------------------------------------------------
// CPbk2SendContactCmd::SelectionListL
// --------------------------------------------------------------------------
//
TInt CPbk2SendContactCmd::SelectionListL() const
    {
    TInt ret( KNoMenu );

    // Show the query if contact has a thumbnail.
    if ( AnyThumbnailsL() )
        {
        ret = R_PHONEBOOK2_SEND_OPTIONS;
        }

    return ret;
    }
#endif

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::MapSelection
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::MapSelection(TInt& aSelection, TInt aShownMenu)
    {
    for (TInt i = 0; SelectionMapping[i].iResId != ESelectionLast; ++i)
        {
        if (SelectionMapping[i].iResId == aShownMenu)
            {
            TInt selection(KErrNotFound);
            switch(aSelection)
                {
                case 0:
                    {
                    selection = SelectionMapping[i].iStSelection;
                    break;
                    }
                case 1:
                    {
                    selection = SelectionMapping[i].iNdSelection;
                    break;
                    }
                case 2:
                    {
                    selection = SelectionMapping[i].iRdSelection;
                    break;
                    }
                default:
                    {
                    __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
                    }
                }

            if (selection == ECancel)
                {
                __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
                }

            aSelection = selection;
            return;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::SelectSentDataL
// --------------------------------------------------------------------------
//
TInt CPbk2SendContactCmd::SelectSentDataL()
    {
    TInt selectionIndex(ESendAllData);

    // Get the resource id of the menu to be shown
    TInt resourceId = SelectionListL();

    if (resourceId)
        {
        // Create a list box
        CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>
            (EikControlFactory::CreateByTypeL
            (EAknCtSinglePopupMenuListBox).iControl);
        CleanupStack::PushL(listBox);

        // Create a popup list
        CAknPopupList* popupList = CAknPopupList::NewL
            (listBox, R_AVKON_SOFTKEYS_OK_CANCEL__OK,
            AknPopupLayouts::EMenuGraphicWindow);
        CleanupStack::PushL(popupList);

        HBufC* headingText= CCoeEnv::Static()->AllocReadResourceLC
            (R_PBK2_BUSINESSCARD_SEND_HEADING);
        popupList->SetTitleL(*headingText);
        CleanupStack::PopAndDestroy(); // headingText

        // Init list box
        listBox->SetContainerWindowL(*popupList);

        TResourceReader resReader;
        CCoeEnv::Static()->CreateResourceReaderLC(resReader, resourceId);
        listBox->ConstructFromResourceL(resReader);
        CleanupStack::PopAndDestroy();  // resReader

        listBox->CreateScrollBarFrameL(ETrue);
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL
            (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

        CleanupStack::Pop(); // popupList

        // Show popuplist dialog
        TInt res = popupList->ExecuteLD();
        if (res)
            {
            selectionIndex = listBox->CurrentItemIndex();

            // We have to remap the selection index since
            // several different listbox configurations
            MapSelection(selectionIndex, resourceId);
            }
        else
            {
            selectionIndex = ECancel;
            }
        CleanupStack::PopAndDestroy(); // listBox
        }

    return selectionIndex;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ShowSendQueryL
// --------------------------------------------------------------------------
//
TUid CPbk2SendContactCmd::ShowSendQueryL()
    {
    /// Sending  params
    TPbk2SendingParams params = CreateParamsLC();

    TUid uid = iSendUi->ShowSendQueryL( NULL, params.iCapabilities, params.iMtmFilter );

    CleanupStack::PopAndDestroy(); // mtmfilter
    return uid;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ProcessDone
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ProcessDone(TInt aCancelCode)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::ProcessDone (%d)"), aCancelCode);
    if ( aCancelCode != KErrCancel && iUiControl )
        {
        // Ignore possible errors in clearing find box contents in release
        // builds; just assert in debug builds. Showing an error here might
        // signal to the user that there was some problem in the send contact
        // command completion.
        TInt err( KErrNone );
        TRAP( err, iUiControl->ResetFindL() );
        __ASSERT_DEBUG( err == KErrNone, Panic( EResetFindResult ) );

        iUiControl->UpdateAfterCommandExecution();
        }

    if ( iCommandObserver )
        {
        // Notify command owner that the command has finished
        iCommandObserver->CommandFinished(*this);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ProcessDismissed( TInt aCancelCode )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::ProcessDismissed (%d)"), aCancelCode);
    if (aCancelCode == EAknSoftkeyCancel)
        {
        ProcessDone( KErrCancel );
        }
    else if ( iState == EStopping )
        {
        IssueStopRequest();
        }
    else
        {
        // Cancel first any outstanding requests
        iVCardSender->Cancel();
        iVCardSender->Start( TCallBack( &CPbk2SendContactCmd::SendvCardsLD,
            this ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::CreateParamsLC
// --------------------------------------------------------------------------
//
TPbk2SendingParams CPbk2SendContactCmd::CreateParamsLC()
    {
    // Construct the MTM filter
    CArrayFixFlat<TUid>* mtmFilter =
        new(ELeave) CArrayFixFlat<TUid>(KGranularity);
    CleanupStack::PushL(mtmFilter);

    mtmFilter->AppendL(KSenduiMtmPostcardUid);    // hide postcard
    mtmFilter->AppendL( KSenduiMtmAudioMessageUid ); // hide audio message
    mtmFilter->AppendL( KMmsDirectUpload ); // hide upload

    // Check must we disable SMS menuitem
    if ( iSendUi->CanSendBioMessage( KMsgBioUidVCard ) )
        {
        mtmFilter->AppendL(KSenduiMtmSmsUid);   // hide SMS
        }

    // If there are items more than one, hide items
    if ( IsMoreThanOneContact() )
        {
        mtmFilter->AppendL(KSenduiMtmSmsUid);   // hide SMS
        }
    else if (!FeatureManager::FeatureSupported(KFeatureIdMMS))
        {
        // Hide MMS if not supported
        mtmFilter->AppendL(KSenduiMtmMmsUid);
        }

    // Hide Email if not supported
    if (!FeatureManager::FeatureSupported(KFeatureIdEmailUi))
        {
        mtmFilter->AppendL(KSenduiMtmSmtpUid);
        mtmFilter->AppendL(KSenduiMtmImap4Uid);
        mtmFilter->AppendL(KSenduiMtmPop3Uid);
        mtmFilter->AppendL(KSenduiMtmSyncMLEmailUid);
        }

    TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsBioSending;
    capabilities.iFlags |= TSendingCapabilities::ESupportsAttachments;
    return TPbk2SendingParams( mtmFilter, capabilities );
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ConvertContactL
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ConvertContactL(TInt aSelectionIndex)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::ConvertContactL(0x%x)"), this);

    delete iDecorator;
    iDecorator = NULL;

    if (aSelectionIndex != ECancel)
        {
        // Start wait note if user continues. If it cancels then don't
        // create decorator because it causes a panic in CActive destructor
        // of this instance.
        iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL(
                R_QTN_SM_WAIT_BUSINESS_CARD, ETrue );
        iDecorator->SetObserver( *this );
        // wait note doesn't care about amount
        iDecorator->ProcessStartedL( 0 );

        if ( iFocusedField )
            {
            iConverter->ConvertContactL(
                iStoreContacts.Array(), iFocusedField,
                aSelectionIndex, *this);
            }
        else if ( iCntIterator )
            {
            iConverter->ConvertContactsL(
                *iCntIterator, aSelectionIndex, *this );
            }
        else
            {
            // iContacts validity is checked in function entry
            iConverter->ConvertContactsL(
                iStoreContacts.Array(), aSelectionIndex, *this);
            }
        }
    else
        {
        IssueStopRequest();
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::ConvertContactL(0x%x), end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::RetrieveContactL(const MVPbkContactLink& aLink)
    {
    iRetrieveOperation = iAppServices->ContactManager().
            RetrieveContactL(aLink, *this);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::FilterErrors
// --------------------------------------------------------------------------
//
TInt CPbk2SendContactCmd::FilterErrors(TInt aErrorCode)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::FilterErrors(%d) start"), aErrorCode);

    TInt result = aErrorCode;
    switch (aErrorCode)
        {
        case KErrNotFound:  // FALLTHROUGH
        case KErrInUse:     // FALLTHROUGH
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
            ProcessDone(aErrorCode);
            break;
            }
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::FilterErrors(%d) end"), result);

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::IssueStopRequest()
    {
    iState = EStopping;
    if (!IsActive())
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::SendMsgUsingSendUI
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::SendMsgUsingSendUI(CMessageData* aMsgData)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::SendMsgUsingSendUI(0x%x), start"), this);

    // Send the message using Send Ui
    TRAPD(error, iSendUi->CreateAndSendMessageL( iMtmUid, aMsgData, KMsgBioUidVCard));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::SendMsgUsingSendUI(%d), error"), error);

    if (error != KErrNone)
        {
        FilterErrors(error);
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SendContactCmd::SendMsgUsingSendUI(0x%x), end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::IsMoreThanOneContact
// --------------------------------------------------------------------------
//
TBool CPbk2SendContactCmd::IsMoreThanOneContact()
    {
    TBool result = EFalse;
    if ( iCntIterator )
        {
        result = ETrue;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::CommandExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2SendContactCmd::CommandExtension(TUid aExtensionUid )
    {
     if( aExtensionUid == TUid::Uid(MPbk2ResourceReleaseUID) )
        {
        return static_cast<MPbk2ResourceRelease *>(this);
        }
     else
        {
        return NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SendContactCmd::ReleaseResource
// --------------------------------------------------------------------------
//
void CPbk2SendContactCmd::ReleaseResource()
    {
    Cancel();
    if (iConverter)
        {
        iConverter->Cancel();
        CPbk2AttachmentFileArray& fileArray = iConverter->AttachmentFileArray();        
        if (fileArray.Count() > 0)
            {
            if ( !iWaiter )
                {
                TInt err( KErrNone );
                TRAP( err,iWaiter = new (ELeave) CActiveSchedulerWait());
                if ( err != KErrNone )
                    {
                    fileArray.ResetAndDestroy();
                    return;
                    }
                 }
            
              iState = EReleaseResource;
              IssueRequest();         
              iWaiter->Start();
            }
       }
    }

// End of File
