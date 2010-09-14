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
*           Provides phonebook send contacts command object methods.
*
*/


// INCLUDE FILES
#include "CPbkSendContactCmd.h"
#include "CPbkvCardConverter.h"
#include "CPbkAppGlobals.h"

#include <BCardEng.h>
#include <CPbkFieldInfo.h>
#include <phonebook.rsg>
#include <CPbkContactEngine.h>
#include <CPbkProgressNoteWrapper.h>
#include <CPbkConstants.h>
#include <CPbkContactItem.h>

#include <AknNoteWrappers.h>
#include <eikclb.h>         // CEikListBox
#include <sendui.h>         // Send UI API
#include <SendUiMtmUids.h>  // Send UI MTM uid's
#include <MsgBioUids.h>
#include <txtrich.h>        // CRichText
#include <barsread.h>       // TResourceReader
#include <CMessageData.h>
#include <PbkUID.h>
#include <MenuFilteringFlags.h>
#include <SendUiConsts.h>   // Postcard Uid
#include <MPbkCommandObserver.h>

#include <pbkdebug.h>

/// Unnamed namespace for local definitions
namespace {

const TUint KNoMenu = 0;

/**
 * Represents the different listbox index selections.
 */
enum TPbkListBoxSelections
    {
    EFirstSelection = 0,
    ESecondSelection,
    EThirdSelection
    };


// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
	EPanicPreCond_SendvCardsL = 1,
    EPanicLogic_CmdSendContactDataL,
    EPanicLogic_MapSelection
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSendContactCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

// ==================== LOCAL FUNCTIONS ====================

/**
 * Creates a rich text object and packages contents of a file to it. The file's
 * data is not converted in any way except that characters are widened to 16 
 * bits.
 *
 * @param aEikEnv   EIKON environment.
 * @param aFileName name of the file to convert.
 * @return  a new rich text object with file's contents. The returned object is
 *          also left on top of the cleanup stack.
 */
CRichText* CreateRichTextFromFileLC
    (CEikonEnv& aEikEnv, const TDesC& aFileName);

/**
 * Helper class for sending the vCard(s) in async callback.
 */
class CVCardSender : public CIdle
	{
	public:  // Constructors
		/*
		 * Creates a new instance of this object.
		 * @param aPriority desired priority
		 */
		static CVCardSender* NewL(TInt aPriority);

	private:  // from CIdle
		void RunL();
		TInt RunError(TInt aError);

	private:
		/*
		 * Constructor.
		 * @param aPriority desired priority
		 */
		CVCardSender(TInt aPriority);
	};


inline CVCardSender::CVCardSender(TInt aPriority) 
	: CIdle(aPriority) 
	{
	CActiveScheduler::Add(this);
	}

CVCardSender* CVCardSender::NewL(TInt aPriority)
	{
	return new(ELeave) CVCardSender(aPriority);
	}

void CVCardSender::RunL()
	{
	CIdle::RunL();
    // Destroy self. 
    // If RunL (the callback) leaves RunError will handle the deletion.
	delete this;
	}

TInt CVCardSender::RunError(TInt aError)
	{
	delete this;
    // Forward all errors to the active scheduler
	return aError;
	}



CRichText* CreateRichTextFromFileLC
        (CEikonEnv& aEikEnv, const TDesC& aFileName)
    {
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
    return richText;
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================
inline CPbkSendContactCmd::CPbkSendContactCmd(
        TPbkSendingParams aParams,
        CPbkContactEngine& aEngine,
        TContactItemId aContactId,
        const CContactIdArray* aContacts,
        TPbkContactItemField* aField
        ) :
            iEngine( aEngine ),            
            iParams( aParams ),            
			iContactId( aContactId ),
            iContacts( aContacts ),
            iField( aField )
	{  
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendContactCmd::CPbkSendContactCmd(0x%x)"), this);
    }

void CPbkSendContactCmd::ConstructL
        (CBCardEngine& aBCardEng)
    {
    iEikEnv = CEikonEnv::Static();
    iConverter = CPbkvCardConverter::NewL(iEikEnv->FsSession(),
		iEngine, aBCardEng);
	iVcardSender = CVCardSender::NewL(CActive::EPriorityIdle);
    iWaitNoteWrapper = CPbkProgressNoteWrapper::NewL();
    }

/**
 * Static constructor. 
 */
CPbkSendContactCmd* CPbkSendContactCmd::NewL
        (TPbkSendingParams aParams,
        CPbkContactEngine& aEngine, CBCardEngine& aBCardEng,
        TContactItemId aContactId,
        TPbkContactItemField* aField)
    {
    CPbkSendContactCmd* self = new(ELeave)
        CPbkSendContactCmd(aParams,aEngine, aContactId, NULL, aField);
    CleanupStack::PushL(self);
    self->ConstructL(aBCardEng);
    CleanupStack::Pop(); // self
    return self;
    }

/**
 * Static constructor (variation for multiple contacts).
 */
CPbkSendContactCmd* CPbkSendContactCmd::NewL
	    (TPbkSendingParams aParams,
	    CPbkContactEngine& aEngine, CBCardEngine& aBCardEng,
		const CContactIdArray& aContacts)
    {
    CPbkSendContactCmd* self = new(ELeave)
		CPbkSendContactCmd(aParams, aEngine, KNullContactId, &aContacts, NULL);
    CleanupStack::PushL(self);
    self->ConstructL(aBCardEng);
    CleanupStack::Pop(); // self
    return self;
    }

/**
 * Destructor.
 */
CPbkSendContactCmd::~CPbkSendContactCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendContactCmd::~CPbkSendContactCmd(0x%x)"), this);

    iUnderDestruction = ETrue;
    delete iWaitNoteWrapper;
    delete iConverter;
	delete iVcardSender;
    }

void CPbkSendContactCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendContactCmd::ExecuteLD(0x%x)"), this);

