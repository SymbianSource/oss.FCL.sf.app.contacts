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
*           Image command event handling class.
*
*/


// INCLUDE FILES
#include "CPbkCodImageCmd.h"
#include <PbkView.hrh>
#include "CPbkImageManager.h"
#include <Phonebook.hrh>
#include <pbkconfig.hrh>

#include <eikmenup.h>               // CEikMenuPane
#include <AknWaitDialog.h>
#include <AknNoteWrappers.h>
#include <SysUtil.h>
#include <StringLoader.h>
#include <PbkView.rsg>              // PbkView resource symbols
#include <CPbkContactItem.h>
#include <MPbkMenuCommandObserver.h>
#include <MGFetch.h>
#include <eikmenup.h> // CEikMenuPane
#include <CPbkContactEngine.h>
#include <CPbkFieldsInfo.h>
#include <DocumentHandler.h>
#include <apmstd.h> // TDataType 
#include <eikapp.h> // CEikApplication
#include <PbkImageData.h>
/// Unnamed namespace for local definitons
namespace {

inline const TDesC8& ToTDesC8(HBufC8* aBuf)
    {
    if (aBuf)
        {
        return *aBuf;
        }
    else
        {
        return KNullDesC8;
        }
    }

// Temp folder path for CFileMan usage
_LIT(KPbkTempPath,"c:\\private\\101f4cce\\temp\\*");            

#ifdef _DEBUG
enum TPanicCode
    {    
    EPanicPreCond_PbkImageSetFailed = 1,
    EPanicPreCond_PbkImageGetImageComplete,
    EPanicPreCond_PbkImageGetImageFailed
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkCodImageCmd");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG


} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkCodImageCmd::CPbkCodImageCmd
        (CPbkContactEngine& aEngine) :
    iEngine(aEngine)
    {
    }

inline void CPbkCodImageCmd::ConstructL()
    {
    iPbkImageManager = CPbkImageManager::NewL(iEngine); 
    iSelectedFile = new(ELeave) CDesCArrayFlat(1);
    iFileMan = CFileMan::NewL(CCoeEnv::Static()->FsSession());
    }

CPbkCodImageCmd* CPbkCodImageCmd::NewL
        (CPbkContactEngine& aEngine)
    {
    CPbkCodImageCmd* self = new (ELeave) CPbkCodImageCmd(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CPbkCodImageCmd::~CPbkCodImageCmd()
    {    
    delete iImageOperation;
    delete iWaitNote;
    delete iPbkImageManager;
    delete iSelectedFile;
    delete iContactItem;
    delete iDocumentHandler;
    delete iImageData;        
    delete iFileName;
    
    // Clean temp folder
    DeleteTempFolder();
    delete iFileMan;
    }

TBool CPbkCodImageCmd::FetchImageL(TContactItemId aId)
    {
    TBool ret(EFalse);

    Cancel();

    // fetch image from media gallery
    TBool res = MGFetch::RunL(*iSelectedFile, EImageFile, EFalse, this);
    if (res && iSelectedFile->Count() > 0)
        {
        CAknWaitDialog* waitNote = new(ELeave) 
            CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitNote));
        waitNote->ExecuteLD(R_QTN_GEN_NOTE_FETCHING);
        CleanupStack::PushL(waitNote);

        // Contact item is open while image is loaded.
        CPbkContactItem* ci = iEngine.OpenContactL(aId);
        CleanupStack::PushL(ci);

        TPtrC fileName = (*iSelectedFile)[0];
        iImageOperation = 
            iPbkImageManager->SetImageAsyncL
                (EPbkFieldIdCodImageID, *this, *ci, fileName );        
        CleanupStack::Pop(ci);
        delete iContactItem;
        iContactItem  = ci;

        CleanupStack::Pop(waitNote);
        iWaitNote = waitNote;

        ret = ETrue;
        }
    return ret;
    }

TBool CPbkCodImageCmd::RemoveImageL
        (TContactItemId aId)
    {
    Cancel();

    CPbkContactItem* ci = iEngine.OpenContactL(aId);
    CleanupStack::PushL(ci);
    iPbkImageManager->RemoveImage(EPbkFieldIdCodImageID, *ci);
    iEngine.CommitContactL(*ci);
    CleanupStack::PopAndDestroy(ci); // ci
    return ETrue;
    }

void CPbkCodImageCmd::PbkImageSetComplete
        (MPbkImageOperation& /*aPbkImageOperation*/)
    {
    // If operation not cancelled
    if (iContactItem)
        {
        TInt err(KErrNone);
        // Set filename to iContactItem after image data fetch
        TRAP(err, SetImageFilenameL())
        if(err==KErrNone)
            {
            // Commit a new contact data to database.
            // Commit also closes the contact.
            TRAP(err, iEngine.CommitContactL(*iContactItem));
            }
        if (err!=KErrNone)
            {
            CCoeEnv::Static()->HandleError(err);
            }
        }

    delete iImageOperation;
    iImageOperation = NULL;

    // Dismiss the wait note
    TRAPD(err, iWaitNote->ProcessFinishedL());
    if (err != KErrNone)
        {
        delete iWaitNote;
        iWaitNote = NULL;
        }

    // Contact not needed anymore
    delete iContactItem;
    iContactItem = NULL;
    }

void CPbkCodImageCmd::PbkImageSetFailed
        (MPbkImageOperation& 
#ifdef _DEBUG
        aOperation
#endif
        ,TInt aError)
    {
    // TODO next fails
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_PbkImageSetFailed));

    delete iImageOperation;
    iImageOperation = NULL;

    // Dismiss the wait note
    TRAPD(err, iWaitNote->ProcessFinishedL());
    if (err != KErrNone)
        {
        delete iWaitNote;
        iWaitNote = NULL;
        }
    // Contact not needed anymore
    delete iContactItem;
    iContactItem = NULL;

    // Report the failure to the user
    CCoeEnv::Static()->HandleError(aError);
    }

