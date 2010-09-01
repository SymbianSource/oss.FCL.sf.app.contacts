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
*     Fetch dialog page interfaces for Phonebook.
*
*/


#ifndef __PbkFetchDlgPageFactory_H__
#define __PbkFetchDlgPageFactory_H__

//  INCLUDES
#include <e32std.h>

//  FORWARD DECLARATIONS
class CCoeControl;
class MPbkFetchDlgPages;
class MPbkFetchDlg;

// CLASS DECLARATION

/**
 * PbkFetchDlgPageFactory factory.
 */
NONSHARABLE_CLASS(PbkFetchDlgPageFactory)
    {
    public:
        /**
         * Creates a custom control of aControlType.
         * @return the created control, NULL if aControlType is not recognized.
         */
        static CCoeControl* CreateCustomControlL(TInt aControlType);

        /**
         * Creates dialog pages for aFetchDlg.
         */
        static MPbkFetchDlgPages* CreatePagesL(MPbkFetchDlg& aFetchDlg);

	private:
        /**
         * Private constructor. Not implemented.
         */
		PbkFetchDlgPageFactory();
    };

#endif // __PbkFetchDlgPageFactory_H__

// End of File
