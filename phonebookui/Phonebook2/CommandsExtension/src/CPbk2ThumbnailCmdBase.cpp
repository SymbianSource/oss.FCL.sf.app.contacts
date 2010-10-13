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
* Description: 
*           Base class for thumbnail commands set and remove.
*
*/


// INCLUDE FILES
#include "CPbk2ThumbnailCmdBase.h"

// Phonebook 2
#include <Pbk2Commands.rsg>
#include <CPbk2ImageManager.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2CommandObserver.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MPbk2ContactUiControl.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <StringLoader.h>
#include <barsread.h>				// TResourceReader

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitons
namespace {
	_LIT(KPbk2CommandsResFile, "Pbk2Commands.rsc");	
	

	#ifdef _DEBUG
	const TInt KAmountOfFieldTypesInResFile(1);

	enum TPanicCode
	    {
	    EPanicCorruptedResFile,
	    EPanicInvalidOpCode,
	    EPanicLogic_RelocateContactL
	    };

	void Panic(TInt aReason)
	    {
	    _LIT(KPanicText, "CPbk2ThumbnailCmdBase");
	    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

CPbk2ThumbnailCmdBase::CPbk2ThumbnailCmdBase
        (MPbk2ContactUiControl& aUiControl) :
	iUiControl(&aUiControl)
    {
    }

CPbk2ThumbnailCmdBase::~CPbk2ThumbnailCmdBase()
    {
    delete iThumbnailManager;
    delete iContactLink;
    delete iRetrieveOperation;
    delete iStoreContact;
    delete iContactRelocator;
    }

void CPbk2ThumbnailCmdBase::BaseConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::BaseConstructL(0x%x), start"), this);

    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();

	iContactManager = &(appServices->ContactManager());

    CleanupStack::PopAndDestroy(); // appServices

	iContactLink = iUiControl->FocusedContactL()->CreateLinkLC();
	CleanupStack::Pop(); // iContactLink
	iFocusedFieldIndex = iUiControl->FocusedFieldIndex();
	
    iThumbnailManager = CPbk2ImageManager::NewL(*iContactManager);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::BaseConstructL(0x%x), end"), this);
    }

void CPbk2ThumbnailCmdBase::ProcessDismissed(TInt aError)
	{
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::ProcessDismissed(%d), aError"), aError);
	
	FilterErrors( aError );
    if (iUiControl)
        {
        TRAPD( error, iUiControl->SetFocusedContactL( *iStoreContact ) );
    	if ( error != KErrNone )
    	    {
    	    CCoeEnv::Static()->HandleError( error );
    	    }
        if ( iFocusedFieldIndex != KErrNotFound )
    	    {
    	    iUiControl->SetFocusedFieldIndex( iFocusedFieldIndex );
    	    }
        iUiControl->UpdateAfterCommandExecution();
        }

	if (iCommandObserver)
		{		
	    // Notify command owner that the command has finished
	    iCommandObserver->CommandFinished(*this);
		}
	}
    
void CPbk2ThumbnailCmdBase::ExecuteLD()
	{
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::ExecuteLD(0x%x), start"), this);
	
	CleanupStack::PushL(this);
	ReadFieldTypeL();
	iRetrieveOperation = iContactManager->RetrieveContactL(
        *iContactLink, *this);
    CleanupStack::Pop(this);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::ExecuteLD(0x%x), end"), this);
	}

void CPbk2ThumbnailCmdBase::AddObserver( MPbk2CommandObserver& aObserver )
	{
	iCommandObserver = &aObserver;
	}

void CPbk2ThumbnailCmdBase::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

void CPbk2ThumbnailCmdBase::FilterErrors( TInt aError )
	{
	if ( aError != KErrNone )
		{
		// Report the failure to the user
	    CCoeEnv::Static()->HandleError( aError );
		}
	}

void CPbk2ThumbnailCmdBase::ContactOperationCompleted
        ( TContactOpResult aResult )
	{
	__ASSERT_DEBUG((aResult.iOpCode == EContactCommit ||
            aResult.iOpCode == EContactLock),
		    Panic(EPanicInvalidOpCode));
		
	if (aResult.iOpCode == EContactLock)
		{
		TBool result (EFalse);        
		TRAPD(err, result = ExecuteCommandL());
		if (err != KErrNone || result == EFalse)
			{		
			ProcessDismissed(err);
			}		
		}
	else
		{
		ProcessDismissed(KErrNone);
		}
	}

void CPbk2ThumbnailCmdBase::ContactOperationFailed
    ( TContactOp PBK2_DEBUG_ONLY( aOpCode ),
      TInt aErrorCode, TBool /*aErrorNotified*/ )
	{
	__ASSERT_DEBUG((aOpCode == EContactCommit || aOpCode == EContactLock), 
		Panic(EPanicInvalidOpCode));
	
	ProcessDismissed( aErrorCode );
	}
    
/**
 * Called when retrieve operation completes.
 */
void CPbk2ThumbnailCmdBase::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation, MVPbkStoreContact* aContact)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

		if (iStoreContact)
			{
			delete iStoreContact;
			iStoreContact = NULL;
			}
        // We now have a store contact
        iStoreContact = aContact;
        }

    TBool contactNeedsRelocation = EFalse;
    TRAP_IGNORE(contactNeedsRelocation = RelocateContactL());

    if (!contactNeedsRelocation)
        {
        TRAPD(err, iStoreContact->LockL(*this));
        if (err != KErrNone)
            {
            ProcessDismissed(err);
            }
        }
    }

