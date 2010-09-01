/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Maps a Contact Model field to a Virtual Phonebook field.
*
*/

// INCLUDES
#include "TContactField.h"

// From VPbkCntModel
#include "CContact.h"
#include "CContactLink.h"
#include "CFieldTypeMap.h"
#include "CFieldFactory.h"
#include "CContactStore.h"

// From VPbkEng
#include <MVPbkContactStoreProperties.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <VPbkStoreUriLiterals.h>
#include <TVPbkContactStoreUriPtr.h>

// System includes
#include <cntdef.h>
#include <cntfield.h>
#include <cntitem.h>
#include <cntdb.h>


namespace VPbkCntModel {

const TInt KSpeedDialKey1 = 1;
const TInt KSpeedDialKey2 = 2;
const TInt KSpeedDialKey3 = 3;
const TInt KSpeedDialKey4 = 4;
const TInt KSpeedDialKey5 = 5;
const TInt KSpeedDialKey6 = 6;
const TInt KSpeedDialKey7 = 7;
const TInt KSpeedDialKey8 = 8;
const TInt KSpeedDialKey9 = 9;

TContactField::TContactField(CContact& aParentContact) :
    iParentContact(&aParentContact),
    iCntModelField(NULL)
    {
    }

TContactField::~TContactField()
    {
    }

void TContactField::SetField
        (CContactItemField& aCntModelField)
    {
    iCntModelField = &aCntModelField;
    iFieldData.SetField(aCntModelField);
    }

void TContactField::SetParentContact(CContact& aParentContact)
    {
    iParentContact = &aParentContact;
    iFieldData.ResetField();
    }

MVPbkBaseContact& TContactField::ParentContact() const
    {
    return *iParentContact;
    }

const MVPbkFieldType* TContactField::MatchFieldType(TInt aMatchPriority) const
    {
    const MVPbkFieldType* type =
        iParentContact->FieldTypeMap().GenericFieldType(*iCntModelField,
            aMatchPriority);

    // Match to the type only if it's supported by the store
    // (=system template).
    // E.g 3rd party can add fields that are not in system template using
    // the Contacts Model API.
    if ( type &&
         iParentContact->Store().FieldFactory().ContainsSame( *type ) )
        {
        return type;
        }
    return NULL;
    }

const MVPbkFieldType* TContactField::BestMatchingFieldType() const
    {
    const MVPbkFieldType* type = NULL;
    TInt maxPriority = iParentContact->ContactStore().StoreProperties().
        SupportedFields().MaxMatchPriority();
    for (TInt i = 0; i <= maxPriority && !type; ++i)
        {
        type = MatchFieldType(i);
        }
    return type;
    }

TBool TContactField::SupportsLabel() const
    {
    return
        iParentContact->ParentStore().StoreProperties().SupportsFieldLabels();
    }

TPtrC TContactField::FieldLabel() const
    {
    return iCntModelField->Label();
    }

void TContactField::SetFieldLabelL(const TDesC& aText)
    {
    iCntModelField->SetLabelL(aText);
    }

TInt TContactField::MaxLabelLength() const
    {
    return KVPbkUnlimitedLabelLength;
    }

const MVPbkContactFieldData& TContactField::FieldData() const
    {
    return iFieldData;
    }

MVPbkContactFieldData& TContactField::FieldData()
    {
    return iFieldData;
    }

TBool TContactField::IsSame(const MVPbkBaseContactField& aOther) const
    {
    if (&aOther.ParentContact() == iParentContact)
        {
        // Two fields have the same identity if they point to the same Contact
        // Model field.
        const TContactField& otherField = static_cast<const TContactField&>(aOther);
        return (iCntModelField == otherField.iCntModelField);
        }
    return EFalse;
    }

MVPbkStoreContactField* TContactField::CloneLC() const
    {
    TContactField* clone =
        new (ELeave) TContactField(*iParentContact);
    clone->SetField(*this->NativeField());
    CleanupDeletePushL(clone);
    return clone;
    }

MVPbkContactLink* TContactField::CreateLinkLC() const
    {
    MVPbkContactLink* ret = NULL;
    TContactItemId itemId( iParentContact->NativeContact()->Id() );
    if ( itemId != KGoldenTemplateId &&
         itemId != KNullContactId )
        {
        ret = CContactLink::NewLC(iParentContact->Store(),
                               iParentContact->NativeContact()->Id(),
                               iCntModelField->Id());
        }
    return ret;
    }

TAny* TContactField::StoreContactFieldExtension(TUid aExtensionUid)
    {
    if (aExtensionUid == KMVPbkStoreContactFieldExtension2Uid)
        {
        return static_cast<MVPbkStoreContactField2*>(this);
        }
    return NULL;
    }

CArrayFix<TInt>* TContactField::SpeedDialIndexesL()
    {
/* Should the store be checked here?
    MVPbkContactStore* store = aContactManager.ContactStoresL().Find(
        KVPbkDefaultCntDbURI());

    if (store && &iParentContact->ContactStore() == store)
        {
*/
        CArrayFix<TInt>* speedDialIndexArray = new(ELeave) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL(speedDialIndexArray);

        const TInt fieldtypeCount = NativeField()->ContentType().FieldTypeCount();

        for (TInt i = 0; i < fieldtypeCount; ++i)
            {
            TFieldType fieldType = NativeField()->ContentType().FieldType(i);
            switch (fieldType.iUid)
                {
                case KUidSpeedDialOneValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey1);
                    break;

                case KUidSpeedDialTwoValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey2);
                    break;

                case KUidSpeedDialThreeValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey3);
                    break;

                case KUidSpeedDialFourValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey4);
                    break;

                case KUidSpeedDialFiveValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey5);
                    break;

                case KUidSpeedDialSixValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey6);
                    break;

                case KUidSpeedDialSevenValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey7);
                    break;

                case KUidSpeedDialEightValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey8);
                    break;

                case KUidSpeedDialNineValue:
                    speedDialIndexArray->AppendL(KSpeedDialKey9);
                    break;

                    // NO DEFAULT CASE - search only for speed dial Uids, ignore others
                }
            }

        if (speedDialIndexArray->Count() == 0)
            {
            CleanupStack::PopAndDestroy(speedDialIndexArray);
            return NULL;
            }
        CleanupStack::Pop(); // speedDialIndexArray
        return speedDialIndexArray;
    }

MVPbkObjectHierarchy& TContactField::ParentObject() const
    {
    return *iParentContact;
    }

void TContactField::ResetNativeField()
    {
    iCntModelField = NULL;
    iFieldData.ResetField();
    }

TInt TContactField::NativeFieldIndex() const
    {
    TInt result = KErrNotFound;

    CContactItemFieldSet& fields = iParentContact->NativeContact()->CardFields();
    const TInt count = fields.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (&fields[i] == NativeField())
            {
            result = i;
            break;
            }
        }

    return result;
    }

}  // namespace VPbkCntModel

