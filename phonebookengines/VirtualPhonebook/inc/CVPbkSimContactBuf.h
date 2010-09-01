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
* Description:  A read-only sim contact buffer
*
*/



#ifndef CVPBKSIMCONTACTBUF_H
#define CVPBKSIMCONTACTBUF_H

//  INCLUDES
#include "CVPbkSimContactBase.h"

// CLASS DECLARATION

/**
*  A read-only sim contact buffer
*
*/
class CVPbkSimContactBuf : public CVPbkSimContactBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aETelContact an ETel contact
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        * @exception KErrBadDescriptor if contact couldn't be initialized
        */
        IMPORT_C static CVPbkSimContactBuf* NewL( const TDesC8& aETelContact,
            MVPbkSimCntStore& aSimStore );

        /**
        * Two-phased constructor. Creates a new empty sim contact.
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimContactBuf* NewL( 
            MVPbkSimCntStore& aSimStore );
        
        /**
        * Two-phased constructor.
        * @param aETelContact an ETel contact
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        * @exception KErrBadDescriptor if contact couldn't be initialized
        */
        IMPORT_C static CVPbkSimContactBuf* NewLC( const TDesC8& aETelContact,
            MVPbkSimCntStore& aSimStore );

        /**
        * Two-phased constructor. Creates a new empty sim contact.
        * @param aSimStore the sim store in which this contact belongs
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimContactBuf* NewLC( 
            MVPbkSimCntStore& aSimStore );
        
        /**
        * Destructor.
        */
        virtual ~CVPbkSimContactBuf();

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
        CVPbkSimContactBuf( MVPbkSimCntStore& aSimStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8* aETelContact );

    private:    // Data
        /// An ETel format contact buffer
        HBufC8* iData;
        /// Own: one and only field instance
        mutable CVPbkSimCntField* iCurrentField;
    };

#endif      // CVPBKSIMCONTACTBUF_H
            
// End of File
