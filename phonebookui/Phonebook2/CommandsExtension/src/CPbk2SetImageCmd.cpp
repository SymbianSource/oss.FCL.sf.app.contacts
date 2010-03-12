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
*           Set image command event handling class.
*
*/


// INCLUDE FILES
#include "CPbk2SetImageCmd.h"

// Phonebook 2
#include "CPbk2ImageManager.h"
#include "CPbk2DrmManager.h"
#include "Pbk2DataCaging.hrh"
#include "Pbk2PresentationUtils.h"
#include <Pbk2Commands.rsg>
#include <Pbk2CommonUi.rsg>
#include <MPbk2CommandObserver.h>
#include <RPbk2LocalizedResourceFile.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <MGFetch.h>
#include <aknnavi.h> 
#include <aknnavide.h> 

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG

	enum TPanicCode
	    {
	    EPanicPreCond_Pbk2ImageSetComplete,
	    EPanicPreCond_Pbk2ImageSetFailed,
	    EPanicPreCond_SetImageRefL
	    };

	void Panic(TInt aReason)
	    {
	    _LIT(KPanicText, "CPbk2SetImageCmd");
	    User::Panic(KPanicText, aReason);
        }
#endif  // _DEBUG

} /// namespace


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::CPbk2SetImageCmd
// --------------------------------------------------------------------------
//
CPbk2SetImageCmd::CPbk2SetImageCmd(
        MPbk2ContactUiControl& aUiControl ) :
	CPbk2ImageCmdBase( aUiControl )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SetImageCmd::ConstructL()
    {
    CPbk2ImageCmdBase::BaseConstructL();
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );
    iDrmManager = CPbk2DrmManager::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2SetImageCmd* CPbk2SetImageCmd::NewL(
        MPbk2ContactUiControl& aUiControl )
    {
    CPbk2SetImageCmd* self = 
            new (ELeave) CPbk2SetImageCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::~CPbk2SetImageCmd
// --------------------------------------------------------------------------
//    
CPbk2SetImageCmd::~CPbk2SetImageCmd()
    {
    delete iDrmManager;
    delete iImageOperation;
    delete iWaitNote;
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::ExecuteCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2SetImageCmd::ExecuteCommandL()
    {
    TBool result = EFalse;
    Cancel();
    
    // fetch image from media gallery
    CDesCArray* selectedFile = new(ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( selectedFile );
    TBool res( EFalse );
    res = MGFetchL( *selectedFile, EImageFile, EFalse, this );
    
    if ( res && selectedFile->Count() > 0 )
        {
        iWaitNote = new(ELeave) CAknWaitDialog( 
                reinterpret_cast<CEikDialog**>( &iWaitNote ), ETrue );
        iWaitNote->ExecuteLD( R_QTN_GEN_NOTE_FETCHING );
       
        TPtrC fileName = (*selectedFile)[0];

        // store reference to image field
        SetImageRefL( fileName );
        
        // write data to thumbnail field
        iImageOperation = 
            iImageManager->SetImageAsyncL(
                *iStoreContact, *iThumbnailFieldType, *this, fileName );
                
        result = ETrue;
        }

    CleanupStack::PopAndDestroy( selectedFile );
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::Pbk2ImageSetComplete
// --------------------------------------------------------------------------
//    
void CPbk2SetImageCmd::Pbk2ImageSetComplete
        ( MPbk2ImageOperation& PBK2_DEBUG_ONLY( aOperation ) )
    {
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_Pbk2ImageSetComplete));

    delete iImageOperation;
    iImageOperation = NULL;
    
	DismissWaitNote();
	
	TRAPD( error, iStoreContact->CommitL( *this ) );
	if ( error != KErrNone )
	    {
	    CCoeEnv::Static()->HandleError( error );
	    }
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::Pbk2ImageSetFailed
// --------------------------------------------------------------------------
//
void CPbk2SetImageCmd::Pbk2ImageSetFailed
        ( MPbk2ImageOperation& PBK2_DEBUG_ONLY( aOperation ), TInt aError )
    {
    __ASSERT_DEBUG( &aOperation == iImageOperation, 
        Panic( EPanicPreCond_Pbk2ImageSetFailed ) );

    delete iImageOperation;
    iImageOperation = NULL;

	DismissWaitNote();

	ProcessDismissed( aError );
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::DismissWaitNote
// --------------------------------------------------------------------------
//
void CPbk2SetImageCmd::DismissWaitNote()
	{
	// Dismiss the wait note
	if ( iWaitNote )
		{
		TRAPD( err, iWaitNote->ProcessFinishedL() );
		if ( err != KErrNone )
			{
			delete iWaitNote;
			iWaitNote = NULL;
			}
		}
	}

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::VerifySelectionL
// --------------------------------------------------------------------------
//	
TBool CPbk2SetImageCmd::VerifySelectionL
        (const MDesCArray* aSelectedFiles)
    {
    TBool result = EFalse;
    if ( aSelectedFiles && aSelectedFiles->MdcaCount() > 0 )
        {
        // DRM for phonebook image fetch
        TPtrC fileName = aSelectedFiles->MdcaPoint( 0 );
        TBool isProtected( ETrue );
        User::LeaveIfError( 
            iDrmManager->IsProtectedFile( fileName, isProtected ) );
        if ( isProtected )
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
        
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::Cancel
// --------------------------------------------------------------------------
//
void CPbk2SetImageCmd::Cancel()
    {
    delete iWaitNote;
    iWaitNote = NULL;
    delete iImageOperation;
    iImageOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::SetImageRefL
// --------------------------------------------------------------------------
//
void CPbk2SetImageCmd::SetImageRefL(const TDesC& aFileName)
    {
    __ASSERT_DEBUG(iImageFieldType && iStoreContact, Panic(EPanicPreCond_SetImageRefL));
    
    MVPbkStoreContactField* field = NULL;
    TInt fieldIndex = FindFieldIndex( *iImageFieldType );
    
    if ( fieldIndex != KErrNotFound )
        {
        // If filename has been set and the contact has the field
        // field should be updated.
        field = &iStoreContact->Fields().FieldAt( fieldIndex );
        MVPbkContactFieldTextData::Cast( field->FieldData() ).
            SetTextL( aFileName );
        }
    else if ( aFileName.Length() > 0 )
        {
        // Contact does not have the field, so create new one.
    	field = iStoreContact->CreateFieldLC( *iImageFieldType );
    	MVPbkContactFieldTextData::Cast( field->FieldData() ).
            SetTextL( aFileName );
    	iStoreContact->AddFieldL( field );
    	CleanupStack::Pop(); // field                    
        }
	}    

TBool CPbk2SetImageCmd::MGFetchL( CDesCArray& aSelectedFiles,
        TMediaFileType aMediaType,
        TBool aMultiSelect,
        MMGFetchVerifier* aVerifier )
    {
    CEikAppUi* pAppUi = CEikonEnv::Static()->EikAppUi();
    CAknNavigationDecorator *pNaviDecorator = NULL;
    if ( pAppUi )
        {
        CEikStatusPane* StatusPane = 
                static_cast<CAknAppUi*>(pAppUi)->StatusPane();
        TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
        CAknNavigationControlContainer* naviPane = 
                static_cast<CAknNavigationControlContainer*>( 
                    StatusPane->ControlL(naviPaneUid) );
        pNaviDecorator = naviPane->Top();
        if ( pNaviDecorator )
            {
            pNaviDecorator->MakeVisible( EFalse );  // Hide navi pane
            pNaviDecorator->DrawNow();
            }
        }
    
    TBool result( EFalse );
    // run image fetch dialog
    TRAPD(error, result = MGFetch::RunL( aSelectedFiles, 
            aMediaType, 
            aMultiSelect,   // multiselection 
            aVerifier));    // provide MMGFetchVerifier interface to check DRM
    
    if ( pNaviDecorator )
        {
        pNaviDecorator->MakeVisible( ETrue );   // Show again
        pNaviDecorator->DrawNow();
        }
    User::LeaveIfError(error);
    
    return result;
    }

//  End of File  
