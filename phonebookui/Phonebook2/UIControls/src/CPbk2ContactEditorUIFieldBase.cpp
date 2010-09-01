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
* Description:  Phonebook 2 contact editor dialog UI base field.
*
*/

#include "CPbk2ContactEditorUIFieldBase.h"
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2UIField.h"
#include <CPbk2IconFactory.h>
#include <TPbk2IconId.h>
#include <Pbk2UIControls.hrh>



// System includes
#include <eikedwin.h>
#include <eikcapc.h>
#include <aknnotewrappers.h>

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::CPbk2ContactEditorUIFieldBase
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactEditorUIFieldBase::CPbk2ContactEditorUIFieldBase
        ( MPbk2UIField* aField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer,
		    TInt aCustomPosition ) :
            iField(aField),
            iUiBuilder( aUiBuilder ),
            iIconInfoContainer( aIconInfoContainer ),
            iCustomPosition( aCustomPosition )
    {
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::~CPbk2ContactEditorUIFieldBase
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactEditorUIFieldBase::~CPbk2ContactEditorUIFieldBase()
    {
    iLebel.Close();
    iText.Close();
    delete iField;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::ControlId
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2ContactEditorUIFieldBase::ControlId()
    {
    return ( TInt ) this;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::UIField
// -----------------------------------------------------------------------------
//
EXPORT_C MPbk2UIField* CPbk2ContactEditorUIFieldBase::UIField() const
    {
    return iField;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactEditorUIFieldBase::HandleCustomFieldCommandL(TInt /*aCommand*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::FieldLabel
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPbk2ContactEditorUIFieldBase::FieldLabel() const
    {
    return iLebel;
    }
  
// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::SetFieldLabelL
// -----------------------------------------------------------------------------
//  
EXPORT_C void CPbk2ContactEditorUIFieldBase::SetFieldLabelL( const TDesC& aLabel )
    {
    iUiBuilder.SetCurrentLineCaptionL( aLabel );
    iLebel.Close();
    iLebel.Create(aLabel.Length());
    iLebel = aLabel;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::ControlTextL
// -----------------------------------------------------------------------------
//  
EXPORT_C const TDesC& CPbk2ContactEditorUIFieldBase::ControlText() const
    {
    return iText;
    }


// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::SetFocus
// -----------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactEditorUIFieldBase::SetFocus()
    {
    // It's not fatal if focusing fails
    TRAP_IGNORE( iUiBuilder.TryChangeFocusL( ControlId() ) );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactEditorUIFieldBase::ConsumesKeyEvent
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool ret(EFalse);
    
    if( aKeyEvent.iScanCode == EStdKeyDevice3 )
    	{
		if( aType == EEventKeyDown )
			{
			iStdKeyDevice3Down = ETrue;
			ret = ETrue;
			}
		else if ( aType == EEventKeyUp )
			{
			if ( iStdKeyDevice3Down == (TInt)ETrue )
				{
				ret = HandleCustomFieldCommandL( EPbk2CmdEditorHandleCustomSelect );
				}
			iStdKeyDevice3Down = EFalse;
			}
    	}
	return ret;
    }   

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIFieldBase::LoadBitmapToFieldL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactEditorUIFieldBase::LoadBitmapToFieldL
        ( const TPbk2IconId& aIconId )
    {
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC( iIconInfoContainer );
    CEikImage* image = factory->CreateImageLC( aIconId );
    
    if ( image )
        {
        image->SetPictureOwnedExternally( EFalse );
        image->CopyControlContextFrom( iUiBuilder.LineControl( ControlId() ) );
        image->SetContainerWindowL( *(iUiBuilder.LineControl( ControlId() )) );
        image->SetNonFocusing();
        image->SetBrushStyle( CGraphicsContext::ENullBrush );

        delete iUiBuilder.LineControl( ControlId() )->iBitmap;
        iUiBuilder.LineControl( ControlId() )->iBitmap = image;
        CleanupStack::Pop( image );

        TRect rect = iUiBuilder.LineControl( ControlId() )->Rect();
        rect.Resize( -1, -1 );
        iUiBuilder.LineControl( ControlId() )->SetRect( rect );
        rect.Resize( 1, 1 );
        iUiBuilder.LineControl( ControlId() )->SetRect( rect );
        }

    CleanupStack::PopAndDestroy( factory );
    }

// End of File  
