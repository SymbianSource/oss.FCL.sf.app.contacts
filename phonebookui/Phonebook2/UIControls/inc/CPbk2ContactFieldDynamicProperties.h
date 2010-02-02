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
* Description:  Manages dynamic properties of the contact
*
*/


#ifndef CPBK2CONTACTFIELDDYNAMICPROPERTIES_H
#define CPBK2CONTACTFIELDDYNAMICPROPERTIES_H

#include <e32base.h>
#include "MPbk2ContactFieldDynamicProperties.h"
    
/**
 * Contact field dynamic properties.
 */ 
NONSHARABLE_CLASS( CPbk2ContactFieldDynamicProperties ) :   
        public CBase, 
        public MPbk2ContactFieldDynamicProperties
    {
    public:
        /**
         * Constructor.
         */ 
        static CPbk2ContactFieldDynamicProperties* NewLC();

        /**
         * Destructor.
         */ 
        ~CPbk2ContactFieldDynamicProperties();
        
    public: // Interface
        /**
         * Adds new field resource id to array.
         * @return KErrNone if field appended successfully otherwise
         *         one of the system wide error codes.
         * @see RArray::Append
         */
        TInt AddHiddenFieldResourceId( TInt aFieldResId );        
    
    public: // From MPbk2ContactFieldDynamicProperties
        TBool IsHiddenField( const MVPbkBaseContactField& aField ) const;
    
    private: // Implementation
        CPbk2ContactFieldDynamicProperties();
        void ConstructL();
            
    private: // data
        /// Own: array of hidden field type resource ids
        RArray<TInt>    iHiddenFieldResourceIds;
    };
    
#endif // CPBK2CONTACTFIELDDYNAMICPROPERTIES_H

// End of file
