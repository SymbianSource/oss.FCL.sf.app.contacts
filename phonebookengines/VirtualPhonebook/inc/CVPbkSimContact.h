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
* Description:  An editable sim contact
*
*/



#ifndef CVPBKSIMCONTACT_H
#define CVPBKSIMCONTACT_H

//  INCLUDES
#include "CVPbkSimContactBase.h"

// FORWARD DECLARATIONS
class MVPbkSimCntStore;
class MVPbkSimContactObserver;
class MVPbkSimStoreOperation;

// CLASS DECLARATION

/**
*  The sim contact
*
*  @lib VPbkSimStoreImpl.lib
*/
class CVPbkSimContact : public CVPbkSimContactBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aETelContact an ETel contact
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        * @exception KErrBadDescriptor if contact couldn't be initialized
        */
        IMPORT_C static CVPbkSimContact* NewL( const TDesC8& aETelContact,
            MVPbkSimCntStore& aSimStore );

        /**
        * Two-phased constructor. Creates a new empty sim contact.
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimContact* NewL( MVPbkSimCntStore& aSimStore );
        
        /**
        * Two-phased constructor.
        * @param aETelContact an ETel contact
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        * @exception KErrBadDescriptor if contact couldn't be initialized
        */
        IMPORT_C static CVPbkSimContact* NewLC( const TDesC8& aETelContact,
            MVPbkSimCntStore& aSimStore );

        /**
        * Two-phased constructor. Creates a new empty sim contact.
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimContact* NewLC( MVPbkSimCntStore& aSimStore );

        /**
        * Destructor.
        */
        virtual ~CVPbkSimContact();

    public: // New functions
        
        /**
        * Creates a new field.
        * @param aType the type of the field
        * @return a new field instance
        */
        IMPORT_C CVPbkSimCntField* CreateFieldLC( 
            TVPbkSimCntFieldType aType ) const;
        
        /**
        * Deletes a field in the specified position
        * @param aIndex the index of the field to be deleted
        */
        IMPORT_C void DeleteField( TInt aIndex );

        /**
        * Adds a new field to the contact.
        * @param aField the new field. Ownership is taken. NULL field
        *   	is ignored.
        */
        IMPORT_C void AddFieldL( CVPbkSimCntField* aField );

        /**
        * Deletes all fields from the contact
        */
        IMPORT_C void DeleteAllFields();

        /**
        * Saves this contact
        * @param aObserver the observer to notify after async save
        * @precond FieldCount() > 0
        *       VPbkSimStoreImpl::Panic(VPbkSimStoreImpl::EZeroFieldCount)
        *       is raised if the precondition does not hold.
        * @return an asynchrnous operation handle that can be used to cancel request.
        */
        IMPORT_C MVPbkSimStoreOperation* SaveL( 
            MVPbkSimContactObserver& aObserver );

        /**
        * Returns a field in given index.
        * @param aIndex the index of the field
        * @return a field in given index.
        */
        inline CVPbkSimCntField& FieldAt( TInt aIndex );

        /**
        * Returns the field array
        * @return the field array
        */
        inline RPointerArray<CVPbkSimCntField>& FieldArray();

        /**
        * Set the sim index of the contact
        * @param aSimIndex the sim index of the contact
        */
        void SetSimIndex( TInt aSimIndex );

    public: // Functions from base classes

        /**
        * From MVPbkSimContact
        */
        const CVPbkSimCntField& ConstFieldAt( TInt aIndex ) const;

        /**
        * From MVPbkSimContact
        */
        TInt FieldCount() const;

        /**
        * From MVPbkSimContact
        */
        TInt SimIndex() const;

        /**
        * From MVPbkSimContact
        */
        const TDesC8& ETelContactL() const;

        /**
        * From MVPbkSimContact
        */
        void SetL( const TDesC8& aETelContact );

    private:

        /**
        * C++ constructor.
        */
        CVPbkSimContact( MVPbkSimCntStore& aSimStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8* aETelContact );

    private:    // New functions
        /// Creates ETel contact from native fields
        void CreateETelContactL() const;
        
		/*
		* Used to indentify the different additional number field types 
		* according the order in the field array.
		*/
        void IndentifyAdditionalFields();

    private:    // Data
        /// The array for the fields
        RPointerArray<CVPbkSimCntField> iFieldArray;
        /// Own: a buffer for ETel format, mutable because ETelContactL()
        /// is const. Calling ETelContactL() doesn't changes the contact data
        /// but makes a conversion to ETel format.
        mutable HBufC8* iData;
        /// The sim index of the contact
        TInt iSimIndex;
    };


// INLINE FUNCTIONS
inline CVPbkSimCntField& CVPbkSimContact::FieldAt( TInt aIndex )
    {
    return *iFieldArray[aIndex];
    }

inline RPointerArray<CVPbkSimCntField>& CVPbkSimContact::FieldArray()
    {
    return iFieldArray;
    }

#endif      // CVPBKSIMCONTACT_H
            
// End of File
