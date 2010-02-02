/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Command helper class
*
*/


#include "CPbk2FieldFocusHelper.h"

// VirtualPhonebook
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>

// Phonebook2
#include <MPbk2ContactUiControl.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPbk2FieldFocusHelper::CPbk2FieldFocusHelper
// --------------------------------------------------------------------------
//
inline CPbk2FieldFocusHelper::CPbk2FieldFocusHelper( 
    MPbk2ContactUiControl& aUiControl,
    MPbk2FieldPropertyArray& aPropertyArray ):
    iUiControl( aUiControl ), 
    iFieldProperties( aPropertyArray ),
    iUiControlValid ( ETrue ) 
    {        
    }

// --------------------------------------------------------------------------
// CPbk2FieldFocusHelper::NewL
// --------------------------------------------------------------------------
//    
EXPORT_C CPbk2FieldFocusHelper* CPbk2FieldFocusHelper::NewL
    ( MPbk2ContactUiControl& aUiControl, 
      MPbk2FieldPropertyArray& aPropertyArray )
    {
    CPbk2FieldFocusHelper* self = 
        new ( ELeave ) CPbk2FieldFocusHelper( aUiControl, aPropertyArray );
    return self;       
    }

// --------------------------------------------------------------------------
// CPbk2FieldFocusHelper::~CPbk2FieldFocusHelper
// --------------------------------------------------------------------------
//    
CPbk2FieldFocusHelper::~CPbk2FieldFocusHelper()    
    {
    delete iFocusedStoreCntField;
    }

// --------------------------------------------------------------------------
// CPbk2FieldFocusHelper::SaveInitiallyFocusedFieldL
// --------------------------------------------------------------------------
//    
EXPORT_C void CPbk2FieldFocusHelper::SaveInitiallyFocusedFieldL
    ( MVPbkStoreContact& aStoreContact )
    {
    if ( iUiControlValid )
        {
        iStoreContact = &aStoreContact;
        iUiFieldIndex = iUiControl.FocusedFieldIndex();    
        if ( iStoreContact && ( iUiFieldIndex != KErrNotFound ) )
            {
            // This command is launched only from contact info view so there 
            // should be store contact
            CPbk2PresentationContact* pCnt = 
            CPbk2PresentationContact::NewL
            ( const_cast<MVPbkStoreContact&>( *iStoreContact ), 
                    iFieldProperties );
            CleanupStack::PushL( pCnt );
            TInt storeFieldIndex( 
                    pCnt->PresentationFields().StoreIndexOfField( iUiFieldIndex ) );
            if ( storeFieldIndex != KErrNotFound )
                {            
                if ( iFocusedStoreCntField )
                    {
                    delete iFocusedStoreCntField;
                    iFocusedStoreCntField = NULL;
                    }
                iFocusedStoreCntField = iStoreContact->Fields().FieldAtLC
                ( storeFieldIndex );
                CleanupStack::Pop(); // iFocusedStoreCntField            
                }
            CleanupStack::PopAndDestroy(); // pCnt 
            }             
        }
    }
// --------------------------------------------------------------------------
// CPbk2FieldFocusHelper::RestoreSavedFieldL
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CPbk2FieldFocusHelper::RestoreSavedFieldL()
    {
    TInt fieldIndex ( KErrNotFound );        
    if ( iStoreContact && iUiControlValid )
        {
        iUiControl.SetFocusedContactL( *iStoreContact );

        if ( iFocusedStoreCntField )
            {
            CPbk2PresentationContact* presentationCnt = 
                CPbk2PresentationContact::NewL
                    ( *iStoreContact, iFieldProperties );
            fieldIndex = presentationCnt->PresentationFields().
                    FindFieldIndex( *iFocusedStoreCntField );
            delete presentationCnt;
            presentationCnt = NULL;
           
            // If the field is added, focused index stays the same.
            // If the field is removed and the focus is in the
            // removed field, focus moves upwards by one.
            if ( fieldIndex == KErrNotFound )
                {
                fieldIndex = --iUiFieldIndex;
                }
            iUiControl.SetFocusedFieldIndex( fieldIndex );
            }                
        }
        
    return fieldIndex;
    }
    
// --------------------------------------------------------------------------
// CPbk2FieldFocusHelper::SetUiControlValid
// --------------------------------------------------------------------------
//    
EXPORT_C void CPbk2FieldFocusHelper::SetUiControlValid( TBool aUiControlValid )
    {
    iUiControlValid = aUiControlValid;
    }
// End of file
