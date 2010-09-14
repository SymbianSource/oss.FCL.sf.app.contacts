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
* Description:  Phonebook 2 contact ringing tone assigner.
*
*/


#include "CPbk2ContactRingingToneAssigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"
#include <CPbk2DrmManager.h>
#include <pbk2serverapp.rsg>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <centralrepository.h>
#include <ProfileEngineDomainCRKeys.h> // KProEngRingingToneMaxSize
#include <f32file.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::CPbk2ContactRingingToneAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactRingingToneAssigner::CPbk2ContactRingingToneAssigner
        ( MPbk2ContactAssignerObserver& aObserver ):
            CActive( EPriorityIdle ), iObserver( aObserver ),
            iIndex( KErrNotSupported )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::~CPbk2ContactRingingToneAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactRingingToneAssigner::~CPbk2ContactRingingToneAssigner()
    {
    Cancel();
    delete iDrmManager;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactRingingToneAssigner* CPbk2ContactRingingToneAssigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver )
    {
    CPbk2ContactRingingToneAssigner* self =
        new ( ELeave ) CPbk2ContactRingingToneAssigner( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::ConstructL()
    {
    iDrmManager = CPbk2DrmManager::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::AssignDataL
        ( MVPbkStoreContact& aStoreContact,
          MVPbkStoreContactField* aContactField,
          const MVPbkFieldType* aFieldType, const HBufC* aDataBuffer )
    {
    TBool isProtected( ETrue );
    TInt error = PassesDrmCheckL( aDataBuffer, isProtected );
    
    if ( !isProtected && error == KErrNone && PassesSizeCheckL( aDataBuffer ) )
        {
        if ( !aContactField )
            {
            MVPbkStoreContactField* field =
                aStoreContact.CreateFieldLC( *aFieldType );
            InsertDataL( *field, *aDataBuffer );
            CleanupStack::Pop(); // field
            iIndex = aStoreContact.AddFieldL( field ); // takes ownership
            }
        else
            {
            InsertDataL( *aContactField, *aDataBuffer );
            }

        iState = EStopping;
        // Notify observer asynchronously
        IssueRequest();
        }
    else
        {
        // Notify observer asynchronously
        iState = EDrmProtected;
        iError = error;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::AssignAttributeL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          TPbk2AttributeAssignData /*aAttributeAssignData*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::RunL()
    {
    switch ( iState )
        {
        case EStopping:
            {
            if ( iError == KErrNone )
                {
                // Just notify the observer
                iObserver.AssignComplete( *this, iIndex );
                }
            else
                {
                iObserver.AssignFailed( *this, iError );
                }
            break;
            }
        case EDrmProtected:
            {
            iObserver.AssignFailed( *this, iError );
            break;
            }            
        default:
            {
            // Do nothing
            break;
            }
        }        
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactRingingToneAssigner::RunError( TInt /*aError*/ )
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::InsertDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::InsertDataL
        ( MVPbkStoreContactField& aField, const HBufC& aDataBuffer )
    {
    MVPbkContactFieldTextData::Cast( aField.FieldData() ).
        SetTextL( aDataBuffer );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::PassesDrmCheckL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactRingingToneAssigner::PassesDrmCheckL( 
        const HBufC* aDataBuffer,
        TBool& aIsProtected )
    {
    aIsProtected = ETrue;
    TInt error( KErrNone );

    if ( aDataBuffer )
        {
        error = iDrmManager->IsRingingToneForbidden( *aDataBuffer, aIsProtected );
        }

    return error;
    }    

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::PassesSizeCheckL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactRingingToneAssigner::PassesSizeCheckL
        ( const HBufC* aDataBuffer )
    {
    TBool dataInvalid = EFalse;

    if ( aDataBuffer )
        {
        TInt sizeLimit = KErrNone;
        MaxToneFileSizeL( sizeLimit );
        TInt rtSizeLimitError = CheckFileSizeLimit
            ( *aDataBuffer, sizeLimit );
        if ( rtSizeLimitError )
            {
            dataInvalid = ETrue;
            }
        }

    return !dataInvalid;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::MaxToneFileSizeL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::MaxToneFileSizeL
        ( TInt& aMaxSizeKB ) const
    {
    CRepository* cenrep = CRepository::NewL( KCRUidProfileEngine );
    CleanupStack::PushL( cenrep );
    TInt error = cenrep->Get( KProEngRingingToneMaxSize, aMaxSizeKB );
    CleanupStack::PopAndDestroy( cenrep );
    if ( error != KErrNone )
        {
        aMaxSizeKB = 0;
        }
    if ( aMaxSizeKB < 0 )
        {
        aMaxSizeKB = 0;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::CheckFileSizeLimitL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactRingingToneAssigner::CheckFileSizeLimit
        ( const TDesC& aFileName, TInt aSizeLimit )
    {
    TInt error = KErrNone;

    if ( aSizeLimit )
        {
        if ( CheckToneFileSize( aFileName, aSizeLimit) != KErrNone )
            {
            TRAP_IGNORE(
                ShowSizeErrorNoteL( aSizeLimit ) );
            error = KErrTooBig;
            }
        }

    return error;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::CheckToneFileSizeL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactRingingToneAssigner::CheckToneFileSize
        ( const TDesC& aFile, TInt aSizeLimitKB )
    {
    // Get file size
    TInt size = 0;
    TInt error = KErrNone;
    RFs fs;
    error = fs.Connect();
    if ( !error )
        {
        TEntry entry;
        error = fs.Entry( aFile, entry );
        if ( !error )
            {
            size = entry.iSize;
            }
        fs.Close();
        }

    // Note: if file size can't be determined, the check fails
    aSizeLimitKB *= KKilo;
    if ( aSizeLimitKB  &&  size > aSizeLimitKB )
        {
        error = KErrTooBig;
        }

    return error;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRingingToneAssigner::ShowSizeErrorNoteL
// --------------------------------------------------------------------------
//
void CPbk2ContactRingingToneAssigner::ShowSizeErrorNoteL
        ( TInt aSizeLimitKB ) const
    {
    HBufC* errorText = StringLoader::LoadLC
        ( R_PROFILE_TEXT_TONE_MAXSIZE_ERROR, aSizeLimitKB );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// End of File
