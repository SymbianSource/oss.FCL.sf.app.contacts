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
* Description:  Phonebook 2 navigator factory.
*
*/


#ifndef PBK2NAVIGATORFACTORY_H
#define PBK2NAVIGATORFACTORY_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbk2ContactNavigation;
class MPbk2NavigationObserver;
class MPbk2ContactNavigationLoader;
class MVPbkContactStore;

// CLASS DECLARATION

/**
 * Phonebook 2 navigator factory.
 * Responsible for creating suitable navigator object.
 * There are several navigation strategies available.
 */
class Pbk2NavigatorFactory
    {
    public: // Interface

        /**
         * Factory function for creating contact navigation navigator.
         *
         * @param aViewId                       Id of the view to navigate.
         * @param aNavigationObserver           Navigation observer.
         * @param aNavigationLoader             Navigation loader.
         * @param aStoreList                    List of stores this navigator
         *                                      observes.
         * @param aNavigatorFormatResourceId    Id of the textual format
         *                                      resource string used in
         *                                      the navigation indicator.
         * @return  Contact navigator.
         */
        IMPORT_C static MPbk2ContactNavigation* CreateContactNavigatorL(
                TUid aViewId,
                MPbk2NavigationObserver& aNavigationObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList,
                const TInt aNavigatorFormatResourceId );

        /**
         * Factory function for creating no navigation navigator.
         *
         * @param aViewId                       Id of the view to navigate.
         * @param aNavigationObserver           Navigation observer.
         * @param aNavigationLoader             Navigation loader.
         * @param aStoreList                    List of stores this navigator
         *                                      observes.
         * @return  Contact navigator.
         */
        IMPORT_C static MPbk2ContactNavigation* CreateVoidNavigatorL(
                TUid aViewId,
                MPbk2NavigationObserver& aNavigationObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList );
    };

#endif // PBK2NAVIGATORFACTORY_H

// End of File
