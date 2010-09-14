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
*           Methods for Phonebook Contact editor field array.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorFieldArray.h"  // this class

#include "MPbkContactEditorUiBuilder.h"
#include <MPbkContactEditorField.h>
#include "PbkContactEditorFieldFactory.h"
#include "PbkFieldFormatterFactory.h"
#include "CPbkContactItem.h"
#include "MPbkPhoneNumberFormat.h"
#include "PbkIconInfo.h"
#include "CPbkFieldInfo.h"
#include "CPbkReadingEditorBinderVisitor.h"
#include <pbkview.rsg>

#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>

#include <PbkDebug.h>

/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

const TInt KArrayGranularity = 5;

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorFieldArray
enum TPanicCode
    {
	EPanicPostCond_ConstructL,
	EPanicInvariant_FieldNotFound,
	EPanicInvariant_MembersNotCreated,
	EPanicPreCond_CreateFieldsFromContactL,
	EPanicPreCond_DoAddFieldL,
	EPanicInvariant_CountMismatch
    };
#endif  // _DEBUG

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorFieldArray");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorFieldArray::CPbkContactEditorFieldArray
        (CPbkContactItem& aContactItem, 
        MPbkContactEditorUiBuilder& aUiBuilder) :
    iContactItem(aContactItem),
    iUiBuilder(aUiBuilder),
    iFieldArray(KArrayGranularity)
    {
    }

inline void CPbkContactEditorFieldArray::ConstructL()
    {
    iIconInfoContainer = CPbkIconInfoContainer::NewL(R_PBK_ICON_INFO_ARRAY);

    // Get extension factory for setting extension icons
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();    
    factory.AddPbkFieldIconsL(iIconInfoContainer);
    CleanupStack::PopAndDestroy(extGlobal);

    // Construct the reading field binder (visitor pattern)
    iReadingFieldBinder = CPbkReadingEditorBinderVisitor::NewL();

    __TEST_INVARIANT;

    //PostCond:
    __ASSERT_DEBUG(iIconInfoContainer, 
            Panic(EPanicPostCond_ConstructL));
    }

CPbkContactEditorFieldArray* CPbkContactEditorFieldArray::NewL
        (CPbkContactItem& aContactItem, 
        MPbkContactEditorUiBuilder& aUiBuilder)
    {
	CPbkContactEditorFieldArray* dlg = new(ELeave) CPbkContactEditorFieldArray(aContactItem, aUiBuilder);
    CleanupStack::PushL(dlg);
    dlg->ConstructL();
    CleanupStack::Pop();  // dlg
    return dlg;
    }

CPbkContactEditorFieldArray::~CPbkContactEditorFieldArray()
    {
	__TEST_INVARIANT;

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEditorFieldArray::~CPbkContactEditorFieldArray"));

    // delete member data
    delete iReadingFieldBinder;
    delete iIconInfoContainer;

    iFieldArray.ResetAndDestroy();
    }

void CPbkContactEditorFieldArray::CreateFieldsFromContactL()
    {
	__ASSERT_DEBUG(iFieldArray.Count() == 0, Panic(EPanicPreCond_CreateFieldsFromContactL));

	__TEST_INVARIANT;

    CPbkFieldArray& fieldSet = iContactItem.CardFields();
    const TInt fieldCount = fieldSet.Count();
    for (TInt i=0; i < fieldCount; ++i)
        {
        TPbkContactItemField& contactItemField = fieldSet[i];
        if (contactItemField.FieldInfo().IsEditable())
            {
	        MPbkContactEditorField* field = PbkContactEditorFieldFactory::CreateFieldL(
                    contactItemField, iUiBuilder, *iIconInfoContainer);
            iFieldArray.InsertL(iFieldArray.Count(), field);
            }
        }

    AcceptL(*iReadingFieldBinder);

	__TEST_INVARIANT;
    }

void CPbkContactEditorFieldArray::AcceptL(MPbkFieldEditorVisitor& aVisitor)
    {
    const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iFieldArray[i]->AcceptL(aVisitor);
        }
    }

TBool CPbkContactEditorFieldArray::AreAllFieldsEmpty() const
    {
	__TEST_INVARIANT;

	TBool result = ETrue;
    const CPbkFieldArray& fields = iContactItem.CardFields();
    const TInt fieldCount = fields.Count();
    for (TInt i=0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& contactItemField = fields[i];
        if ( !contactItemField.IsEmptyOrAllSpaces() &&
                // The Synchronization field does not count here
                contactItemField.FieldInfo().FieldId()
                != EPbkFieldIdSyncronization )
            {
			result = EFalse;
			break;
            }
        }

	__TEST_INVARIANT;

    return result;
    }

void CPbkContactEditorFieldArray::AddFieldL
        (TPbkContactItemField& aField)
    {
	__TEST_INVARIANT;

	DoAddFieldL(aField, iFieldArray.Count());

	__TEST_INVARIANT;
    }

void CPbkContactEditorFieldArray::RemoveField
        (MPbkContactEditorField& aField)
    {
	__TEST_INVARIANT;

	const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        MPbkContactEditorField* field = iFieldArray[i];
        if (field == &aField)
            {
            iReadingFieldBinder->NotifyEditorDeletion( field->FieldId() );
            iUiBuilder.DeleteControl(field->ControlId());     
            iFieldArray.Delete(i);
			iContactItem.RemoveField(
				iContactItem.FindFieldIndex(aField.ContactItemField()));
			delete field;
			break;
            }
        }

	__TEST_INVARIANT;
    }

TInt CPbkContactEditorFieldArray::ContactItemFieldCount() const
    {
	__TEST_INVARIANT;

    return iContactItem.CardFields().Count();
    }

TInt CPbkContactEditorFieldArray::EditorCount() const
    {
	__TEST_INVARIANT;

    return iFieldArray.Count();
    }