	CleanupStack::PushL(this);
	
	iMtmUid = ShowSendQueryL();
	
    if ( ( iContactId == KNullContactId && 
          ( !iContacts || iContacts->Count()==0 ) ) ||
         iMtmUid == KNullUid )
        {
        CleanupStack::PopAndDestroy();
        return;
        }
    
    TInt selectionIndex(ESendAllData);

    // Ask the user to select the contact data to be send, if needed
    selectionIndex = SelectSentDataL();

    if (selectionIndex != ECancel)
        {
	    if (iContactId != KNullContactId)
		    {
		    iConverter->ConvertContactL(iContactId, iField, selectionIndex);
		    }
	    else
		    {
            // iContacts validity is checked in function entry
		    iConverter->ConvertContactsL(*iContacts, selectionIndex);
		    }
	
        // Then send contact(s)
        CPbkWaitNoteWrapperBase::TNoteParams noteParams;
        noteParams.iObserver = this;
	    // ProcessFinished will be called when execution is finished.
        iWaitNoteWrapper->ExecuteL
            (*iConverter, R_QTN_SM_WAIT_BUSINESS_CARD, noteParams);
        
    	if ( iObserver )
            {
            iObserver->CommandFinished( *this );
            }

        CleanupStack::Pop(); // this

	    // The promised self destruction will happen in SendvCardsL,
	    // which is called by ProcessFinished
        }
    else
        {
        // User canceled the sending, exit
        CleanupStack::PopAndDestroy();
        }
    }


/**
 * Send the vCards from an idle callback to get standard active scheduler 
 * error handling. This is especially important to handle special leave
 * code KLeaveExit which is propagated by SendUi in case the 'Exit" is 
 *  picked from the message editor's menu.
 */
void CPbkSendContactCmd::ProcessFinished(MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSendContactCmd::ProcessFinished(0x%x)"), this);
        
    // Cancel before usage of active object.
    iVcardSender->Cancel();
    iVcardSender->Start(TCallBack(&CPbkSendContactCmd::SendvCardsLD, this));
    }

/**
 * Sends prepared vCard files using send UI.
 */
