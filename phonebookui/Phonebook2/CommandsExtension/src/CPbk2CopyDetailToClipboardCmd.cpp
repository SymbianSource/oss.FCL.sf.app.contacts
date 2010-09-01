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
#include "CPbk2CopyDetailToClipboardCmd.h"

#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
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
#include <Pbk2AddressTools.h>

#include <txtetext.h>   // CPlainText
#include <baclipb.h>    // CClipboard
#include <StringLoader.h>
#include <aknnotewrappers.h>    // CAknInformationNote


// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CopyDetailToClipboardCmd* CPbk2CopyDetailToClipboardCmd::NewL(
        MPbk2ContactUiControl& aUiControl   )
    {
    CPbk2CopyDetailToClipboardCmd* self = 
        new ( ELeave ) CPbk2CopyDetailToClipboardCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::CPbk2CopyDetailToClipboardCmd
// --------------------------------------------------------------------------
//
CPbk2CopyDetailToClipboardCmd::CPbk2CopyDetailToClipboardCmd(
        MPbk2ContactUiControl& aUiControl) : 
    iUiControl(&aUiControl)
    {
    // do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::~CPbk2CopyDetailToClipboardCmd
// --------------------------------------------------------------------------
//    
CPbk2CopyDetailToClipboardCmd::~CPbk2CopyDetailToClipboardCmd()
    {
    delete iStoreContact;
    delete iRetrieveOperation;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CopyDetailToClipboardCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CopyDetailToClipboardCmd::ExecuteLD()
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
// CPbk2CopyDetailToClipboardCmd::AddObserver
// --------------------------------------------------------------------------
//    
void CPbk2CopyDetailToClipboardCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::ResetUiControl
// --------------------------------------------------------------------------
//    
void CPbk2CopyDetailToClipboardCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CopyDetailToClipboardCmd::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact)
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        
        delete iStoreContact;
        iStoreContact = aContact;
        // should not execute copyDetail command when
        // there is no UI Control
        if ( iUiControl )
            {
            TRAP_IGNORE(CopyDetailToClipboardL());
            iUiControl->UpdateAfterCommandExecution();
            }
        iObserver->CommandFinished(*this);                     
        }    
    }

// --------------------------------------------------------------------------
// CPbk2CopyDetailToClipboardCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPbk2CopyDetailToClipboardCmd::VPbkSingleContactOperationFailed(
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
// CPbk2CopyDetailToClipboardCmd::CopyDetailToClipboardL
// --------------------------------------------------------------------------
//    
void CPbk2CopyDetailToClipboardCmd::CopyDetailToClipboardL()
    {
    // Construct a plain text from the detail
    const TInt KBeginning = 0;
    // Fetch field from current UI control
    const MVPbkBaseContactField* field = iUiControl->FocusedField();
    TPtrC detail;
    RBuf addText; 
    TVPbkFieldStorageType fieldType = field->FieldData().DataType();
    
    switch(fieldType)
        {
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkFieldType* vPbkFieldType = field->BestMatchingFieldType();
            TPbk2FieldGroupId groupId = Pbk2AddressTools::MapVPbkFieldTypeToAddressGroupId( vPbkFieldType );
            if( groupId == EPbk2FieldGroupIdHomeAddress || 
            	groupId == EPbk2FieldGroupIdCompanyAddress || 
            	groupId == EPbk2FieldGroupIdPostalAddress ) //the focused field belongs to Address
            	{             
                MVPbkStoreContact* vPbkStoreContact = const_cast<MVPbkStoreContact*>( iUiControl->FocusedStoreContact() );
                //address view is empty
                if( Pbk2AddressTools::IsAddressPreviewEmptyL(*vPbkStoreContact, groupId) )
                	{
                    detail.Set( KNullDesC() ); //KNullDesC will be copied to clipboard
                	}
                else //address view is not empty
                	{
                    Pbk2AddressTools::GetAddressPreviewLC(*vPbkStoreContact, groupId, addText);
                    detail.Set( addText );
                	}
            	}
            else //the focused field doesn't belong to Address
            	{
                detail.Set(MVPbkContactFieldTextData::Cast(field->FieldData()).Text());
            	}            
            break;
            }
        case EVPbkFieldStorageTypeDateTime:
            {
            TLocale locale;
            TBuf<64> dateBuffer;

            const MVPbkContactFieldDateTimeData& date = 
                MVPbkContactFieldDateTimeData::Cast(field->FieldData());

            TTime time(date.DateTime());

            HBufC* dateFormat = CCoeEnv::Static()->AllocReadResourceLC
                (R_QTN_DATE_USUAL_WITH_ZERO); // in avkon.rsg
            
            time.FormatL(dateBuffer, *dateFormat, locale);
            CleanupStack::PopAndDestroy(dateFormat);           
            
            detail.Set(dateBuffer);
            break;
            }
        }
		
    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL(plainText);
    plainText->InsertL(KBeginning, detail);

    // Copy to clipboard
    CClipboard* cb = CClipboard::NewForWritingLC(
        iAppServices->ContactManager().FsSession());
    plainText->CopyToStoreL(cb->Store(), cb->StreamDictionary(),
        KBeginning, plainText->DocumentLength());
    cb->CommitL();

    CleanupStack::PopAndDestroy(2); // cb, plainText	
    if( addText.Length() )
    	{
        CleanupStack::PopAndDestroy(&addText);
    	}

    // Show a note
    HBufC* prompt = StringLoader::LoadLC(R_QTN_CCA_INFO_NOTE_COPIED_TO_CLIPBOARD);
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(prompt);
    }

// End of File
