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
* Description:  A command for editing a contact
*
*/



// INCLUDE FILES
#include "CPbk2EditContactCmd.h"

// Phonebook 2
#include <TPbk2ContactEditorParams.h>
#include <CPbk2ContactEditorDlg.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkUtils.h>

// System includes
#include <coemain.h>


#ifdef _DEBUG
namespace 
{
enum TPanicCode
    {
    EExecuteLD_PreCond,
    EVPbkSingleContactOperationComplete_PreCond,
    EPanicPreCond_Null_Pointer
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "k2EditContactCmd");
    User::Panic(KPanicText, aReason);
    }
}
#endif // _DEBUG

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::CPbk2EditContactCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPbk2EditContactCmd::CPbk2EditContactCmd(
        MPbk2ContactUiControl& aUiControl) :   
    iUiControl( &aUiControl ),
    iIsEditingAborted( EFalse )
    {
    iUiControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::~CPbk2EditContactCmd
// --------------------------------------------------------------------------
//
CPbk2EditContactCmd::~CPbk2EditContactCmd()
    {   
    delete iContactLink;
    delete iContact;
    delete iContactRetriever;
    if ( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CPbk2EditContactCmd* CPbk2EditContactCmd::NewL(
        MPbk2ContactUiControl& aUiControl)
    {
    CPbk2EditContactCmd* self = new( ELeave ) CPbk2EditContactCmd(aUiControl);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    const MVPbkBaseContact* contact = iUiControl->FocusedContactL();
    __ASSERT_DEBUG(contact, Panic(EPanicPreCond_Null_Pointer));
    
    User::LeaveIfNull( contact );
    
    iContactLink = contact->CreateLinkLC();
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);
    iContactRetriever = iAppServices->ContactManager().RetrieveContactL(
            *iContactLink, *this);
    CleanupStack::Pop(this);
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    __ASSERT_DEBUG(aContact, 
        Panic(EVPbkSingleContactOperationComplete_PreCond));
        
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));

    if (aContact)
        {
        iContact = aContact;
        TRAPD( error, iContact->LockL(*this) );
        if ( error != KErrNone )
            {
            iCommandObserver->CommandFinished(*this);
            CCoeEnv::Static()->HandleError(error);
            }        
        }
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::VPbkSingleContactOperationFailed
        (MVPbkContactOperationBase& /*aOperation*/,  TInt aError)
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));
    
    iCommandObserver->CommandFinished(*this);
    CCoeEnv::Static()->HandleError(aError);
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::ContactOperationCompleted
        (TContactOpResult /*aResult*/)
    {
    // open editing dialog
    TRAPD(result, ExecuteEditorL());
    iCommandObserver->CommandFinished(*this);
    if (result != KErrNone)
        {
        CCoeEnv::Static()->HandleError(result);
        }
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::ContactOperationFailed
        (TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));
    
    iCommandObserver->CommandFinished(*this);
    CCoeEnv::Static()->HandleError(aErrorCode);
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ContactEditingComplete
// --------------------------------------------------------------------------
//    
void CPbk2EditContactCmd::ContactEditingComplete
        (MVPbkStoreContact* aEditedContact)
    {
    iContact = aEditedContact;
    }
    
// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ContactEditingDeletedContact
// --------------------------------------------------------------------------
//    
void CPbk2EditContactCmd::ContactEditingDeletedContact
        (MVPbkStoreContact* aEditedContact)
    {
    iContact = aEditedContact;
    }

// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ContactEditingAborted
// --------------------------------------------------------------------------
//
void CPbk2EditContactCmd::ContactEditingAborted()
    {
    iIsEditingAborted = ETrue;
    }
    
// --------------------------------------------------------------------------
// CPbk2EditContactCmd::ExecuteEditorL
// --------------------------------------------------------------------------
//    
void CPbk2EditContactCmd::ExecuteEditorL()
    {
    // verify syncronization field existance and content
    VPbkUtils::VerifySyncronizationFieldL(
            CCoeEnv::Static()->FsSession(),
            iContact->ParentStore().StoreProperties().SupportedFields(), 
            *iContact );
    
    // adjust focus
    TInt focusedFieldIndex = KErrNotFound;
    if (iUiControl)
        {
        focusedFieldIndex = iUiControl->FocusedFieldIndex();
        }

    TPbk2ContactEditorParams params;
    if ( focusedFieldIndex != KErrNotFound )
        {
        params.iFocusedContactField = 
            FocusedFieldLC( *iContact );
        }
    
    // create and execute editing dialog
    CPbk2ContactEditorDlg* dlg =
        CPbk2ContactEditorDlg::NewL( params, iContact, *this );
    iContact = NULL; // ownership went to editor
    dlg->ExecuteLD();
    
    // clean after dialog execution
    if ( focusedFieldIndex != KErrNotFound )
        {
        CleanupStack::PopAndDestroy(); //params.iFocusedContactField
        }
    
    if (iUiControl)
        {
        iUiControl->ResetFindL();
        
        // If editing has been aborted we don't set focused contact.
        if ( iContact && !iIsEditingAborted)
            {
            // It is possible that the contact got changed while in edit
            iUiControl->SetFocusedFieldIndex( params.iFocusedIndex );
            iUiControl->SetFocusedContactL( *iContact );        
            }

        // If contact got deleted in the editor, this call ensures
        // that the contact info view is switched to the names list
        iUiControl->UpdateAfterCommandExecution();
        }
    }

MVPbkStoreContactField* CPbk2EditContactCmd::FocusedFieldLC
    ( MVPbkStoreContact& aStoreContact )
    {
    CPbk2PresentationContact* presentationContact = 
            CPbk2PresentationContact::NewL( aStoreContact, 
                iAppServices->FieldProperties() );
    CleanupStack::PushL( presentationContact );

    TInt index = KErrNotFound;
    if (iUiControl)
        {
        index = presentationContact->PresentationFields().StoreIndexOfField( 
                iUiControl->FocusedFieldIndex() );
        }

    CleanupStack::PopAndDestroy( presentationContact );
    
    if ( index != KErrNotFound )
        {
        // Use FieldAtLC to avoid the unvalidity of the field after new
        // FieldAt call.        
        return aStoreContact.Fields().FieldAtLC( index );
        }
    return NULL;
    }    

//  End of File  
