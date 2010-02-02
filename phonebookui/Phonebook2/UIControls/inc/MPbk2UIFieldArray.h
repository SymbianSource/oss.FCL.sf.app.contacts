/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor dialog UI field array.
*
*/

#ifndef MPBK2UIFIELDARRAY_H_
#define MPBK2UIFIELDARRAY_H_


// INCLUDE FILES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbk2UIField;
class MPbk2UIFieldFactory;

/**
 * Phonebook 2 UI field array interface.
 *
 * @see MPbk2UIField
 */
class MPbk2UIFieldArray
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2UIFieldArray()
                {}

        /**
         * Returns the number of fields in the array.
         *
         * @return Number of properties in the array.
         */
        virtual TInt Count() const = 0;

        /**
         * Returns the field at given index.
         *
         * @param aIndex        The index of the property.
         * @return  Property at given index.
         */
        virtual MPbk2UIField& At(
                TInt aIndex ) const = 0;

        /**
		 * Returns the field at given index.
		 *
		 * @param aIndex        The index of the property.
		 * @return  Property at given index.
		 */
		virtual const MPbk2UIFieldFactory& GetUIFieldFactory() const = 0;
		
		/**
		 * Appends fields array.
		 *
		 * @param aUIFieldArray        Array of fields.
		 */
		virtual void AppendL(MPbk2UIFieldArray& aUIFieldArray) = 0;
		
		/**
		 * Removes the field at given index.
		 *
		 */
		virtual void RemoveAt( TInt aIndex ) = 0;
              
        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* FieldArrayExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif /*MPBK2UIFIELDARRAY_H_*/

// End of File
