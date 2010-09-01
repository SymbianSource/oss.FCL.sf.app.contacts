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
* Description:  A vpbk field type list that contains sim supported types
*
*/



// INCLUDE FILES
#include "CSupportedFieldTypes.h"

#include "CFieldTypeMappings.h"
#include <TVPbkSimStoreProperty.h>

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::CSupportedFieldTypes
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes::CSupportedFieldTypes()
    {
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSupportedFieldTypes::ConstructL( 
    const CFieldTypeMappings& aFieldTypeMappings,
    TVPbkGsmStoreProperty& aSimStoreProperty,
    TVPbkUSimStoreProperty* aUSimStoreProperty )
    {
    // SIM supports always the name field
    const MVPbkFieldType* vpbkType = aFieldTypeMappings.Match( EVPbkSimName );
    iSupportedTypes.AppendL( vpbkType );
    // SIM supports always one number field
    vpbkType = aFieldTypeMappings.Match( EVPbkSimGsmNumber );
    const MVPbkFieldType* vpbkGsmType = vpbkType;
    iSupportedTypes.AppendL( vpbkType );
    // USIM can support additional numbers
    if ( aSimStoreProperty.iCaps & VPbkSimStoreImpl::KAdditionalNumUsed )
        {
        if( aUSimStoreProperty )
            {
            for( TInt i = 1; i <= aUSimStoreProperty->iMaxNumOfAnrs; i ++ )
                {
                TVPbkSimCntFieldType type;
                switch( i )
                    {
                    case 1:  // first additional number field type 
                        type = EVPbkSimAdditionalNumber1;
                        break;
                    case 2: // second additional number field type
                        type = EVPbkSimAdditionalNumber2;
                        break;
                    case 3: // third additional number field type
                        type = EVPbkSimAdditionalNumber3;
                        break;
                    default: // four and so on 
                        type = EVPbkSimAdditionalNumberLast;
                        break;
                    }
                vpbkType = aFieldTypeMappings.Match( type );
                if( !( vpbkGsmType->IsSame( *vpbkType ) ) )   // if not the same as GSM type.
                    {
                    iSupportedTypes.AppendL( vpbkType );
                    }
                }
            }
        else
            {
            vpbkType = aFieldTypeMappings.Match( EVPbkSimAdditionalNumber );
            // Check if the VPbk type is different than the type for
            // already added EVPbkSimGsmNumber
            if ( iSupportedTypes.Find( vpbkType ) == KErrNotFound )
                {
                iSupportedTypes.AppendL( vpbkType );
                }
            }
        }
    if ( aSimStoreProperty.iCaps & VPbkSimStoreImpl::KSecondNameUsed )
        {
		// Second name field of the USIM is used for nick name in 
        // non-Japanese variants
		vpbkType = aFieldTypeMappings.Match( EVPbkSimNickName );    
        iSupportedTypes.AppendL( vpbkType );
        }
    if ( aSimStoreProperty.iCaps & VPbkSimStoreImpl::KEmailAddressUsed )
        {
        vpbkType = aFieldTypeMappings.Match( EVPbkSimEMailAddress );
        iSupportedTypes.AppendL( vpbkType );
        }
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes* CSupportedFieldTypes::NewL(
    const CFieldTypeMappings& aFieldTypeMappings,
    TVPbkGsmStoreProperty& aSimStoreProperty )
    {
    CSupportedFieldTypes* self = new( ELeave ) CSupportedFieldTypes;
    CleanupStack::PushL( self );
    self->ConstructL( aFieldTypeMappings, aSimStoreProperty );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes* CSupportedFieldTypes::NewL(
    const CFieldTypeMappings& aFieldTypeMappings,
    TVPbkGsmStoreProperty& aSimStoreProperty,
    TVPbkUSimStoreProperty& aUSimStoreProperty )  
    {
    CSupportedFieldTypes* self = new( ELeave ) CSupportedFieldTypes;
    CleanupStack::PushL( self );
    self->ConstructL( aFieldTypeMappings, aSimStoreProperty, &aUSimStoreProperty );  
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CSupportedFieldTypes::~CSupportedFieldTypes()
    {    
    iSupportedTypes.Close();
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FieldTypeCount
// -----------------------------------------------------------------------------
//
TInt CSupportedFieldTypes::FieldTypeCount() const
    {
    return iSupportedTypes.Count();
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FieldTypeAt
// -----------------------------------------------------------------------------
//
const MVPbkFieldType& CSupportedFieldTypes::FieldTypeAt( TInt aIndex ) const
    {
    return *iSupportedTypes[aIndex];
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FieldTypeAt
// -----------------------------------------------------------------------------
//
TBool CSupportedFieldTypes::ContainsSame( 
    const MVPbkFieldType& aFieldType ) const
    {
    TInt count = iSupportedTypes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iSupportedTypes[i] == &aFieldType )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FieldTypeAt
// -----------------------------------------------------------------------------
//
TInt CSupportedFieldTypes::MaxMatchPriority() const
    {
    // SIM contacts have max 1 versit property
    const TInt simTypeMaxPriority = 1;
    return simTypeMaxPriority;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FindMatch
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CSupportedFieldTypes::FindMatch( 
    const TVPbkFieldVersitProperty& aMatchProperty,
    TInt aMatchPriority ) const
    {
    TInt count = iSupportedTypes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iSupportedTypes[i]->Matches( aMatchProperty, aMatchPriority ) )
            {
            return iSupportedTypes[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FindMatch
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CSupportedFieldTypes::FindMatch( 
    TVPbkNonVersitFieldType aNonVersitType ) const
    {
    TInt count = iSupportedTypes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iSupportedTypes[i]->NonVersitType() == aNonVersitType )
            {
            return iSupportedTypes[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::Find
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CSupportedFieldTypes::Find(
        TInt aFieldTypeResId) const
    {
    const MVPbkFieldType* fieldType = NULL;
    for (TInt i = 0; i < iSupportedTypes.Count(); ++i)
        {
        if (iSupportedTypes[i]->FieldTypeResId() == aFieldTypeResId)
            {
            fieldType = iSupportedTypes[i];
            break;
            }
        }
    return fieldType;
    }

} // namespace VPbkSimStore

//  End of File  
