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
*           Methods for phonebook Contact editor syncronization field.
*           Needed to get radio button selection to dialog.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorSyncField.h"
#include "MPbkContactEditorUiBuilder.h"
#include "MPbkFieldEditorVisitor.h"

#include <pbkview.rsg>

#include <eikdialg.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <eikcapc.h>
#include <featmgr.h>

#include <AknPopupField.h>
#include <AknQueryValueText.h>
#include <badesca.h>
#include <barsread.h>

#include <CPbkSyncronizationConstants.h>
#include <TPbkContactItemField.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <CPbkConstants.h>


/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorSyncField
enum TPanicCode
    {
    EPanicPreCond_ConstructL,
	EPanicPreCond_SaveFieldL
    };
#endif  // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorSyncField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactEditorSyncField::CPbkContactEditorSyncField
        (TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorFieldBase(aField, aIconInfoContainer, aUiBuilder),
    iIsUsingDefaultSyncValue(EFalse),
    iInitialSyncConfSelection(-1)
    {
    }

inline void CPbkContactEditorSyncField::ConstructL()
    {
    __ASSERT_DEBUG(ContactItemField().StorageType() == KStorageTypeText, 
        Panic(EPanicPreCond_ConstructL));

    // create textarray
    iSyncArrayText = CEikonEnv::Static()->ReadDesCArrayResourceL(
            R_PBK_SYNC_POPUP_ITEMS);
    iSyncArray = CAknQueryValueTextArray::NewL();
    iSyncArray->SetArray(*iSyncArrayText);
    iSyncTextValues = CAknQueryValueText::NewL();
    iSyncTextValues->SetArrayL(iSyncArray);

    // set selection to correct item, if aField contains some 
    DoSetFieldValueAsCurrentSelection();

    // Create and insert a line in the dialog
    iControl = static_cast<CAknPopupField*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EAknCtPopupField));

    // construct the CAknPopupField from resources
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC(
            reader, R_PBK_SYNCRONIZATION_POPUP_DLG);
    iControl->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(1); // R_PBK_SYNCRONIZATION_POPUP_DLG

    // ctrl is now owned by the dialog
    iControl->SetQueryValueL(iSyncTextValues);

    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    iCaptionedCtrl->SetOfferHotKeys(ETrue);
    }

CPbkContactEditorSyncField* CPbkContactEditorSyncField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
		CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorSyncField* self =
        new(ELeave) CPbkContactEditorSyncField(aField,
        aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorSyncField::~CPbkContactEditorSyncField()
    {
    delete iSyncArray;
    delete iSyncTextValues; 
    delete iSyncArrayText;   
    }

void CPbkContactEditorSyncField::SaveFieldL()
    {
    __ASSERT_DEBUG(iControl && Field().StorageType() == KStorageTypeText, 
            Panic(EPanicPreCond_SaveFieldL));

    iContactDataHasChanged = EFalse;
    
    // retrieve the selected index
    const TInt selectedIndex = iSyncTextValues->CurrentValueIndex();
    // retrieve the syncronization setting text corresponding to the selection
    const TDesC* syncSetting = &KPbkContactSyncPrivate;
    // map setting indexes to sync settings
    switch(selectedIndex)
        {
        case EPbkSyncConfPrivate:
            {
            syncSetting = &KPbkContactSyncPrivate;
            break;
            }
        case EPbkSyncConfPublic:
            {
            syncSetting = &KPbkContactSyncPublic;
            break;
            }
        case EPbkSyncConfNoSync:
            {
            syncSetting = &KPbkContactSyncNoSync;
            break;
            }
        default:
            {
            syncSetting = &KPbkContactSyncPrivate;
            break;
            }
        }

    // compare sync setting to existing setting in field
    if (Field().TextStorage()->Text().CompareF(*syncSetting))
        {
        // store the text in the contact item
        Field().TextStorage()->SetTextL(*syncSetting);
        iContactDataHasChanged = ETrue;
        }
    }

TBool CPbkContactEditorSyncField::FieldDataChanged() const
    {
    TBool result = EFalse;
    if (iContactDataHasChanged)
        {
        if (iIsUsingDefaultSyncValue && 
            iSyncTextValues->CurrentValueIndex() == iInitialSyncConfSelection)
            {
            // default value hasnt been changed
            result = EFalse;
            }
        else
            {
            result = ETrue;
            }
        }
    return result;
    }

void CPbkContactEditorSyncField::AddFieldL(CPbkContactItem& aContact)
    {
    // create field to contact item with default sync value
    TPbkContactItemField* field = aContact.AddOrReturnUnusedFieldL
        (Field().FieldInfo());
    if (field)
        {
        field->TextStorage()->SetTextL(KPbkContactSyncPrivate);
        }
    }

void CPbkContactEditorSyncField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

CEikEdwin* CPbkContactEditorSyncField::Control()
    {
    // not called by the visitor, doesnt contain a edwin
    return NULL;
    }

TPbkFieldId CPbkContactEditorSyncField::FieldId()
    {
    return ContactItemField().FieldInfo().FieldId();
    }

void CPbkContactEditorSyncField::AcceptL
        (MPbkFieldEditorVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

TBool CPbkContactEditorSyncField::ConsumesKeyEvent(
        const TKeyEvent& aKeyEvent, 
        TEventCode aType)
    {
    if (aType == EEventKey && aKeyEvent.iCode == EKeyOK)
        {
        return ETrue;
        }
    return EFalse;
    }

void CPbkContactEditorSyncField::DoSetFieldValueAsCurrentSelection()
    {
    TInt selectedIndex = EPbkSyncConfPrivate;
    iIsUsingDefaultSyncValue = ETrue;

    // get the text from the field
    const TDesC& fieldText = Field().TextStorage()->Text();
    // safely detect whether theres syncronization setting in the field
    if (!fieldText.CompareF(KPbkContactSyncPrivate))
        {
        selectedIndex = EPbkSyncConfPrivate;
        iIsUsingDefaultSyncValue = EFalse;
        }
    else if (!fieldText.CompareF(KPbkContactSyncPublic))
        {
        selectedIndex = EPbkSyncConfPublic;
        iIsUsingDefaultSyncValue = EFalse;
        }
    else if (!fieldText.CompareF(KPbkContactSyncNoSync))
        {
        selectedIndex = EPbkSyncConfNoSync;
        iIsUsingDefaultSyncValue = EFalse;
        }

    // set default value
    iSyncTextValues->SetCurrentValueIndex(selectedIndex);
    // store initial sync selection index
    iInitialSyncConfSelection = selectedIndex;
    }

// End of File

