/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*           Methods for Phonebook Contact editor base class.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorFieldBase.h"

#include <eikcapc.h>
#include <eikedwin.h>
#include <eikimage.h>

#include <AknsUtils.h>

#include "TPbkContactItemField.h"
#include "CPbkFieldInfo.h"
#include "PbkIconInfo.h"
#include "MPbkContactEditorUiBuilder.h"
#include "CPbkContactItem.h"

// ================= MEMBER FUNCTIONS =======================

CPbkContactEditorFieldBase::CPbkContactEditorFieldBase
        (const TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    iContactItemField(aField),
	iIconInfoContainer(aIconInfoContainer),
	iUiBuilder(aUiBuilder),
    iContactDataHasChanged(EFalse)
    {
    }

void CPbkContactEditorFieldBase::BaseConstructL()
    {
    }

CPbkContactEditorFieldBase::~CPbkContactEditorFieldBase()
    {
    }

TInt CPbkContactEditorFieldBase::ControlId() const
    {
    return iContactItemField.UniqueFieldIdentity();
    }

TPbkFieldId CPbkContactEditorFieldBase::FieldId() const
    {
    return iContactItemField.FieldInfo().FieldId();
    }

TPtrC CPbkContactEditorFieldBase::FieldLabel() const
    {
    return iContactItemField.Label();
    }

void CPbkContactEditorFieldBase::SetFieldLabelL
		(HBufC* aLabel)
    {
	iUiBuilder.SetCurrentLineCaptionL(*aLabel);
    iContactItemField.ItemField().SetLabel(aLabel);
    }

const TPbkContactItemField& CPbkContactEditorFieldBase::ContactItemField() const
    {
    return iContactItemField;
    }

TBool CPbkContactEditorFieldBase::FieldDataChanged() const
    {
    return iContactDataHasChanged;
    }

HBufC* CPbkContactEditorFieldBase::ControlTextL() const
	{
	HBufC* textBuf = NULL;
	CCoeControl* ctrl = iUiBuilder.Control(ControlId());
	if (ctrl && iContactItemField.StorageType() == KStorageTypeText)
		{
        textBuf = static_cast<CEikEdwin*>(ctrl)->GetTextInHBufL();
		}	
	return textBuf;
	}

TBool CPbkContactEditorFieldBase::ConsumesKeyEvent(
        const TKeyEvent& /*aKeyEvent*/, 
        TEventCode /*aType*/)
    {
    return EFalse;
    }   

TPbkContactItemField& CPbkContactEditorFieldBase::Field()
	{
    return iContactItemField;
    }

void CPbkContactEditorFieldBase::LoadBitmapToFieldL
        (MPbkContactEditorUiBuilder& aUiBuilder)
    {
    const TPbkIconInfo* iconInfo = iIconInfoContainer.Find(iContactItemField.IconId());

    if (iconInfo)
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        CEikImage* image = new (ELeave) CEikImage;
        CleanupStack::PushL(image);
        
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsUtils::CreateIconL(skin, iconInfo->SkinId(), 
                               bitmap, mask, iconInfo->MbmFileName(), 
                               iconInfo->IconId(), iconInfo->MaskId());
        image->SetPictureOwnedExternally(EFalse);
        image->SetPicture(bitmap, mask);
        image->CopyControlContextFrom(aUiBuilder.LineControl(ControlId()));
        image->SetContainerWindowL(*aUiBuilder.LineControl(ControlId()));
        image->SetNonFocusing();
        image->SetBrushStyle(CGraphicsContext::ENullBrush);

        delete aUiBuilder.LineControl(ControlId())->iBitmap;
        aUiBuilder.LineControl(ControlId())->iBitmap = image;
        CleanupStack::Pop(image);

        // TODO: There should be a simpler way to do this
        TRect rect = aUiBuilder.LineControl(ControlId())->Rect();
        rect.Resize(-1, -1);
        aUiBuilder.LineControl(ControlId())->SetRect(rect);
        rect.Resize(1, 1);
        aUiBuilder.LineControl(ControlId())->SetRect(rect);
        }
    }

// End of File
