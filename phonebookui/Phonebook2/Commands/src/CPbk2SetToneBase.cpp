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
* Description:  Phonebook 2 set ringing tone command base.
*
*/


// INCLUDES
#include "CPbk2SetToneBase.h"

// Phonebook 2
#include <CPbk2RingingToneFetch.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <MVPbkContactFieldTextData.h>
#include <Pbk2Commands.rsg>
#include <Pbk2UIControls.rsg>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>

// VPbk includes
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkFieldType.h>
#include <VPbkEng.rsg>

// System includes
#include <barsread.h>
#include <coemain.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement = 0;
    
#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_NULL_Pointer = 1
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPbk2SetToneBase" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG    

} /// namespace

// --------------------------------------------------------------------------
// CPbk2SetToneBase::CPbk2SetToneBase
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2SetToneBase::CPbk2SetToneBase( 
        MPbk2ContactUiControl& aUiControl ):
    CActive( CActive::EPriorityIdle ), 
    iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::~CPbk2SetToneBase
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2SetToneBase::~CPbk2SetToneBase()
    {
    Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    delete iToneFetch;
    delete iContactRelocator;
    delete iSelectedContacts;
    delete iRetrieveOperation;
    delete iStoreContact;
    }


// --------------------------------------------------------------------------
// CPbk2SetToneBase::BaseConstructL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::BaseConstructL()
    {
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL( 
            CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );
    iToneFetch = CPbk2RingingToneFetch::NewL();
    iUiControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::RunL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::RunL()
    {
    switch( iState )
        {
        case EFetchingRingingTone:
            {
            TBool retval =  EFalse;
            retval = FetchRingingToneL();
            if ( retval )
                {
                // User has selected a ringing tone
                iState = ERetrievingContact;
                IssueRequest();
                }
            else
                {
                // User has pressed cancel
                IssueStopRequest();
                }
            break;
            }

        case ERetrievingContact:
            {
            if (iUiControl)
                {
                iSelectedContacts = 
                    iUiControl->SelectedContactsOrFocusedContactL();
                    
                if ( iSelectedContacts )
                    {
                    const MVPbkContactLink& link =
                    iSelectedContacts->At( KFirstElement );
                    RetrieveContactL( link );
                    }
                else
                    {
                    ProcessDismissed( KErrNotFound );    
                    }
                }
            else
                {
                // dismiss process
                ProcessDismissed( KErrNotFound );
                }
            break;
            }

        case EStopping:
            {
            ProcessDismissed( KErrNone );
            break;
            }

        default:;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::DoCancel
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::DoCancel()
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::RunError
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2SetToneBase::RunError( 
        TInt aError )
    {
	TInt ret = aError;

	if ( aError != KLeaveExit )
	    {
	    ProcessDismissed( aError );
	    
	    // ProcessDismissed handled errors
	    ret = KErrNone;
	    }
	
	return ret;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::ExecuteLD()
    {
    // Start the command process
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::ResetUiControl
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == & aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::AddObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::AddObserver(
        MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::ProcessDismissed
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::ProcessDismissed( 
        TInt aCancelCode )
    {
    __ASSERT_DEBUG( iCommandObserver, Panic(EPanicPreCond_NULL_Pointer));
    
    if ( aCancelCode != KErrCancel && iUiControl)
        {
        // Don't remove list markings if user canceled
        iUiControl->SetBlank( EFalse );
        iUiControl->UpdateAfterCommandExecution();
        }

    // Notify command owner that the command has finished
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::RetrieveContactL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::RetrieveContactL
        ( const MVPbkContactLink& aLink )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    iRetrieveOperation =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            RetrieveContactL( aLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::RelocateContactL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2SetToneBase::RelocateContactL
        ( MVPbkStoreContact* aContact )
    {
    TBool ret = EFalse;

    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    if ( !iContactRelocator->IsPhoneMemoryContact( *aContact ) &&
        iContactRelocator->IsPhoneMemoryInConfigurationL() )
        {
        // Asynchronously relocate contact
        iContactRelocator->RelocateContactL( aContact, *this,
            Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery,
            CPbk2ContactRelocator::EPbk2RelocatorExistingContact );
        // relocator has taken the ownership
        aContact = NULL;
        ret = ETrue;        // Indicate that contact needs relocating
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::SetToneAndCommitL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::SetToneAndCommitL
        ( MVPbkStoreContact* aContact )
    {
    MVPbkStoreContactField* field = NULL;
    // First check if the field is available already
    TInt fieldIndex = KErrNotFound;
    TPbk2StoreContactAnalyzer analyzer(
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
        aContact );
    fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_RINGTONE_SELECTOR );

    if ( fieldIndex != KErrNotFound)
        {
        // Check if the iToneFilename is set
        if (iToneFilename.Length() <= 0)
            {
            // If filename is not set and the contact has the field
            // field can be removed.
            aContact->RemoveField( fieldIndex );
            }
        else
            {
            // If filename has been set and the contact has the field
            // field should be updated.
            field = &aContact->Fields().FieldAt( fieldIndex );
            MVPbkContactFieldTextData::Cast( field->FieldData() ).
                SetTextL( iToneFilename );
            }
        }
    else
        {
        // Contact does not have the field, so create new one.
        if ( iToneFilename.Length() > 0 )
            {
            if ( !iFieldType )
                {
                ReadFieldTypeL();
                }
            // iToneFilename is set
            field = aContact->CreateFieldLC( *iFieldType );
            MVPbkContactFieldTextData::Cast( field->FieldData() ).
                SetTextL( iToneFilename );
            aContact->AddFieldL( field );
            CleanupStack::Pop(); // field
            }
        }

    aContact->CommitL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::IssueRequest
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::IssueStopRequest
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SetToneBase::IssueStopRequest()
    {
    iState = EStopping;
    if (!IsActive())
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::FetchRingingToneL
// --------------------------------------------------------------------------
//
TBool CPbk2SetToneBase::FetchRingingToneL()
    {
    TBool retval = EFalse;
    RetrieveExistingRingingToneFilenameL( iToneFilename );
    if ( iToneFetch->FetchRingingToneL( iToneFilename ) )
        {
        retval = ETrue;
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::RetrieveExistingRingingToneFilenameL
// --------------------------------------------------------------------------
//
void CPbk2SetToneBase::RetrieveExistingRingingToneFilenameL( 
        TFileName& aFilename )
    {
    if (iUiControl)
        {
        const MVPbkStoreContact* contact = iUiControl->FocusedStoreContact();
        if ( contact )
            {
            TInt fieldCount = contact->Fields().FieldCount();
            for ( TInt i = 0; i < fieldCount; ++i )
                {
                const MVPbkStoreContactField& field =
                    contact->Fields().FieldAt( i );
                const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
                if ( fieldType &&  ( fieldType->FieldTypeResId()
                        == R_VPBK_FIELD_TYPE_RINGTONE ) )
                    {
                    aFilename.Copy
                        ( MVPbkContactFieldTextData::Cast( field.FieldData() ).
                            Text() );
                    break;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SetToneBase::ReadFieldType
// --------------------------------------------------------------------------
//
void CPbk2SetToneBase::ReadFieldTypeL()
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, R_RTONE_FIELD_TYPE );
    const TInt count = reader.ReadInt16();

    TVPbkFieldTypeMapping mapping(reader);
    iFieldType = mapping.FindMatch
        (Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            FieldTypes());
    if (!iFieldType)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PopAndDestroy(); // CreateResourceReaderLC
    }

// End of File