TBool CPbkCodImageCmd::VerifySelectionL
        (const MDesCArray* aSelectedFiles)
    {
    return (aSelectedFiles && aSelectedFiles->MdcaCount() > 0)? ETrue: EFalse;
    }

void CPbkCodImageCmd::Cancel()
    {
    delete iContactItem;
    iContactItem = NULL;
    delete iWaitNote;
    iWaitNote = NULL;
    delete iImageOperation;
    iImageOperation = NULL;
    }

void CPbkCodImageCmd::SetImageFilenameL()
    {
    // Use text field if exists
    TPbkContactItemField* field = iContactItem->FindField(
        EPbkFieldIdCodTextID );
    if (!field)
        {
        CPbkFieldInfo* fieldInfo = iEngine.FieldsInfo().Find(
            EPbkFieldIdCodTextID);

        // Otherwise create new field for filename purposes
        field = iContactItem->AddOrReturnUnusedFieldL( 
        *fieldInfo);
        }
    TParse parse;
    parse.Set((*iSelectedFile)[0],NULL,NULL);
    field->TextStorage()->SetTextL(parse.Name());
    }

void CPbkCodImageCmd::ViewImageL(TContactItemId aId)
    {
    Cancel();
    // The file might exist in temp folder e.g. if image viewing fails.
    DeleteTempFolder();    
    
    
    // TODO Image loading should be asynchronous including 
    // also the image scaling
    /*     
    CAknWaitDialog* waitNote = new(ELeave) 
        CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitNote));
    waitNote->ExecuteLD(R_QTN_GEN_NOTE_FETCHING);
    CleanupStack::PushL(waitNote);
    */
    if (!iDocumentHandler)
        {        
        iDocumentHandler = CDocumentHandler::NewL();
        iDocumentHandler->SetExitObserver(this);                    
        }
    

    /*// Contact item is open while image is loaded.
    CPbkContactItem* ci = iEngine.OpenContactL(aId);
    CleanupStack::PushL(ci);
    */  
    CPbkContactItem* ci = iEngine.OpenContactLCX(aId);        

    TPbkContactItemField* imageField = ci->FindField( 
        EPbkFieldIdCodImageID );
        
    if (!imageField)
        {
        // Cod image field must exist
        User::Leave(KErrNotSupported);
        }
    
    delete iFileName;
    iFileName = NULL;    
        
    TPbkContactItemField* textField = ci->FindField( 
        EPbkFieldIdCodTextID );        
    
    if(textField)
        {
        // If filename saved. Otherwise image viewer's default name is used.
        iFileName = HBufC::NewMaxL(KMaxFileName);
        if(textField->Text().Length()<=KMaxFileName)
            {
            TPtr ptr=iFileName->Des();
            const TPtrC text=textField->Text();            
            ptr= text;
            }        
        }
    else
        {
        iFileName = TFileName().AllocL();
        }
            
    // Ownership not transferred
    HBufC8* imageBuf = 
        imageField->ContactItemField().StoreStorage()->Thing();
    
    TDataType datatype = TDataType( KNullUid );
    DoViewImageL(ToTDesC8(imageBuf));
            
    CleanupStack::PopAndDestroy(2); // ci, lock
    /*CleanupStack::PopAndDestroy(ci);
    iImageOperation = 
          iPbkImageManager->GetImageAsyncL(EPbkFieldIdCodImageID, *this, *ci);
    */

    /*
    CleanupStack::Pop(ci);
    delete iContactItem;
    iContactItem  = ci;
    CleanupStack::Pop(waitNote);
    iWaitNote = waitNote;
    */
    
    }

