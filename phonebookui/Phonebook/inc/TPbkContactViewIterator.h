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
*      Safe CContactViewBase iterator.
*
*/


#ifndef __TPbkContactViewIterator_H__
#define __TPbkContactViewIterator_H__

// INCLUDES
#include <cntdef.h>


// FORWARD DECLARATIONS
class CContactViewBase;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Safe CContactViewBase iterator.
 */
class TPbkContactViewIterator 
    {
    public: // Interface
        /**
         * Constructor.
         * @param aContactView  the contact view to iterate.
         */
        inline TPbkContactViewIterator(const CContactViewBase& aContactView);

        /**
         * Returns the current index of iteration.
		 * @return current iteration index
         */
        inline TInt Index() const;

        /**
         * Forwards this iterator to the next contact in contact view.
         * @return  next contact item's id in contact view. KNullContactId if
         *          at the end of iteration.
         */
        IMPORT_C TContactItemId NextL();
                    
    private:  // Data
		/// Ref: contact view
        const CContactViewBase& iContactView;
		/// Own: index number
        TInt iIndex;
    };


// INLINE FUNCTIONS
inline TPbkContactViewIterator::TPbkContactViewIterator
        (const CContactViewBase& aContactView) :
    iContactView(aContactView), iIndex(-1) // initialize to errornous value
    {
    }

inline TInt TPbkContactViewIterator::Index() const
    {
    return iIndex;
    }


#endif // __TPbkContactViewIterator_H__

// End of File