void CPbkSendContactCmd::SendvCardsLD()
    {
    // Relinquish ownership, iVcardSender takes care of it self
	iVcardSender = NULL;

	CleanupStack::PushL(this);
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );

    if (iConverter->FileNames().MdcaCount() > 0 &&
		!iUnderDestruction)
        {
		// Get globals (does not take ownership)
        CPbkAppGlobals* globals = CPbkAppGlobals::InstanceL();              

		if (iMtmUid == KSenduiMtmSmsUid)
			{
			// Sending through SMS -> there should be only one vCard
			// attachment. Package the attachment to a rich text object and
			// send it as the message body.
			__ASSERT_DEBUG(iConverter->FileNames().MdcaCount()==1, 
				Panic(EPanicPreCond_SendvCardsL));

			// Copy the one and only attachment into a rich text object
			CRichText* msgBody = CreateRichTextFromFileLC
				(*iEikEnv, iConverter->FileNames().MdcaPoint(0));

            messageData->SetBodyTextL( msgBody );

			// Send the message using Send Ui
			globals->SendUiL()->CreateAndSendMessageL(
				iMtmUid, messageData, KMsgBioUidVCard );

			CleanupStack::PopAndDestroy(msgBody);
			}
		else
			{
			// Not sending through SMS, just pass the attachments
			__ASSERT_DEBUG(iConverter->FileNames().MdcaCount()>=1, 
				Panic(EPanicPreCond_SendvCardsL));

            //Fill message data
            const TInt count( iConverter->FileNames().MdcaCount());
            for( TInt i( 0 ); i < count; ++i )
                {
                messageData->AppendAttachmentL( 
                        iConverter->FileNames().MdcaPoint( i ) );
                }

			// Send the message using Send Ui
			globals->SendUiL()->CreateAndSendMessageL(
				iMtmUid, messageData, KMsgBioUidVCard );            
			}
        }

    // Destroy itself as promised
	CleanupStack::PopAndDestroy(2); //this, messageData		
    }

TInt CPbkSendContactCmd::SendvCardsLD(TAny* aThis)
    {
	CPbkSendContactCmd* self = static_cast<CPbkSendContactCmd*>(aThis);
	self->SendvCardsLD();

    return EFalse;
    }


/**
 * If necessary, shows a popup selection list from which the
 * the user selects what details are sent in a vCard.
 * @return user selection mapped into TPbkChoiceItemEnumerations
 */

TInt CPbkSendContactCmd::SelectSentDataL()
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
		    (listBox, R_AVKON_SOFTKEYS_OK_CANCEL,
            AknPopupLayouts::EMenuGraphicWindow);
        CleanupStack::PushL(popupList);

        HBufC* headingText= CCoeEnv::Static()->AllocReadResourceLC
		    (R_PBK_BUSINESSCARD_SEND_HEADING);
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


/**
 * Decides what selection list to show the user.
 * @return resource id of the menu to show
 */
TInt CPbkSendContactCmd::SelectionListL() const
    {
    TInt ret(KNoMenu);
    TBool supportsFieldType(ETrue);

    // Check is the 'send selected fields' feature enabled
    TBool sendSelectedFeatureEnabled(iEngine.Constants()->
        LocallyVariatedFeatureEnabled(EPbkLVSendSelectedContactFields));

    // Check is the sending media SMS and does the contact
    // have a thumbnail
    TBool smsAndThumbnail(EFalse);
    if ((AnyThumbnailsL()) && (IsSmsMtmL()))
        {
        smsAndThumbnail = ETrue;
        }

    // If focused field is supplied, the command object was
    // launched from contact info view and that requires
    // we have to check is the field supported by vCard spec
    if (iField)
        {
        CBCardEngine& bcardEng = CPbkAppGlobals::InstanceL()->BCardEngL(iEngine);
        if (!bcardEng.SupportsFieldType(iField->FieldInfo().FieldId()))
            {
            supportsFieldType = EFalse;
            }
        }

    // Now check the cases when the menu needs to be shown
    if (sendSelectedFeatureEnabled)
        {
        // There are two main branches depending on which view
        // this command object was launched from
        if (iField)
            {
            // Command object was launched from contact info view.

            if (!smsAndThumbnail)
                {
                // When there is no thumbnail involved the selection
                // menu is shown only if we are over vCard supported
                // field
                if (supportsFieldType)
                    {
                    ret = R_PBK_CONTACTINFO_SEND_OPTIONS;
                    }
                }
            else
                {
                // Thumbnail is involved, the selection menu depends
                // on whether we are over vCard supported field or not
                if (supportsFieldType)
                    {
                    ret = R_PBK_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL;
                    }
                else
                    {
                    ret = R_PBK_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL_NO_FIELD;
                    }
                }
            }
        else
            {
            // Command object was launched from contact list view,
            // the menu is shown only in case there is thumbnail
            // involded and the sending media is SMS
            if (smsAndThumbnail)
                {
                ret = R_PHONEBOOK_SEND_OPTIONS;
                }
            }
        }
    else
        {
        // If the 'send selected fields' is disabled, the menu
        // needs to be shown only if the user is in contact info
        // view and over a vCard supported field
        if ((iField) && (supportsFieldType))
            {
            ret = R_PBK_CONTACTINFO_SEND_OPTIONS;
            }
        }
    return ret;
    }