void CPbkCodImageCmd::PbkImageGetImageComplete
    (MPbkImageOperation& 
#ifdef _DEBUG
    aOperation
#endif    
    , CPbkImageDataWithInfo* aImageData)
    {
    // TODO next fails 
    __ASSERT_DEBUG(&aOperation == iImageOperation && aImageData && !iImageData,
        Panic(EPanicPreCond_PbkImageGetImageComplete));

    // Contact not needed anymore
    delete iContactItem;
    iContactItem = NULL;    
    
    iImageData = aImageData;
    
    TRAPD(err, DoViewImageL(iImageData->ImageData().GetBufferL()));
    delete iImageData;
    iImageData = NULL;
    
    delete iImageOperation;
    iImageOperation = NULL;

    TRAPD(err2,iWaitNote->ProcessFinishedL())
    if (err2!=KErrNone)
        {
        delete iWaitNote;
        iWaitNote = NULL;
        }

    // Report the first occurred failure to the user
    if(err!=KErrNone)
        {
        CCoeEnv::Static()->HandleError(err);
        }
    }

void CPbkCodImageCmd::DoViewImageL
    (const TDesC8& aImageData)
    {    
    TDataType dataType = TDataType( KNullUid );
    
    RFile handle;
    iDocumentHandler->SaveTempFileL(
        aImageData, dataType, *iFileName, handle);
    
    CleanupClosePushL(handle);

    // iFileName content will now contain the temp filename
    // which is deleted later on
    TPtr ptr = iFileName->Des();
    
    TInt err(iDocumentHandler->GetPath(ptr));
    if (err==KErrNone)
        {
        TRAP(err,iDocumentHandler->OpenFileL(handle, dataType));
        }
    if (err!=KErrNone)
        {
        DeleteTempFile();
        User::LeaveIfError(err);
        }
    
    CleanupStack::PopAndDestroy();//handle    
    }

void CPbkCodImageCmd::PbkImageGetImageFailed
    (MPbkImageOperation& 
#ifdef _DEBUG
    aOperation
#endif    
    , TInt aError)
    {
    // TODO next fails 
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_PbkImageGetImageFailed));

    delete iImageOperation;
    iImageOperation = NULL;

    // Dismiss the wait note
    TRAPD(err, iWaitNote->ProcessFinishedL());
    if (err != KErrNone)
        {
        delete iWaitNote;
        iWaitNote = NULL;
        }
    // Contact not needed anymore
    delete iContactItem;
    iContactItem = NULL;

    // Report the failure to the user
    CCoeEnv::Static()->HandleError(aError);
    }

void CPbkCodImageCmd::HandleServerAppExit(TInt /*aReason*/)
    {
    // File is not needed anymore.
    DeleteTempFile();
    }

TInt CPbkCodImageCmd::DeleteTempFile()
    {    
    TInt ret(KErrCorrupt);
    if(iFileName)
        {
        // Try to delete the file        
        ret = CCoeEnv::Static()->FsSession().Delete(*iFileName);
        
        delete iFileName;
        iFileName = NULL;
        }
    return ret;
    }

/**
* DeleteTempFolder cleans the temp folder
* and takes care of that temp folder is 
* cleaned after possible error situations.
*/    
TInt CPbkCodImageCmd::DeleteTempFolder()
    {
    return iFileMan->Delete(KPbkTempPath());
    }
    
//  End of File  
