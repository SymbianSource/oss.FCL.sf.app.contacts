/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// TTestContactField.cpp

// INCLUDES
#include "TTestContactField.h"


void TTestContactField::SetFieldType( const MVPbkFieldType& aFieldtype )
    {
    iFieldType = &aFieldtype;
    }

MVPbkBaseContact& TTestContactField::ParentContact() const
    {
    MVPbkBaseContact* contact = NULL;
    return *contact;
    }

const MVPbkFieldType* TTestContactField::MatchFieldType(TInt /*aMatchPriority*/) const
    {
    return iFieldType;
    }
    
const MVPbkFieldType* TTestContactField::BestMatchingFieldType() const
	{
    return iFieldType;
	}

const MVPbkContactFieldData& TTestContactField::FieldData() const
    {
    const MVPbkContactFieldData* data = NULL;
    return *data;
    }

TBool TTestContactField::IsSame(const MVPbkBaseContactField& /*aOther*/) const
    {
    return EFalse;
    }

MVPbkObjectHierarchy& TTestContactField::ParentObject() const
    {
    MVPbkObjectHierarchy* hier = NULL;
    return *hier;
    }

// End of file
