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
* Description:  Phonebook 2 ringing tone commands.
*
*/


// INCLUDES

// Phonebook 2
#include <e32base.h>
#include "Pbk2ImageCommands.h"
#include "MPbk2ImageFieldObserver.h"
#include "MPbk2BaseCommand.h"

#include <MVPbkContactFieldTextData.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <TPbk2StoreContactAnalyzer.h>
#include "CPbk2ImageManager.h"
#include "CPbk2DrmManager.h"
#include <MPbk2ImageOperationObservers.h> // MPbk2ImageSetObserver
#include <MMGFetchVerifier.h>             // MMGFetchVerifier

#include <RPbk2LocalizedResourceFile.h>
#include "Pbk2DataCaging.hrh"
#include "Pbk2PresentationUtils.h"
#include <pbk2commonui.rsg>
#include <pbk2uicontrols.rsg>
#include <pbk2commands.rsg>

#include <MPbk2ImageOperationObservers.h>   // MPbk2ImageSetObserver
#include <AknServerApp.h>                   // MAknServerAppExitObserver

// VPbk includes
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <CVPbkContactManager.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkFieldType.h>
#include <vpbkeng.rsg>

// System includes
#include <barsread.h>
#include <coemain.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <MGFetch.h>
#include <DocumentHandler.h>            // CDocumentHandler
#include <apmstd.h>                     // TDataType
#include <pbk2cmdextres.rsg>
#include <sysutil.h>
#include <CPbk2ApplicationServices.h>
#include <AknProgressDialog.h>

/// Unnamed namespace for local definitions
namespace {

