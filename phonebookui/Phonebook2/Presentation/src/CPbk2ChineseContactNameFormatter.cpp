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
*     Chinese contact name formatter.
*
*/


#include "CPbk2ChineseContactNameFormatter.h"

// ================= MEMBER FUNCTIONS =======================
// --------------------------------------------------------------------------
// CPbk2ChineseContactNameFormatter::CPbk2ChineseContactNameFormatter
// --------------------------------------------------------------------------
//
CPbk2ChineseContactNameFormatter::CPbk2ChineseContactNameFormatter(
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    const CPbk2SortOrderManager& aSortOrderManager) 
    :   CPbk2ContactNameFormatter( aMasterFieldTypeList,
            aSortOrderManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ChineseContactNameFormatter::~CPbk2ChineseContactNameFormatter
// --------------------------------------------------------------------------
//
CPbk2ChineseContactNameFormatter::~CPbk2ChineseContactNameFormatter()
    {
    }
    
// --------------------------------------------------------------------------
// CPbk2ChineseContactNameFormatter::NewL
// --------------------------------------------------------------------------
//
CPbk2ChineseContactNameFormatter* CPbk2ChineseContactNameFormatter::NewL(
        const TDesC& aUnnamedText, 
        const MVPbkFieldTypeList& aMasterFieldTypeList, 
        const CPbk2SortOrderManager& aSortOrderManager,
        CVPbkFieldTypeSelector* aTitleFieldSelector )
	{
    CPbk2ChineseContactNameFormatter* self = 
        new(ELeave) CPbk2ChineseContactNameFormatter(
            aMasterFieldTypeList, 
            aSortOrderManager );
    CleanupStack::PushL( self );
    self->ConstructL( aUnnamedText, aTitleFieldSelector );
    CleanupStack::Pop( self );
    return self;
	}
    
// --------------------------------------------------------------------------
// CPbk2ChineseContactNameFormatter::IsFindSeparatorChar
// --------------------------------------------------------------------------
//
TBool CPbk2ChineseContactNameFormatter::IsFindSeparatorChar( TChar aCh )
    {
    return aCh==' '||aCh=='-'||aCh=='\t';
    }

// End of File
