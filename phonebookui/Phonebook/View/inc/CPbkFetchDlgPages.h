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
*     MPbkFetchDlgPages implementation.
*
*/


#ifndef __CPbkFetchDlgPages_H__
#define __CPbkFetchDlgPages_H__

//  INCLUDES
#include <e32base.h>            // CBase
#include "MPbkFetchDlgPage.h"   // MPbkFetchDlgPages

// CLASS DECLARATION

/**
 * MPbkFetchDlgPages implementation.
 */
NONSHARABLE_CLASS(CPbkFetchDlgPages) : 
        public CBase, 
        public MPbkFetchDlgPages
    {
    public:  // Interface
        /**
         * Constructor.
         */
        CPbkFetchDlgPages();

        /**
         * Destructor.
         */
        ~CPbkFetchDlgPages();

        /**
         * Adds a new page aPage to this collection.
		 * Takes ownership of the page.
         */
        void AddL(MPbkFetchDlgPage* aPage);

    public:  // from MPbkFetchDlgPage
        TInt DlgPageCount() const;
        MPbkFetchDlgPage& DlgPageAt(TInt aIndex) const;
        MPbkFetchDlgPage* DlgPageWithId(TInt aPageId) const;

    private:  // Data
		/// Own: the array of pages
        RPointerArray<MPbkFetchDlgPage> iPages;
    };

inline CPbkFetchDlgPages::CPbkFetchDlgPages()
    {
    }

inline void CPbkFetchDlgPages::AddL(MPbkFetchDlgPage* aPage)
    {
    User::LeaveIfError(iPages.Append(aPage));
    }


#endif // __CPbkFetchDlgPages_H__
            
// End of File
