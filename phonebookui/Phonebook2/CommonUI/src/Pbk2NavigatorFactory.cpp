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

// INCLUDE FILES
#include <Pbk2NavigatorFactory.h>

// Phonebook 2
#include "CPbk2ContactNavigation.h"
#include "CPbk2VoidNavigation.h"
#include <MPbk2ContactNavigationLoader.h>
#include <Pbk2ViewId.hrh>
#include <MPbk2ViewExplorer.h>

// --------------------------------------------------------------------------
// Pbk2NavigatorFactory::CreateContactNavigatorL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2ContactNavigation*
    Pbk2NavigatorFactory::CreateContactNavigatorL
        ( TUid aViewId, MPbk2NavigationObserver& aNavigationObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList,
          const TInt aFormatResourceId )
    {
    return CPbk2ContactNavigation::NewL
        ( aViewId, aNavigationObserver, aNavigationLoader, aStoreList,
          aFormatResourceId );
    }

// --------------------------------------------------------------------------
// Pbk2NavigatorFactory::CreateVoidNavigatorL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2ContactNavigation*
    Pbk2NavigatorFactory::CreateVoidNavigatorL
        ( TUid aViewId, MPbk2NavigationObserver& aNavigationObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList )
    {
    return CPbk2VoidNavigation::NewL
        ( aViewId, aNavigationObserver, aNavigationLoader, aStoreList );
    }

// End of File
