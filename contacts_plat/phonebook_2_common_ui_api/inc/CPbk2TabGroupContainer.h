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
* Description:  Phonebook 2 tab group container.
*
*/


#ifndef CPBK2TABGROUPCONTAINER_H
#define CPBK2TABGROUPCONTAINER_H

// INCLUDES
#include <e32base.h>
#include <AknTabObserver.h>
#include <w32std.h>
#include <pbk2viewinfo.hrh>
#include <AknNaviDecoratorObserver.h>

// FORWARD DECLARATIONS
class MPbk2ViewExplorer;
class CAknNavigationDecorator;
class CAknTabGroup;
class CPbk2IconInfoContainer;
class CPbk2ViewNode;
class MPbk2TabGroupContainerCallback;

// CLASS DECLARATION

/**
 * Phonebook 2 tab group container.
 * Responsible for creating Phonebook 2 application tab groups
 * and managing the tab groups. The tab groups are created
 * from the view graph data. Clients may also add new tab groups
 * to the container.
 */
class CPbk2TabGroupContainer : public CBase,
                               private MAknTabObserver,
                               private MAknNaviDecoratorObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aViewExplorer     Phonebook 2 view explorer.
         * @param aCallback         Tab group callback.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2TabGroupContainer* NewL(
                MPbk2ViewExplorer& aViewExplorer,
                MPbk2TabGroupContainerCallback& aCallback );

        /**
         * Destructor.
         */
        ~CPbk2TabGroupContainer();

    public: // Interface
        /**
         * Returns the tab group associated with given id.
         *
         * @param aId   Id of the tab group.
         * @return  Tab group with given id or
         *          NULL if no tab group was found.
         */
        IMPORT_C CAknNavigationDecorator* TabGroupFromViewId(
                TInt aId ) const;

        /**
         * Handles navigation key events.
         *
         * @param aKeyEvent     Key event to handle.
         * @param aType         Type of the key event.
         * @return  ETrue if event was navigation event and it was consumed,
         *          EFalse otherwise.
         */
        IMPORT_C TBool HandleNavigationKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

        /**
         * Returns the tab icons.
         *
         * @return  Tab icons.
         */
        IMPORT_C CPbk2IconInfoContainer& TabIcons() const;

    private: // From MAknTabObserver
        void TabChangedL(
                TInt aIndex );

    private: // From MAknNaviDecoratorObserver
        void HandleNaviDecoratorEventL(
                TInt aEventID );

    private: // Implementation
        CPbk2TabGroupContainer(
                MPbk2ViewExplorer& aViewExplorer,
                MPbk2TabGroupContainerCallback& aCallback );
        void ConstructL();
        TInt DoCalculateTabGroupWidth(
                TInt aViewCount ) const;
        void DoReadViewNodeTabResourcesL(
                const CPbk2ViewNode& aNode,
                CAknTabGroup& aAknTabGroup,
                TInt aViewCount );
        void CreateTabGroupsL();
        CAknTabGroup& FindTabGroupWithIdL(
                TPbk2TabGroupId aTabGroupId );
        void SkinTabGroupsL();

    private: // Data
        /// Ref: Phonebook 2 view explorer
        MPbk2ViewExplorer& iViewExplorer;
        /// Own: View navigation tab groups
        class CPbk2TabGroupEntry;
        RPointerArray<CPbk2TabGroupEntry> iViewTabGroups;
        /// Own: Tab icons
        CPbk2IconInfoContainer* iTabIcons;
        ///Ref: Tab group callback
        MPbk2TabGroupContainerCallback& iCallback;
    };

#endif // CPBK2TABGROUPCONTAINER_H

// End of File
