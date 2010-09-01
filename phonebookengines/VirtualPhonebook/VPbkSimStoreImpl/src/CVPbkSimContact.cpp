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
* Description:  The sim contact
*
*/



// INCLUDE FILES
#include <featmgr.h>
#include "CVPbkSimContact.h"

#include "CVPbkSimCntField.h"
#include "MVPbkSimCntStore.h"
#include "CVPbkETelCntConverter.h"
#include "VPbkSimStoreTemplateFunctions.h"
#include "VPbkSimStoreImplError.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkSimContact::CVPbkSimContact
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkSimContact::CVPbkSimContact( MVPbkSimCntStore& aSimStore )
:   CVPbkSimContactBase( aSimStore ),
    iSimIndex( KVPbkSimStoreFirstFreeIndex )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkSimContact::ConstructL( const TDesC8* aETelContact )
    {
    if ( aETelContact )
        {
        SetL( *aETelContact );
        if( FeatureManager::FeatureSupported( KFeatureIdFfTdClmcontactreplicationfromphonebooktousimcard ) )
        	{
			IndentifyAdditionalFields();
        	}
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContact* CVPbkSimContact::NewL( const TDesC8& aETelContact,
    MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContact* self = NewLC( aETelContact, aSimStore );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContact* CVPbkSimContact::NewL( MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContact* self = NewLC( aSimStore );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContact* CVPbkSimContact::NewLC( const TDesC8& aETelContact,
    MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContact* self = new( ELeave ) CVPbkSimContact( aSimStore );
    CleanupStack::PushL( self );
    self->ConstructL( &aETelContact );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContact* CVPbkSimContact::NewLC( MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContact* self = new( ELeave ) CVPbkSimContact( aSimStore );
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    return self;
    }

// Destructor
CVPbkSimContact::~CVPbkSimContact()
    {
    iFieldArray.ResetAndDestroy();
    iFieldArray.Close();
    delete iData;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::CreateFieldLC
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimCntField* CVPbkSimContact::CreateFieldLC( 
    TVPbkSimCntFieldType aType ) const
    {
    return CVPbkSimCntField::NewLC( aType, *this );
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::DeleteField
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimContact::DeleteField( TInt aIndex )
    {
    delete iFieldArray[aIndex];
    iFieldArray.Remove( aIndex );
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::AddFieldL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimContact::AddFieldL( CVPbkSimCntField* aField )
    {
    if ( aField )
        {
        iFieldArray.AppendL( aField ); 
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::DeleteAllFields
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimContact::DeleteAllFields()
    {
    for ( TInt i = iFieldArray.Count() - 1; i >= 0; --i )
        {
        // Destroys instances but doesn't free the array memory
        DeleteField( i );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::SaveL
// -----------------------------------------------------------------------------
//
EXPORT_C MVPbkSimStoreOperation* CVPbkSimContact::SaveL( 
        MVPbkSimContactObserver& aObserver )
    {
    __ASSERT_DEBUG( iFieldArray.Count() > 0,
        VPbkSimStoreImpl::Panic( VPbkSimStoreImpl::EZeroFieldCount ) );
    // Convert to ETel contact
    CreateETelContactL();
    return iStore.SaveL( *iData, iSimIndex, aObserver );
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::SetSimIndex
// -----------------------------------------------------------------------------
//
void CVPbkSimContact::SetSimIndex( TInt aSimIndex )
    {
    iSimIndex = aSimIndex;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::ConstFieldAt
// -----------------------------------------------------------------------------
//
const CVPbkSimCntField& CVPbkSimContact::ConstFieldAt( TInt aIndex ) const
    {
    return *iFieldArray[aIndex];
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::FieldCount
// -----------------------------------------------------------------------------
//
TInt CVPbkSimContact::FieldCount() const
    {
    return iFieldArray.Count();
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::SimIndex
// -----------------------------------------------------------------------------
//
TInt CVPbkSimContact::SimIndex() const
    {
    return iSimIndex;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::ETelContactL
// -----------------------------------------------------------------------------
//
const TDesC8& CVPbkSimContact::ETelContactL() const
    {
    CreateETelContactL();
    return *iData;
    }
  
// -----------------------------------------------------------------------------
// CVPbkSimContact::SetL
// -----------------------------------------------------------------------------
// 
void CVPbkSimContact::SetL( const TDesC8& aETelContact )
    {
    iFieldArray.ResetAndDestroy();
    
    HBufC8* buf = aETelContact.AllocLC();
    TPtr8 ptr( buf->Des() );
    iStore.ContactConverter().ConvertFromETelToVPbkSimContactL( 
        ptr, *this );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::IndentifyAdditionalFields()
// -----------------------------------------------------------------------------
//
void CVPbkSimContact::IndentifyAdditionalFields()
    {
    TInt anrNumberFieldCount = 0;
    for( int i = 0; i < iFieldArray.Count(); i ++ )
        {
        if( iFieldArray[i]->Type() == EVPbkSimAdditionalNumber )
            {
            anrNumberFieldCount ++;
            switch( anrNumberFieldCount )
                {
                case 1:
                    iFieldArray[i]->SetType( EVPbkSimAdditionalNumber1 );
                    break;
                case 2:
                    iFieldArray[i]->SetType( EVPbkSimAdditionalNumber2 );
                    break;
                case 3:
                    iFieldArray[i]->SetType( EVPbkSimAdditionalNumber3 );
                    break;
                default:
                    iFieldArray[i]->SetType( EVPbkSimAdditionalNumberLast );
                    break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimContact::CreateETelContactL
// -----------------------------------------------------------------------------
// 
void CVPbkSimContact::CreateETelContactL() const
    {
    // Convert to ETel contact
    HBufC8* buf = 
        iStore.ContactConverter().ConvertFromVPbkSimFieldsToETelCntLC(
        iFieldArray.Array(), iSimIndex );
    
    if (!iData)
        {
        iData = buf;
        CleanupStack::Pop(buf);
        }
    else
        {
        TPtr8 ptr( iData->Des() );
        VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iData, ptr, buf->Length() );
        ptr.Copy( *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    }

//  End of File  
