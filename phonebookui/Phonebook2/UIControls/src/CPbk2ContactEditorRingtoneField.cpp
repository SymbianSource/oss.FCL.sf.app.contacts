/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor ringtone field.
*
*/


#include "CPbk2ContactEditorRingtoneField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorUiBuilderExtension.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <Pbk2PresentationUtils.h>
#include "Pbk2RingtoneCommands.h"
#include "CPbk2ContactEditorReadonlyField.h"
#include "Pbk2EditorLineIds.hrh"
#include <Pbk2UIControls.hrh>
#include <Pbk2Config.hrh>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>
#include <VPbkFieldType.hrh>
#include <MVPbkFieldType.h>
#include <Pbk2UIControls.rsg>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::CPbk2ContactEditorRingtoneField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorRingtoneField::CPbk2ContactEditorRingtoneField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::~CPbk2ContactEditorRingtoneField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorRingtoneField::~CPbk2ContactEditorRingtoneField()
    {    
    delete iInitialText;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorRingtoneField* CPbk2ContactEditorRingtoneField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer,
          TInt aCustomPosition) 
    {
    CPbk2ContactEditorRingtoneField* self = 
        new ( ELeave ) CPbk2ContactEditorRingtoneField( aContactField,
            aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL(aCustomPosition);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::ConstructL
// Create and insert a line in the dialog
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorRingtoneField::ConstructL(TInt aCustomPosition)
    {
    // Create and insert a line in the dialog
    MPbk2ContactEditorUiBuilderExtension* ext = 
        static_cast<MPbk2ContactEditorUiBuilderExtension*>(
            iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));
    
    iControl = static_cast<CPbk2ContactEditorReadonlyField*>(
	ext->AddCustomFieldToFormL(aCustomPosition, R_PBK2_RINGTONE_LINE));     
    iControl->InitializeL(this);    
    SetTextL();      
    iInitialText = iControl->GetTextInHBufL();    
    iControl->CreateTextViewL(); 
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId()); 
    iUiBuilder.TryChangeFocusL(ControlId()); 
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorRingtoneField::ControlId() const
    {
    return EPbk2EditorLineRingtone;  
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorRingtoneField::SetTextL()
    {
    // Get ringtone or default text
    TBool getDefaultText(ETrue);      
    TVPbkFieldStorageType type = iContactField.FieldData().DataType();    
    
    if(type == EVPbkFieldStorageTypeText)
         {
         getDefaultText = iContactField.FieldData().IsEmpty();
         }

    //If cold filename for ringtone, show default text (the filename in database
    //must remain intact however)
    if( getDefaultText ||    
       !Pbk2RingtoneCommands::IsRingtoneFieldValidL(iContactField)) 
        {
        HBufC* defaultText = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_PHOB_FIELD_DEFAULT_TONE );
        TPtrC dataPtr(*defaultText);
        FormatTextL( dataPtr, ETrue );  
        CleanupStack::PopAndDestroy(defaultText);
        iTextState = ENoData;
        }
    else
        {
        TPtrC dataPtr(MVPbkContactFieldTextData::Cast(  
            iContactField.FieldData()).Text());
        FormatTextL( dataPtr, EFalse );   
        iTextState = EFilename;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::FormatTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorRingtoneField::FormatTextL(TPtrC aDataPtr, TBool aIsDefault)
    {
    HBufC* textBuf = HBufC::NewLC(aDataPtr.Length());
    TPtr text = textBuf->Des();        
    text.Zero();
    Pbk2PresentationUtils::AppendWithNewlineTranslationL(text, aDataPtr);
    
    TParsePtr fileNameParser( text );
    TPtrC baseName;
    if ( aIsDefault )
    	{
    	// If it is a default string, then use FullName() to parse.
    	// The reason is there might be a dot in the end of the string and the
    	// characters after that would be removed if using Name().
    	// Example: "Them h.anh" in Vietnamese.
    	baseName.Set( fileNameParser.FullName() );
    	}
    else
    	{
    	baseName.Set( fileNameParser.Name() );
    	}
    // Set formatted text to editor control
    iControl->SetTextL(&baseName);    
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy(textBuf);
    iUiBuilder.LoadBitmapToFieldL
        ( iContactField.FieldProperty(), iIconInfoContainer, ControlId() );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorRingtoneField::Control() const
    {
    // We don't show anything out
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::ControlTextL
// -----------------------------------------------------------------------------
//  
HBufC* CPbk2ContactEditorRingtoneField::ControlTextL() const
    {
    return CPbk2ContactEditorFieldBase::ControlTextL();    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorRingtoneField::SaveFieldL()
    {
    HBufC* text = iControl->GetTextInHBufL();
    CleanupStack::PushL(text);
    
    if(iInitialText->Compare(*text) )
        {
        iContactDataHasChanged = ETrue;
        }
    else
        {
        iContactDataHasChanged = EFalse;
        }
    
    CleanupStack::PopAndDestroy(text);
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorRingtoneField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    MPbk2ContactEditorUiBuilderExtension* ext = 
         static_cast<MPbk2ContactEditorUiBuilderExtension*>(
             iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));        
    
    if(aCommand == EPbk2CmdEditorHandleCustomSelect)
        {
#ifdef ECE_DISABLE_CONTEXT_MENU
        if (ContactField().FieldData().IsEmpty() 
              || !Pbk2RingtoneCommands::IsRingtoneFieldValidL(ContactField()) )
            {
            ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorChangeRingtone);
            }
        else
            {
            ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorContextMenu);
            }
#else
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorChangeRingtone);
#endif
        }
    else if(aCommand == EPbk2CmdEditorHandleCustomRemove)
        {
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorRemoveRingtone);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorRingtoneField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorRingtoneField::AcceptL
        ( MPbk2ContactEditorFieldVisitor& aVisitor )
    {
    aVisitor.VisitL( *this );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorRingtoneField::ConsumesKeyEvent
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse keyResponse = iControl->OfferKeyEventL( aKeyEvent, aType );

    if ( EKeyWasConsumed == keyResponse )
        {
        return ETrue;
        }
            
    return EFalse;
    }   

// -----------------------------------------------------------------------------
// CPbk2ContactEditorImageField::TextState
// -----------------------------------------------------------------------------
//
TInt CPbk2ContactEditorRingtoneField::TextState()
    {
    return iTextState;
    }   

// -----------------------------------------------------------------------------
// CPbk2ContactEditorRingtoneField::ContactEditorFieldExtension
// -----------------------------------------------------------------------------
//
TAny* CPbk2ContactEditorRingtoneField::ContactEditorFieldExtension(
	TUid aExtensionUid )
    {
         if( aExtensionUid == KMPbk2ContactEditorFieldExtension2Uid )
         {
         return static_cast<MPbk2ContactEditorField2*>( this );
         }    
         
         return this;    
    }



// End of File  
