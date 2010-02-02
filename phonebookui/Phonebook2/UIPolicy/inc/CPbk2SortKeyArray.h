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
* Description:  Sort key array
*
*/


#ifndef CPBK2SORTKEYARRAY_H
#define CPBK2SORTKEYARRAY_H

// INCLUDE FILES
#include <SortUtil.h>

// CLASS DECLARATION

/**
 * Sort key array.
 * Responsible for implementing MSortKeyArray for
 * contact name sorting purposes.
 * Used by contact view sort policy plug-in.
 */
class CPbk2SortKeyArray : public CBase,
                          public MSortKeyArray
    {
    public: // Constructor and destructor

        /**
         * Two-phased constructor.
         *
         * @return  A new instance of this class.
         */
        static CPbk2SortKeyArray* NewL();

        /**
         * Destructor.
         */
        ~CPbk2SortKeyArray();

    public: // Interface

        /**
         * Appends new sortkey to the array.
         *
         * @param aKey  New sortkey to append.
         */
        void AppendL(
                const TSortKey& aKey );

        /**
         * Resets the sortkey array.
         */
        void Reset();

        /**
         * Sets the text of the key in specified index.
         *
         * @param aText     The text to set to key in index.
         * @param aIndex    The index where the key to be modified is.
         */
        void SetText(
                const TDesC& aText,
                TInt aIndex );

    public: // From MSortKeyArray
        TInt SortKeyCount() const;
        TSortKey SortKeyAt(
                TInt aIndex ) const;

    private: // Implementation
        CPbk2SortKeyArray();

    private: // Data
        /// Own: Array of key types
        RArray<TSortKeyType> iKeyTypes;
        /// Own: Array of texts that correspond to types in iKeyTypes array
        RArray<TPtrC> iTexts;
    };

#endif // CPBK2SORTKEYARRAY_H

// End of File
