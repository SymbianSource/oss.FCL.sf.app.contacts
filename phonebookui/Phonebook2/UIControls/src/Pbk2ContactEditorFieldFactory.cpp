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
* Description:  Phonebook 2 contact editor field factory.
*
*/


#include "Pbk2ContactEditorFieldFactory.h"

// Phonebook 2
#include "CPbk2ContactEditorTextField.h"
#include "CPbk2ContactEditorNumberField.h"
#include "CPbk2ContactEditorPhoneNumberField.h"
#include "CPbk2ContactEditorPostalCodeField.h"
#include "CPbk2ContactEditorReadingField.h"
#include "CPbk2ContactEditorUrlField.h"
#include "CPbk2ContactEditorImppField.h"
#include "CPbk2ContactEditorDateField.h"
#include "CPbk2ContactEditorSyncField.h"
#include "CPbk2ContactEditorDTMFField.h"
#include "CPbk2ContactEditorEmailField.h"
#include "CPbk2ContactEditorRingtoneField.h"
#include "CPbk2ContactEditorImageField.h"
#include "MPbk2UIField.h"
#include "CPbk2ContactEditorUIAddressField.h"
#include "CPbk2ContactEditorUIAddressHomeField.h"
#include "CPbk2ContactEditorUIAddressOfficeField.h"
#include <MPbk2ContactEditorExtensionExtension.h>
#include "CPbk2IconInfoContainer.h"
#include <pbk2commonui.rsg>

#include <MPbk2ContactEditorExtension.h>
#include <Pbk2FieldProperty.hrh>

#include <MPbk2FieldProperty.h>
#include <MPbk2FieldProperty2.h>
#include <CPbk2PresentationContactField.h>

// Virtual Phonebook
#include <VPbkFieldType.hrh>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldUriData.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EInvalidType_EPbk2FieldCtrlTypeTextEditor,
    EInvalidType_EPbk2FieldCtrlTypeDateEditor,
    EInvalidType_EPbk2FieldCtrlTypeNumberEditor,
    EInvalidType_EPbk2FieldCtrlTypePhoneNumberEditor,
    EInvalidType_EPbk2FieldCtrlURLEditor,
    EInvalidType_EPbk2FieldCtrlEmailEditor,
    EInvalidType_EPbk2FieldCtrlPostalCodeEditor,
    EInvalidType_EPbk2FieldCtrlTypeReadingEditor,
    EInvalidType_EPbk2FieldCtrlTypeChoiceItemEditor,
    EInvalidType_EPbk2FieldCtrlTypeDTMFEditor,
    EInvalidType_EPbk2FieldCtrlTypeRingtoneEditor,
    EInvalidType_EPbk2FieldCtrlTypeImageEditor,
    ECreateFieldL_PostCond
    };

