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
*           Base class for image commands set, view and remove.
*
*/


// INCLUDE FILES
#include "CPbk2ImageCmdBase.h"
#include "CPbk2FieldFocusHelper.h"

// Phonebook 2
#include <CPbk2ImageManager.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2CommandObserver.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MPbk2ContactUiControl.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <vpbkeng.rsg>

// System headers
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicInvalidOpCode,
        EPanicLogic_RelocateContactL
        };

    void Panic(TInt aReason)
        {
        _LIT(KPanicText, "CPbk2ImageCmdBase");
        User::Panic(KPanicText, aReason);
        }
#endif  // _DEBUG

} // namespace


CPbk2ImageCmdBase::CPbk2ImageCmdBase
        (MPbk2ContactUiControl& aUiControl) :
    iUiControl(&aUiControl)
    {
    }

CPbk2ImageCmdBase::~CPbk2ImageCmdBase()
    {
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    delete iFieldFocusHelper;
    delete iImageManager;
    delete iContactLink;
    delete iRetrieveOperation;
    delete iStoreContact;
    delete iContactRelocator;
    }

void CPbk2ImageCmdBase::BaseConstructL()
    {
    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();

    iContactManager =
        &(appServices->ContactManager());

    iContactLink = iUiControl->FocusedContactL()->CreateLinkLC();
    CleanupStack::Pop(); // iContactLink

    iImageManager = CPbk2ImageManager::NewL(*iContactManager);
    iFieldFocusHelper = CPbk2FieldFocusHelper::NewL( *iUiControl, 
        appServices->FieldProperties() );

    CleanupStack::PopAndDestroy(); // appServices

    iUiControl->RegisterCommand( this );        
    }

void CPbk2ImageCmdBase::ProcessDismissed(TInt aError)
    {
    if ( aError != KErrNone && 
         aError != KLeaveExit )
        {
        // Report the failure to the user
        CCoeEnv::Static()->HandleError( aError );
        }

    if (iUiControl)
        {
        // Not matter if saved field can not focused
        TRAP_IGNORE( iFieldFocusHelper->RestoreSavedFieldL() );
        iUiControl->UpdateAfterCommandExecution();
        }

    if (iCommandObserver)
        {
        // Notify command owner that the command has finished
        iCommandObserver->CommandFinished(*this);
        }
    }

void CPbk2ImageCmdBase::ExecuteLD()
    {
    CleanupStack::PushL(this);
    ReadFieldTypesL();
    iRetrieveOperation = iContactManager->RetrieveContactL(
        *iContactLink, *this);
    CleanupStack::Pop(this);
    }

void CPbk2ImageCmdBase::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

void CPbk2ImageCmdBase::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

void CPbk2ImageCmdBase::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    __ASSERT_DEBUG((aResult.iOpCode == EContactCommit ||
            aResult.iOpCode == EContactLock),
            Panic(EPanicInvalidOpCode));

    if (aResult.iOpCode == EContactLock)
        {
        // Not matter, if focusing not success
        TRAP_IGNORE( iFieldFocusHelper->SaveInitiallyFocusedFieldL( *iStoreContact ) );
        TBool result (EFalse);
        TRAPD(err, result = ExecuteCommandL());
        if (err != KErrNone || !result)
            {
            ProcessDismissed(err);
            }
        }
    else
        {
        ProcessDismissed(KErrNone);
        }
    }

void CPbk2ImageCmdBase::ContactOperationFailed
    ( TContactOp PBK2_DEBUG_ONLY( aOpCode ), TInt aErrorCode,
      TBool /*aErrorNotified*/ )
    {
    __ASSERT_DEBUG( ( aOpCode == EContactCommit || aOpCode == EContactLock ),
        Panic( EPanicInvalidOpCode ) );

    ProcessDismissed( aErrorCode );
    }

/**
 * Called when retrieve operation completes.
 */
void CPbk2ImageCmdBase::VPbkSingleContactOperationComplete(
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
    TRAPD( err, contactNeedsRelocation = RelocateContactL());
    if (err == KErrNone && !contactNeedsRelocation)
        {
        TRAP(err, iStoreContact->LockL(*this));
        }
    if (err != KErrNone)
        {
        ProcessDismissed(err);
        }
    }

/**
 * Called when retrieve operation fails.
 */
void CPbk2ImageCmdBase::VPbkSingleContactOperationFailed(
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

inline TBool CPbk2ImageCmdBase::RelocateContactL()
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

void CPbk2ImageCmdBase::ContactRelocatedL
        (MVPbkStoreContact* aRelocatedContact)
    {
    iStoreContact = aRelocatedContact; // take ownership
    }

void CPbk2ImageCmdBase::ContactRelocationFailed
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

void CPbk2ImageCmdBase::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Do nothing
    }

void CPbk2ImageCmdBase::RelocationProcessComplete()
    {
    // now lock the contact to start the actual process
    TRAPD( res, iStoreContact->LockL( *this ) );
    if ( res != KErrNone )
        {
        ProcessDismissed( res );
        }
    }

void CPbk2ImageCmdBase::ReadFieldTypesL()
    {
    iThumbnailFieldType =
        iContactManager->FieldTypes().Find(R_VPBK_FIELD_TYPE_THUMBNAILPIC);
    iImageFieldType =
        iContactManager->FieldTypes().Find(R_VPBK_FIELD_TYPE_CALLEROBJIMG);
        
    if (!iThumbnailFieldType || !iImageFieldType)
        {
        User::Leave(KErrNotFound);
        }
    }
    
TInt CPbk2ImageCmdBase::FindFieldIndex(const MVPbkFieldType& aFieldType)
	{
	if( iStoreContact )
	    {
    	const MVPbkBaseContactFieldCollection& fields = iStoreContact->Fields();	
    	const TInt fieldCount = fields.FieldCount();
        
        for (TInt i = 0; i < fieldCount; ++i)
            {
            const MVPbkFieldType* fieldType = 
                fields.FieldAt(i).BestMatchingFieldType();
            if (fieldType && fieldType->IsSame(aFieldType))
                {
                return i;
                }
            }
	    }
    return KErrNotFound;
	}    

//  End of File
