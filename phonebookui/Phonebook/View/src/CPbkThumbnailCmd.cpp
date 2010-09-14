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
*           Thumbnail command event handling class.
*
*/


// INCLUDE FILES
#include "CPbkThumbnailCmd.h"

#include <eikmenup.h>               // CEikMenuPane
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <sysutil.h>
#include <StringLoader.h>

#include <pbkview.rsg>              // PbkView resource symbols
#include <PbkView.hrh>

#include "CPbkThumbnailManager.h"
#include "CPbkThumbnailPopup.h"
#include <CPbkDrmManager.h>

#include <CPbkContactItem.h>
#include <MPbkMenuCommandObserver.h>

#include <MGFetch.h>

/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_PbkThumbnailSetComplete,
    EPanicPreCond_PbkThumbnailSetFailed
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkThumbnailCmd");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkThumbnailCmd::CPbkThumbnailCmd
        (CPbkContactEngine& aEngine, 
        CPbkThumbnailPopup& aThumbnailPopup) :
    iEngine(aEngine),
    iThumbnailPopup(aThumbnailPopup)
    {
    }

inline void CPbkThumbnailCmd::ConstructL()
    {
    iThumbnailManager = CPbkThumbnailManager::NewL(iEngine);
    iDrmManager = CPbkDrmManager::NewL();
    }

CPbkThumbnailCmd* CPbkThumbnailCmd::NewL
        (CPbkContactEngine& aEngine,
        CPbkThumbnailPopup& aThumbnailPopup)
    {
    CPbkThumbnailCmd* self = new (ELeave) 
        CPbkThumbnailCmd(aEngine, aThumbnailPopup);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CPbkThumbnailCmd::~CPbkThumbnailCmd()
    {
    if (iDestroyed && *iDestroyed==EFalse)
        {
        *iDestroyed=ETrue;
        }
    delete iDrmManager;
    delete iThumbOperation;
    delete iWaitNote;
    delete iThumbnailManager;
    }

TBool CPbkThumbnailCmd::FetchThumbnailL
        (CPbkContactItem& aContactItem)
    {
    TBool result = EFalse;
    Cancel();
    
    // fetch thumbnail from media gallery
    CDesCArray* selectedFile = new(ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(selectedFile);
    TBool destroyed(EFalse);
    iDestroyed=&destroyed;
	TBool res = MGFetch::RunL(*selectedFile, EImageFile, EFalse, this);
    if ( *iDestroyed==EFalse && res && selectedFile->Count() > 0)
        {
        CAknWaitDialog* waitNote = 
            new(ELeave) CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitNote));
        waitNote->ExecuteLD(R_QTN_GEN_NOTE_FETCHING);
        CleanupStack::PushL(waitNote);

        TPtrC fileName = (*selectedFile)[0];
        iThumbOperation = 
            iThumbnailManager->SetThumbnailAsyncL
                (*this, aContactItem, fileName);
        
        CleanupStack::Pop(waitNote);
        iWaitNote = waitNote;
        iContactItem = &aContactItem;
        result = ETrue;
        }
    iDestroyed = NULL;
    CleanupStack::PopAndDestroy(selectedFile);
    return result;
    }

TBool CPbkThumbnailCmd::RemoveThumbnailL
        (CPbkContactItem& aContactItem)
    {
    Cancel();
    iThumbnailManager->RemoveThumbnail(aContactItem);
    if (iThumbnailPopup.ContactId() == aContactItem.Id())
        {
        iThumbnailPopup.CancelLoading();
        }
    return ETrue;
    }

void CPbkThumbnailCmd::DynInitMenuPaneL
        (TInt /*aResourceId*/, 
        CEikMenuPane& aMenuPane,
        CPbkContactItem& aContactItem)
    {    
    // if context pane not present, dim all thumbnail related items.    
    if ( !iAvkonAppUi->StatusPane()->
        PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsInCurrentLayout() )
        {
        aMenuPane.SetItemDimmed(EPbkCmdRemoveThumbnail, ETrue);            
        aMenuPane.SetItemDimmed(EPbkCmdFetchThumbnail, ETrue);
        }
        
    // Filter remove thumbnail item               
    else if (!iThumbnailManager->HasThumbnail(aContactItem))
        {
        aMenuPane.SetItemDimmed(EPbkCmdRemoveThumbnail, ETrue);
        }
    }

void CPbkThumbnailCmd::PbkThumbnailSetComplete
        (MPbkThumbnailOperation& 
#ifdef _DEBUG
        aOperation
#endif
        )
    {
    __ASSERT_DEBUG(&aOperation == iThumbOperation, 
        Panic(EPanicPreCond_PbkThumbnailSetComplete));

    delete iThumbOperation;
    iThumbOperation = NULL;
    
    // Display the thumbnail
    iThumbnailPopup.Load(*iContactItem, NULL );

    // Dismiss the wait note
	TRAPD(err, iWaitNote->ProcessFinishedL());
	if (err != KErrNone)
		{
		delete iWaitNote;
        iWaitNote = NULL;
		}
    }

void CPbkThumbnailCmd::PbkThumbnailSetFailed
        (MPbkThumbnailOperation& 
#ifdef _DEBUG
        aOperation
#endif
        ,TInt aError)
    {
    __ASSERT_DEBUG(&aOperation == iThumbOperation, 
        Panic(EPanicPreCond_PbkThumbnailSetFailed));

    delete iThumbOperation;
    iThumbOperation = NULL;

    // Dismiss the wait note
	TRAPD(err, iWaitNote->ProcessFinishedL());
	if (err != KErrNone)
		{
		delete iWaitNote;
        iWaitNote = NULL;
		}

    // Report the failure to the user
    CCoeEnv::Static()->HandleError(aError);
    }

TBool CPbkThumbnailCmd::VerifySelectionL
        (const MDesCArray* aSelectedFiles)
    {
    TBool result = EFalse;
    if (iDestroyed && *iDestroyed==EFalse && aSelectedFiles && aSelectedFiles->MdcaCount() > 0)
        {
        // DRM for phonebook thumbnail fetch
        TPtrC fileName = aSelectedFiles->MdcaPoint(0);
        if (iDrmManager->IsProtectedFile(fileName))
            {
            // show user copyright note
            HBufC* prompt = CCoeEnv::Static()->AllocReadResourceLC(R_QTN_DRM_NOT_ALLOWED);
            CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
            dlg->ExecuteLD(*prompt);
            CleanupStack::PopAndDestroy(); // prompt
            }
        else
            {
            result = ETrue;
            }
        }
    return result;
    }

void CPbkThumbnailCmd::Cancel()
    {
    iContactItem = NULL;
    delete iWaitNote;
    iWaitNote = NULL;
    delete iThumbOperation;
    iThumbOperation = NULL;
    }

TBool CPbkThumbnailCmd::OkToExit()
    {
    TBool ret(ETrue);
    if (iDestroyed && *iDestroyed==EFalse)
        {
        // MGFetch call not returned
        ret=EFalse;
        }
    return ret;    
    }
//  End of File  
