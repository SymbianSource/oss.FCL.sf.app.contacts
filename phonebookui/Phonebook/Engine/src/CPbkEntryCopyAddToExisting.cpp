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
*		Copies information from Phonebook entry to an existing entry.
*
*/


// INCLUDE FILES

// Phonebook copy
#include    "CPbkEntryCopyAddToExisting.h"

// Phonebook debug
#include    <PbkDebug.h>

// Phonebook engine
#include    <CPbkContactEngine.h>
#include    <CPbkContactItem.h>
#include    <CPbkFieldsInfo.h>
#include    <MPbkContactNameFormat.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_FindExistingNameL = 0,
    EPanicPreCond_CopyL
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkEntryCopyAddToExisting");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace


// MODULE DATA STRUCTURES

// ================= MEMBER FUNCTIONS =======================

inline CPbkEntryCopyAddToExisting::CPbkEntryCopyAddToExisting
        (const CPbkContactItem& aEntry,
        CPbkContactEngine& aEngine,
        TContactItemId aContactId) :
    iEntry(aEntry),
    iEngine(aEngine),
    iContactId(aContactId)
    {
    }

CPbkEntryCopyAddToExisting* CPbkEntryCopyAddToExisting::NewLC
        (const CPbkContactItem& aEntry,
        CPbkContactEngine& aEngine,
        TContactItemId aContactId)
    {
    CPbkEntryCopyAddToExisting* self = 
        new(ELeave) CPbkEntryCopyAddToExisting(aEntry, aEngine, aContactId);
    CleanupStack::PushL(self);
    return self;
    }

TContactItemId CPbkEntryCopyAddToExisting::CopyL()
    {
    __ASSERT_DEBUG(iContactId != KNullContactId, 
            Panic(EPanicPreCond_CopyL));

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkEntryCopyAddToExisting::CopyL start"));
    
    CPbkContactItem* item = iEngine.OpenContactLCX(iContactId);

    TBool result = EFalse;
    // find if phone number already in contact    
    const TInt fieldCount = iEntry.CardFields().Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iEntry.CardFields()[i];
        if (field.FieldInfo().IsPhoneNumberField())
            {
            const TDesC& number = field.Text();
            TInt fieldIndex = 0;
            TPbkContactItemField* numberField = 
                item->FindNextFieldWithPhoneNumber(number, 0, fieldIndex);
            if (!numberField)
                {
                // number not found in entry -> copy number to entry
                CPbkFieldInfo* stdNumField = iEngine.FieldsInfo().Find(
                    EPbkFieldIdPhoneNumberMobile);

                TPbkContactItemField* field = item->AddOrReturnUnusedFieldL(*stdNumField);
                if (field->StorageType() == KStorageTypeText)
                    { 
                    // Put the data to the field
                    field->TextStorage()->SetTextL(number);
                    result = ETrue;
                    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkEntryCopyAddToExisting::CopyL number added to existing contact"));
                    }
                }
            else
                {
                // identical entry is already in phonebook
                // this is counted as copied
                result = ETrue;
                PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkEntryCopyAddToExisting::CopyL number duplicate existed, copy counted"));
                }
            }
        if (field.FieldInfo().FieldId() == EPbkFieldIdEmailAddress)
            {
            const TDesC& emailAddr = field.Text();
            TInt fieldIndex = 0;
            TPbkContactItemField* emailField = 
                item->FindNextFieldWithText(emailAddr, fieldIndex);
            if (!emailField)
                {
                // email address not found in entry -> copy number to entry
                CPbkFieldInfo* stdEmailField = iEngine.FieldsInfo().Find(
                    EPbkFieldIdEmailAddress);

                TPbkContactItemField* field = item->AddOrReturnUnusedFieldL(*stdEmailField);
                if (field->StorageType() == KStorageTypeText)
                    { 
                    // Put the data to the field
                    field->TextStorage()->SetTextL(emailAddr);
                    result = ETrue;
                    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkEntryCopyAddToExisting::CopyL email added to existing contact"));
                    }
                }
            else
                {
                // identical entry is already in phonebook this is counted as copied
                result = ETrue;
                PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkEntryCopyAddToExisting::CopyL email duplicate existed, copy counted"));
                }
            }
        }

    TContactItemId contactId = KNullContactId;
    if (result)
        {
        iEngine.CommitContactL(*item, ETrue);
        contactId = item->Id();
        }

    CleanupStack::PopAndDestroy(2); // lock, item
    
    return contactId;
    }

//  End of File  
