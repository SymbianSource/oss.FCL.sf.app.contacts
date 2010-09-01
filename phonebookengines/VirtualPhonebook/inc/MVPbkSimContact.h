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
* Description:  An API for sim contacts
*
*/



#ifndef MVPBKSIMCONTACT_H
#define MVPBKSIMCONTACT_H

//  INCLUDES
#include <e32std.h>
#include "VPbkSimCntFieldTypes.hrh"

// FORWARD DECLARATIONS
class MVPbkSimContactObserver;
class MVPbkSimCntStore;
class CVPbkSimCntField;
class MVPbkSimStoreOperation;

// CLASS DECLARATION

/**
*  An API for sim contacts
*
*/
class MVPbkSimContact
    {
    public: // Types
        class TFieldLookup;

    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MVPbkSimContact() {}

    public: // New functions
        
        /**
        * Returns the parent store of the contact
        * @return the parent store of the contact
        */
        virtual MVPbkSimCntStore& ParentStore() const = 0;
        
        /**
        * Deletes this contact from the sim.
        * @param aObserver The observer that is notified after delete
        * @return An operation handle. 
        *         Cancelling can be done by deleting the operation
        */
        virtual MVPbkSimStoreOperation* DeleteL( 
            MVPbkSimContactObserver& aObserver ) = 0;

        /**
        * Finds the first field of given type
        * @param the field type to search for.
        * @return a lookup with field index. Index is negative if not found.
        */
        virtual TFieldLookup FindField( TVPbkSimCntFieldType aType ) const = 0;

        /**
        * Finds the next field.
        * @param aLookup from the FindField function
        * @return a lookup with field index
        */
        virtual void FindNextField( TFieldLookup& aLookup ) const = 0;

        /**
        * Returns a const field in given index.
        * @param aIndex the index of the field
        * @return a const field in given index.
        */ 
        virtual const CVPbkSimCntField& ConstFieldAt( TInt aIndex ) const = 0;

        /**
        * Returns the amount of fields in the contact
        * @return the field count
        */
        virtual TInt FieldCount() const = 0;

        /**
        * Returns the sim index of the contact
        * @return the sim index
        */
        virtual TInt SimIndex() const = 0;

        /**
        * Returns the contact in ETel format
        * @return the ETel contact
        */
        virtual const TDesC8& ETelContactL() const = 0;

        /**
        * Sets the contact data
        * @param an ETel contact
        */
        virtual void SetL( const TDesC8& aETelContact ) = 0;
    };

/**
* A class for finding fields with certain type
*/
class MVPbkSimContact::TFieldLookup
    {
    public: // Construction
        inline TFieldLookup();

    public: // New functions

        /**
        * Returns the field index of the look up
        * @return the field index of the look up
        */
        inline TInt Index();

        /**
        * @return EFalse if look up is not finished with the field array.
        */
        inline TBool EndOfLookup() const;

    public: // Private API
        inline void SetType( TVPbkSimCntFieldType aType );
        inline TVPbkSimCntFieldType Type();
        inline void SetPos( TInt aPos );
        inline TInt Pos();
        inline void SetIndex( TInt aIndex );
    private:    // Data
        /// the type of the field
        TVPbkSimCntFieldType iType;
        /// The position of the cursor
        TInt iPos;
        /// The field index
        TInt iIndex;
    };

// INLINE FUNCTIONS
inline MVPbkSimContact::TFieldLookup::TFieldLookup()
:   iIndex( KErrNotFound )
    {
    }

inline TInt MVPbkSimContact::TFieldLookup::Index()
    {
    return iIndex;
    }

inline TBool MVPbkSimContact::TFieldLookup::EndOfLookup() const
    {
    return iPos < 0;
    }

inline void MVPbkSimContact::TFieldLookup::SetType( 
    TVPbkSimCntFieldType aType )
    {
    iType = aType;
    }

inline TVPbkSimCntFieldType MVPbkSimContact::TFieldLookup::Type()
    {
    return iType;
    }

inline void MVPbkSimContact::TFieldLookup::SetPos( TInt aPos )
    {
    iPos = aPos;
    }

inline TInt MVPbkSimContact::TFieldLookup::Pos()
    {
    return iPos;
    }

inline void MVPbkSimContact::TFieldLookup::SetIndex( TInt aIndex )
    {
    iIndex = aIndex;
    }

#endif      // MVPBKSIMCONTACT_H
            
// End of File
