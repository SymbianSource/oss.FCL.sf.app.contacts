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
* Description:  Phonebook 2 MMC UI extension copy to MMC command.
*
*/


#include "cpmucopytommccmd.h"

// Phonebook 2
#include <mpbk2contactuicontrol.h>
#include <mpbk2applicationservices.h>
#include <mpbk2appui.h>
#include <mpbk2contactnameformatter.h>
#include <mpbk2commandobserver.h>
#include <pbk2processdecoratorfactory.h>
#include <mpbk2processdecorator.h>
#include <tpbk2copycontactsresults.h>
#include <pbk2mmcuires.rsg>

// Virtual Phonebook
#include <mvpbkcontactoperationbase.h>
#include <mvpbkstorecontact.h>
#include <cvpbkvcardeng.h>
#include <cvpbkcontactmanager.h>
#include <cvpbkcontactlinkarray.h>

// System includes
#include <pathinfo.h>
#include <stringloader.h>
#include <aknnotewrappers.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KPbk2VCardFileExtension, ".vcf" );
_LIT( KInvalidFileNameChars, "?*<>/\"|\\:" );
_LIT( KNumberFormat, "%d" );
_LIT( KOpeningParenthesis, "(" );
_LIT( KClosingParenthesis, ")" );
_LIT( KZero, "0" );
const TInt KOneContact = 1;

/**
 * Removes invalid characters from given descriptor.
 *
 * @param aPtr  The descriptor to inspect.
 */
void RemoveInvalidCharacters( TPtr& aPtr )
    {
    for ( TInt i = aPtr.Length() - 1; i >= 0; --i )
        {
        TChar ch = aPtr[i];
        if ( KInvalidFileNameChars().Locate( ch ) != KErrNotFound )
            {
            aPtr.Delete( i, 1 ); // ch is not a valid character
            }
        }
    }

enum TPmuCopyToMmcCmdState
    {
    EPmuCopyToMmcCmdStartProcess,
    EPmuCopyToMmcCmdDeleteOldEntries,
    EPmuCopyToMmcCmdRun,
    EPmuCopyToMmcCmdComplete,
    EPmuCopyToMmcCmdCancel
    };

#ifdef _DEBUG

