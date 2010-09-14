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
* Description:  A command for setting ringing tone.
*
*/


// INCLUDE FILES
#include "CPbk2SetToneCmd.h"

#include <MPbk2CommandObserver.h>
#include <CVPbkContactManager.h>
#include <MPbk2ContactUiControl.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <CPbk2ContactRelocator.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <MVPbkContactOperationBase.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldFocusHelper.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include "CPbk2RingingToneFetch.h"

#include <pbk2uicontrols.rsg>
#include <coemain.h>

#include <Pbk2Debug.h>

// ============================ LOCAL FUNCTIONS =============================

namespace 
    {    
    #ifdef _DEBUG
    	enum TPanicCode
    	    {
    	    EPanicLogic_ReadFieldTypeL,
    	    EPanicLogic_ContactCommitCompelete,
    	    EPanicPreCond_RelocationProcessCompleteL
    	    };

    	void Panic(TInt aReason)
    	    {
    	    _LIT(KPanicText, "CPbk2SetToneCmd");
    	    User::Panic(KPanicText, aReason);
    	    }
    #endif // _DEBUG	    
    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::CPbk2SetToneCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPbk2SetToneCmd::CPbk2SetToneCmd(
        MPbk2ContactUiControl& aUiControl ) :  
    CPbk2SetToneBase( aUiControl )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::~CPbk2SetToneCmd
// --------------------------------------------------------------------------
//
CPbk2SetToneCmd::~CPbk2SetToneCmd()
    {
    delete iFieldFocusHelper;
    iFieldFocusHelper = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CPbk2SetToneCmd* CPbk2SetToneCmd::NewL(
        MPbk2ContactUiControl& aUiControl)
    {
    CPbk2SetToneCmd* self = 
        new( ELeave ) CPbk2SetToneCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::ConstructL()
    {
    BaseConstructL();
    iFieldFocusHelper = CPbk2FieldFocusHelper::NewL( 
        *iUiControl,
        Phonebook2::Pbk2AppUi()->ApplicationServices().FieldProperties() );
    }
	
// --------------------------------------------------------------------------
// CPbk2SetToneCmd::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::ContactRelocatedL(
        MVPbkStoreContact* aRelocatedContact)
    {
    // Takes ownership of the aRelocatedContact
    delete iStoreContact;
    iStoreContact = aRelocatedContact;
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::ContactRelocationFailed(
        TInt aReason, MVPbkStoreContact* aContact)
    {
    // We got the ownership of aContact
    delete aContact;
    if ( aReason == KErrCancel )
        {
        // No error note is to be shown to the user when she
        // manually cancels the relocation process, therefore
        // the error code must be converted
        aReason = KErrNone;
        }
    ProcessDismissed( aReason );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::ContactsRelocationFailed(
        TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::RelocationProcessComplete()
    {
    __ASSERT_DEBUG( iStoreContact, 
        Panic( EPanicPreCond_RelocationProcessCompleteL ) );
    // Lock contact for setting tone
    TRAPD( res, iStoreContact->LockL( *this ) );
    if ( res != KErrNone )
        {
        ProcessDismissed( res );
        }
    }
	
// --------------------------------------------------------------------------
// CPbk2SetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iStoreContact;    
   	iStoreContact = aContact;
   	
    if ( !ContactHasRingToneField( iStoreContact ) && iToneFilename.Length() <=0 )
        {
        // User has selected 'no personal ringtone' for contact which 
        // does not have ring tone field, so we can quit now
        ProcessDismissed( KErrNone );
        }
    else
        {
        TBool contactNeedsRelocation = EFalse;
        TInt err = KErrNone;
        TRAP( err, contactNeedsRelocation = 
                    RelocateContactL( iStoreContact ) );
                 
        if (err != KErrNone)
            {
            ProcessDismissed( err );
            }

        if ( !contactNeedsRelocation )
            {
            TRAP( err, iStoreContact->LockL( *this ) );
            if ( err != KErrNone )
                {
                ProcessDismissed( err );
                }
            }
        else
            {
            // Relocator has taken the ownership of the contact
            iStoreContact = NULL;
            }   	    
        }
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt aError )
    {
    ProcessDismissed( aError );
    }
      
// --------------------------------------------------------------------------
// CPbk2SetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::ContactOperationCompleted(
        TContactOpResult aResult)
    {
    if ( aResult.iOpCode == EContactLock )
        {
        // We have to save the initially focused field, so that we can put
        // it back focused after setting and committing ringingtone.
        // If leaves the focus will not be set.
        TRAP_IGNORE( iFieldFocusHelper->SaveInitiallyFocusedFieldL
                        ( *iStoreContact ) );        
        TRAPD( err, SetToneAndCommitL( iStoreContact ) );        
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            ProcessDismissed( err );
            }
        }
    else if ( aResult.iOpCode == EContactCommit )
        {
        // Set the original field focused after command execution     
        TRAP_IGNORE( iFieldFocusHelper->RestoreSavedFieldL() );
        // Now, its time to exit the command
        IssueStopRequest();
        }    
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPbk2SetToneCmd::ContactOperationFailed(
        TContactOp /*aOpCode*/, 
        TInt aErrorCode, 
        TBool /*aErrorNotified*/)
    {
    if ( aErrorCode != KErrNone )
        {
        CCoeEnv::Static()->HandleError( aErrorCode );
        }
    
    ProcessDismissed( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2SetToneCmd::ContactHasRingToneField
// --------------------------------------------------------------------------
//
inline TBool CPbk2SetToneCmd::ContactHasRingToneField( 
        MVPbkStoreContact* aContact  )
    {
    TBool ret( ETrue );

    TPbk2StoreContactAnalyzer analyzer
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
            aContact );
    TInt fieldIndex( KErrNotFound );
    // No harm done if field finding leaves
    // it just might produce unnecessary relocation note but nothing more
    TRAP_IGNORE( fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_RINGTONE_SELECTOR ) );
    if ( fieldIndex == KErrNotFound  )
        {
        ret = EFalse;
        }

    return ret;
    }

//  End of File  
