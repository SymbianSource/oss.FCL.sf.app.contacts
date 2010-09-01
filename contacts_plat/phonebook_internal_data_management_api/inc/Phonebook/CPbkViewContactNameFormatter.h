/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     CViewContact contact name formatter.
*
*/


#ifndef __CPbkViewContactNameFormatter_H__
#define __CPbkViewContactNameFormatter_H__

// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MPbkContactNameFormat;
class CViewContact;
class RContactViewSortOrder;


// CLASS DECLARATIONS

/**
 * Formats a contact name from a CViewContact object contents.
 */
class CPbkViewContactNameFormatter : public CBase
    {
    public:  // Constructors and desturctor
        /**
         * Creates a new instance of this class.
		 * @param aNameFormatter contact name formatter instance
         */
        IMPORT_C static CPbkViewContactNameFormatter* NewL
            (MPbkContactNameFormat& aNameFormatter);

        /**
         * C++ constructor. Usage restricted to the DLL that implements this 
         * class for binary compability reasons.
		 * @param aNameFormatter contact name formatter instance
         */
        CPbkViewContactNameFormatter(MPbkContactNameFormat& aNameFormatter);

        /**
         * Destructor.
         */
        ~CPbkViewContactNameFormatter();

    public:  // Interface
        /**
         * Returns a contact name from a CViewContact object contents.
         *
         * @param aViewContact  the CViewContact object to format a name from.
         * @param aSortOrder    sort order of the CContactViewBase where 
         *                      aViewContact is from.
         * @precond aViewContact.FieldCount()==aSortOrder.Count()
         * @return  formatted contact title. The returned reference is 
         *          invalidated when this function is called the next time.
         */
        IMPORT_C const TDesC& ContactTitleL
            (const CViewContact& aViewContact, 
            const RContactViewSortOrder& aSortOrder) const;

    private:  // Implementation
        TPtr ReserveBufferL(TInt aLength) const;

    private:  // Data
        /// Ref: Phonebook name formatter implementation
        MPbkContactNameFormat& iNameFormatter;
        /// Own: buffer for the formatted name
        mutable HBufC* iNameBuf;
    };


#endif // __CPbkViewContactNameFormatter_H__

// End of File
