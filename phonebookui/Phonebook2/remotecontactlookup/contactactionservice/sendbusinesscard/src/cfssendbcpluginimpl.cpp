/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CFscSendBusinessCardPluginImpl.
 *
*/


#include "emailtrace.h"
#include <implementationproxy.h>
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknsConstants.h>
#include <AknWaitDialog.h>

// System includes
#include <txtrich.h>
#include <barsread.h>
#include <sendui.h>
#include <TSendingCapabilities.h>
#include <MsgBioUids.h>
#include <SendUiConsts.h>
#include <eikenv.h>
#include <CMessageData.h>
#include <eikclb.h>
#include <aknPopup.h>
#include <featmgr.h>

//Rsgs
#include <pbk2rclsendbusinesscardpluginimpl.rsg>
#include <pbk2rclsendbusinesscardplugin.mbg>

//pbk2
#include <MVPbkStoreContact.h>
#include <CPbk2StoreConfiguration.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <e32cmn.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkVCardEng.h>
#include "CPbk2AttachmentFile.h"
#include <Pbk2ContactNameFormatterFactory.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>

#include "mfsccontactset.h"
#include "mfscactionutils.h"
#include "cfsccontactaction.h"
#include "fsccontactactionservicedefines.h"
#include "mfsccontactactionpluginobserver.h"
#include "fsccontactactionserviceuids.hrh"
#include "fscactionpluginactionuids.h"
#include "fscactionplugincrkeys.h"
#include "cfscsendbcpluginimpl.h"
#include "FscActionPluginUtils.h"
#include "fssendcmdutils.h"
#include "cfsvcardconverter.h"

const TImplementationProxy ImplementationTable[] =
        {
        IMPLEMENTATION_PROXY_ENTRY(
                KFscSendBusinessCardPluginImplDllUid,
                CFscSendBusinessCardPluginImpl::NewL )
        };

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_SendvCardsL = 1,    
    EPanicLogic_MapSelection,
    EPanicLogic_ConversionDone,
    EPanicPreCond_ErrArgument
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CFscSendBusinessCardPluginImpl");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

// CONSTANTS DECLARATIONS
const TInt KGranularity( 4 );
const TInt KActionCount = 1;
const TInt KInvalidReasonId = 0;
const TUint KNoMenu = 0;

_LIT( KSendBusinessCardPluginIconPath,
        "\\resource\\apps\\pbk2rclsendbusinesscardplugin.mif" );
_LIT( KResourceFilePath, "\\resource\\apps\\pbk2rclsendbusinesscardpluginimpl.rsc" );


struct TFSSelectionMapping
    {
    /// Maps to first selection
    TFSSendvCardChoiceItemEnumerations iStSelection;
    /// Maps to second selection
    TFSSendvCardChoiceItemEnumerations iNdSelection;
    /// Maps to third selection
    TFSSendvCardChoiceItemEnumerations iRdSelection;
    //// Resource id
    TInt iResId;
    };

enum TSelection
    {
    ESelectionLast = -1
    };

/// Maps listbox index selections to 'sending selected detail'
/// menu selections If command not supported selection is ECancel.
static const TFSSelectionMapping SelectionMapping[] =
    {
    {ESendCurrentItem, ESendAllData, ECancel,
        R_FSSENDBC_CONTACTINFO_SEND_OPTIONS},
    {ESendCurrentItem, ESendAllDataWithoutPicture, ESendAllData,
        R_FSSENDBC_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL},
    {ESendAllDataWithoutPicture, ESendAllData, ECancel,
        R_FSSENDBC_CONTACTINFO_SEND_OPTIONS_SMS_THUMBNAIL_NO_FIELD},
    {ESendAllDataWithoutPicture, ESendAllData, ECancel,
        R_FSSENDBC_SEND_OPTIONS},
    {ECancel, ECancel, ECancel, ESelectionLast}
    };

// ======== MEMBER FUNCTIONS ========

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

}  /// namespace

/**
 * Sending parameters.
 */
class TFSSendingParams
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aMtmFilter    MTM filter
         * @param aCapabilities Sending capabilities.
         */
        TFSSendingParams(
                CArrayFixFlat<TUid>* aMtmFilter,
                TSendingCapabilities aCapabilities );

    public: // Data
        /// Ref: MTM filter
        CArrayFixFlat<TUid>* iMtmFilter;
        /// Own: Sending capabilities
        TSendingCapabilities iCapabilities;
    };

