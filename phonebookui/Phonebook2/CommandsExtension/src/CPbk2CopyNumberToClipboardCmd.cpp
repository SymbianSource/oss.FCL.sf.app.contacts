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
* Description:  A command for clipboard copy
*
*/


// INCLUDES
#include "CPbk2CopyNumberToClipboardCmd.h"

#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactFieldTextData.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <Pbk2Commands.rsg>
#include <Pbk2CmdExtRes.rsg>

#include <txtetext.h>   // CPlainText
#include <baclipb.h>    // CClipboard
#include <StringLoader.h>
#include <aknnotewrappers.h>    // CAknInformationNote


// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CopyNumberToClipboardCmd* CPbk2CopyNumberToClipboardCmd::NewL(
        MPbk2ContactUiControl& aUiControl   )
    {
    CPbk2CopyNumberToClipboardCmd* self = 
        new ( ELeave ) CPbk2CopyNumberToClipboardCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::CPbk2CopyNumberToClipboardCmd
// --------------------------------------------------------------------------
//
CPbk2CopyNumberToClipboardCmd::CPbk2CopyNumberToClipboardCmd(
        MPbk2ContactUiControl& aUiControl) : 
    iUiControl(&aUiControl)
    {
    // do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::~CPbk2CopyNumberToClipboardCmd
// --------------------------------------------------------------------------
//    
CPbk2CopyNumberToClipboardCmd::~CPbk2CopyNumberToClipboardCmd()
    {
    delete iStoreContact;
    delete iRetrieveOperation;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CopyNumberToClipboardCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CopyNumberToClipboardCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);
    
    if (iUiControl)
        {
        MVPbkContactLink* link = iUiControl->FocusedContactL()->CreateLinkLC();
        iRetrieveOperation = 
            iAppServices->ContactManager().RetrieveContactL(*link, *this);
        CleanupStack::PopAndDestroy(); // link
        }
    else
        {
        iObserver->CommandFinished(*this);                     
        }
    CleanupStack::Pop(this);
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::AddObserver
// --------------------------------------------------------------------------
//    
void CPbk2CopyNumberToClipboardCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::ResetUiControl
// --------------------------------------------------------------------------
//    
void CPbk2CopyNumberToClipboardCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CopyNumberToClipboardCmd::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        
        delete iStoreContact;
        iStoreContact = aContact;
        
        TRAP_IGNORE(CopyNumberToClipboardL());

        if (iUiControl)
            {
            iUiControl->UpdateAfterCommandExecution();
            }
        iObserver->CommandFinished(*this);                     
        }    
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2CopyNumberToClipboardCmd::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& aOperation, 
    TInt /*aError*/)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        if (iUiControl)
            {
            iUiControl->UpdateAfterCommandExecution();
            }
        iObserver->CommandFinished(*this);
        } 
    }

// --------------------------------------------------------------------------
// CPbk2CopyNumberToClipboardCmd::CopyNumberToClipboardL
// --------------------------------------------------------------------------
//    
void CPbk2CopyNumberToClipboardCmd::CopyNumberToClipboardL()
    {
    // Construct a plain text from the number
    const TInt KBeginning = 0;
    
    MVPbkStoreContactField* field = FocusedFieldLC( *iStoreContact );
    TPtrC number = MVPbkContactFieldTextData::Cast(field->FieldData()).Text();   
    CleanupStack::PopAndDestroy(); // field 
    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL(plainText);
    plainText->InsertL(KBeginning, number);

    // Copy to clipboard
    CClipboard* cb = CClipboard::NewForWritingLC(
        iAppServices->ContactManager().FsSession());
    plainText->CopyToStoreL(cb->Store(), cb->StreamDictionary(),
        KBeginning, plainText->DocumentLength());
    cb->CommitL();

    CleanupStack::PopAndDestroy(2); // cb, plainText
    
    // Show a note
    HBufC* prompt = StringLoader::LoadLC( R_QTN_CCA_INFO_NOTE_NUMBER_COPIED_TO_CLIPBOARD );
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(); // prompt        
    }
    	
MVPbkStoreContactField* CPbk2CopyNumberToClipboardCmd::FocusedFieldLC
    ( MVPbkStoreContact& aStoreContact )
    {
    TInt index = KErrNotFound;
    CPbk2PresentationContact* presentationContact = 
        CPbk2PresentationContact::NewL( aStoreContact,
            iAppServices->FieldProperties() );
    CleanupStack::PushL( presentationContact );

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

// End of File
