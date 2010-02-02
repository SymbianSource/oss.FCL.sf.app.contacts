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

// TTestContactField.h

#ifndef TTestContactField_H
#define TTestContactField_H

// INCLUDES
#include "MVPbkBaseContactField.h"

// FORWARD DECLARATIONS
class MVPbkFieldType;

class TTestContactField : public MVPbkBaseContactField
    {
    public: // Interface
        void SetFieldType(
        		const MVPbkFieldType& aFieldtype );

    public: // From MVPbkBaseContactField
        MVPbkBaseContact& ParentContact() const;
        const MVPbkFieldType* MatchFieldType(
        		TInt aMatchPriority ) const;
        const MVPbkFieldType* BestMatchingFieldType() const;
        const MVPbkContactFieldData& FieldData() const;
        TBool IsSame(
        		const MVPbkBaseContactField& aOther ) const;

    public: // From MVPbkObjectHierarchy
        MVPbkObjectHierarchy& ParentObject() const;

    private: // Data
        const MVPbkFieldType* iFieldType;
    };

#endif // TTestContactField_H

// End of file
