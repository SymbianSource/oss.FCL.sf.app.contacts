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
* Description:  Phonebook 2 MMC UI extension copy from MMC command.
*
*/


#include "cpmucopyfrommmccmd.h"

// Phonebook2
#include <mpbk2contactuicontrol.h>
#include <mpbk2commandobserver.h>
#include <pbk2processdecoratorfactory.h>
#include <mpbk2processdecorator.h>
#include <cpbk2storeconfiguration.h>
#include <tpbk2copycontactsresults.h>
#include <mpbk2contactnameformatter.h>
#include <cpbk2storeproperty.h>
#include <cpbk2storepropertyarray.h>
#include <mpbk2storevalidityinformer.h>
#include <mpbk2appui.h>
#include <mpbk2applicationservices.h>
#include <pbk2mmcuires.rsg>

// Virtual Phonebook
#include <mvpbkcontactoperationbase.h>
#include <mvpbkstorecontact.h>
#include <mvpbkcontactstorelist.h>
#include <cvpbkvcardeng.h>
#include <cvpbkcontactmanager.h>
#include <tvpbkcontactstoreuriptr.h>
#include <mvpbkcontactstore.h>
#include <mvpbkcontactlinkarray.h>
#include <vpbkcontactstoreuris.h>
#include <mvpbkcontactstoreproperties.h>
#include <cvpbkcontactstoreuriarray.h>

// System includes
#include <pathinfo.h>
#include <stringloader.h>
#include <aknnotewrappers.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KZero = 0;
const TInt KOneContact = 1;

enum TPmuCopyToMmcCmdState
    {
    EPmuCopyFromMmcCmdPrepare,
    EPmuCopyFromMmcCmdRun,
    EPmuCopyFromMmcCmdComplete    
    }; 

#ifdef _DEBUG

enum TPanicCode
    {
    EExecuteLD_PreCond = 1,
    EPanic_CopyNextL_OOB,
    EPanic_Wrong_State
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPmuCopyFromMmcCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/**
 * Shows store not available note.
 *
 * @param aStoreProperties      Store properties.
 */
void ShowStoreNotAvailableNoteL( CPbk2StorePropertyArray& aStoreProperties )
    {
    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfig );
    const TDesC& uri = storeConfig->DefaultSavingStoreL().UriDes();

    // Get the property of the default saving store
    const CPbk2StoreProperty* prop =
        aStoreProperties.FindProperty( uri );
    if ( prop && prop->StoreName().Length() > 0 )
        {
        HBufC* text = StringLoader::LoadLC( R_QTN_PHOB_STORE_NOT_AVAILABLE,
            prop->StoreName() );
        CAknInformationNote* note = new(ELeave) CAknInformationNote;
        // Show "not available" note
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        }

    CleanupStack::PopAndDestroy( storeConfig );
    }

/**
 * Inspects is the store valid.
 *
 * @param aInfomer      Store validity informer.
 * @param aStore        Store to inspect.
 * @return  ETrue if the store is valid.s
 */