void Panic(TInt aReason)
    {
    _LIT( KPanicText, "Pbk2ContactEditorFieldFactory" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldFactory::CPbk2ContactEditorFieldFactory
// --------------------------------------------------------------------------
//
CPbk2ContactEditorFieldFactory::CPbk2ContactEditorFieldFactory
        ( MPbk2ContactEditorUiBuilder& aUiBuilder, 
          MPbk2ContactEditorExtension& aEditorExtension )
          :iUiBuilder(aUiBuilder), iEditorExtension(aEditorExtension)
    {
    }
        
// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldFactory::~CPbk2ContactEditorFieldFactory
// --------------------------------------------------------------------------
//
CPbk2ContactEditorFieldFactory::~CPbk2ContactEditorFieldFactory()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldFactory::NewL
// --------------------------------------------------------------------------
//
 CPbk2ContactEditorFieldFactory* CPbk2ContactEditorFieldFactory::NewL
        ( MPbk2ContactEditorUiBuilder& aUiBuilder, 
          MPbk2ContactEditorExtension& aEditorExtension )
    {
    CPbk2ContactEditorFieldFactory* self = 
    new(ELeave) CPbk2ContactEditorFieldFactory( aUiBuilder, aEditorExtension );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
 
 // --------------------------------------------------------------------------
 // CPbk2ContactEditorFieldFactory::NewL
 // --------------------------------------------------------------------------
 //
  void CPbk2ContactEditorFieldFactory::ConstructL()
     {
     }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldFactory::CreateFieldL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorField* CPbk2ContactEditorFieldFactory::CreateFieldLC
        ( CPbk2PresentationContactField& aField,
          TInt aCustomPosition, CPbk2IconInfoContainer& aIconInfoContainer )
    {
    MPbk2ContactEditorField* field = NULL;
    TPbk2FieldCtrlType ctrlType = aField.FieldProperty().CtrlType();

    // Offer the field first to extensions
    field = iEditorExtension.CreateEditorFieldL( aField.StoreField(),
        aField.FieldProperty(), iUiBuilder, aIconInfoContainer );
    if ( field )
        {
        CleanupDeletePushL( field );
        }
    else
        {
        switch( ctrlType )
            {
            case EPbk2FieldCtrlTypeTextEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeTextEditor ) );
                field = CPbk2ContactEditorTextField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlTypeDateEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeDateTime,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeDateEditor ) );
                field = CPbk2ContactEditorDateField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlTypeNumberEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeNumberEditor ) );
                field = CPbk2ContactEditorNumberField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlTypePhoneNumberEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlTypePhoneNumberEditor ) );
                field = CPbk2ContactEditorPhoneNumberField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlURLEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlURLEditor ) );
                field = CPbk2ContactEditorUrlField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlEmailEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlEmailEditor ) );
                field = CPbk2ContactEditorEmailField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlPostalCodeEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlPostalCodeEditor ) );
                field = CPbk2ContactEditorPostalCodeField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlTypeReadingEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeReadingEditor ) );
                field = CPbk2ContactEditorReadingField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlTypeChoiceItems:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeChoiceItemEditor ) );
                field = CPbk2ContactEditorSyncField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;
                }
            case EPbk2FieldCtrlTypeDTMFEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeText,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeDTMFEditor ) );
                field = CPbk2ContactEditorDTMFField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer );
                break;            
                }
            case EPbk2FieldCtrlTypeIMPPEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                    EVPbkFieldStorageTypeUri,
                        Panic( EInvalidType_EPbk2FieldCtrlTypeTextEditor ) );
                
                MPbk2FieldProperty2* fieldPropertyExtension =
                    reinterpret_cast<MPbk2FieldProperty2*>(
                        const_cast<MPbk2FieldProperty&>( aField.FieldProperty() ).
                            FieldPropertyExtension(
                                KMPbk2FieldPropertyExtension2Uid ) );
                User::LeaveIfNull( fieldPropertyExtension ); 
                
                // There is no support for fields with unspecified service name
                TBool invalid = ((fieldPropertyExtension->XSpName() == KNullDesC) &&
                        (MVPbkContactFieldUriData::Cast(aField.FieldData()).Scheme()
                        == KNullDesC));
                        
                if ( !invalid )
                    {
                    field = CPbk2ContactEditorImppField::NewLC
                        ( aField, iUiBuilder, aIconInfoContainer );
                    }
                break;
                }
            case EPbk2FieldCtrlTypeRingtoneEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                EVPbkFieldStorageTypeText, 
                        Panic( EInvalidType_EPbk2FieldCtrlTypeRingtoneEditor ) );
                field = CPbk2ContactEditorRingtoneField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer, aCustomPosition ); 
                break;
                }       
            case EPbk2FieldCtrlTypeImageEditor:
                {
                __ASSERT_DEBUG( aField.FieldData().DataType() ==
                EVPbkFieldStorageTypeText, 
                        Panic( EInvalidType_EPbk2FieldCtrlTypeImageEditor ) );
                field = CPbk2ContactEditorImageField::NewLC
                    ( aField, iUiBuilder, aIconInfoContainer, aCustomPosition );
                break;
                }                
            case EPbk2FieldCtrlTypeNone:    // FALLTHROUGH
            default:
                {
                PBK2_DEBUG_ONLY( Panic( ECreateFieldL_PostCond ) );
                break;
                }
            }
        }

    return field;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldFactory::CreateFieldL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorUIField* CPbk2ContactEditorFieldFactory::CreateFieldLC
        ( MPbk2UIField& aField,
          TInt aCustomPosition, 
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          const TDesC& aCustomText, CPbk2IconInfoContainer& aIconInfoContainer )
    {
    MPbk2ContactEditorUIField* field = NULL;
    MPbk2ContactEditorExtensionExtension* extext = 
    	static_cast<MPbk2ContactEditorExtensionExtension*>
    		(iEditorExtension.ContactEditorExtensionExtension(TUid::Null()));
    
    if( extext )
    	{
    	field = extext->CreateFieldLC( aField, aCustomPosition, aUiBuilder, KNullDesC(), aIconInfoContainer );
    	if( field )
    		{
    		return field;
    		}
    	}
    
    switch(aField.CtrlType())
    	{
    	case EPbk2FieldCtrlTypeExtAddressEditor:
    		field = CPbk2ContactEditorUIAddressField::NewL(&aField, aUiBuilder, aIconInfoContainer, aCustomPosition, aCustomText);
    		CleanupStack::PushL(field);
    		break;
    	case EPbk2FieldCtrlTypeExtAddressHomeEditor:
    		field = CPbk2ContactEditorUIAddressHomeField::NewL(&aField, aUiBuilder, aIconInfoContainer, aCustomPosition, aCustomText);
    		CleanupStack::PushL(field);
    		break;
    	case EPbk2FieldCtrlTypeExtAddressOfficeEditor:
    		field = CPbk2ContactEditorUIAddressOfficeField::NewL(&aField, aUiBuilder, aIconInfoContainer, aCustomPosition, aCustomText);
    		CleanupStack::PushL(field);
    		break;
    	}
    return field;
    }

// End of File  