/**
 * Called when retrieve operation fails.
 */
void CPbk2ThumbnailCmdBase::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, TInt aError)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        
        // We cannot get the contact, so we have to
        // fail. We cannot continue, since this operation
        // was executed only in case of one contact.
        ProcessDismissed(aError);
        }
    }
   
    
void CPbk2ThumbnailCmdBase::ContactRelocatedL
        (MVPbkStoreContact* aRelocatedContact)
    {
    iStoreContact = aRelocatedContact; // take ownership
    }

void CPbk2ThumbnailCmdBase::ContactRelocationFailed
        (TInt aReason, MVPbkStoreContact* aContact)
    {
    // Take the contact back
    iStoreContact = aContact;
    if ( aReason == KErrCancel )
        {
        // No error note is to be shown to the user when she
        // manually cancels the relocation process, therefore
        // the error code must be converted
        aReason = KErrNone;
        }
    ProcessDismissed(aReason);
    }

void CPbk2ThumbnailCmdBase::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Do nothing
    }

void CPbk2ThumbnailCmdBase::RelocationProcessComplete()
    {
    TRAPD( res, iStoreContact->LockL( *this ) );
    if ( res != KErrNone )
        {
        ProcessDismissed( res );
        }
    }

void CPbk2ThumbnailCmdBase::ReadFieldTypeL()
	{
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::ReadFieldTypeL(0x%x), start"), this);
	
	const MVPbkFieldTypeList& masterFieldTypeList = 
    	iContactManager->FieldTypes();
	RPbk2LocalizedResourceFile resFile(*CCoeEnv::Static());
    resFile.OpenLC(KPbk2RomFileDrive, 
                   KDC_RESOURCE_FILES_DIR, KPbk2CommandsResFile);
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, R_THUMBNAIL_FIELD_TYPE);
    const TInt count = reader.ReadInt16();
    
    __ASSERT_DEBUG(count == KAmountOfFieldTypesInResFile, 
    	Panic(EPanicCorruptedResFile));
    
    TVPbkFieldTypeMapping mapping(reader);
    iFieldType = mapping.FindMatch(masterFieldTypeList);
    if (!iFieldType)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PopAndDestroy(2); // resFile, CreateResourceReaderLC

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ThumbnailCmdBase::ReadFieldTypeL(0x%x), end"), this);
	}

inline TBool CPbk2ThumbnailCmdBase::RelocateContactL()
    {
    __ASSERT_DEBUG( iStoreContact, Panic( EPanicLogic_RelocateContactL ) );
    TBool ret = EFalse;

    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    if ( !iContactRelocator->IsPhoneMemoryContact( *iStoreContact ) &&
        iContactRelocator->IsPhoneMemoryInConfigurationL() )
        {
        // Asynchronously relocate contact
        iContactRelocator->RelocateContactL( iStoreContact, *this,
            Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery,
            CPbk2ContactRelocator::EPbk2RelocatorExistingContact );
        iStoreContact = NULL;    // Ownership was taken
        ret = ETrue;        // Indicate that contact needs relocating
        }

    return ret;
    }    
	
//  End of File  