TBool IsValidStoreL( MPbk2StoreValidityInformer& aInformer,
        MVPbkContactStore* aStore )
    {
    TBool isValid = EFalse;

    if ( aStore )
        {
        CVPbkContactStoreUriArray* currentlyValidStores =
            aInformer.CurrentlyValidStoresL();
        TVPbkContactStoreUriPtr uri =
            aStore->StoreProperties().Uri();
        isValid = currentlyValidStores->IsIncluded( uri );
        delete currentlyValidStores;
        }
    return isValid;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::CPmuCopyFromMmcCmd
// --------------------------------------------------------------------------
//
CPmuCopyFromMmcCmd::CPmuCopyFromMmcCmd( MPbk2ContactUiControl& aUiControl ) :
        CActive( EPriorityStandard ),
        iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::~CPmuCopyFromMmcCmd
// --------------------------------------------------------------------------
//
CPmuCopyFromMmcCmd::~CPmuCopyFromMmcCmd()
    {
    Cancel();

    iReadStream.Close();
    delete iDir;
    delete iDecorator;
    delete iImportOperation;
    delete iVCardEngine;
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::NewL
// --------------------------------------------------------------------------
//
CPmuCopyFromMmcCmd* CPmuCopyFromMmcCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPmuCopyFromMmcCmd* self =
        new ( ELeave ) CPmuCopyFromMmcCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPmuCopyFromMmcCmd::ConstructL()
    {
    iVCardEngine = CVPbkVCardEng::NewL
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
        ( R_PMU_COPY_PROGRESS_NOTE, EFalse );

    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfig );
    iTargetStore = Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().ContactStoresL().Find(
                storeConfig->DefaultSavingStoreL() );
            
    CleanupStack::PopAndDestroy( storeConfig );
    
    // set the default contacts path
    iContactsPath = PathInfo::MemoryCardContactsPath();
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::RunL
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::RunL()
    {    
    switch ( iState )
        {
        case EPmuCopyFromMmcCmdPrepare:
            {
            if ( iUiControl )
                {
                // Blank UI control to get performance improvement
                iUiControl->SetBlank( ETrue );
                }

            iState = EPmuCopyFromMmcCmdRun;
            IssueRequest();
            break;
            }

        case EPmuCopyFromMmcCmdRun:
            {
            if ( iDir && iCurrentContactIndex < iDir->Count() )
                {
                CopyNextL();
                }
            else
                {
                // decorator calls processdismissed
                iDecorator->ProcessStopped();
                }
            break;
            }
        case EPmuCopyFromMmcCmdComplete:
            {
            CommandCompletedL();
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanic_Wrong_State ) );
            break;
            }
        };
    
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::DoCancel()
    {
    delete iImportOperation;
    iImportOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPmuCopyFromMmcCmd::RunError( TInt /*aError*/ )
    {
    delete iImportOperation;
    iImportOperation = NULL;
    
    // decorator calls processdismissed
    iDecorator->ProcessStopped();
    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::ExecuteLD()
    {
    __ASSERT_DEBUG( iCommandObserver, Panic( EExecuteLD_PreCond ) );

    CleanupStack::PushL( this );

    if ( !iTargetStore ||
          !IsValidStoreL( Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreValidityInformer(), iTargetStore ) )
        {
        // if target store not available finish command
        ShowStoreNotAvailableNoteL
            ( Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties() );
        CleanupStack::Pop(this);
        iCommandObserver->CommandFinished(*this);
        return;
        }

    Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
        FsSession().GetDir(
            iContactsPath,
            KEntryAttNormal | KEntryAttHidden | KEntryAttSystem,
            ESortNone,
            iDir );

    if ( iDir )
        {
        iDecorator->ProcessStartedL( iDir->Count() );
        iDecorator->SetObserver( *this );
        iCurrentContactIndex = 0;
        iCountOfContacts = 0;

        iState = EPmuCopyFromMmcCmdPrepare;
        IssueRequest();
        }
    else
        {
        ShowResultsL();
        iCommandObserver->CommandFinished( *this );
        }

    CleanupStack::Pop(this);
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl->SetBlank( EFalse );
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::FieldAddedToContact
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::FieldAddedToContact
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    // This command does not operate on contact field level
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::FieldAddingFailed
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::FieldAddingFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    // This command does not operate on contact field level
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ContactsSaved
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::ContactsSaved
        ( MVPbkContactOperationBase& aOperation,
          MVPbkContactLinkArray* aResults )
    {
    if ( &aOperation == iImportOperation )
        {
        ++iCountOfContacts;
        
        delete iImportOperation;
        iImportOperation = NULL;
        iReadStream.Close();
        IssueRequest();
        delete aResults;
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ContactsSavingFailed
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::ContactsSavingFailed
        ( MVPbkContactOperationBase& aOperation, TInt /*aError*/ )
    {
    if ( &aOperation == iImportOperation )
        {
        delete iImportOperation;
        iImportOperation = NULL;
        iReadStream.Close();
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {        
    Cancel();

    iState = EPmuCopyFromMmcCmdComplete;
    IssueRequest();
    }


// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::CopyNextL
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::CopyNextL()
    {
    __ASSERT_DEBUG( iDir->Count() > iCurrentContactIndex,
        EPanic_CopyNextL_OOB );

    TParse parse;
    parse.Set(( *iDir )[iCurrentContactIndex].iName,
        &iContactsPath,
        NULL );

    User::LeaveIfError( 
        iReadStream.Open( Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().FsSession(), parse.FullName(), EFileRead ) );
    iImportOperation =
        iVCardEngine->ImportVCardL( *iTargetStore, iReadStream, *this );
    ++iCurrentContactIndex;
    iDecorator->ProcessAdvance( 1 );              
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();        
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ShowResultsL
// --------------------------------------------------------------------------
//
void CPmuCopyFromMmcCmd::ShowResultsL()
    {    
    if ( iDir )
        {
        const TInt contactCount = iDir->Count();
        if ( contactCount == KOneContact )
            {
            TParse parse;
            parse.Set( ( *iDir )[0].iName,
                &iContactsPath,
                NULL );

            TPbk2CopyContactsResults results
                ( iCountOfContacts, parse.Name() );
            // If default saving store is not phone memory, show different
            // after copy note
            if ( !IsDefaultStorePhoneMemoryL() )
                {
                results.SetOneContactCopiedTextRes
                    ( R_QTN_PBCOP_NOTE_CONTACT_COPIED_PB2 );
                }
            results.ShowNoteL();
            }
        else
            {
            TPbk2CopyContactsResults results(
                iCountOfContacts, iDir->Count() );
            // If default saving store is not phone memory, show different
            // after copy note
            if ( !IsDefaultStorePhoneMemoryL() )
                {
                results.SetMultipleContactsCopiedTextRes
                    ( R_QTN_PBCOP_NOTE_N_ENTRY_COPY_PB );
                }
            results.ShowNoteL();
            }
        }
    else
        {
        // The directory did not found, so the card migth be formatted.
        // Show "0 copied" note
        TPbk2CopyContactsResults results(
            iCountOfContacts, KZero );
        // If default saving store is not phone memory, show different
        // after copy note
        if ( !IsDefaultStorePhoneMemoryL() )
            {
            results.SetMultipleContactsCopiedTextRes
                ( R_QTN_PBCOP_NOTE_N_ENTRY_COPY_PB );
            }
        results.ShowNoteL();
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::IsDefaultStorePhoneMemoryL
// --------------------------------------------------------------------------
//
TBool CPmuCopyFromMmcCmd::IsDefaultStorePhoneMemoryL() const
    {
    TBool ret = EFalse;

    const TVPbkContactStoreUriPtr uri =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().DefaultSavingStoreL();

    TVPbkContactStoreUriPtr phoneMemoryUri
        ( VPbkContactStoreUris::DefaultCntDbUri() );

    if ( uri.Compare( phoneMemoryUri,
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType ) == 0 )
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::CommandCompletedL
// --------------------------------------------------------------------------
//    
void CPmuCopyFromMmcCmd::CommandCompletedL()
    {
    if (iUiControl)
        {
        iUiControl->SetBlank( EFalse );
        }

    ShowResultsL();
    
    if (iUiControl)
        {
        iUiControl->UpdateAfterCommandExecution();
        }
    
    if ( iCommandObserver )    
        {        
        iCommandObserver->CommandFinished( *this );
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyFromMmcCmd::ShowMemorySelectionDialogL
// --------------------------------------------------------------------------
//
TBool CPmuCopyFromMmcCmd::ShowMemorySelectionDialogL()
    {
    return EFalse;
    }

// End of File