    const TInt KSelectedFilesDefaultGranularity = 1;

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_NULL_Contact = 1
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "MPbk2ImageCommands" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG    

// --------------------------------------------------------------------------
// ImageFieldTypeL
// --------------------------------------------------------------------------
//
inline const MVPbkFieldType* ImageFieldTypeL( 
        MPbk2ApplicationServices& aApplicationServices )
    {
    return aApplicationServices.ContactManager().
        	FieldTypes().Find( R_VPBK_FIELD_TYPE_CALLEROBJIMG );
    }

// --------------------------------------------------------------------------
// ThumbnailFieldTypeL
// --------------------------------------------------------------------------
//
inline const MVPbkFieldType* ThumbnailFieldTypeL( 
        MPbk2ApplicationServices& aApplicationServices )
    {
    return aApplicationServices.ContactManager().
        	FieldTypes().Find( R_VPBK_FIELD_TYPE_THUMBNAILPIC );
    }

// --------------------------------------------------------------------------
// Set image operation.
// Uses asynchronous operation of CPbk2ImageManager to set image. 
// MMGFetchVerifier implemented to perform DRM check for select image dialog.
// --------------------------------------------------------------------------
//
NONSHARABLE_CLASS( CSetImageCmd ):
    public CBase,
	public MPbk2BaseCommand,
    private MPbk2ImageSetObserver,
    public MProgressDialogCallback,
    private MMGFetchVerifier
	{
	public:
		static CSetImageCmd* NewL(); 
		~CSetImageCmd();
		
		void ExecuteL( MVPbkStoreContact* aContact,MPbk2ImageFieldObserver* aObserver );
		
	private: // from MPbkImageSetObserver
	    void Pbk2ImageSetComplete(
	            MPbk2ImageOperation& aOperation);
	    void Pbk2ImageSetFailed(
	            MPbk2ImageOperation& aOperation,
	            TInt aError);
	
	private: // from MProgressDialogCallback
	        void DialogDismissedL( TInt aButtonId );
	        
	private: // from MMGFetchVerifier
	    TBool VerifySelectionL(
	            const MDesCArray* aSelectedFiles);
	private:
		CSetImageCmd();
		void ConstructL();
		void Cancel();
		void DismissWaitNote();
		void SetImageRefL( MVPbkStoreContact* aContact,
				const TDesC& aFileName);
		TBool DoVerifySelectionL(
		        const MDesCArray* aSelectedFiles);
		void ShowErrorNoteL();
		
	private:
        /// Own: image set operation
        MPbk2ImageOperation* iImageOperation;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Own: DRM support
        CPbk2DrmManager* iDrmManager;
        const MVPbkFieldType* iThumbnailFieldType;
        /// Own: Image manager
        CPbk2ImageManager* iImageManager;
        
        // Own
        HBufC* iImageFilename;
        
        // Not own
        MVPbkStoreContact* iContact; 
        MPbk2ImageFieldObserver* iObserver;
        // Ref:
        CPbk2ApplicationServices* iServices;	
        TBool iVerificationFailed;
        TBool iImageSetFailed;
        TBool iImageSetError;
	};

CSetImageCmd* CSetImageCmd::NewL()
	{
	CSetImageCmd* self = new(ELeave) CSetImageCmd();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSetImageCmd::~CSetImageCmd()
	{
	Cancel();
	delete iImageFilename;
	delete iDrmManager;
	delete iImageManager;
	Release( iServices );	
	}

void CSetImageCmd::ExecuteL( MVPbkStoreContact* aContact,MPbk2ImageFieldObserver* aObserver )
	{
    iImageSetFailed = EFalse;
    iObserver=aObserver;
    CDesCArray* selectedFile = new(ELeave) CDesCArrayFlat(
            KSelectedFilesDefaultGranularity );
    CleanupStack::PushL( selectedFile );
	TBool res( EFalse );
		
	// run image fetch dialog
 	res = MGFetch::RunL( *selectedFile, 
	        EImageFile, 
	        EFalse, /* multiselection */
	        this /* provide MMGFetchVerifier interface to check DRM */);
 	        
    if ( !iVerificationFailed && res && selectedFile->Count() > 0 )
        {
        if (iWaitNote==NULL)
            {
            iWaitNote =new(ELeave) CAknWaitDialog(reinterpret_cast<CEikDialog**>( &iWaitNote ),ETrue );
            }
        iWaitNote->SetCallback( this );
        
        // doesn't delete waitNote
        iWaitNote->ExecuteLD( R_QTN_GEN_NOTE_FETCHING );

        TPtrC fileName = (*selectedFile)[0];

        // delete old filename
        delete iImageFilename;
        iImageFilename=NULL;
        // store these for later use
        iImageFilename=fileName.AllocL();
        iContact=aContact;        
        
        // write data to thumbnail field        
        iImageOperation = 
            iImageManager->SetImageAsyncL(
                *aContact, *iThumbnailFieldType, *this, fileName );                                       
        }        
 	
    CleanupStack::PopAndDestroy( selectedFile );
	}

CSetImageCmd::CSetImageCmd()
	{
	}

void CSetImageCmd::ConstructL()
	{
    /*
     * In Social phonebook case, the contact editor (that uses this class) is launched outside of pbk2 context.
     * Then the Pbk2AppUi is not valid.
     */
    iServices = CPbk2ApplicationServices::InstanceL();
    iImageManager = CPbk2ImageManager::NewL( iServices->ContactManager() );

	iThumbnailFieldType = ThumbnailFieldTypeL(*iServices);
	iDrmManager = CPbk2DrmManager::NewL();
	
	}

void CSetImageCmd::Cancel()
	{
    delete iWaitNote;
    iWaitNote = NULL;
    delete iImageOperation;
    iImageOperation = NULL;
    if (iObserver)
        {
        iObserver->ImageLoadingCancelled();
        }
	}

void CSetImageCmd::DismissWaitNote()
	{
	// ProcessFinishedL initiate iWaitNote destruction 
	// and iWaitNote is set to NULL during CAknWaitDialog destruction
	TRAPD( err, iWaitNote->ProcessFinishedL() );
	if ( err != KErrNone )
		{
		delete iWaitNote;
		iWaitNote = NULL;
		
		if( iImageSetFailed )
            {
            ShowErrorNoteL();
            }
		}
	}

void CSetImageCmd::Pbk2ImageSetComplete
        ( MPbk2ImageOperation& /*aOperation*/ )
    {
    
    // set the image reference after image has been loaded.
    SetImageRefL( iContact, iImageFilename->Des() );    
    
    delete iImageOperation;
    iImageOperation = NULL;
	DismissWaitNote();
    if (iObserver)
        {
        iObserver->ImageLoadingComplete();
        }	
    }

// --------------------------------------------------------------------------
// CPbk2SetImageCmd::Pbk2ImageSetFailed
// --------------------------------------------------------------------------
//
void CSetImageCmd::Pbk2ImageSetFailed
        ( MPbk2ImageOperation& /*aOperation*/, TInt aError )
    {        
    delete iImageOperation;
    iImageOperation = NULL;
    
    iImageSetFailed = ETrue;
    iImageSetError = aError;
    
	DismissWaitNote();    		   		
    }

void CSetImageCmd::SetImageRefL( MVPbkStoreContact* aContact,
		const TDesC& aFileName)
    {
    CVPbkContactManager& manager = iServices->ContactManager();
    
    TPbk2StoreContactAnalyzer analyzer(
        	        manager,
        	        aContact );
    TInt fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_IMAGE_SELECTOR );
    
    MVPbkStoreContactField* field;
    if ( fieldIndex != KErrNotFound )
        {
        // If filename has been set and the contact has the field
        // field should be updated.
        field = &aContact->Fields().FieldAt( fieldIndex );
        MVPbkContactFieldTextData::Cast( field->FieldData() ).
            SetTextL( aFileName );
        }
    else if ( aFileName.Length() > 0 )
        {
        const MVPbkFieldType* imageFieldType = ImageFieldTypeL(*iServices);
        
        // Contact does not have the field, so create new one.
    	field = aContact->CreateFieldLC( *imageFieldType );
    	MVPbkContactFieldTextData::Cast( field->FieldData() ).
            SetTextL( aFileName );
    	aContact->AddFieldL( field );
    	CleanupStack::Pop(); // field                    
        }
    }

// --------------------------------------------------------------------------
// CSetImageCmd::DialogDismissedL
// --------------------------------------------------------------------------
//  
void CSetImageCmd::DialogDismissedL( TInt /*aButtonId*/ )
    {
    if( iImageSetFailed )
        {
        ShowErrorNoteL();
        }
    }

// --------------------------------------------------------------------------
// CSetImageCmd::VerifySelectionL
// --------------------------------------------------------------------------
//	
TBool CSetImageCmd::VerifySelectionL
    (const MDesCArray* aSelectedFiles)
    {
    iVerificationFailed = EFalse;
    TBool ret = ETrue;
       
    TRAPD( err, ret = DoVerifySelectionL( aSelectedFiles ) );
    
    if( err != KErrNone )
       {
       iVerificationFailed = ETrue;
       ShowErrorNoteL();    
       } 
    
    // Selection is always accepted if the image is not drm protected.
    // Image fetch dialog functionality is always same in spite of error 
    // type (DRM check, ImageDecoder, etc. errors) Dialog is always closed.    
    return ret;
    }

// --------------------------------------------------------------------------
// CSetImageCmd::DoVerifySelectionL
// --------------------------------------------------------------------------
//  
TBool CSetImageCmd::DoVerifySelectionL
    (const MDesCArray* aSelectedFiles)
    {     
    TBool ret = ETrue;
    
    if ( aSelectedFiles && aSelectedFiles->MdcaCount() > 0 )
        {    
        // DRM for phonebook image fetch
        TPtrC fileName = aSelectedFiles->MdcaPoint( 0 );
                
        TBool isProtected( ETrue );
        User::LeaveIfError( 
            iDrmManager->IsProtectedFile( fileName, isProtected ) );
                
        if ( isProtected )
            {        
            ret = EFalse;
            
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
        }  
    
    return ret;
    }	

// --------------------------------------------------------------------------
// CSetImageCmd::ShowErrorNoteL
// --------------------------------------------------------------------------
//
void CSetImageCmd::ShowErrorNoteL()
    {                     
    HBufC* prompt = StringLoader::LoadLC( R_QTN_ALBUM_ERR_FORMAT_UNKNOWN );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    
    if( iImageSetFailed )
        {
        iImageSetFailed = EFalse;
        
        if (iObserver)
           {
           iObserver->ImageLoadingFailed();
           }
       
       if ( iImageSetError != KErrNone )
           {
           CCoeEnv::Static()->HandleError( iImageSetError );
           iImageSetError = KErrNone;
           }
        }    
    }

// --------------------------------------------------------------------------
// Starts external image viewer via doc.handler and waits for its exit
// --------------------------------------------------------------------------
//
NONSHARABLE_CLASS( CViewImageCmd ):
    public CBase,
	public MPbk2BaseCommand,
	private MAknServerAppExitObserver
	{
	public:
		static CViewImageCmd* NewL(); 
		~CViewImageCmd();
		
		void ExecuteL( MVPbkStoreContact* aContact );
    
	private: // from MAknServerAppExitObserver
        void HandleServerAppExit(
                TInt aReason );		
	private:
		CViewImageCmd();
		void ConstructL();
		void ShowImageNotOpenedNoteL();
		
	private:
		/// Own: Document handler for image opening
		CDocumentHandler* iDocHandler;        /// Own: wait note dialog
		TFileName iFileName;
		CPbk2ApplicationServices* iServices;
	};

CViewImageCmd*	CViewImageCmd::NewL()
	{
	CViewImageCmd* self = new(ELeave) CViewImageCmd();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}

CViewImageCmd::CViewImageCmd()
	{
	}

void CViewImageCmd::ConstructL()
	{	
	iDocHandler = CDocumentHandler::NewL();
	iDocHandler->SetExitObserver( this );
	
	/*
	 * In Social phonebook case, the contact editor (that uses this class) is launched outside of pbk2 context.
	 * Then the Pbk2AppUi is not valid.
	 */
	iServices = CPbk2ApplicationServices::InstanceL();
	}

CViewImageCmd::~CViewImageCmd()
	{
	delete iDocHandler;
	Release( iServices );
	}
	
void CViewImageCmd::HandleServerAppExit( TInt aReason )
    {
    // make base call first
    MAknServerAppExitObserver::HandleServerAppExit( aReason );
    }

void CViewImageCmd::ShowImageNotOpenedNoteL()
    {
    HBufC* prompt = StringLoader::LoadLC( R_QTN_ALBUM_ERR_FORMAT_UNKNOWN );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

void CViewImageCmd::ExecuteL( MVPbkStoreContact* aContact )
	{
	CVPbkContactManager& manager = iServices->ContactManager();
	
	TPbk2StoreContactAnalyzer analyzer(
					manager,
					aContact );
    TInt fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_IMAGE_SELECTOR );

    if( fieldIndex != KErrNotFound )
        {
        // retrieve field pointer and its current text for call
        MVPbkStoreContactField& field =
        	aContact->Fields().FieldAt( fieldIndex );
        iFileName.Copy(
            MVPbkContactFieldTextData::Cast( field.FieldData() ).Text() );

        // empty datatype for automatic recognization
        TDataType dataType;

        TRAPD( error, iDocHandler->OpenFileEmbeddedL( iFileName, dataType ) );
        
        // KErrNotReady, when no mmc inserted
        // KErrNotFound, when image is deleted
        if ( error == KErrNotFound || error == KErrUnderflow
                                   || error == KErrCorrupt 
                                   || error == KErrNotReady )
            {
            // if the image has been deleted from Image Gallery
            // or it is corrupted, show a note instead of leave
            ShowImageNotOpenedNoteL();
            }
        }	
	}
} /// namespace

// --------------------------------------------------------------------------
// MPbk2ImageCommands::SetImageL
// --------------------------------------------------------------------------
//
MPbk2BaseCommand* Pbk2ImageCommands::SetImageL(
		MVPbkStoreContact* aContact, MPbk2ImageFieldObserver* aObserver )
	{
	__ASSERT_DEBUG( aContact, Panic( EPanicPreCond_NULL_Contact ) );
    
	RFs fs = CCoeEnv::Static()->FsSession();
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs ) )
        {
        User::Leave( KErrDiskFull );
        }
    
    CSetImageCmd* cmd = CSetImageCmd::NewL();
    CleanupStack::PushL( cmd );
    cmd->ExecuteL( aContact , aObserver);
    CleanupStack::Pop();
    
    return cmd;
	}

