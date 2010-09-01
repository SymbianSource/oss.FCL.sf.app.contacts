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
*           Methods for Contact editor field factory class.
*
*/


// INCLUDE FILES
#include "PbkContactEditorFieldFactory.h"  // this class
#include "TPbkContactItemField.h"
#include "CPbkFieldInfo.h"
#include "CPbkContactEditorPhoneNumberField.h"
#include "CPbkContactEditorTextField.h"
#include "CPbkContactEditorNumberField.h"
#include "CPbkContactEditorDateField.h"
#include "CPbkContactEditorUrlField.h"
#include "CPbkContactEditorEmailAddressField.h"
#include "CPbkContactEditorReadingField.h"
#include "CPbkContactEditorPostalCodeField.h"
#include "CPbkContactEditorSyncField.h"

/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
/// Panic codes for PbkContactEditorFieldFactory
enum TPanicCode
    {
    EPanicUnknownControlType_CreateFieldL
    };
#endif  // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "PbkContactEditorFieldFactory");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

MPbkContactEditorField* PbkContactEditorFieldFactory::CreateFieldL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
        CPbkIconInfoContainer& aIconInfoContainer)
    {
    MPbkContactEditorField* editorField = NULL;
    switch(aField.FieldInfo().CtrlType())
        {
        case EPbkFieldCtrlTypeTextEditor:
            {
            if (aField.FieldInfo().IsPhoneNumberField()
                || aField.FieldInfo().FieldId() == EPbkFieldIdDTMFString)
                {
                editorField = CPbkContactEditorPhoneNumberField::NewL(
                        aField, aUiBuilder,
                        aIconInfoContainer);
                }
            else if (aField.FieldInfo().FieldId() == EPbkFieldIdURL)
                {
                editorField = CPbkContactEditorUrlField::NewL(
                        aField, aUiBuilder, aIconInfoContainer);
                }
            else if (aField.FieldInfo().FieldId() == EPbkFieldIdEmailAddress)
                {
                editorField = CPbkContactEditorEmailAddressField::NewL(
                        aField, aUiBuilder, aIconInfoContainer);
                }
            else if (aField.FieldInfo().FieldId() == EPbkFieldIdPostalCode)
                {
                editorField = CPbkContactEditorPostalCodeField::NewL(
                        aField, aUiBuilder, aIconInfoContainer);
                }
            else if (aField.FieldInfo().IsReadingField())
                {
                editorField = CPbkContactEditorReadingField::NewL(
                        aField, aUiBuilder, aIconInfoContainer);
                }
            else
                {
                editorField = CPbkContactEditorTextField::NewL(
                        aField, aUiBuilder, aIconInfoContainer);
                }
            break;
            }

        case EPbkFieldCtrlTypeNumberEditor:
            {
            editorField = CPbkContactEditorNumberField::NewL(
                    aField, aUiBuilder, aIconInfoContainer);
            break;
            }

        case EPbkFieldCtrlTypeDateEditor:
            {
            editorField = CPbkContactEditorDateField::NewL(
                    aField, aUiBuilder, aIconInfoContainer);
            break;
            }

        case EPbkFieldCtrlTypeChoiseItems:
            {
            editorField = CPbkContactEditorSyncField::NewL(
                    aField, aUiBuilder, aIconInfoContainer);
            break;
            }

        default:
            {
            __ASSERT_DEBUG(EFalse,
                Panic(EPanicUnknownControlType_CreateFieldL));
            break;
            }
        }
    return editorField;
    }


// End of File
