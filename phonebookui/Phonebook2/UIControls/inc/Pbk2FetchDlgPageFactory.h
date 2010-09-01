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
* Description:  Phonebook 2 fetch dialog page factory.
*
*/


#ifndef PBK2FETCHDLGPAGEFACTORY_H
#define PBK2FETCHDLGPAGEFACTORY_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CCoeControl;
class MPbk2FetchDlgPages;
class MPbk2FetchDlg;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog page factory.
 * Responsible for creating fetch dialog pages.
 */
NONSHARABLE_CLASS( Pbk2FetchDlgPageFactory )
    {
    public: // Interface

        /**
         * Creates a custom control for given control type.
         *
         * @param aControlType      Control type.
         * @param aParent           Parent control.
         * @param aFetchDlg         Fetch dialog.
         * @return  The created control or NULL.
         */
        static CCoeControl* CreateCustomControlL(
                TInt aControlType,
                const CCoeControl* aParent,
                MPbk2FetchDlg& aFetchDlg);

        /**
         * Creates dialog pages for given dialog.
         *
         * @param aParentDlg        Parent dialog.
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  Dialog pages.
         */
        static MPbk2FetchDlgPages* CreatePagesL(
                MPbk2FetchDlg& aFetchDlg,
                CVPbkContactManager& aContactManager );

    private: // Implementation
        Pbk2FetchDlgPageFactory();
    };

#endif // PBK2FETCHDLGPAGEFACTORY_H

// End of File
