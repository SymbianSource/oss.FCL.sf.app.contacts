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
* Description:  The virtual phonebook store field data implementation
*
*/


#ifndef VPBKSIMSTORE_TCONTACTFIELDDATA_H
#define VPBKSIMSTORE_TCONTACTFIELDDATA_H


// INCLUDES
#include <MVPbkContactFieldTextData.h>


// FORWARD DECLARATIONS
/// Reserved name for a future extension to TContactFieldData
class TContactFieldDataExt;
class CVPbkSimCntField;

// CLASS DECLARATIONS

namespace VPbkSimStore {

/**
 * SIM Store field. All fields are of text type.
 */
NONSHARABLE_CLASS( TContactFieldData ): 
        public MVPbkContactFieldTextData
    {
    public:  // Constructor and destructor
        TContactFieldData();
        ~TContactFieldData();

    public:  // New functions
        
        /**
        * Set SIM field that contains the data
        */
        void SetField( CVPbkSimCntField& aSimField );

    public:  // Functions from base classes

        /**
        * From MVPbkContactFieldData
        */
        TBool IsEmpty() const;

        /**
        * From MVPbkContactFieldData
        */
        void CopyL( const MVPbkContactFieldData& aFieldData );

        /**
        * From MVPbkContactFieldTextData
        */
        TPtrC Text() const;

        /**
        * From MVPbkContactFieldTextData
        */
        void SetTextL(const TDesC& aText);

        /**
        * From MVPbkContactFieldTextData
        */
        TInt MaxLength() const;
        
    private:  // Data
        /// Ref. the sim field containing the data
        CVPbkSimCntField* iSimField;
    };

}  // namespace VPbkSimStore

#endif  // VPBKSIMSTORE_TCONTACTFIELDDATA_H

