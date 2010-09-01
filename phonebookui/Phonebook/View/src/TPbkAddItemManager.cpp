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
*       Provides methods for phonebook add item manager.
*
*/


// INCLUDE FILES

// This class
#include "TPbkAddItemManager.h"

// PbkView classes
#include "TPbkAddItemWrapper.h"
#include "CPbkMemoryEntrySelectAddItemDlg.h"
#include "MPbkContactEditorCreateField.h"
#include <MPbkContactEditorField.h>

// PbkEng classes
#include <CPbkFieldsInfo.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldInfoGroup.h>
#include <CPbkContactItem.h>

// ==================== MEMBER FUNCTIONS ====================
TPbkAddItemManager::TPbkAddItemManager
		(CPbkContactItem& aItem,
		MPbkContactEditorCreateField& aAddField) :
	iContactItem(aItem),
	iAddField(aAddField)
    {
    }

TPbkAddItemManager::~TPbkAddItemManager()
    {
    }

void TPbkAddItemManager::AddFieldsL(
        const CPbkFieldsInfo& aFieldsInfo, 
        RArray<TInt>& aAddedItemsControlIds)
    {    
    RArray<TPbkAddItemWrapper> addItems(4);
    CleanupClosePushL(addItems);
    
    // Create array of TPbkAddItemWrapper items to show in the
    // select dialog
    CreateAddItemArrayL(aFieldsInfo, addItems);
    
    // Create and show dialog for selecting fieldinfo(s) to add to contact
    CPbkMemoryEntrySelectAddItemDlg* addDlg =
		new (ELeave) CPbkMemoryEntrySelectAddItemDlg;
    TPbkAddItemWrapper* addWrapper = addDlg->ExecuteLD(addItems);
    
    if (addWrapper)
        {
        AddFieldsToContactL(*addWrapper, aAddedItemsControlIds);                
        }

    CleanupStack::PopAndDestroy(); // addItems
    }

TBool TPbkAddItemManager::ContactHasGroupItems(
        const CPbkFieldInfoGroup& aGroup) const
    {    
    for (TInt i = 0; i < aGroup.Count(); ++i)
        {
        if (iContactItem.FindField(aGroup.At(i)))
            {
            // Field defined in group found from contact
            return ETrue;
            }
        }
    // None of the fields of the group was found from the contact
    return EFalse;
    }

void TPbkAddItemManager::AddFieldsToContactL(
        const TPbkAddItemWrapper& aItemWrapper,
        RArray<TInt>& aAddedItemsControlIds)
    {
    CleanupClosePushL( aAddedItemsControlIds );
    aAddedItemsControlIds.Reset();

    for (TInt i = 0; i < aItemWrapper.FieldInfoCount(); ++i)
        {
        CPbkFieldInfo* fieldInfo = aItemWrapper.FieldInfoAt(i);    
        if (fieldInfo)
            {
            // add field to contact and list of added fields
			MPbkContactEditorField& field = iAddField.CreateFieldL(*fieldInfo);
            User::LeaveIfError(aAddedItemsControlIds.Append(field.ControlId()));
			}
        }
    CleanupStack::Pop();
    }

/**
 * This function creates the list of selectable items for the 
 * query dialog in aAddItems array. The items are either CPbkFieldInfo
 * or CPbkFieldInfoGroup instances wrapped in TPbkAddItemWrapper.
 */
void TPbkAddItemManager::CreateAddItemArrayL(
        const CPbkFieldsInfo& aFieldsInfo,
        RArray<TPbkAddItemWrapper>& aAddItems) const
    {
    CleanupClosePushL( aAddItems );
    aAddItems.Reset();
    const TInt fieldInfoCount = aFieldsInfo.Count();
    
    RPointerArray<CPbkFieldInfoGroup> addedGroups;
    CleanupClosePushL(addedGroups);

    for (TInt i=0; i < fieldInfoCount; ++i)
        {
        CPbkFieldInfo* fieldInfo = aFieldsInfo[i];
        if ( fieldInfo->UserCanAddField() &&
             ( fieldInfo->Multiplicity() == EPbkFieldMultiplicityMany ||
               !iContactItem.FindField(*fieldInfo)) )
            {
            const CPbkFieldInfoGroup* group = fieldInfo->Group(); 
            if (group)
                {
                // If item has group definition we must decide whether to 
                // add the field info, field info group or nothing.
                if (addedGroups.Find(group) == KErrNotFound)
                    {
                    // Add the group or item only if the group in question
                    // is not already added.
                    if (ContactHasGroupItems(*group))
                        {
                        // Contact already has at least one item of the group;
                        // add only current field.
                        User::LeaveIfError(aAddItems.Append(
							TPbkAddItemWrapper(*fieldInfo)));
                        }
                    else
                        {
                        // No items of the group exist in contact: add the whole
                        // group and not only the field in question.
                        User::LeaveIfError(aAddItems.Append(
							TPbkAddItemWrapper(*group)));
                        // Add group to "added groups" list
                        User::LeaveIfError(addedGroups.Append(group));
                        }
                    }
                }
            else
                {
                // No group defined for the field; add only the current field.
                User::LeaveIfError(aAddItems.Append(TPbkAddItemWrapper(*fieldInfo)));                
                }
            }
        }
    CleanupStack::PopAndDestroy(); // addedGroups
    CleanupStack::Pop();
    }

// End of File