// --------------------------------------------------------------------------
// MPbk2ImageCommands::ViewImageL
// --------------------------------------------------------------------------
//
MPbk2BaseCommand* Pbk2ImageCommands::ViewImageL(
		MVPbkStoreContact* aContact )
	{
	__ASSERT_DEBUG( aContact, Panic( EPanicPreCond_NULL_Contact ) );

    CViewImageCmd* cmd = CViewImageCmd::NewL();
    CleanupStack::PushL( cmd );
    cmd->ExecuteL( aContact );
    CleanupStack::Pop();

    return cmd;	
	}

// --------------------------------------------------------------------------
// MPbk2ImageCommands::RemoveImageL
// --------------------------------------------------------------------------
//
MPbk2BaseCommand* Pbk2ImageCommands::RemoveImageL(
		MVPbkStoreContact* aContact )
	{
	__ASSERT_DEBUG( aContact, Panic( EPanicPreCond_NULL_Contact ) );

    // If Social phonebook is enabled, the contact editor (that uses this class) 
	// is launched outside of pbk2 context. Then the Pbk2AppUi is not valid.
	MPbk2ApplicationServices& services = *CPbk2ApplicationServices::InstanceLC();
    
	CPbk2ImageManager* imageManager = CPbk2ImageManager::NewL(
        services.ContactManager() );
	CleanupStack::PushL( imageManager );
	
	const MVPbkFieldType* thumbType = services.ContactManager().FieldTypes().Find(
	                R_VPBK_FIELD_TYPE_THUMBNAILPIC );
	
	// show a confirmation query if contact has an image
	if (thumbType && imageManager->HasImage( *aContact, *thumbType ) )
	    {
	    if ( CAknQueryDialog::NewL()->ExecuteLD( 
	            R_PHONEBOOK2_REMOVE_IMAGE_CONFIRMATION_DIALOG ) )
	        {
	        TPbk2StoreContactAnalyzer analyzer(
	                services.ContactManager(), aContact );
	        TInt imageIndex = analyzer.HasFieldL( R_PHONEBOOK2_IMAGE_SELECTOR );
        
	        if ( imageIndex != KErrNotFound )
	            {
	            //aContact->RemoveField( imageIndex );
	            //Instead of above we don't remove the field, just zero it's data
	            MVPbkContactFieldData& data(aContact->Fields().
	                    FieldAt( imageIndex ).FieldData());
	            MVPbkContactFieldTextData::Cast( data ).SetTextL(KNullDesC);
	            }
        
	        const MVPbkFieldType* thumbFieldType = ThumbnailFieldTypeL(services);
	        imageManager->RemoveImage( *aContact, *thumbFieldType );
	        }
	    }
	
    CleanupStack::PopAndDestroy( imageManager );
    CleanupStack::PopAndDestroy(); // services
    
    return NULL; // synchronous operation
	}

