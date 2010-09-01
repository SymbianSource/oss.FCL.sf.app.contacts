/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An adapter between virtual phonebook framework 
*                and VPbkSimStoreImpl field
*
*/


// INCLUDES
#include "TStoreContactField.h"
#include "CContact.h"
#include "CContactLink.h"
#include "CContactStore.h"
#include "CFieldTypeMappings.h"
#include <MVPbkBaseContact.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkSimCntField.h>
#include <CVPbkSimContact.h>
#include <VPbkSimStoreCommon.h>

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TStoreContactField::TStoreContactField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TStoreContactField::TStoreContactField()
    {
    }

// -----------------------------------------------------------------------------
// TStoreContactField::SetParentContact
// -----------------------------------------------------------------------------
//
void TStoreContactField::SetParentContact( CContact& aParentContact )
    {
    iParentContact = &aParentContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::SetSimField
// -----------------------------------------------------------------------------
//
void TStoreContactField::SetSimField( CVPbkSimCntField& aSimField )
    {
    iSimField = &aSimField;
    iFieldData.SetField( aSimField );
    }

// -----------------------------------------------------------------------------
// TStoreContactField::ParentObject
// -----------------------------------------------------------------------------
//
MVPbkObjectHierarchy& TStoreContactField::ParentObject() const
    {
    return *iParentContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& TStoreContactField::ParentContact() const
    {
    return *iParentContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::MatchFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TStoreContactField::MatchFieldType( 
    TInt /*aMatchPriority*/ ) const
    {
    return iParentContact->Store().FieldTypeMappings().Match( 
        iSimField->Type() );
    }

// -----------------------------------------------------------------------------
// TStoreContactField::BestMatchingFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TStoreContactField::BestMatchingFieldType() const
    {
    TInt dummy = 0;
    return MatchFieldType( dummy );
    }

// -----------------------------------------------------------------------------
// TStoreContactField::FieldData
// -----------------------------------------------------------------------------
//
const MVPbkContactFieldData& TStoreContactField::FieldData() const
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::IsSame
// -----------------------------------------------------------------------------
//
TBool TStoreContactField::IsSame( const MVPbkBaseContactField& aOther ) const
    {
    if ( &ParentContact() == &aOther.ParentContact() )
        {
        return iSimField == static_cast<const TStoreContactField&>
            ( aOther ).iSimField; 
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::SupportsLabel
// -----------------------------------------------------------------------------
//
TBool TStoreContactField::SupportsLabel() const
    {
    // Currently sim store doesn't support field labels. However, it's possible
    // to use labels in additional numbers of the USIM (EF AAS) if the lower
    // level software and USIM card supports it.
    return 
        iParentContact->ParentStore().StoreProperties().SupportsFieldLabels();
    }
    
// -----------------------------------------------------------------------------
// TStoreContactField::FieldLabel
// -----------------------------------------------------------------------------
//
TPtrC TStoreContactField::FieldLabel() const
    {
    return KNullDesC();
    }

// -----------------------------------------------------------------------------
// TStoreContactField::SetFieldLabelL
// -----------------------------------------------------------------------------
//
void TStoreContactField::SetFieldLabelL( const TDesC& /*aText*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// TStoreContactField::MaxLabelLength
// -----------------------------------------------------------------------------
//
TInt TStoreContactField::MaxLabelLength() const
    {
    // SIM store doesn't support label editing. Return 0 as documented in
    // MVPbkStoreContactField API
    return 0;
    }
    
// -----------------------------------------------------------------------------
// TStoreContactField::FieldData
// -----------------------------------------------------------------------------
//
MVPbkContactFieldData& TStoreContactField::FieldData()
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::CloneLC
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* TStoreContactField::CloneLC() const
    {
    TStoreContactField* clone =
        new (ELeave) TStoreContactField;
    clone->SetParentContact(*this->iParentContact);
    clone->SetSimField(*this->iSimField);
    CleanupDeletePushL(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::CreateLinkLC
// -----------------------------------------------------------------------------
//    
MVPbkContactLink* TStoreContactField::CreateLinkLC() const
    {
    MVPbkContactLink* ret = NULL;
    if ( iParentContact->SimContact().SimIndex() != KVPbkSimStoreFirstFreeIndex )
        {
        TInt index = KErrNotFound;
        
        // Find the index of this field in the parent contact
        const TInt count = iParentContact->SimContact().FieldCount();
        for (TInt i = 0; i < count; ++i)
            {
            if (&iParentContact->SimContact().FieldAt(i) == iSimField)
                {
                index = i;
                break;
                }
            }
        ret = CContactLink::NewLC(
                iParentContact->Store(), 
                iParentContact->SimContact().SimIndex(),
                index);
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// TContactNewField::TContactNewField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TContactNewField::TContactNewField( CVPbkSimCntField* aSimField ) 
    {
    // This instance has an ownership of the aSimField now
    TStoreContactField::SetSimField( *aSimField );
    iOwnsField = ETrue;
    }

// Destructor
TContactNewField::~TContactNewField() 
    {
    if ( iOwnsField )
        {
        delete TStoreContactField::SimField();
        }
    }

// -----------------------------------------------------------------------------
// TContactNewField::SimField
// -----------------------------------------------------------------------------
//
CVPbkSimCntField* TContactNewField::SimField()
    {
    iOwnsField = EFalse;
    return TStoreContactField::SimField();
    }
}  // namespace VPbkSimStore

