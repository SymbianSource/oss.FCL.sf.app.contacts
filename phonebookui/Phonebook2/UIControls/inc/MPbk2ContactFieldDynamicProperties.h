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
* Description:  Interface for dynamic properties of the contact class
*
*/

#ifndef MPBK2CONTACTFIELDDYNAMICPROPERTIES_H
#define MPBK2CONTACTFIELDDYNAMICPROPERTIES_H

class MVPbkBaseContactField;

/**
 * Interface for dynamic properties of the contact class
 */
class MPbk2ContactFieldDynamicProperties
    {
    public:
        /**
         * Is field hidden.
         *
         * @param aField, Contact field 
         * @return ETrue if hidden else EFalse
         */
        virtual TBool IsHiddenField( 
            const MVPbkBaseContactField& aField ) const =0;
        
    protected:
        virtual ~MPbk2ContactFieldDynamicProperties() {}
    };
    
#endif // MPBK2CONTACTFIELDDYNAMICPROPERTIES_H

// End of file