// --------------------------------------------------------------------------
// TFSSendingParams::TFSSendingParams
// --------------------------------------------------------------------------
//
inline TFSSendingParams::TFSSendingParams
            ( CArrayFixFlat<TUid>* aMtmFilter,
              TSendingCapabilities aCapabilities ) :
                iMtmFilter( aMtmFilter ), iCapabilities( aCapabilities )
    {
    }



// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount )
    {
    aTableCount =
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// ======== MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CFscSendBusinessCardPluginImpl* CFscSendBusinessCardPluginImpl::NewL(
        TAny* aParams)
    {
    FUNC_LOG;

    TFscContactActionPluginParams* params =
            reinterpret_cast< TFscContactActionPluginParams* >(aParams );

    CFscSendBusinessCardPluginImpl* self = new (ELeave) CFscSendBusinessCardPluginImpl( *params );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::~CFscSendBusinessCardPluginImpl
// ---------------------------------------------------------------------------
//
CFscSendBusinessCardPluginImpl::~CFscSendBusinessCardPluginImpl()
    {
    FUNC_LOG; 
    delete iConverter;  
    delete iNameFormatter;
    delete iSortOrderManager;
    delete iSendUi;
    delete iVCardSender;
    delete iVCardEngine;
    iStoreContacts.Close();
    delete iSendBusinessCardContactAction;
    delete iActionList;
    iCoeEnv.DeleteResourceFile(iResourceHandle);
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::Uid
// ---------------------------------------------------------------------------
//
TUid CFscSendBusinessCardPluginImpl::Uid() const
    {
    FUNC_LOG;
    TUid uid =
        {
        KFscSendBusinessCardPluginImplDllUid
        };
    return uid;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ActionList
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>* CFscSendBusinessCardPluginImpl::ActionList() const
    {
    return iActionList;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::GetActionL
// ---------------------------------------------------------------------------
//
const MFscContactAction& CFscSendBusinessCardPluginImpl::GetActionL(
        TUid aActionUid) const
    {
    FUNC_LOG;

    const MFscContactAction* action= NULL;
    if (aActionUid == KFscSendBusinessCard)
        {
        action = iSendBusinessCardContactAction;
        }
    else
        {
        User::Leave(KErrNotFound);
        }

    return *action;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::PriorityForContactSetL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::PriorityForContactSetL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TFscContactActionVisibility& aActionMenuVisibility,
        TFscContactActionVisibility& aOptionsMenuVisibility,
        MFscContactActionPluginObserver* aObserver)
    {
    FUNC_LOG;
    iActionUid = aActionUid;
    iContactSet = &aContactSet;
    iActionMenuVisibility = &aActionMenuVisibility;
    iOptionsMenuVisibility = &aOptionsMenuVisibility;

    iIsExecute = EFalse;

    iContactSet->SetToFirstContact();

    iActionPriority = KFscActionPriorityNotAvailable;

    iCanDisplay = KErrNone;

    if (iActionUid == KFscSendBusinessCard)
        {
        // We save only one contact and no groups:
        iCanDisplay = (aContactSet.GroupCount() == 0
                && aContactSet.ContactCount() == 1 ) ? KErrNone : KErrArgument;

        //Fix for EJKA-7MFDHU
        if (iCanDisplay == KErrNone)
            {
            aActionMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionVisible;
            aOptionsMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionVisible;

            iActionPriority = iParams.iUtils->ActionPriority( 
                                KFscCrUidSendBusinesscard,
                                KFscActionPrioritySendBusinesscard );
        
            }
        else
            {
            iActionPriority = KFscActionPriorityNotAvailable;
            aActionMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionHidden;
            aActionMenuVisibility.iReasonId = KInvalidReasonId;
            aOptionsMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionHidden;
            aOptionsMenuVisibility.iReasonId = KInvalidReasonId;
            }
        ResetData();
        aObserver->PriorityForContactSetComplete(iActionPriority);
        }
    else
        {
        ResetData();
        aObserver->PriorityForContactSetFailed(KErrArgument);
        }
    }
// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::CancelPriorityForContactSet 
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::CancelPriorityForContactSet()
    {
    FUNC_LOG;
    iLastEvent = EActionEventCanceled;
    ResetData();
    }
// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ExecuteL 
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginObserver* aObserver)
    {
    FUNC_LOG;
    switch (iLastEvent)
        {
        case EActionEventIdle:
            {
            iPluginObserver = aObserver;
            iMtmUid = ShowSendQueryL();
            if (iMtmUid != KNullUid)
                {   
                iLastEvent = EActionEventCanExecuteLaunched;
                
                iActionUid = aActionUid;
                iContactSet = &aContactSet;
                
                iIsExecute = ETrue;
                
                iContactSet->SetToFirstContact();
                
                CanExecuteL(aActionUid, aContactSet);
                }
            else
                {
                ProcessDone( KErrCancel );
                }

            break;
            }

        case EActionEventCanExecuteFinished:
            {
            
            if (iCanDisplay == KErrNone)
                {
                iLastEvent = EActionEventContactRetrieve;
                iPluginObserver = aObserver;
                aContactSet.NextContactL(this);
                }
            else
                {
                ProcessDone( KErrGeneral );                
                }
            break;
            }            
        case EActionEventCanceled:
            {
            iLastEvent = EActionEventIdle;
            break;
            }
        default:
            {
            ResetData();
            break; 
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::CancelExecute
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::CancelExecute()
    {
    FUNC_LOG;
    
    iLastEvent = EActionEventCanceled;
    ResetData();
    }
// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::GetReasonL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::GetReasonL(
        TUid /* aActionUid */,
        TInt /* aReasonId */,
        HBufC*& /* aReason */) const
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::NextContactComplete
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::NextContactComplete(
        MVPbkStoreContact* aContact)
    {
    FUNC_LOG;
    TInt err( KErrNone );
    switch (iLastEvent)
        {
        case EActionEventContactRetrieve:
            {
            iStoreContacts.Append(aContact);
            iRetrievedStoreContact = aContact;
            iLastEvent = EActionEventExecuteLaunched;        
            TRAP (err, PrepareStoreL() );
            break;
            }
        case EActionEventCanceled:
            {
            iLastEvent = EActionEventIdle;
            break;
            }
        default:
            {
            ResetData();
            break; 
            }
        }
    if ( err != KErrNone )
        {
        iCoeEnv.HandleError( err );
        }
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::NextContactFailed
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::NextContactFailed(
        TInt aError)
    {
    FUNC_LOG;
    ProcessDone( aError );    
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::NextGroupComplete
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::NextGroupComplete(
        MVPbkStoreContact* /*aContact*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::NextGroupFailed
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::NextGroupFailed(
        TInt /*aError*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::GetGroupContactComplete(
        MVPbkStoreContact* /*aContact*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::GetGroupContactFailed
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::GetGroupContactFailed(
        TInt /*aError*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::IsValidStoreL
// ---------------------------------------------------------------------------
//
TBool CFscSendBusinessCardPluginImpl::IsValidStoreL()
    {
    FUNC_LOG;
    TBool isValid = EFalse;
    
    //Currently method is checking if saving store is cntdb
    //if saving store is SIM, contact can't be saved
    //(SIM store is not supported yet)
    
    CPbk2StoreConfiguration* storeCnf = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(storeCnf);
    TVPbkContactStoreUriPtr defaultStorePtr = 
        storeCnf->DefaultSavingStoreL();
    TVPbkContactStoreUriPtr validStorePtr( 
            VPbkContactStoreUris::DefaultCntDbUri() 
            );
    
    if ( !defaultStorePtr.Compare( 
            validStorePtr.UriDes(), 
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) 
            )
        {
        isValid = ETrue;
        }
    
    CleanupStack::PopAndDestroy(storeCnf);
    return isValid;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::PrepareStoreL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::PrepareStoreL()
    {
    FUNC_LOG;
    CPbk2StoreConfiguration* storeCnf = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(storeCnf);
    TVPbkContactStoreUriPtr storePtr = storeCnf->DefaultSavingStoreL();
    CVPbkContactManager& contactManager = iParams.iUtils->ContactManager();
    contactManager.LoadContactStoreL( storePtr );            
    iTargetStore = contactManager.ContactStoresL().
                        Find( storePtr );
    if ( iTargetStore )
        {
        iTargetStore->OpenL( *this );
        }
    else //store not found
        {
        ProcessDone( KErrNotFound );        
        }
    CleanupStack::PopAndDestroy(storeCnf);
    }


// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::StoreReady
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::StoreReady( 
        MVPbkContactStore& aContactStore )
    {
    FUNC_LOG;
    iTargetStore = &aContactStore;
    TRAPD( err, SendBusinessCardL() );
    if ( err != KErrNone )
        {
        ProcessDone( err );        
        }
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    FUNC_LOG;
    ProcessDone( KErrArgument );    
    }
        
// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::HandleStoreEventL
// ---------------------------------------------------------------------------
//       
void CFscSendBusinessCardPluginImpl::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    FUNC_LOG;
    // Do nothing
    }


// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::SendBusinessCardL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::SendBusinessCardL()
    {
    FUNC_LOG;
    TInt selection(ESendAllData);
    if ( iStoreContacts.Count() == 1 )
        {
        // If there is only one contact or launched in info view
        // do the data selection
        selection = SelectSentDataL();
        }
    ConvertContactL(selection);    
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::CanExecuteL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::CanExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet)
    {
    FUNC_LOG;

    if (aActionUid == KFscSendBusinessCard)
        {
        iCanDisplay = (aContactSet.GroupCount() == 0
                && aContactSet.ContactCount() == 1 )
                                                     ? KErrNone
                                                     : KErrArgument;
        iLastEvent = EActionEventCanExecuteFinished;
        ResumeAsync(aActionUid, aContactSet, iIsExecute);
        }
    else
        {
        //Just in case
        ProcessDone( KErrArgument );        
        }

    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::UpdateActionIconL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::UpdateActionIconL(
        TUid aActionUid)
    {
    FUNC_LOG;

    TFileName dllFileName;
    Dll::FileName(dllFileName);
    TParse parse;
    User::LeaveIfError(parse.Set(
            KSendBusinessCardPluginIconPath, &dllFileName, 
            NULL) );

    if (aActionUid == KFscSendBusinessCard)
        {
        iSendBusinessCardContactAction->SetIcon(iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsRclActionSave, parse.FullName(),
                EMbmPbk2rclsendbusinesscardpluginQgn_prop_cmail_action_save,
                EMbmPbk2rclsendbusinesscardpluginQgn_prop_cmail_action_save_mask) );
        }

    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::CFscSendBusinessCardPluginImpl
// ---------------------------------------------------------------------------
//
CFscSendBusinessCardPluginImpl::CFscSendBusinessCardPluginImpl(
        const TFscContactActionPluginParams& aParams) :
    CFscContactActionPlugin(), iParams(aParams), iResourceHandle( 0),
            iLastEvent(EActionEventIdle), iCanDisplay(KErrNone),
            iIsExecute(EFalse),            
            iCoeEnv( *CCoeEnv::Static() )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ConstructL()
    {
    FUNC_LOG;
    
    TFileName dllFileName;
    Dll::FileName(dllFileName);
    TParse parse;
    User::LeaveIfError(parse.Set(
            KResourceFilePath, &dllFileName, NULL) );
    TFileName resourceFileName(parse.FullName() );
    BaflUtils::NearestLanguageFile(
            iCoeEnv.FsSession(), resourceFileName);

    iResourceHandle = iCoeEnv.AddResourceFileL(resourceFileName);

    iActionList = new( ELeave )CArrayFixFlat<TUid>( KActionCount );
    iActionList->AppendL(KFscSendBusinessCard);

    User::LeaveIfError(parse.Set(
            KSendBusinessCardPluginIconPath, &dllFileName, NULL) );
    iSendBusinessCardContactAction = iParams.iUtils->CreateActionL(
            *this, KFscSendBusinessCard, KFscAtSendBusinessCard, 
            R_FS_ACTION_SEND_BUSINSESS_CARD, iParams.iUtils->SkinIconL(
                    KAknsIIDQgnFsRclActionSave, parse.FullName(),
                    EMbmPbk2rclsendbusinesscardpluginQgn_prop_cmail_action_save,
                    EMbmPbk2rclsendbusinesscardpluginQgn_prop_cmail_action_save_mask) );
    CVPbkContactManager& contactManager( iParams.iUtils->ContactManager() );
    iVCardSender = CVCardSender::NewL(CActive::EPriorityIdle);
    iSendUi = CSendUi::NewL();
    iVCardEngine = CVPbkVCardEng::NewL( contactManager );
    
    // Create sort order manager
    iSortOrderManager =
            CPbk2SortOrderManager::NewL( contactManager.FieldTypes() );
    
    // Create name formatter
    iNameFormatter =
            Pbk2ContactNameFormatterFactory::CreateL(
                    contactManager.FieldTypes(), *iSortOrderManager );
        
    iConverter = CFscvCardConverter::NewL(
            CEikonEnv::Static()->FsSession(),
            contactManager,
            *iVCardEngine,
            *iNameFormatter );
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ResetData
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ResetData()
    {
    FUNC_LOG;
    if (iContactSet)
        {
        iContactSet->SetToFirstContact();
        iContactSet->SetToFirstGroup();
        TRAP_IGNORE( iContactSet->CancelNextContactL() );
        TRAP_IGNORE( iContactSet->CancelNextGroupL() );
        iContactSet = NULL;
        }
    

    iLastEvent = EActionEventIdle;
    iActionMenuVisibility = NULL;
    iOptionsMenuVisibility = NULL;
    iCanDisplay = KErrNone;
    iRetrievedStoreContact = NULL;
    iStoreContacts.Close();
    }

// ---------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ResumeAsync
// ---------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ResumeAsync(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TBool aIsExecute )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    if ( aIsExecute )
        {
        TRAP( err, ExecuteL( aActionUid, aContactSet, iPluginObserver ) );
        }
    else
        {
        TRAP( err, PriorityForContactSetL(aActionUid, aContactSet, 
                *iActionMenuVisibility, *iOptionsMenuVisibility, 
                iPluginObserver ) );
        }
    
    if ( err != KErrNone )
        {
        ResetData();
        if ( aIsExecute )
            {
            ProcessDone( err );            
            }
        else
            {
            iPluginObserver->PriorityForContactSetFailed( err );
            }
        }
    
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ShowSendQueryL
// --------------------------------------------------------------------------
//
TUid CFscSendBusinessCardPluginImpl::ShowSendQueryL()
    {
    /// Sending  params
    TFSSendingParams params = CreateParamsLC();

    TUid uid = iSendUi->ShowSendQueryL( NULL, params.iCapabilities, params.iMtmFilter );

    CleanupStack::PopAndDestroy(); // mtmfilter
    return uid;
    }
// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::SendvCardsLD
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::SendvCardsLD()
    {
    
    // Relinquish ownership, iVCardSender takes care of it self
    iVCardSender = NULL;
    iVCardSender = CVCardSender::NewL(CActive::EPriorityIdle);

    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );

    if (iConverter->FileNames().MdcaCount() > 0)
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
            
            // Sending through SMS -> there should be only one vCard
            // attachment. Package the attachment to a rich text object and
            // send it as the message body.
            __ASSERT_DEBUG(iConverter->FileNames().MdcaCount()==1,
                Panic(EPanicPreCond_SendvCardsL));

            // Copy the one and only attachment into a rich text object
            CRichText* msgBody = CreateRichTextFromFileLC
                (*CEikonEnv::Static(), iConverter->FileNames().MdcaPoint(0));

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
        
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::SendvCardsLD
// --------------------------------------------------------------------------
//
TInt CFscSendBusinessCardPluginImpl::SendvCardsLD(TAny* aThis)
    {
    __ASSERT_DEBUG( aThis, Panic(EPanicPreCond_ErrArgument) );

    CFscSendBusinessCardPluginImpl* self = static_cast<CFscSendBusinessCardPluginImpl*>(aThis);
    self->SendvCardsLD();

    return EFalse;
    }


// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::CreateParamsLC
// --------------------------------------------------------------------------
//
TFSSendingParams CFscSendBusinessCardPluginImpl::CreateParamsLC()
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
    return TFSSendingParams( mtmFilter, capabilities );
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ConvertContactL
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ConvertContactL(TInt aSelectionIndex)
    {
    if (aSelectionIndex != ECancel)
        {
        /*
        if ( iFocusedField )
            {
            iConverter->ConvertContactL(
                iStoreContacts.Array(), iFocusedField,
                aSelectionIndex, *this);
            }
        else*/
            {
            // iContacts validity is checked in function entry
            iConverter->ConvertContactsL(
                iStoreContacts.Array(), aSelectionIndex, *this);
            }
        }
    }


// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::SendMsgUsingSendUI
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::SendMsgUsingSendUI(CMessageData* aMsgData)
    {    
    // Send the message using Send Ui
    TRAPD(error, iSendUi->CreateAndSendMessageL( iMtmUid, aMsgData, KMsgBioUidVCard));

    if (error != KErrNone)
        {
        FilterErrors(error);
        }
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::IsMoreThanOneContact
// --------------------------------------------------------------------------
//
TBool CFscSendBusinessCardPluginImpl::IsMoreThanOneContact()
    {
    TBool result = EFalse;
    if ( iStoreContacts.Count() > 1 )
        {
        result = ETrue;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::FilterErrors
// --------------------------------------------------------------------------
//
TInt CFscSendBusinessCardPluginImpl::FilterErrors(TInt aErrorCode)
    {
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
            ProcessDone(aErrorCode);
            break;
            }
        }   

    return result;
    }


// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ProcessDone
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ProcessDone(TInt aErrCode)
    {
    ResetData(); 
    if ( KErrNone == aErrCode )
        {
        iPluginObserver->ExecuteComplete();
        }
    else
        {
        iPluginObserver->ExecuteFailed(aErrCode);
        }
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ConversionDone
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ConversionDone( TInt aCount )
    {
    __ASSERT_DEBUG( iStoreContacts.Count() == aCount,
        Panic( EPanicLogic_ConversionDone ) );

    iVCardSender->Cancel();
    iVCardSender->Start( TCallBack( &CFscSendBusinessCardPluginImpl::SendvCardsLD,
        this ) );
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::ConversionError
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::ConversionError(TInt aError)
    {
    ProcessDone( aError );
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::SelectSentDataL
// --------------------------------------------------------------------------
//
TInt CFscSendBusinessCardPluginImpl::SelectSentDataL()
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

        HBufC* headingText= iCoeEnv.AllocReadResourceLC
            (R_FSSENDBC_BUSINESSCARD_SEND_HEADING);
        popupList->SetTitleL(*headingText);
        CleanupStack::PopAndDestroy(); // headingText

        // Init list box
        listBox->SetContainerWindowL(*popupList);

        TResourceReader resReader;
        iCoeEnv.CreateResourceReaderLC(resReader, resourceId);
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
// CFscSendBusinessCardPluginImpl::SelectionListL
// --------------------------------------------------------------------------
//
TInt CFscSendBusinessCardPluginImpl::SelectionListL() const
    {
    TInt ret( KNoMenu );

    // Show the query if contact has a thumbnail.
    if ( AnyThumbnailsL() )
        {
        ret = R_FSSENDBC_SEND_OPTIONS;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CFscSendBusinessCardPluginImpl::MapSelection
// --------------------------------------------------------------------------
//
void CFscSendBusinessCardPluginImpl::MapSelection(TInt& aSelection, TInt aShownMenu)
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
// CFscSendBusinessCardPluginImpl::AnyThumbnailsL
// --------------------------------------------------------------------------
//
TBool CFscSendBusinessCardPluginImpl::AnyThumbnailsL() const
    {
    TInt cntCount(iStoreContacts.Count());

    for (TInt i=0; i < cntCount; ++i)
        {
        MVPbkStoreContact* contact = (iStoreContacts)[i];
        const MVPbkBaseContactField* field =
                FsSendCmdUtils::FindFieldL(
                *contact,
                R_FSSENDBC_THUMBNAIL_FIELD_TYPE,
                iParams.iUtils->ContactManager().FieldTypes());

        if (field)
            {
            // We can exit the loop as soon as a thumbnail
            // is found
            return ETrue;
            }
        }
    return EFalse;
    }


//  End of File
