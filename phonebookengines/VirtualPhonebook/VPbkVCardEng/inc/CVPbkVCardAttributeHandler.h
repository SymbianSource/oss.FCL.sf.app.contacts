/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook VCard attribute handler
*
*/



#ifndef CVPBKVCARDATTRIBUTEHANDLER_H
#define CVPBKVCARDATTRIBUTEHANDLER_H

#include <e32base.h>
#include <VPbkFieldType.hrh>

class MVPbkContactAttribute;

NONSHARABLE_CLASS( CVPbkVCardAttributeHandler ) : public CBase
    {
    public:
        static CVPbkVCardAttributeHandler* NewL();
        
        ~CVPbkVCardAttributeHandler();
        
    public: // interface
        /**
         * Decides whether to create an attribute from aParameter or not.
         * @param aParameter Field type parameter
         * @return ETrue if creates the attribute, otherwise EFalse.
         */
        TBool CreateAttributeL( TVPbkFieldTypeParameter aParameter );
        
        /**
         * Returns the created attribute or NULL.
         * The ownership does not transfer.
         * @return Created attribute or NULL
         */
        MVPbkContactAttribute* Attribute() const;
        
    private: // implementation
        CVPbkVCardAttributeHandler();
        
    private: // data
    
        /// Own: attribute
        MVPbkContactAttribute* iAttribute;
    };
    
#endif // CVPBKVCARDATTRIBUTEHANDLER_H
//End of file
