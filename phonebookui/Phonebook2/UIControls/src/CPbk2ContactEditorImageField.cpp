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
* Description:  Phonebook 2 contact editor iamge field.
*
*/


#include "CPbk2ContactEditorImageField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorUiBuilderExtension.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <Pbk2PresentationUtils.h>
#include "Pbk2ImageCommands.h"
#include "CPbk2ContactEditorReadonlyField.h"
#include "Pbk2EditorLineIds.hrh"
#include <Pbk2UIControls.hrh>
#include <Pbk2Config.hrh>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>
#include <VPbkFieldType.hrh>
#include <MVPbkFieldType.h>
#include <pbk2uicontrols.rsg>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::CPbk2ContactEditorImageField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorImageField::CPbk2ContactEditorImageField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::~CPbk2ContactEditorImageField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorImageField::~CPbk2ContactEditorImageField()
    {    
    delete iInitialText;    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorImageField* CPbk2ContactEditorImageField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer,
          TInt aCustomPosition) 
    {
    CPbk2ContactEditorImageField* self = 
        new ( ELeave ) CPbk2ContactEditorImageField( aContactField,
            aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL(aCustomPosition);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ConstructL
// Create and insert a line in the dialog
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorImageField::ConstructL(TInt aCustomPosition)
    {
    MPbk2ContactEditorUiBuilderExtension* ext = 
        static_cast<MPbk2ContactEditorUiBuilderExtension*>(
            iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));
    
    iControl = static_cast<CPbk2ContactEditorReadonlyField*>(
        ext->AddCustomFieldToFormL(aCustomPosition, R_PBK2_IMAGE_LINE));
    iControl->InitializeL(this);
    SetTextL();      
    iInitialText = iControl->GetTextInHBufL();    
    iControl->CreateTextViewL(); 
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId()); 
    iUiBuilder.TryChangeFocusL(ControlId());  
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorImageField::ControlId() const
    {
    return EPbk2EditorLineImage;  
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorImageField::SetTextL()
    {
    // Get image name or set descriptive text
    TBool isFilename(EFalse);     
    TVPbkFieldStorageType type = iContactField.FieldData().DataType();    
    
    if(type == EVPbkFieldStorageTypeText)
         {
         isFilename = !iContactField.FieldData().IsEmpty();
         }

    TBool isThumbnail(Pbk2ImageCommands::IsThumbnailFieldL(iContactField));
    TBool imagefileValid(Pbk2ImageCommands::IsImageFieldValidL(iContactField));    
    
    if( !isFilename && !isThumbnail )    
        {
        HBufC* defaultText = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_PHOB_FIELD_ADD_IMAGE );
        TPtrC dataPtr(*defaultText);
           
        FormatTextL( dataPtr, EDontClipFileExtension );  
        CleanupStack::PopAndDestroy(defaultText);
        iTextState = ENoData; 
        }
    else if( !imagefileValid ) 
        {
        //cold filename, show "Thumbnail" (e.g mem card not present)
        HBufC* defaultText = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_PHOB_FIELD_THUMBNAIL );
        TPtrC dataPtr(*defaultText);
               
        FormatTextL( dataPtr, EClipFileExtension );  
        CleanupStack::PopAndDestroy(defaultText);
        iTextState = EThumbnail;
        }        
    else
        {
        TPtrC dataPtr(MVPbkContactFieldTextData::Cast(  
            iContactField.FieldData()).Text());
        
        FormatTextL( dataPtr, EClipFileExtension );      
        iTextState = EFilename;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::FormatTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorImageField::FormatTextL(TPtrC aDataPtr, TClipFileExtension aClipFileExtension)
    {
    HBufC* textBuf = HBufC::NewLC(aDataPtr.Length());
    TPtr text = textBuf->Des();        
    text.Zero();
    Pbk2PresentationUtils::AppendWithNewlineTranslationL(text, aDataPtr);
    
    if (aClipFileExtension == EClipFileExtension )
        {
        TParsePtr fileNameParser( text );
        TPtrC baseName( fileNameParser.Name() );
        // Set formatted text to editor control
        iControl->SetTextL(&baseName);    
        }
    else
        {
        // Don't use filename parsing if a default string "Add image" is used.
        // There might be a dot in the end of the string and it would be then removed.
        // Example: "Adaug. imag."
        iControl->SetTextL(&text); 
        }
    
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy(textBuf);
    iUiBuilder.LoadBitmapToFieldL
        ( iContactField.FieldProperty(), iIconInfoContainer, ControlId() );

    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorImageField::Control() const
    {
    // We don't show anything out
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ControlTextL
// -----------------------------------------------------------------------------
//  
HBufC* CPbk2ContactEditorImageField::ControlTextL() const
    {
    return CPbk2ContactEditorFieldBase::ControlTextL();    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorImageField::SaveFieldL()
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
// CPbk2ContactEditorImageField::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorImageField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    MPbk2ContactEditorUiBuilderExtension* ext = 
         static_cast<MPbk2ContactEditorUiBuilderExtension*>(
             iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));  
         
    if(aCommand == EPbk2CmdEditorHandleCustomSelect)
        {
#ifdef ECE_DISABLE_CONTEXT_MENU
        if (ContactField().FieldData().IsEmpty())
            {
            ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorChangeImage);
            }
        else
            {
            ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorContextMenu);
            }
#else
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorChangeImage);
#endif
        }
    else if(aCommand == EPbk2CmdEditorHandleCustomRemove)
        {
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorRemoveImage);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorImageField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImageField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorImageField::AcceptL
        ( MPbk2ContactEditorFieldVisitor& aVisitor )
    {
    aVisitor.VisitL( *this );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorImageField::ConsumesKeyEvent
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
TInt CPbk2ContactEditorImageField::TextState()
    {
    return iTextState;
    }   

// -----------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ContactField
// -----------------------------------------------------------------------------
//
CPbk2PresentationContactField& CPbk2ContactEditorImageField::ContactField()
    {
    return iContactField;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorImageField::ContactEditorFieldExtension
// -----------------------------------------------------------------------------
//
TAny* CPbk2ContactEditorImageField::ContactEditorFieldExtension(
	TUid aExtensionUid )
    {
    if( aExtensionUid == KMPbk2ContactEditorFieldExtension2Uid )
       {
         return static_cast<MPbk2ContactEditorField2*>( this );
       }    
        
    return this;
    }



// End of File  
