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
* Description:  
*
*/


#include "TPbk2SortOrderFieldMapper.h"

// From Virtual Phonebook Engine
#include <MVPbkFieldType.h>
#include <MVPbkViewContact.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkBaseContactFieldCollection.h>

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// TPbk2SortOrderFieldMapper::TPbk2SortOrderFieldMapper
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2SortOrderFieldMapper::TPbk2SortOrderFieldMapper()
    {
	}

// --------------------------------------------------------------------------
// TPbk2SortOrderFieldMapper::SetSortOrder
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2SortOrderFieldMapper::SetSortOrder(
        const MVPbkFieldTypeList& aSortOrder )
    {
    iSortOrder = &aSortOrder;
    }

// --------------------------------------------------------------------------
// TPbk2SortOrderFieldMapper::SetContactFields
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2SortOrderFieldMapper::SetContactFields(
        const MVPbkBaseContactFieldCollection& aContactFields )
    {
    iContactFields = &aContactFields;
    }

// --------------------------------------------------------------------------
// TPbk2SortOrderFieldMapper::FieldCount
// --------------------------------------------------------------------------
//
EXPORT_C TInt TPbk2SortOrderFieldMapper::FieldCount() const
    {
    return iSortOrder->FieldTypeCount();
    }

// --------------------------------------------------------------------------
// TPbk2SortOrderFieldMapper::FieldAt
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkBaseContactField* TPbk2SortOrderFieldMapper::FieldAt(
        TInt aIndex ) const
    {
    const MVPbkBaseContactField* result = NULL;
    const MVPbkFieldType& sortOrderType = iSortOrder->FieldTypeAt( aIndex );

    const TInt maxMatchPriority = iSortOrder->MaxMatchPriority();
    const TInt fieldCount = iContactFields->FieldCount();

    for ( TInt matchPriority = 0; 
          !result && matchPriority <= maxMatchPriority; 
          ++matchPriority )
        {
        for ( TInt i = 0; i < fieldCount; ++i )
            {
            const MVPbkFieldType* contactFieldType =
                iContactFields->FieldAt( i ).MatchFieldType( matchPriority );
            if ( contactFieldType && sortOrderType.IsSame( *contactFieldType ) )
                {
                result = &iContactFields->FieldAt( i );
                break;
                }
            }
        }

    return result;
    }
// End of file
