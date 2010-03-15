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
* Description:  Remove speed dial command object.
*
*/


#include "CPbk2RemoveSpeedDialCmd.h"

// From Phonebook 2
#include "MPbk2CommandObserver.h"
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <Pbk2CmdExtRes.rsg>
#include <CPbk2AppUiBase.h>

// From Virtual Phonebook
#include <CVPbkContactIdConverter.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactAttributeManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactLink.h>

#include <coemain.h>
#include <AknQueryDialog.h>

/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPreCond_Null_Pointer
        };

    void Panic(TInt aReason)
        {
        _LIT(KPanicText, "CPbk2RemoveSpeedDialCmd");
        User::Panic(KPanicText, aReason);
        }
#endif  // _DEBUG

} // namespace

// ============================== MEMBER FUNCTIONS ==========================

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::CPbk2RemoveSpeedDialCmd
// --------------------------------------------------------------------------
//
CPbk2RemoveSpeedDialCmd::CPbk2RemoveSpeedDialCmd(
        MPbk2ContactUiControl& aUiControl) :
    iUiControl(&aUiControl)
    {
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2RemoveSpeedDialCmd* CPbk2RemoveSpeedDialCmd::NewL(
        MPbk2ContactUiControl& aUiControl)
    {
    CPbk2RemoveSpeedDialCmd* self = new(ELeave) CPbk2RemoveSpeedDialCmd(
            aUiControl);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::~CPbk2RemoveSpeedDialCmd
// --------------------------------------------------------------------------
//
CPbk2RemoveSpeedDialCmd::~CPbk2RemoveSpeedDialCmd()
    {
    delete iStoreContact;
    delete iOperation;
    Release( iAppServices );
    
    delete iContactLink;
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2RemoveSpeedDialCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::ExecuteLD
// --------------------------------------------------------------------------
//    
void CPbk2RemoveSpeedDialCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);
    
    iContactLink = iUiControl->FocusedContactL()->CreateLinkLC();
    CleanupStack::Pop(); // iContactLink
    iContactLink->ContactStore().OpenL( *this );

    iContactLink->ContactStore().Close( *this );
    
    if ( iUiControl )
        {
        iOperation = iAppServices->ContactManager().RetrieveContactL(
                *iContactLink, *this);
        }
    else
        {
        FinishCommand( KErrNotFound );
        }
    
    delete iContactLink;
    iContactLink = NULL;

    CleanupStack::Pop(this);
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::AddObserver
// --------------------------------------------------------------------------
//    
void CPbk2RemoveSpeedDialCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::ResetUiControl
// --------------------------------------------------------------------------
//   
void CPbk2RemoveSpeedDialCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2RemoveSpeedDialCmd::StoreReady( 
		MVPbkContactStore& /*aContactStore*/ )
	{
	}

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2RemoveSpeedDialCmd::StoreUnavailable( 
		MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
	{
	}

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2RemoveSpeedDialCmd::HandleStoreEventL( 
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent aStoreEvent )
	{
	if ( iContactLink && 
		aStoreEvent.iContactLink->IsSame( *iContactLink ) )
		{
		switch( aStoreEvent.iEventType )
			{
			case TVPbkContactStoreEvent::EContactDeleted: // FALLTHROUGH
			case TVPbkContactStoreEvent::EContactChanged: // FALLTHROUGH
				
			default:
				break;
			}
		}
	}

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//   
void CPbk2RemoveSpeedDialCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    delete iOperation;
    iOperation = NULL;

    delete iStoreContact;    
    iStoreContact = aContact;
    
    TRAPD(error, iStoreContact->LockL(*this));
    if (error != KErrNone)
        {
        FinishCommand(error);
        }
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2RemoveSpeedDialCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt aError)
    {
    delete iOperation;
    iOperation = NULL;
    
    FinishCommand(aError);
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//    
void CPbk2RemoveSpeedDialCmd::ContactOperationCompleted
    (TContactOpResult aResult)
    {
    if (aResult.iOpCode == EContactCommit)
        {
        FinishCommand(KErrNone);        
        }
    else if (aResult.iOpCode == EContactLock)
        {    
        TInt index = KErrNotFound;
        TInt err = KErrNone;
        
        if (iUiControl)
            {
            index = iUiControl->FocusedFieldIndex();
            }
    
        if ( index != KErrNotFound )
            {
            MVPbkStoreContactField& field = 
                iStoreContact->Fields().FieldAt( index );            

            TBool hasAttribute = EFalse;
            TRAP(err, hasAttribute = 
                iAppServices->ContactManager().
                    ContactAttributeManagerL().
                HasFieldAttributeL(CVPbkSpeedDialAttribute::Uid(), field));
                
            if (hasAttribute)
                {
                TRAP(err, iOperation = iAppServices->ContactManager().
                    ContactAttributeManagerL().RemoveFieldAttributeL(
                    field, CVPbkSpeedDialAttribute::Uid(), *this) );
                }
            else
                {
                FinishCommand( err );
                }
            }        
            
        if (err != KErrNone)
            {
            FinishCommand( err );
            }        
        }
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2RemoveSpeedDialCmd::ContactOperationFailed
    (TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    delete iOperation;
    iOperation = NULL;
    
    FinishCommand(aErrorCode);    
    }    

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::AttributeOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2RemoveSpeedDialCmd::AttributeOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/)
    {
    delete iOperation;
    iOperation = NULL;

    TRAP_IGNORE(iStoreContact->CommitL(*this));
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::AttributeOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2RemoveSpeedDialCmd::AttributeOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, TInt aError)
    {
    delete iOperation;
    iOperation = NULL;
    
    FinishCommand(aError);
    }

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::FinishCommand
// --------------------------------------------------------------------------
//        
void CPbk2RemoveSpeedDialCmd::FinishCommand(TInt aError)
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));
    
    TInt error( aError );
    if (iUiControl && error == KErrNone)
        {
        CPbk2AppUiBase<CAknViewAppUi>& appUi = static_cast<CPbk2AppUiBase<CAknViewAppUi>&>
                                               ( *CEikonEnv::Static()->EikAppUi() );
        // If appUi is not in foreground, It means phonebook2 has hidden itself in background,
        // no need to reload its contact, only phonebook2 in foreground case, need to reload
        // call SetFocusedContactL        
        if ( appUi.IsForeground() )
            {
            TRAP( error, 
                 iUiControl->SetFocusedContactL(
                      *iUiControl->FocusedStoreContact() ) );
                
            iUiControl->SetFocusedFieldIndex( iUiControl->FocusedFieldIndex() );
            iUiControl->UpdateAfterCommandExecution();	
            }
        }

    iCommandObserver->CommandFinished(*this);

	if (error != KErrNone)	
		{
		CCoeEnv::Static()->HandleError(error);
		}        
    }    

