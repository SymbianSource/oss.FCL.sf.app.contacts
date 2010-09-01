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
* Description:  A command for setting ringing tone.
*
*/


// INCLUDE FILES
#include "CPguSetToneCmd.h"

// Phonebook 2
#include <CPbk2ContactRelocator.h>
#include <MPbk2ContactUiControl.h>
#include <Pbk2GroupUIRes.rsg>
#include <Pbk2CommonUi.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactLink.h>

// System includes
#include <barsread.h>
#include <barsc.h>
#include <coemain.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

// Debugging headers
#include <Pbk2Debug.h>

// ============================ LOCAL FUNCTIONS =============================

namespace 
    {
    const TInt KFirstElement( 0 );
    
    #ifdef _DEBUG
    	enum TPanicCode
    	    {
    	    EPanicLogic_ReadFieldTypeL,
    	    EPanicLogic_ContactCommitCompelete,
    	    EPanicPreCond_RelocationProcessCompleteL
    	    };

    	void Panic(TInt aReason)
    	    {
    	    _LIT(KPanicText, "CPguSetToneCmd");
    	    User::Panic(KPanicText, aReason);
    	    }
    #endif // _DEBUG	    
    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPguSetToneCmd::CPguSetToneCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPguSetToneCmd::CPguSetToneCmd(
        MPbk2ContactUiControl& aUiControl ) :  
    CPbk2SetToneBase( aUiControl ),
    iIsStarted( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::~CPguSetToneCmd
// --------------------------------------------------------------------------
//
CPguSetToneCmd::~CPguSetToneCmd()
    {
    if ( iContacts )
        {
        iContacts->ResetAndDestroy();
        }
    delete iContacts;
    delete iCurrentContact;
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CPguSetToneCmd* CPguSetToneCmd::NewL(
    MPbk2ContactUiControl& aUiControl)
    {
    CPguSetToneCmd* self = 
        new( ELeave ) CPguSetToneCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ConstructL()
    {    
    BaseConstructL();
    }
	
// --------------------------------------------------------------------------
// CPguSetToneCmd::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ContactRelocatedL
    ( MVPbkStoreContact* aRelocatedContact )
    {
    // Takes ownership of the aRelocatedContact
    delete iCurrentContact;
    iCurrentContact = aRelocatedContact;
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ContactRelocationFailed
        (TInt aReason, MVPbkStoreContact* aContact)
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
// CPguSetToneCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ContactsRelocationFailed
        ( TInt /*aReason*/, CVPbkContactLinkArray* /*aContacts*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::RelocationProcessComplete()
    {
    __ASSERT_DEBUG( iCurrentContact, 
        Panic( EPanicPreCond_RelocationProcessCompleteL ) );
    // Lock contact for setting ringingtone
    TRAPD( res, iCurrentContact->LockL( *this ) );
    if ( res != KErrNone )
        {
        ProcessDismissed( res );
        }
    }
	
// --------------------------------------------------------------------------
// CPguSetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    if ( !iStoreContact )
        {
        // This is the main (group) contact
       	iStoreContact = aContact;        
       	iGroup = iStoreContact->Group(); 
       	// If leaves the iContacts will be not created
       	TRAP_IGNORE(
       	    {
           	CreateLinkArrayL( iGroup->ItemsContainedLC() );       	    
           	CleanupStack::PopAndDestroy(); // group items
       	    });
       	
       	if ( iContacts && iContacts->Count() > 0 )
       	    {
            TRAPD( err, RetrieveContactL( iContacts->At( KFirstElement ) ) );
            if ( err )
                {
                ProcessDismissed( err );
                }
       	    }
       	else
       	    {
       	    // There is no contacts linked to group.
       	    // Stop the process
            // Show note
            // If leave occurs the note just go on and exit the command
            TRAP_IGNORE( ConfirmationL() );
            // Now, its time to exit the command
            IssueStopRequest();                   	    
       	    }
        }
    else
        {
        // Here we are retrieved a contact from group
        delete iCurrentContact;
        iCurrentContact = NULL;
        
        iCurrentContact = aContact;
        
        TInt err = KErrNone;
        if ( !iIsStarted )
            {
            iIsStarted = ETrue;
            }
            
        TBool contactNeedsRelocation = EFalse;
        TRAP( err, contactNeedsRelocation = RelocateContactL( iCurrentContact ) );
        if (err != KErrNone)
            {
            ProcessDismissed( err );
            }

        if ( !contactNeedsRelocation )
            {
            TRAP( err, iCurrentContact->LockL( *this ) );
            if ( err != KErrNone )
                {
                ProcessDismissed( err );
                }
            }
        else
            {
            // Relocator has taken the ownership of the contact
            iCurrentContact = NULL;
            }   	                
        }   	
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt aError )
    {
    ProcessDismissed( aError );
    }
      
// --------------------------------------------------------------------------
// CPguSetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ContactOperationCompleted( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == EContactLock )
        {
        TRAPD( err, SetToneAndCommitL( iCurrentContact ) );        
        if ( err != KErrNone )
            {
            ProcessDismissed( err );
            }
        }
    else if ( aResult.iOpCode == EContactCommit )
        {
        // Remove first item 'cause it has already been gone through
        iContacts->Delete( KFirstElement );
        if ( iContacts->Count() > 0 )
            {
            TRAPD( err, RetrieveContactL( iContacts->At( KFirstElement ) ) );
            if ( err )
                {
                ProcessDismissed( err );
                }
            }
        else
            {
            // Show note
            TRAP_IGNORE( ConfirmationL() );
            // Now, its time to exit the command
            IssueStopRequest();            
            }
        }    
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::RelocationProcessCompleteL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ContactOperationFailed
    (TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    ProcessDismissed( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::CreateLinkArrayL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::CreateLinkArrayL( MVPbkContactLinkArray* aArray )
    {
    if ( iContacts )
        {
        iContacts->ResetAndDestroy();
        delete iContacts;
        iContacts = NULL;
        }
    
    iContacts = CVPbkContactLinkArray::NewL();
    TInt count( aArray->Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        iContacts->AppendL( ( *aArray )[i].CloneLC() );
        CleanupStack::Pop(); // clone
        }
    }

// --------------------------------------------------------------------------
// CPguSetToneCmd::ConfirmationNoteL
// --------------------------------------------------------------------------
//
void CPguSetToneCmd::ConfirmationL()
	{
    TParse toneName;
    toneName.Set( iToneFilename, NULL, NULL );	
    HBufC* noteText = NULL;
    if ( toneName.Name().Length() == 0 )
        {
        HBufC* defaultName = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_PHOP_SELI_DEFAULT_RTONE );
        noteText = StringLoader::LoadL(
            R_QTN_PHOB_NOTE_RTONE_SET_TO_GRP, *defaultName );
        CleanupStack::PopAndDestroy(); // defaultName
        CleanupStack::PushL(noteText);
        }
    else
        {
        noteText = StringLoader::LoadLC(
            R_QTN_PHOB_NOTE_RTONE_SET_TO_GRP, toneName.Name() );
        }

    CAknConfirmationNote* noteDlg = 
        new (ELeave) CAknConfirmationNote( ETrue );
    noteDlg->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy(); // notetext	
	}

//  End of File  
