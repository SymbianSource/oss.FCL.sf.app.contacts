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
* Description:  Phonebook 2 contact navigator.
*
*/


#ifndef CPBK2CONTACTNAVIGATION_H
#define CPBK2CONTACTNAVIGATION_H

//  INCLUDES
#include <e32base.h>
#include "CPbk2NavigationBase.h"
#include <MPbk2ContactNavigationLoader.h>
#include <AknNaviDecoratorObserver.h>

//  FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactViewBase;
class MPbk2NavigationObserver;

//  CLASS DECLARATION

/**
 * Phonebook 2 contact navigator.
 * Responsible for implementing 'sideways navigation' navigation strategy.
 * Listens to users navigation events, which are then processed by it.
 * The next action is determined by the set scroll view. A navigation
 * event initates the loading of the next viewed contact.
 */
class CPbk2ContactNavigation : public CPbk2NavigationBase,
                               private MAknNaviDecoratorObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aViewId                       View id.
         * @param aObserver                     Navigation observer.
         * @param aNavigationLoader             Navigation interface for
         *                                      loading contacts.
         * @param aStoreList                    List of stores this
         *                                      navigator observes.
         * @param aNavigatorFormatResourceId    Id of the textual format
         *                                      resource string used in
         *                                      the indicator.
         * @return  A new instance of this class.
         */
        static CPbk2ContactNavigation* NewL(
                TUid aViewId,
                MPbk2NavigationObserver& aObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList,
                const TInt aNavigatorFormatResourceId );

        /**
         * Destructor.
         */
        ~CPbk2ContactNavigation();

    public: // From MPbk2ContactNavigation
        void UpdateNaviIndicatorsL(
                const MVPbkContactLink& aContactLink ) const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // From MVPbkContactViewObserver
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        
    private: // From MAknNaviDecoratorObserver
        void HandleNaviDecoratorEventL(
                TInt aEventID );

    private: // Implementation
        CPbk2ContactNavigation(
                TUid aViewId,
                MPbk2NavigationObserver& aObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList,
                const TInt aNavigatorFormatResourceId );
        void ConstructL();
        void NavigateContactL(
                TInt aDirection );
        void UpdateAfterViewEventL() const;
        void SetTabTextL(
                const TUid aViewId,
                const TDesC& aText ) const;

    private: // Data
        /// Own: View id
        TUid iViewId;
        /// Own: Format for the navigation text
        const TInt iNavigatorFormatResourceId;
    };

#endif // CPBK2CONTACTNAVIGATION_H

// End of File
