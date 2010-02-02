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
* Description:  The sim contact field.
*
*/



#ifndef CVPBKSIMCNTFIELD_H
#define CVPBKSIMCNTFIELD_H

//  INCLUDES
#include <e32base.h>
#include "VPbkSimCntFieldTypes.hrh"

// FORWARD DECLARATIONS
class MVPbkSimContact;

// CLASS DECLARATION

/**
*  The sim contact field.
*
*  @lib VPbkSimStoreImpl
*/
class CVPbkSimCntField : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aType the type of field.
        * @param aParentContact the contact that owns this field
        * @return a new instance of this class
        */
        static CVPbkSimCntField* NewL( TVPbkSimCntFieldType aType,
            const MVPbkSimContact& aParentContact );
        
        /**
        * Two-phased constructor.
        * @param aType the type of field.
        * @param aParentContact the contact that owns this field
        * @return a new instance of this class
        */
        static CVPbkSimCntField* NewLC( TVPbkSimCntFieldType aType,
            const MVPbkSimContact& aParentContact );

        /**
        * Destructor.
        */
        virtual ~CVPbkSimCntField();

    public: // New functions
        
        /**
        * Sets data to this field. The field owns the data
        * and doesn't use the data pointer to contact buffer after this
        * @param aData the data to be set
        */
        IMPORT_C void SetDataL( const TDesC& aData );
        
        /**
        * Returns the maximum length of the field data
        * @return the maximum length of the field data
        */
        IMPORT_C TInt MaxDataLength();
        
        /**
        * Sets the field data that already exists in the sim card
        * This is used the set the maximum length of the field correctly
        * @param aData the data to be set
        */
        void SetInitialSimDataL( const TDesC& aData );
        
        /**
        * Sets the fields data pointer to contact buffer 
        * and deletes the field's own data buffer if exists
        * @param aDataPtr a pointer to the field's data in the contact buffer
        */
        void SetDataPtr( TPtrC aDataPtr );

        /**
        * Sets the type of the field
        * @param aType the new type of the field
        */
        void SetType( TVPbkSimCntFieldType aType );
        
        /**
        * Returns the data from the field
        * @return the field data.
        */
        inline const TDesC& Data() const;

        /**
        * Returns the type of the field
        * @return the type of the field
        */
        inline TVPbkSimCntFieldType Type() const;
    
    private:

        /**
        * C++ constructor.
        */
        CVPbkSimCntField( TVPbkSimCntFieldType aType,
            const MVPbkSimContact& aParentContact );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// A data pointer to the contacts data buffer
        TPtrC iDataPtr;
        /// The type of the field
        TVPbkSimCntFieldType iType;
        /// The contact that owns this field
        const MVPbkSimContact& iParentContact;
        /// Own: field data
        HBufC* iData;
        /// The maximum length of the field data
        TInt16 iMaxLength;
    };

// INLINE FUNCTIONS

inline const TDesC& CVPbkSimCntField::Data() const
    {
    return iDataPtr;
    }

inline TVPbkSimCntFieldType CVPbkSimCntField::Type() const
    {
    return iType;
    }

#endif      // CVPBKSIMCNTFIELD_H
            
// End of File
