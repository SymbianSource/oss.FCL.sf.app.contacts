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


#include "CPmuCopyFromMmcCmd.h"

// Phonebook2
#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <MPbk2ProcessDecorator.h>
#include <CPbk2StoreConfiguration.h>
#include <TPbk2CopyContactsResults.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2StoreValidityInformer.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2MmcUIRes.rsg>

// Virtual Phonebook
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkVCardEng.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactLinkArray.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreInfo.h>

// System includes
#include <pathinfo.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

#include <AknCommonDialogsDynMem.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <driveinfo.h>

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
 * @param aInformer     Store validity informer.
 * @param aStore        Store to inspect.
 * @return  ETrue if the store is valid.s
 */
TBool IsValidStoreL
        ( MPbk2StoreValidityInformer& aInformer, MVPbkContactStore* aStore )
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
        iUiControl( &aUiControl ),
        iCheckDuplicates( ETrue )
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
    Release( iAppServices );
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
    iAppServices = CPbk2ApplicationServices::InstanceL();
    
    iVCardEngine = CVPbkVCardEng::NewL
        ( iAppServices->ContactManager() );
    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
        ( R_PMU_COPY_PROGRESS_NOTE, EFalse );

    
    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfig );
    iTargetStore = iAppServices->ContactManager().ContactStoresL().Find(
            storeConfig->DefaultSavingStoreL() );
    CleanupStack::PopAndDestroy( storeConfig );
    iCheckDuplicates = ( iTargetStore->StoreInfo().NumberOfContactsL() > 0 );

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
          !IsValidStoreL( iAppServices->StoreValidityInformer(), iTargetStore ) )
        {
        // if target store not available finish command
        ShowStoreNotAvailableNoteL
            ( iAppServices->StoreProperties() );
        iCommandObserver->CommandFinished(*this);
        }
    else
        {
        if ( !ShowMemorySelectionDialogL() )
            {
            iCommandObserver->CommandFinished(*this);
            }
        else
            {
            iAppServices->ContactManager().
                FsSession().GetDir(
                    iContactsPath,
                    KEntryAttNormal | KEntryAttMatchMask,
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
            }
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
        ( MVPbkContactOperationBase& aOperation, TInt aError )
    {
    // Stop copying if the disk is full
    if ( aError == KErrDiskFull )
        {
        RunError( aError );
        }
    else if ( &aOperation == iImportOperation )
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

    delete iImportOperation;
    iImportOperation = NULL;
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
        Panic( EPanic_CopyNextL_OOB ) );

    TParse parse;
    parse.Set(( *iDir )[iCurrentContactIndex].iName,
        &iContactsPath,
        NULL );

    User::LeaveIfError( 
        iReadStream.Open( iAppServices->ContactManager().FsSession(),
                parse.FullName(), EFileRead ) );
    
    // Duplicate checking is one of the major performance bottlenecks in
    // contact importing. ImportVCardForSyncL ignores the duplicate check so
    // it is used if the target store is empty before importing.
    if ( iCheckDuplicates )
        {
        iImportOperation =
                iVCardEngine->ImportVCardL( *iTargetStore, iReadStream, *this );
        }
    else
        {
        iImportOperation =
            iVCardEngine->ImportVCardForSyncL( *iTargetStore, iReadStream, *this );
        }
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
            iAppServices->StoreConfiguration().DefaultSavingStoreL();

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
    TBool driveSelected( EFalse );

    TDriveNumber selectedMem;
    CAknMemorySelectionDialogMultiDrive* dlg =
        CAknMemorySelectionDialogMultiDrive::NewL(
            ECFDDialogTypeSelect,
            R_PHONEBOOK2_MEMORY_SELECTION_DIALOG,
            EFalse,
            AknCommonDialogsDynMem::EMemoryTypeMMCExternal |
            AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage );
    CleanupStack::PushL( dlg );
    driveSelected = dlg->ExecuteL( selectedMem );
    CleanupStack::PopAndDestroy(); // dlg

    // If user didn't cancel the selection
    if ( driveSelected )
        {
        TInt err = PathInfo::GetFullPath
            ( iContactsPath, selectedMem,
              PathInfo::EMemoryCardContactsPath );
        User::LeaveIfError( err );
        }

    return driveSelected;
}
// End of File
