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
* Description:  A command for clipboard copy
*
*/


// INCLUDES
#include "CPbk2CopyAddressToClipboardCmd.h"

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
#include <MVPbkContactFieldUriData.h>

#include <txtetext.h>   // CPlainText
#include <baclipb.h>    // CClipboard
#include <StringLoader.h>
#include <aknnotewrappers.h>    // CAknInformationNote


// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CopyAddressToClipboardCmd* CPbk2CopyAddressToClipboardCmd::NewL(
        MPbk2ContactUiControl& aUiControl   )
    {
    CPbk2CopyAddressToClipboardCmd* self = 
        new ( ELeave ) CPbk2CopyAddressToClipboardCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::CPbk2CopyAddressToClipboardCmd
// --------------------------------------------------------------------------
//
CPbk2CopyAddressToClipboardCmd::CPbk2CopyAddressToClipboardCmd(
        MPbk2ContactUiControl& aUiControl) : 
    iUiControl(&aUiControl)
    {
    // do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::~CPbk2CopyAddressToClipboardCmd
// --------------------------------------------------------------------------
//    
CPbk2CopyAddressToClipboardCmd::~CPbk2CopyAddressToClipboardCmd()
    {
    delete iStoreContact;
    delete iRetrieveOperation;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CopyAddressToClipboardCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CopyAddressToClipboardCmd::ExecuteLD()
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
// CPbk2CopyAddressToClipboardCmd::AddObserver
// --------------------------------------------------------------------------
//    
void CPbk2CopyAddressToClipboardCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::ResetUiControl
// --------------------------------------------------------------------------
//    
void CPbk2CopyAddressToClipboardCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CopyAddressToClipboardCmd::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        
        delete iStoreContact;
        iStoreContact = aContact;
        
        TRAP_IGNORE(CopyAddressToClipboardL());

        if (iUiControl)
            {
            iUiControl->UpdateAfterCommandExecution();
            }
        iObserver->CommandFinished(*this);                     
        }    
    }

// --------------------------------------------------------------------------
// CPbk2CopyAddressToClipboardCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2CopyAddressToClipboardCmd::VPbkSingleContactOperationFailed(
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
// CPbk2CopyAddressToClipboardCmd::CopyAddressToClipboardL
// --------------------------------------------------------------------------
//    
void CPbk2CopyAddressToClipboardCmd::CopyAddressToClipboardL()
    {
    // Construct a plain text from the number
    const TInt KBeginning = 0;
    
    MVPbkStoreContactField* field = FocusedFieldLC( *iStoreContact );
    TPtrC address;
    TVPbkFieldStorageType fieldType = field->FieldData().DataType();
    
    switch(fieldType)
        {
        case EVPbkFieldStorageTypeText:
            {
            address.Set(MVPbkContactFieldTextData::Cast(field->FieldData()).Text());
            break;
            }
        case EVPbkFieldStorageTypeUri:
            {
            address.Set(MVPbkContactFieldUriData::Cast(field->FieldData()).Text());
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(); // field 
    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL(plainText);
    plainText->InsertL(KBeginning, address);

    // Copy to clipboard
    CClipboard* cb = CClipboard::NewForWritingLC(
        iAppServices->ContactManager().FsSession());
    plainText->CopyToStoreL(cb->Store(), cb->StreamDictionary(),
        KBeginning, plainText->DocumentLength());
    cb->CommitL();

    CleanupStack::PopAndDestroy(2); // cb, plainText

 
    // Show a note
    HBufC* prompt = StringLoader::LoadLC( R_QTN_CCA_INFO_NOTE_ADDRESS_COPIED_TO_CLIPBOARD );
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(prompt);
    }
    	
MVPbkStoreContactField* CPbk2CopyAddressToClipboardCmd::FocusedFieldLC
    ( MVPbkStoreContact& aStoreContact )
    {
    TInt index = KErrNotFound;
    
    if (iUiControl)
        {
        index = iUiControl->FocusedFieldIndex();
        }

    if ( index != KErrNotFound )
        {
        // Use FieldAtLC to avoid the unvalidity of the field after new
        // FieldAt call.        
        return aStoreContact.Fields().FieldAtLC( index );
        }
    return NULL;
    }    

// End of File
