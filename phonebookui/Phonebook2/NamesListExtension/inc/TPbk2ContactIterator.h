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
* Description: 
*
*/


#ifndef TPBK2CONTACTITERATOR_H
#define TPBK2CONTACTITERATOR_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactLink.h>
#include <MPbk2ContactLinkIterator.h>
#include <MVPbkContactLinkArray.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
 * Counts the number of SIM contacts.
 */
class TPbk2ContactIterator : public MPbk2ContactLinkIterator
    {
    public: // New methods
    
        /**
         * Sets the link array that is to be iterated.
         * @param aArray The array that is to be iterated.
         */
        void SetLinkArray( MVPbkContactLinkArray& aArray );

    public: // From MPbk2ContactLinkIterator

        TBool HasNext() const;

        TBool HasPrevious() const;

        MVPbkContactLink* NextL();

        MVPbkContactLink* PreviousL();

        void SetToFirst();

        void SetToLast();

    private: // Data
        MVPbkContactLinkArray* iLinkArray; // not owned
        TInt iIndexOfNext;
    };

#endif // TPBK2CONTACTITERATOR_H

// End of File