/**
 * Maps selection index to choice item TPbkChoiceItemEnumerations.
 * @param aSelection goes in as a selection index made by user in
 *      the selection list, when exiting contains the selection value
 *      mapped to TPbkChoiceItemEnumerations
 * @param aShownMenu what menu was shown to the user (resource id)
 */
void CPbkSendContactCmd::MapSelection(TInt& aSelection,
        TInt aShownMenu)
    {
    switch (aShownMenu)
        {
        case R_PBK_CONTACTINFO_SEND_OPTIONS:
            //'send item data'
            //'send all data'
            {
            switch (aSelection)
                {
                case EFirstSelection:
                    {
                    aSelection = ESendCurrentItem;
                    break;
                    }

                case ESecondSelection:
                    {
                    aSelection = ESendAllData;
                    break;
                    }

                default:
                    {
                    __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
                    break;
                    }
                }
            break;
            }

        case R_PBK_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL:
            //'send item data'
            //'send detail without image'
            //'send detail with image'
            {
            switch (aSelection)
                {
                case EFirstSelection:
                    {
                    aSelection = ESendCurrentItem;
                    break;
                    }

                case ESecondSelection:
                    {
                    aSelection = ESendAllDataWithoutPicture;
                    break;
                    }

                case EThirdSelection:
                    {
                    aSelection = ESendAllData;
                    break;
                    }

                default:
                    {
                    __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
                    break;
                    }
                }
            break;
            }

        case R_PBK_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL_NO_FIELD:
            //'send detail without image'
            //'send detail with image'
            {
            switch (aSelection)
                {
                case EFirstSelection:
                    {
                    aSelection = ESendAllDataWithoutPicture;
                    break;
                    }

                case ESecondSelection:
                    {
                    aSelection = ESendAllData;
                    break;
                    }

                default:
                    {
                    __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
                    break;
                    }
                }
            break;
            }

        case R_PHONEBOOK_SEND_OPTIONS:
            //'send without image'
            //'send with image'
            {
            switch (aSelection)
                {
                case EFirstSelection:
                    {
                    aSelection = ESendAllDataWithoutPicture;
                    break;
                    }

                case ESecondSelection:
                    {
                    aSelection = ESendAllData;
                    break;
                    }

                default:
                    {
                    __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
                    break;
                    }
                }
            break;
            }
        
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_MapSelection));
            break;
            }
        }
    }


/**
 * Checks are there any thumbnails in the contact set.
 * @return ETrue if there was even one thumbnail, EFalse otherwise
 */
TBool CPbkSendContactCmd::AnyThumbnailsL() const
    {
    TBool ret(EFalse);
    if (iContactId != KNullContactId)
        {
        CPbkContactItem* contact = iEngine.ReadContactLC(iContactId);
        ret = HasThumbnail(*contact);
        CleanupStack::PopAndDestroy(contact);
        }
	else if (iContacts && iContacts->Count() > 0)
		{
        for (TInt i=0; i < iContacts->Count(); ++i)
            {
            CPbkContactItem* contact = iEngine.ReadContactLC((*iContacts)[i]);
            ret = HasThumbnail(*contact);
            CleanupStack::PopAndDestroy(contact);
            if (ret)
                {
                // We can exit the loop as soon as a thumbnail
                // is found
                break;
                }
            }
		}

    return ret;
    }


/**
 * Checks does aItem have a thumbnail.
 * @return ETrue if the contact has a thumbnail, EFalse otherwise
 */
TBool CPbkSendContactCmd::HasThumbnail(CPbkContactItem& aItem) const
    {
    const TPbkContactItemField* field =
        aItem.FindField(EPbkFieldIdThumbnailImage);
    return (field && !field->IsEmptyOrAllSpaces());
    }

/**
 * Checks is the selected sending media SMS.
 * @return ETrue if SMS is the sending media, EFalse otherwise
 */
TBool CPbkSendContactCmd::IsSmsMtmL() const
    {
    TBool ret(EFalse);
    if ( iMtmUid == KSenduiMtmSmsUid )
        {
        ret = ETrue;
        }
    return ret;
    }

TUid CPbkSendContactCmd::ShowSendQueryL()
    {
	return CPbkAppGlobals::InstanceL()->SendUiL()
		->ShowSendQueryL( NULL, iParams.iCapabilities, iParams.iMtmFilter );
    }    

void CPbkSendContactCmd::AddObserver( MPbkCommandObserver& aObserver )
    {
    iObserver = &aObserver;
    }


//  End of File  
