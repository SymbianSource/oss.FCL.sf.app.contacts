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
* Description:  Phonebook 2 contact editor delete item manager.
*
*/


#include "TPbk2DeleteItemManager.h"

// Phonebook 2
#include "MPbk2ContactEditorFieldArray.h"
#include <MPbk2ContactEditorField.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2PresentationContactField.h>
#include <CVPbkContactManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include "CPbk2ContactEditorArrayItem.h"
#include <MPbk2ApplicationServices.h>

// System includes
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <featmgr.h>

// --------------------------------------------------------------------------
// TPbk2DeleteItemManager::TPbk2DeleteItemManager
// --------------------------------------------------------------------------
//
TPbk2DeleteItemManager::TPbk2DeleteItemManager
        ( CPbk2PresentationContact& aContact,
          MPbk2ContactEditorFieldArray& aFieldArray,
          MPbk2ApplicationServices* aAppServices ):
            iContact( aContact ),
            iFieldArray( aFieldArray ),
            iAppServices( aAppServices )
    {
    }

// --------------------------------------------------------------------------
// TPbk2DeleteItemManager::DeleteFieldL
// --------------------------------------------------------------------------
//
TBool TPbk2DeleteItemManager::DeleteFieldL( TInt aControlId )
    {
    TBool result = EFalse;
    CPbk2ContactEditorArrayItem* uiField = iFieldArray.Find(aControlId);
    if (uiField)
        {
        HBufC* prompt = NULL;
        if(uiField->ContactEditorField())
        	{
        	prompt = StringLoader::LoadLC(R_QTN_PHOB_QUERY_DELETE_ITEM, 
						uiField->ContactEditorField()->FieldLabel());
        	}
        else if(uiField->ContactEditorUIField())
        	{
        	prompt = StringLoader::LoadLC(R_QTN_PHOB_QUERY_DELETE_ITEM, 
						uiField->ContactEditorUIField()->FieldLabel());
        	}
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		CleanupStack::PushL(dlg);
        dlg->SetPromptL(*prompt);
		CleanupStack::Pop(); // dlg
        if(dlg->ExecuteLD(R_PBK2_GENERAL_CONFIRMATION_QUERY))
            {
            if(uiField->ContactEditorField())
            	{
            	MVPbkStoreContactField& storeField = 
					uiField->ContactEditorField()->ContactField();
		 
				InformDeleteSpeedDialL(storeField);
            	}
                                     
            // Delete the field
            iFieldArray.RemoveField(*uiField);
            result = ETrue;
            }
        CleanupStack::PopAndDestroy(prompt);
        }
    return result;
    }

// --------------------------------------------------------------------------
// TPbk2DeleteItemManager::InformDeleteSpeedDialL
// --------------------------------------------------------------------------
//
inline void TPbk2DeleteItemManager::InformDeleteSpeedDialL
        ( const MVPbkStoreContactField& storeField ) const
    {
    // Utilise attribute manager to find out does
    // the contact field have a speed dial defined
    TBool hasSpeedDial = EFalse;
    if( iAppServices )
        {
        hasSpeedDial = iAppServices->ContactManager().ContactAttributeManagerL().HasFieldAttributeL
			( CVPbkSpeedDialAttribute::Uid(), storeField );
        }
    else
        {
        hasSpeedDial = Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().ContactAttributeManagerL().HasFieldAttributeL
                ( CVPbkSpeedDialAttribute::Uid(), storeField );
        }
        
	if ( hasSpeedDial )
        {
        HBufC* note = StringLoader::LoadLC
            ( R_QTN_PHOB_NOTE_SPEED_DIAL_DEL );
        CAknConfirmationNote* dlg = 
            new ( ELeave ) CAknConfirmationNote( ETrue );
        dlg->ExecuteLD( *note );
        CleanupStack::PopAndDestroy(); // note
        }
    }

// End of File  