const MPbkContactEditorField& CPbkContactEditorFieldArray::FieldAt
		(TInt aFieldIndex)
	{
	__TEST_INVARIANT;

	return *iFieldArray[aFieldIndex];
	}

MPbkContactEditorField* CPbkContactEditorFieldArray::Find
        (const TPbkContactItemField& aField) const
    {
	__TEST_INVARIANT;

    MPbkContactEditorField* result = NULL;
	const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldArray[i]->ContactItemField().IsSame(aField))
            {
            result = iFieldArray[i];
            break;
            }
        }

	__TEST_INVARIANT;
    return result;
    }

MPbkContactEditorField* CPbkContactEditorFieldArray::Find
        (TInt aControlId) const
    {
	__TEST_INVARIANT;

    MPbkContactEditorField* field = NULL;
	const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldArray[i]->ControlId() == aControlId)
            {
            field = iFieldArray[i];
            break;
            }
        }

	__TEST_INVARIANT;
    return field;
    }

TInt CPbkContactEditorFieldArray::FindContactItemFieldIndex
        (const TPbkContactItemField& aField) const
    {
	__TEST_INVARIANT;

    return iContactItem.FindFieldIndex(aField);
    }

void CPbkContactEditorFieldArray::SaveFieldsL()
    {
	__TEST_INVARIANT;

    for (TInt i = 0; i < iFieldArray.Count(); ++i)
        {
        iFieldArray[i]->SaveFieldL();
        }

	__TEST_INVARIANT;
	}

TBool CPbkContactEditorFieldArray::FieldsChanged() const
    {
	__TEST_INVARIANT;

	TBool result = EFalse;
    for (TInt i = 0; i < iFieldArray.Count(); ++i)
        {
        if (iFieldArray[i]->FieldDataChanged())
			{
			result = ETrue;
			}
        // can break because at least one field has changed
        if (result)
            {
            break;
            }
        }

	__TEST_INVARIANT;
    return result;
    }

void CPbkContactEditorFieldArray::SetFocusToIndex
		(TInt aFocusIndex)
	{
	__TEST_INVARIANT;

	MPbkContactEditorField* field = Find(iContactItem.CardFields()[aFocusIndex]);
	TInt err = KErrNotFound;
	if (field)
		{
		TRAP(err, iUiBuilder.TryChangeFocusL(field->ControlId()));
		}

	for (TInt i = 0; i < iFieldArray.Count() && err!=KErrNone; ++i)
		{
		if (iUiBuilder.Control(iFieldArray[i]->ControlId()))
			{
			TRAP(err,iUiBuilder.TryChangeFocusL(iFieldArray[i]->ControlId()));
			}
		}

	__TEST_INVARIANT;
	}

MPbkContactEditorField& CPbkContactEditorFieldArray::CreateFieldL
		(CPbkFieldInfo& aFieldInfo)
	{
	__TEST_INVARIANT;

	TPbkContactItemField& newField = iContactItem.AddFieldL(aFieldInfo);
	
	// Find field from iContact item. 
	// Its editable field index is the insertion position
	TInt fieldIndex = 0;
	for (TInt i = 0; i < iContactItem.CardFields().Count(); ++i)
		{
		const TPbkContactItemField& field = iContactItem.CardFields()[i];
		const TBool match = field.IsSame(newField);
		if(field.FieldInfo().IsEditable() && !match)
			{
			fieldIndex++;
			}
		if (match)
			{
			break;
			}
		}
		
	MPbkContactEditorField& addedField = DoAddFieldL(newField, fieldIndex);
    addedField.AcceptL(*iReadingFieldBinder);
    addedField.ActivateL();
	
	__TEST_INVARIANT;

	return addedField;
	}

/**
 * Not class invariant compliant.
 */
MPbkContactEditorField& CPbkContactEditorFieldArray::DoAddFieldL
        (TPbkContactItemField& aField,
		TInt aFieldInsertIndex)
    {
	__ASSERT_DEBUG(aFieldInsertIndex <= iFieldArray.Count(), 
		Panic(EPanicPreCond_DoAddFieldL));

    // create same order in UI
	if (aFieldInsertIndex > 0)
		{
		// Focus must be set to the field preceeding the insertion point.
		// Please note that this means it is not possible to insert to the 
		// first dialog position!
		const TInt controlId = iFieldArray[aFieldInsertIndex-1]->ControlId();
		iUiBuilder.TryChangeFocusL(controlId);
		}

	MPbkContactEditorField* field = PbkContactEditorFieldFactory::CreateFieldL(
            aField, iUiBuilder, *iIconInfoContainer);
    iFieldArray.InsertL(aFieldInsertIndex, field);
	return *field;
    }

void CPbkContactEditorFieldArray::__DbgTestInvariant() const
	{
	#if defined(_DEBUG)

	__ASSERT_DEBUG(iFieldArray.Count() <= iContactItem.CardFields().Count(), 
			Panic(EPanicInvariant_CountMismatch));

	const TInt count = iFieldArray.Count();
	for (TInt i = 0; i < count; ++i)
		{
		TBool result = EFalse;
		for (TInt j = 0; j < iContactItem.CardFields().Count() && !result; ++j)
			{
			TPbkContactItemField& field = iContactItem.CardFields()[j];
			if (&field.ContactItemField() == &iFieldArray[j]->ContactItemField().ContactItemField() &&
				field.FieldInfo().IsEditable())
				{
				result = ETrue;
				}
			}
		__ASSERT_DEBUG(result, Panic(EPanicInvariant_FieldNotFound));
		}

    __ASSERT_DEBUG(iIconInfoContainer, 
            Panic(EPanicInvariant_MembersNotCreated));

	#endif
	}

// End of File
