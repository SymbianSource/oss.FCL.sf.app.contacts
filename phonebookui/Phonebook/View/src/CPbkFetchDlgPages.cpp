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
*       Provides methods for MPbkFetchDlgPages implementation.
*
*/


// INCLUDE FILES
#include "CPbkFetchDlgPages.h"


// ================= MEMBER FUNCTIONS =======================

CPbkFetchDlgPages::~CPbkFetchDlgPages()
    {
    for (TInt i=iPages.Count()-1; i>=0; --i)
        {
        delete iPages[i];
        }
    iPages.Close();
    }

TInt CPbkFetchDlgPages::DlgPageCount() const
    {
    return iPages.Count();
    }

MPbkFetchDlgPage& CPbkFetchDlgPages::DlgPageAt(TInt aIndex) const
    {
    return *const_cast<MPbkFetchDlgPage*>(iPages[aIndex]);
    }

MPbkFetchDlgPage* CPbkFetchDlgPages::DlgPageWithId(TInt aPageId) const
    {
    const TInt count = iPages.Count();
    for (TInt i=0; i < count; ++i)
        {
        const MPbkFetchDlgPage* page = iPages[i];
        if (page->FetchDlgPageId() == aPageId)
            {
            return const_cast<MPbkFetchDlgPage*>(page);
            }
        }
    return NULL;
    }


// End of File