// --------------------------------------------------------------------------
// CPbk2RemoveSpeedDialCmd::ToStoreFieldIndexL
// --------------------------------------------------------------------------
//  
TInt CPbk2RemoveSpeedDialCmd::ToStoreFieldIndexL(
		TInt aPresIndex,
		const MVPbkStoreContact* aStoreContact )
    {
    // It is ok to remove const, we don't make any changes
    MVPbkStoreContact* storeContact =
        const_cast<MVPbkStoreContact*>( aStoreContact );

    TInt index( KErrNotFound );
    CPbk2PresentationContact* presentationContact = 
        CPbk2PresentationContact::NewL( 
            *storeContact,
            iAppServices->FieldProperties() );
    CleanupStack::PushL( presentationContact );

    index = presentationContact->PresentationFields().StoreIndexOfField
                ( aPresIndex );
    
    CleanupStack::PopAndDestroy( presentationContact );
    
    return index;
    } 

// --------------------------------------------------------------------------
// CPbk2CommandHandler::HasSpeedDialL
// --------------------------------------------------------------------------
//
TBool CPbk2RemoveSpeedDialCmd::HasSpeedDialL()
    {
    TBool hasAttribute(EFalse);

    const MVPbkStoreContact* storeContact =
            iUiControl->FocusedStoreContact();

   
    TInt index = iUiControl->FocusedFieldIndex();


    if ( index != KErrNotFound )
        {
        const MVPbkStoreContactField& field =
            storeContact->Fields().FieldAt( index );

        hasAttribute =
        	iAppServices->ContactManager().ContactAttributeManagerL().
                HasFieldAttributeL(
                    CVPbkSpeedDialAttribute::Uid(),
                    field );
        }
    return hasAttribute;
    }

// End of File
