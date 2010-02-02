/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 field selector.
*
*/


// INCLUDES

#include "CPbk2FieldSelector.h"

// --------------------------------------------------------------------------
// CPbk2FieldSelector::CPbk2FieldSelector
// --------------------------------------------------------------------------
//
inline CPbk2FieldSelector::CPbk2FieldSelector()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2FieldSelector::NewLC
// --------------------------------------------------------------------------
//    
CPbk2FieldSelector* CPbk2FieldSelector::NewLC()
    {    
    CPbk2FieldSelector* self = new ( ELeave ) CPbk2FieldSelector();
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldSelector::~CPbk2FieldSelector
// --------------------------------------------------------------------------
//    
CPbk2FieldSelector::~CPbk2FieldSelector()
    {
    iIncludedSelectorArray.ResetAndDestroy();
    iExcludedSelectorArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2FieldSelector::IsIncluded
// --------------------------------------------------------------------------
//        
inline TBool CPbk2FieldSelector::IsIncluded( 
        TArray<CVPbkFieldTypeSelector*> aArray, 
        const MVPbkBaseContactField& aField ) const
    {
    const TInt count = aArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aArray[i]->IsFieldIncluded( aField ) )
            {
            return ETrue;
            }
        }
    return EFalse;        
    }

// --------------------------------------------------------------------------
// CPbk2FieldSelector::IsFieldIncluded
// --------------------------------------------------------------------------
//
TBool CPbk2FieldSelector::IsFieldIncluded(
        const MVPbkBaseContactField& aField) const
    {
    // First find from excluded array
    TBool isExcluded = IsIncluded( iExcludedSelectorArray.Array(), aField );
    TBool isIncluded = EFalse;
    // If already found from excluded array so not necessary to search from 
    // included array
    if ( !isExcluded )
        {
        isIncluded = IsIncluded( iIncludedSelectorArray.Array(), aField );    
        }
    return isExcluded ? EFalse : isIncluded;        
    }    

// --------------------------------------------------------------------------
// CPbk2FieldSelector::AddIncludedSelector
// --------------------------------------------------------------------------
//    
void CPbk2FieldSelector::AddIncludedSelector( 
        CVPbkFieldTypeSelector* aIncludedSelector )
    {
    if ( iIncludedSelectorArray.Find( aIncludedSelector ) == KErrNotFound )
        {
        TInt result = iIncludedSelectorArray.Append( aIncludedSelector );
        if ( result != KErrNone )
            {
            delete aIncludedSelector;
            aIncludedSelector = NULL;
            }
        }
    else
        {
        delete aIncludedSelector;
        aIncludedSelector = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldSelector::AddExcludedSelector
// --------------------------------------------------------------------------
//    
void CPbk2FieldSelector::AddExcludedSelector( 
        CVPbkFieldTypeSelector* aExcludedSelector )
    {
    if ( iExcludedSelectorArray.Find( aExcludedSelector ) == KErrNotFound )
        {
        TInt result = iExcludedSelectorArray.Append( aExcludedSelector );
        if ( result != KErrNone )
            {
            delete aExcludedSelector;
            aExcludedSelector = NULL;
            }
        }    
    else
        {
        delete aExcludedSelector;
        aExcludedSelector = NULL;        
        }
    }
    
// End of file   
    
