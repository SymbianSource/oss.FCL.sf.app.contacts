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
* Description:  Implements a sim store contact field
*
*/


#ifndef VPBKSIMSTORE_TSTORECONTACTFIELD_H
#define VPBKSIMSTORE_TSTORECONTACTFIELD_H


// INCLUDES
#include <MVPbkStoreContactField.h>
#include "TContactFieldData.h"

// FORWARD DECLARATIONS
class CVPbkSimCntField;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContact;

// CLASS DECLARATIONS

/**
 * Maps a Contact Model field to a Virtual Phonebook field.
 */
NONSHARABLE_CLASS( TStoreContactField ): 
        public MVPbkStoreContactField
    {
    public:  // Constructors
        /**
         * Constructor. Sets the parent contact and resets other data to null 
         * values.
         */
        TStoreContactField();

    public:  // New functions
        
        /**
         * Sets a new parent contact for this field. Resets the Contact Model 
         * field mapping to null.
         */
        void SetParentContact( CContact& aParentContact );

        /**
        * Sets the native sim field for this vpbk field
        */
        void SetSimField( CVPbkSimCntField& aSimField );
        
    public:  // Functions from base classes

        /**
        * From MVPbkObjectHierarchy
        */
        MVPbkObjectHierarchy& ParentObject() const;

        /**
        * From MVPbkBaseContactField
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        * From MVPbkBaseContactField
        */
        const MVPbkFieldType* MatchFieldType( TInt aMatchPriority ) const;
        
        /**
        * From MVPbkBaseContactField
        */
        const MVPbkFieldType* BestMatchingFieldType() const;

        /**
        * From MVPbkBaseContactField
        */
        const MVPbkContactFieldData& FieldData() const;

        /**
        * From MVPbkBaseContactField
        */
        TBool IsSame( const MVPbkBaseContactField& aOther ) const;
        
        /**
        * From MVPbkBaseContactField
        */
        TBool SupportsLabel() const;
        
        /**
        * From MVPbkStoreContactField
        */
        TPtrC FieldLabel() const;

        /**
        * From MVPbkStoreContactField
        */
        void SetFieldLabelL( const TDesC& aText );
        
        /**
        * From MaxLabelLength
        */
        TInt MaxLabelLength() const;
        
        /**
        * From MVPbkStoreContactField
        */
        MVPbkContactFieldData& FieldData();

        /**
        * From MVPbkStoreContactField
        */
        MVPbkStoreContactField* CloneLC() const;
        
        /**
        * From MVPbkStoreContactField
        */
        MVPbkContactLink* CreateLinkLC() const;
        
    protected:  // New functions
        
        /**
        * Returns the native sim field
        */
        inline CVPbkSimCntField* SimField();

    private:  // Data
        /// Ref: the contact whose field this is
        CContact* iParentContact;
        /// Ref: the native sim field.
        CVPbkSimCntField* iSimField;
        /// Own: The data container
        TContactFieldData iFieldData;
    };

/**
* A wrapper class that destroys the native field in destructor
* This class is used when client creates a new field.
*/
NONSHARABLE_CLASS( TContactNewField ): public TStoreContactField
    {
    public: // Construction and destruction

        /**
        * C++ constructor
        * @param aNewField a new sim field. Ownership is taken
        */
        TContactNewField( CVPbkSimCntField* aSimField );
            
        // Destructor
        ~TContactNewField();

    public: // New functions

        /**
        * Returns the sim field and gives up the ownership
        */
        CVPbkSimCntField* SimField();
    private: // Data
        TBool iOwnsField;
    };

// INLINE FUNCTIONS
inline CVPbkSimCntField* TStoreContactField::SimField()
    {
    return iSimField;
    }
}  // namespace VPbkSimStore

#endif  // VPBKSIMSTORE_TSTORECONTACTFIELD_H
//End of file


