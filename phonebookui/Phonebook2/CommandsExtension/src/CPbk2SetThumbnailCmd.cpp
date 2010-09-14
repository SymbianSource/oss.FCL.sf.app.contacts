/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*           SetThumbnail command event handling class.
*
*/


// INCLUDE FILES
#include "CPbk2SetThumbnailCmd.h"

// Phonebook 2
#include "CPbk2ImageManager.h"
#include "CPbk2DrmManager.h"
#include "Pbk2DataCaging.hrh"
#include "Pbk2PresentationUtils.h"
#include <pbk2commands.rsg>
#include <pbk2commonui.rsg>
#include <MPbk2CommandObserver.h>
#include <RPbk2LocalizedResourceFile.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>

// System includes
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <MGFetch.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG

	enum TPanicCode
	    {
	    EPanicPreCond_Pbk2ThumbnailSetComplete,
	    EPanicPreCond_Pbk2ThumbnailSetFailed
	    };

	void Panic(TInt aReason)
	    {
	    _LIT(KPanicText, "CPbk2SetThumbnailCmd");
	    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace


// ========================= MEMBER FUNCTIONS ===============================

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::CPbk2SetThumbnailCmd
// --------------------------------------------------------------------------
//
CPbk2SetThumbnailCmd::CPbk2SetThumbnailCmd(
        MPbk2ContactUiControl& aUiControl ) :
	CPbk2ThumbnailCmdBase( aUiControl )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SetThumbnailCmd::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::ConstructL(0x%x), start"), this);
            
    CPbk2ThumbnailCmdBase::BaseConstructL();
    
    iDrmManager = CPbk2DrmManager::NewL();
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::ConstructL(0x%x), end"), this);    
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2SetThumbnailCmd* CPbk2SetThumbnailCmd::NewL(
        MPbk2ContactUiControl& aUiControl )
    {
    CPbk2SetThumbnailCmd* self = 
        new (ELeave) CPbk2SetThumbnailCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::~CPbk2SetThumbnailCmd
// --------------------------------------------------------------------------
//    
CPbk2SetThumbnailCmd::~CPbk2SetThumbnailCmd()
    {
    delete iDrmManager;
    delete iThumbOperation;
    delete iWaitNote;
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::ExecuteCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2SetThumbnailCmd::ExecuteCommandL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::ExecuteCommandL(0x%x), start"), this);

    TBool result = EFalse;
    Cancel();
    
    // fetch thumbnail from media gallery
    CDesCArray* selectedFile = new(ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( selectedFile );
	TBool res( EFalse );
	res = MGFetch::RunL( *selectedFile, EImageFile, EFalse, this );
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::ExecuteCommandL(%d), MGFetch res"), TInt(res));	
                
    if ( res && selectedFile->Count() > 0 )
        {
        CAknWaitDialog* waitNote = 
            new(ELeave) CAknWaitDialog( 
                reinterpret_cast<CEikDialog**>( &iWaitNote ) );
        waitNote->ExecuteLD( R_QTN_GEN_NOTE_FETCHING );
        CleanupStack::PushL( waitNote );

        TPtrC fileName = (*selectedFile)[0];

        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPbk2SetThumbnailCmd::ExecuteCommandL(%S), fileName"), &fileName);	        

        iThumbOperation = 
            iThumbnailManager->SetImageAsyncL(
                *iStoreContact, *iFieldType, *this, fileName );
                
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPbk2SetThumbnailCmd::ExecuteCommandL(0x%x), iThumbOperation"), 
                iThumbOperation);
            
        CleanupStack::Pop( waitNote );
        iWaitNote = waitNote;
        result = ETrue;
        }
    CleanupStack::PopAndDestroy( selectedFile );
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::ExecuteCommandL(0x%x), end"), this);    
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::Pbk2ImageSetComplete
// --------------------------------------------------------------------------
//    
void CPbk2SetThumbnailCmd::Pbk2ImageSetComplete(
        MPbk2ImageOperation& PBK2_DEBUG_ONLY( aOperation ) )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::Pbk2ImageSetComplete(0x%x), start"), this);
            
    __ASSERT_DEBUG(&aOperation == iThumbOperation, 
        Panic(EPanicPreCond_Pbk2ThumbnailSetComplete));

    delete iThumbOperation;
    iThumbOperation = NULL;
    
	DismissWaitNote();
	
	TRAPD( error, iStoreContact->CommitL( *this ) );
	if ( error != KErrNone )
	    {
	    CCoeEnv::Static()->HandleError( error );
	    }
		
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::Pbk2ImageSetComplete(0x%x), end"), this);	
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::Pbk2ImageSetFailed
// --------------------------------------------------------------------------
//
void CPbk2SetThumbnailCmd::Pbk2ImageSetFailed
        ( MPbk2ImageOperation& PBK2_DEBUG_ONLY( aOperation ), TInt aError )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::Pbk2ImageSetFailed(0x%x), aError"), this);
            
    __ASSERT_DEBUG(&aOperation == iThumbOperation, 
        Panic(EPanicPreCond_Pbk2ThumbnailSetFailed));

    delete iThumbOperation;
    iThumbOperation = NULL;

	DismissWaitNote();

	ProcessDismissed( aError );
	
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::Pbk2ImageSetFailed(0x%x), end"), this);	
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::DismissWaitNote
// --------------------------------------------------------------------------
//
void CPbk2SetThumbnailCmd::DismissWaitNote()
	{
    // Dismiss the wait note
	TRAPD( err, iWaitNote->ProcessFinishedL() );
	if ( err != KErrNone )
		{
		delete iWaitNote;
        iWaitNote = NULL;
		}	
	}

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::VerifySelectionL
// --------------------------------------------------------------------------
//	
TBool CPbk2SetThumbnailCmd::VerifySelectionL
        (const MDesCArray* aSelectedFiles)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::VerifySelectionL(0x%x), start"), this);
            
    TBool result = EFalse;
    if ( aSelectedFiles && aSelectedFiles->MdcaCount() > 0 )
        {
        // DRM for phonebook thumbnail fetch
        TPtrC fileName = aSelectedFiles->MdcaPoint( 0 );
        TBool IsProtected( ETrue );
        User::LeaveIfError( 
            iDrmManager->IsProtectedFile( fileName, IsProtected ) );
        if ( IsProtected )
            {
			RPbk2LocalizedResourceFile resFile( *CCoeEnv::Static() );
			resFile.OpenLC( 
			    KPbk2RomFileDrive, 
				KDC_RESOURCE_FILES_DIR, 
				Pbk2PresentationUtils::PresentationResourceFile() );
            // show user copyright note
            HBufC* prompt = 
                CCoeEnv::Static()->AllocReadResourceLC( R_PBK2_QTN_DRM_NOT_ALLOWED );
            CAknInformationNote* dlg = new(ELeave) CAknInformationNote( ETrue );
            dlg->ExecuteLD( *prompt );
            CleanupStack::PopAndDestroy( 2 ); // resFile, prompt
            }
        else
            {
            result = ETrue;
            }
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SetThumbnailCmd::VerifySelectionL(0x%x), end"), this);
        
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SetThumbnailCmd::Cancel
// --------------------------------------------------------------------------
//
void CPbk2SetThumbnailCmd::Cancel()
    {
    delete iWaitNote;
    iWaitNote = NULL;
    delete iThumbOperation;
    iThumbOperation = NULL;
    }

//  End of File  