enum TPanicCode
    {
    EPanic_CopyNextL_OOB = 1,
    EPanic_OperationComplete_PreCond,
    EPanic_ShowResultsL_PreCond
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPmuCopyToMmcCmd" );
    User::Panic( KPanicCat(), aPanic );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::CPmuCopyToMmcCmd
// --------------------------------------------------------------------------
//
CPmuCopyToMmcCmd::CPmuCopyToMmcCmd( MPbk2ContactUiControl& aUiControl ) :
        CActive( EPriorityStandard ),
        iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::~CPmuCopyToMmcCmd
// --------------------------------------------------------------------------
//
CPmuCopyToMmcCmd::~CPmuCopyToMmcCmd()
    {
    Cancel();

    if ( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    iWriteStream.Close();
    delete iFileMan;
    delete iDecorator;
    delete iStoreContact;
    delete iPreviousContact;
    delete iContactLinks;
    delete iVCardEngine;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::NewL
// --------------------------------------------------------------------------
//
CPmuCopyToMmcCmd* CPmuCopyToMmcCmd::NewL( MPbk2ContactUiControl& aUiControl )
    {
    CPmuCopyToMmcCmd* self = new ( ELeave ) CPmuCopyToMmcCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPmuCopyToMmcCmd::ConstructL()
    {
    iVCardEngine = CVPbkVCardEng::NewL
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );

    iContactLinks = iUiControl->SelectedContactsOrFocusedContactL();

    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
        ( R_PMU_COPY_PROGRESS_NOTE, ETrue );
    iDecorator->SetObserver( *this );

    iUiControl->RegisterCommand( this );

    // set the default contacts path
    iContactsPath = PathInfo::MemoryCardContactsPath();
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::RunL
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::RunL()
    {
    switch( iState )
        {
        case EPmuCopyToMmcCmdStartProcess:
            {
            iDecorator->ProcessStartedL(iContactLinks->Count());

            if ( iDeleteOldEntries )
                {
                iState = EPmuCopyToMmcCmdDeleteOldEntries;
                IssueRequest();
                }
            else
                {
                Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
                    FsSession().MkDirAll( iContactsPath );

                iState = EPmuCopyToMmcCmdRun;
                IssueRequest();
                }
            break;
            }
        case EPmuCopyToMmcCmdDeleteOldEntries:
            {
            DeleteOldEntriesL();
            Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
                FsSession().MkDirAll( iContactsPath );
            break;
            }
        case EPmuCopyToMmcCmdRun:
            {
            if (iFileMan)
                {
                delete iFileMan;
                iFileMan = NULL;
                }

            if (iStoreContact)
                {
                HBufC* fileName = CreateFileNameLC(*iStoreContact);
                User::LeaveIfError( 
                    iWriteStream.Create(
                        Phonebook2::Pbk2AppUi()->ApplicationServices().
                            ContactManager().FsSession(),
                                *fileName, EFileWrite ));
                    
                iExportOperation = iVCardEngine->ExportVCardForSyncL(
                    iWriteStream, *iStoreContact, *this );
                CleanupStack::PopAndDestroy(fileName);
                }
            else if (iCurrentIndex < iContactLinks->Count())
                {
                CopyNextL();
                }
            else
                {
                iState = EPmuCopyToMmcCmdComplete;
                IssueRequest();
                }
            break;
            }
        case EPmuCopyToMmcCmdComplete:
            {
            // Copy complete, decorator calls processdismissed
            iDecorator->ProcessStopped();
            break;
            }
        default:
            {
            }
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::DoCancel()
    {
    iState = EPmuCopyToMmcCmdCancel;
    
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    delete iExportOperation;
    iExportOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPmuCopyToMmcCmd::RunError( TInt /*aError*/ )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    delete iExportOperation;
    iExportOperation = NULL;

    if ( iDecorator )
        {
        iDecorator->ProcessStopped();
        }

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::ExecuteLD()
    {
    CleanupStack::PushL( this );

    ShowConfirmationQueryL();

    CleanupStack::Pop( this );
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    Cancel();
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    delete iExportOperation;
    iExportOperation = NULL;

    // It is a not big deal if result note is not shown to user
    TRAP_IGNORE( ShowResultsL() );
    if ( iUiControl )
        {
        iUiControl->UpdateAfterCommandExecution();
        }

    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
          MVPbkStoreContact* aContact )
    {
    __ASSERT_DEBUG( iDecorator, Panic( EPanic_OperationComplete_PreCond ));

    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        iStoreContact = aContact;
        }
    else if ( &aOperation == iExportOperation )
        {
        delete iExportOperation;
        iExportOperation = NULL;

        // After the copying take the contact pointer,
        // because if this was last contact we need it in result note
        delete iPreviousContact;
        iPreviousContact = iStoreContact; // iPreviousContact takes ownership
        iStoreContact = NULL;

        iWriteStream.Close();

        // Copy next and update progressbar
        ++iCurrentIndex;
        ++iCountOfContacts;
        iDecorator->ProcessAdvance( 1 );
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& aOperation, TInt /*aError*/ )
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        }
    else if (&aOperation == iExportOperation)
        {
        delete iExportOperation;
        iExportOperation = NULL;
        delete iStoreContact;
        iStoreContact = NULL;
        }

    // Copy next
    ++iCurrentIndex;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::NotifyFileManStarted
// --------------------------------------------------------------------------
//    
MFileManObserver::TControl CPmuCopyToMmcCmd::NotifyFileManStarted()
    {
    if ( iState == EPmuCopyToMmcCmdCancel )
        {
        return MFileManObserver::EAbort;
        }
    return MFileManObserver::EContinue;
    }
    
// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::NotifyFileManOperation
// --------------------------------------------------------------------------
//
MFileManObserver::TControl CPmuCopyToMmcCmd::NotifyFileManOperation()
    {
    if ( iState == EPmuCopyToMmcCmdCancel )
        {
        return MFileManObserver::EAbort;
        }
    return MFileManObserver::EContinue;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::CopyNextL
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::CopyNextL()
    {
    __ASSERT_DEBUG( iContactLinks->Count() > iCurrentIndex,
         EPanic_CopyNextL_OOB );
    iRetrieveOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().RetrieveContactL
                ( iContactLinks->At( iCurrentIndex ), *this );
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::CreateFileNameLC
// --------------------------------------------------------------------------
//
HBufC* CPmuCopyToMmcCmd::CreateFileNameLC
        ( const MVPbkStoreContact& aContact ) const
    {
    HBufC* title = Phonebook2::Pbk2AppUi()->ApplicationServices().
        NameFormatter().GetContactTitleL( aContact.Fields(), 0 );

    CleanupStack::PushL(title);
    TPtr titlePtr = title->Des();

    RemoveInvalidCharacters(titlePtr);

    TBool fileNameFound = EFalse;
    TInt number = 0;
    TFileName fileName;
    while (!fileNameFound)
        {
        fileName.Zero();
        fileName.Append( iContactsPath );
        fileName.Append(titlePtr);

        if (number > 0)
            {
            fileName.Append(KOpeningParenthesis);
            if (number < 10)
                {
                fileName.Append(KZero);
                }
            fileName.AppendFormat(KNumberFormat, number);
            fileName.Append(KClosingParenthesis);
            }

        fileName.Append(KPbk2VCardFileExtension);

        TEntry entry;
        TInt err = Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().FsSession().Entry(fileName, entry);
        if (err == KErrNotFound)
            {
            fileNameFound = ETrue;
            }
        else
            {
            User::LeaveIfError( err );
            ++number;
            }
        }
    CleanupStack::PopAndDestroy(title);

    return fileName.AllocLC();
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::DeleteOldEntriesL
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::DeleteOldEntriesL()
    {
    iFileMan = CFileMan::NewL
        (Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            FsSession(), this);
    User::LeaveIfError( iFileMan->Delete(
            iContactsPath,
            CFileMan::ERecurse, iStatus ) );
    iState = EPmuCopyToMmcCmdRun;
    SetActive();
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ShowConfirmationQueryL
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::ShowConfirmationQueryL()
    {
    HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_QUERY_COPY_MMC_ALL);
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();

    TInt result = dlg->ExecuteLD
        (R_PHONEBOOK2_CONFIRM_DELETE_EXISTING_CONTACTS, *prompt);
    CleanupStack::PopAndDestroy(prompt);
    iDeleteOldEntries = (result != 0);

    iState = EPmuCopyToMmcCmdStartProcess;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ShowMemorySelectionDialogL
// --------------------------------------------------------------------------
//
TBool CPmuCopyToMmcCmd::ShowMemorySelectionDialogL()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPmuCopyToMmcCmd::ShowResultsL
// --------------------------------------------------------------------------
//
void CPmuCopyToMmcCmd::ShowResultsL()
    {
    const TInt contactCount = iContactLinks->Count();
    if (contactCount == KOneContact)
        {
        HBufC* title = NULL;

        // If we do not have iPreviousContact, then probably coping is failed
        if ( iPreviousContact )
            {
            title = Phonebook2::Pbk2AppUi()->ApplicationServices().
                NameFormatter().GetContactTitleL( iPreviousContact->Fields(),
                    MPbk2ContactNameFormatter::EPreserveLeadingSpaces);
            CleanupStack::PushL(title);
            }
        else if ( iStoreContact )
            {
            // If we still have iStoreContact, then retrieve is atleast
            // completed and we can show note.
            title = Phonebook2::Pbk2AppUi()->ApplicationServices().
                NameFormatter().GetContactTitleL( iStoreContact->Fields(),
                MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
            CleanupStack::PushL(title);
            }

        if ( title )
            {
            TPbk2CopyContactsResults results( iCountOfContacts, *title );
            results.SetOneContactCopiedTextRes
                ( R_QTN_PBCOP_NOTE_CONTACT_COPIED_PB2 );
            results.SetOneContactNotCopiedTextRes
                ( R_QTN_PBCOP_NOTE_ENTRY_NOT_COPIED );
            results.ShowNoteL();

            CleanupStack::PopAndDestroy( title );
            }
        }
    else
        {
        TPbk2CopyContactsResults results(
            iCountOfContacts, iContactLinks->Count());
        results.ShowNoteL();
        }
    }

// End of File