// --------------------------------------------------------------------------
// MPbk2ImageCommands::IsImageFieldL
// --------------------------------------------------------------------------
//
TBool Pbk2ImageCommands::IsImageFieldL(
		const MVPbkStoreContactField& aField )
	{
	/*
	 * In Social phonebook case, the contact editor (that uses this class) is launched outside of pbk2 context.
	 * Then the Pbk2AppUi is not valid.
	 */
	CPbk2ApplicationServices* services = CPbk2ApplicationServices::InstanceL();
	TPbk2StoreContactAnalyzer analyzer(
			services->ContactManager(),
			NULL );
	Release( services );
    
    return analyzer.IsFieldTypeIncludedL(
    		aField, R_PHONEBOOK2_IMAGE_SELECTOR ); 
	}

// --------------------------------------------------------------------------
// MPbk2ImageCommands::IsImageFieldL
// --------------------------------------------------------------------------
//
TBool Pbk2ImageCommands::IsThumbnailFieldL(
		const MVPbkStoreContactField& aField )
	{
	/*
	 * In Social phonebook case, the contact editor (that uses this class) is launched outside of pbk2 context.
	 * Then the Pbk2AppUi is not valid.
	 */
	CPbk2ApplicationServices* services = CPbk2ApplicationServices::InstanceL();
	TPbk2StoreContactAnalyzer analyzer(
			services->ContactManager(),
			NULL );
	Release( services );
    
    return analyzer.IsFieldTypeIncludedL(
    		aField, R_PHONEBOOK2_THUMBNAIL_SELECTOR ); 
	}

// --------------------------------------------------------------------------
// MPbk2ImageCommands::IsImageFieldValidL
// --------------------------------------------------------------------------
//
TBool Pbk2ImageCommands::IsImageFieldValidL(
		const MVPbkStoreContactField& aField )
	{
    const MVPbkContactFieldTextData& textData = 
        MVPbkContactFieldTextData::Cast( aField.FieldData() );
    TEntry ignore;
    return CCoeEnv::Static()->FsSession().Entry( textData.Text(), ignore ) == KErrNone;
	}

// End of File
