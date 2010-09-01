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
* Description:  Phonebook 2 contact link iterator.
*
*/


#ifndef CPBK2CONTACTVIEWITERATOR_H
#define CPBK2CONTACTVIEWITERATOR_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ContactLinkIterator.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CPbk2ContactViewIterator ) : public CBase,
                                                public MPbk2ContactLinkIterator
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aView             Contact view base.
         * @param aSelectedIndexes  Array of selected indexes.
         * @return  A new instance of this class.
         */
        static CPbk2ContactViewIterator* NewLC(
                MVPbkContactViewBase& aView,
                TArray<TInt> aSelectedIndexes );

        /**
         * Destructor.
         */
        ~CPbk2ContactViewIterator();

    public: // From MPbk2ContactLinkIterator
        TBool HasNext() const;
        TBool HasPrevious() const;
        MVPbkContactLink* NextL();
        MVPbkContactLink* PreviousL();
        void SetToFirst();
        void SetToLast();

    private: // Implementation
        CPbk2ContactViewIterator(
                MVPbkContactViewBase& aView,
                TArray<TInt> aSelectedIndexes );

    private: // Data
        /// Ref: Contact view
        MVPbkContactViewBase& iView;
        /// Ref: Selected indexes
        TArray<TInt> iSelectedIndexes;
        /// Own: Current index
        TInt iCursor;
    };

#endif // CPBK2CONTACTVIEWITERATOR_H

// End of File
