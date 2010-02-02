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
* Description: 
*     Default saving storage settings item list box model for Phonebook 2.
*
*/


#ifndef CPBK2DEFAULTSAVINGSTORAGEQUERYLISTBOXMODEL_H
#define CPBK2DEFAULTSAVINGSTORAGEQUERYLISTBOXMODEL_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>

// CLASS DECLARATION

/**
 * Default saving storage settings item list box model for Phonebook 2.
 */
NONSHARABLE_CLASS(CPbk2DefaultSavingStorageQueryListBoxModel) :
		public CBase,
		public MDesCArray
    {
    public:  // Constructors & destructor	
		/**
         * Creates a new instance of this class.
         *
         * @param aSelectionList    Selection list array.
         * @return A new instance of this class.
         */
        static CPbk2DefaultSavingStorageQueryListBoxModel* NewL( 
                const CArrayPtr<HBufC>& aSelectionList);

		/**
         * Creates a new instance of this class.
         *
         * @param aSelectionList    Selection list array.
         * @return A new instance of this class.
         */
        static CPbk2DefaultSavingStorageQueryListBoxModel* NewLC(
                const CArrayPtr<HBufC>& aSelectionList);                            
		/**
         * Destructor.
         */
        ~CPbk2DefaultSavingStorageQueryListBoxModel();               
		 
    private: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC MdcaPoint(TInt aIndex) const;

    private: // Implementation
        CPbk2DefaultSavingStorageQueryListBoxModel( 
                const CArrayPtr<HBufC>& aSelectionList);
        void ConstructL();
        
    private: // Data
        /// Own: List box model buffer
        HBufC* iBuffer;
        /// Ref: Selection list array
        const CArrayPtr<HBufC>* iSelectionList;          
    };

#endif // CPBK2DEFAULTSAVINGSTORAGEQUERYLISTBOXMODEL_H

// End of File
