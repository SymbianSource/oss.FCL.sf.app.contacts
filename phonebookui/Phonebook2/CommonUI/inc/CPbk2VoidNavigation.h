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
* Description:  Phonebook 2 no navigation navigator.
*
*/


#ifndef CPBK2VOIDNAVIGATION_H
#define CPBK2VOIDNAVIGATION_H

// INCLUDES
#include "CPbk2NavigationBase.h"

// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactViewBase;
class MPbk2NavigationObserver;
class MVPbkContactStore;

// CLASS DECLARATION

/**
 * Phonebook 2 contact no navigation navigator.
 * Responsible for implementing the void navigation strategy.
 */
class CPbk2VoidNavigation : public CPbk2NavigationBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aViewId               View id.
         * @param aObserver             Observer.
         * @param aNavigationLoader     Navigation interface for
         *                              loading contacts.
         * @param aStoreList            List of stores this
         *                              navigator observes.
         * @return  A new instance of this class.
         */
        static CPbk2VoidNavigation* NewL(
                TUid aViewId,
                MPbk2NavigationObserver& aObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList );

        /**
         * Destructor.
         */
        ~CPbk2VoidNavigation();

    public: // From MPbk2ContactNavigation
        void UpdateNaviIndicatorsL(
                const MVPbkContactLink& aContactLink ) const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2VoidNavigation(
                TUid aViewId,
                MPbk2NavigationObserver& aObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList );
        void ConstructL();
        void SetTabTextToEmptyL(
            const TUid aViewId ) const;

    private: // Data
        /// Own: View id
        TUid iViewId;
        /// Own: Scroll view
        MVPbkContactViewBase* iScrollView;
    };

#endif // CPBK2VOIDNAVIGATION_H

// End of File
