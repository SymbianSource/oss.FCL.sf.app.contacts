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
*           Methods for Phonebook Contact editor date field.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorDateField.h"
#include "TPbkContactItemField.h"
#include "MPbkContactEditorUiBuilder.h"
#include "MPbkFieldEditorVisitor.h"

#include <CPbkContactItem.h>

#include <e32base.h>
#include <eikmfne.h>
#include <barsread.h>
#include <coemain.h>
#include <eikcapc.h>

#include <PbkView.rsg>


/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorDateField
enum TPanicCode
    {
    EPanicPreCond_InitControlL,
    EPanicPreCond_SaveFieldL,
    EPanicPostCond_SaveFieldL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorDateField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorDateField::CPbkContactEditorDateField
        (TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorFieldBase(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorDateField::ConstructL()
    {
    __ASSERT_DEBUG(ContactItemField().StorageType() == KStorageTypeDateTime, 
            Panic(EPanicPreCond_InitControlL));

    iControl = static_cast<CEikDateEditor*>(iUiBuilder.CreateLineL(
            ContactItemField().Label(), ControlId(), EEikCtDateEditor));

    // ctrl is now owned by the dialog
    TResourceReader reader;
	CCoeEnv::Static()->CreateResourceReaderLC(reader, R_PBK_EDITOR_DATE_SETTINGS);
    iControl->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // R_PBK_EDITOR_DATE_SETTINGS

	TTime time = ContactItemField().DateTimeStorage()->Time();
	if (time == Time::NullTTime())
		{
        time.HomeTime();
		}

    iControl->SetDate(time);

    // Postcond ceased to work, maybe CEikDateEditor clips the time of the day part?
//    __ASSERT_DEBUG(aCtrl->Date() == aField.DateTimeStorage()->Time(), Panic(EPanicPostCond_InitControlL));

    LoadBitmapToFieldL(iUiBuilder);
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
	iUiBuilder.SetEditableL(ETrue);
    }

CPbkContactEditorDateField* CPbkContactEditorDateField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
		CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorDateField* self = new(ELeave) CPbkContactEditorDateField(
			aField, aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorDateField::~CPbkContactEditorDateField()
    {
    }

void CPbkContactEditorDateField::SaveFieldL()
    {
    // PreCond:
    __ASSERT_DEBUG(iControl && Field().StorageType() == KStorageTypeDateTime, 
            Panic(EPanicPreCond_SaveFieldL));

    iContactDataHasChanged = EFalse;
    
    // fetch the date from the date control
    const TTime dateTime = iControl->Date();
    if (Field().DateTimeStorage()->Time() != dateTime)
        {
        // put the date in the contact item
        Field().DateTimeStorage()->SetTime(dateTime);
        Field().SetHidden(EFalse);
        iContactDataHasChanged = ETrue;
        }
    
    //PostCond:
    __ASSERT_DEBUG(Field().DateTimeStorage()->Time() == dateTime && 
					!Field().IsHidden(), 
            Panic(EPanicPostCond_SaveFieldL));
    }

void CPbkContactEditorDateField::AddFieldL(CPbkContactItem& aContact)
    {
    const TTime dateTime = iControl->Date();
    TPbkContactItemField* field = aContact.AddOrReturnUnusedFieldL
            ( Field().FieldInfo() );
    if (field)
        {
        field->DateTimeStorage()->SetTime(dateTime);
        }
    }

HBufC* CPbkContactEditorDateField::ControlTextL() const
	{
	return NULL;
	}

void CPbkContactEditorDateField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

void CPbkContactEditorDateField::AcceptL(MPbkFieldEditorVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File
