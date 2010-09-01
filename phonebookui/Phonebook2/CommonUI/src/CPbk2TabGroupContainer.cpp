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


#include "CPbk2TabGroupContainer.h"

// Phonebook 2
#include <CPbk2ViewGraph.h>
#include <CPbk2IconInfoContainer.h>
#include <CPbk2IconFactory.h>
#include <TPbk2IconId.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2AppViewBase.h>
#include <Pbk2UID.h>
#include <MPbk2TabGroupContainerCallback.h>

// System includes
#include <barsread.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <eikspane.h>
#include <aknnavide.h>

// Debugging headers
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * The maximum amount of visible tabs in the tab group.
 */
const TInt KMaxTabsInTabGroup = 4;

/**
 * Returns the topmost tab group.
 *
 * @return  Topmost tab group.
 */
CAknTabGroup* GetTabGroupOnTop()
    {
    CAknTabGroup* result = NULL;

    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>
            ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL
                ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    if ( naviPane )
        {
        CAknNavigationDecorator* topDecorator = naviPane->Top();
        if ( topDecorator &&  topDecorator->ControlType() ==
                CAknNavigationDecorator::ETabGroup )
            {
            result = static_cast<CAknTabGroup*>(
                topDecorator->DecoratedControl() );
            }
        }
    return result;
    }

} /// namespace


/**
 * Phonebook 2 tab group entry.
 * Responsible for tab groups. Clients may modify the
 * tab group through this class.
 */
class CPbk2TabGroupContainer::CPbk2TabGroupEntry : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aTabGroupId   Tab group id.
         * @param aTabGroup     Tab group.
         * @return  A new instance of this class.
         */
        static CPbk2TabGroupEntry* NewL(
                TPbk2TabGroupId aTabGroupId,
                CAknNavigationDecorator* aTabGroup );

        /**
         * Destructor.
         */
        ~CPbk2TabGroupEntry();

    public: // Interface

        /**
         * Returns the tab group id.
         *
         * @return  Tab group id.
         */
        inline TPbk2TabGroupId TabGroupId() const;

        /**
         * Returns the tab group.
         *
         * @return  Tab group.
         */
        inline CAknTabGroup* TabGroup() const;

        /**
         * Returns the navigation decorator.
         *
         * @return  Navigation decorator.
         */
        inline CAknNavigationDecorator* Decorator() const;

    private: // Implementation
        CPbk2TabGroupEntry(TPbk2TabGroupId aTabGroupId,
                CAknNavigationDecorator* aTabGroup );

    private: // Data
        /// Own: Phonebook 2 tab group id
        TPbk2TabGroupId iTabGroupId;
        /// Own: Avkon tab group
        CAknNavigationDecorator* iTabGroup;
    };


// --------------------------------------------------------------------------
// CPbk2TabGroupEntry::CPbk2TabGroupEntry
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer::CPbk2TabGroupEntry::CPbk2TabGroupEntry
        ( TPbk2TabGroupId aTabGroupId, CAknNavigationDecorator* aTabGroup ) :
            iTabGroupId( aTabGroupId ),
            iTabGroup( aTabGroup )
    {
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::CPbk2TabGroupEntry::~CPbk2TabGroupEntry
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer::CPbk2TabGroupEntry::~CPbk2TabGroupEntry()
    {
    delete iTabGroup;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::CPbk2TabGroupEntry::CPbk2TabGroupEntry
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer::CPbk2TabGroupEntry*
        CPbk2TabGroupContainer::CPbk2TabGroupEntry::NewL
         ( TPbk2TabGroupId aTabGroupId, CAknNavigationDecorator* aTabGroup )
    {
    CPbk2TabGroupEntry* self = new ( ELeave ) CPbk2TabGroupEntry
        ( aTabGroupId, aTabGroup );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::CPbk2TabGroupEntry::TabGroupId
// --------------------------------------------------------------------------
//
inline TPbk2TabGroupId
        CPbk2TabGroupContainer::CPbk2TabGroupEntry::TabGroupId() const
    {
    return iTabGroupId;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::CPbk2TabGroupEntry::TabGroup
// --------------------------------------------------------------------------
//
inline CAknTabGroup*
        CPbk2TabGroupContainer::CPbk2TabGroupEntry::TabGroup() const
    {
    return static_cast<CAknTabGroup*>( iTabGroup->DecoratedControl() );
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupEntry::Decorator
// --------------------------------------------------------------------------
//
inline CAknNavigationDecorator*
        CPbk2TabGroupContainer::CPbk2TabGroupEntry::Decorator() const
    {
    return iTabGroup;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::CPbk2TabGroupContainer
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer::CPbk2TabGroupContainer(
        MPbk2ViewExplorer& aViewExplorer,
        MPbk2TabGroupContainerCallback& aCallback ) :
    iViewExplorer( aViewExplorer ),
    iCallback ( aCallback )
    {
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::~CPbk2TabGroupContainer
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer::~CPbk2TabGroupContainer()
    {
    delete iTabIcons;
    iViewTabGroups.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2TabGroup::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2TabGroupContainer::ConstructL()
    {
    iTabIcons = CPbk2IconInfoContainer::NewL();

    PBK2_PROFILE_START(Pbk2Profile::ETabGroupCreateTabGroups);
    // creates all the tab groups and tabs
    CreateTabGroupsL();
    PBK2_PROFILE_END(Pbk2Profile::ETabGroupCreateTabGroups);

    PBK2_PROFILE_START(Pbk2Profile::ETabGroupSkinTabGroups);
    // skins the created tabs by looping through all of them
    SkinTabGroupsL();
    PBK2_PROFILE_END(Pbk2Profile::ETabGroupSkinTabGroups);
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2TabGroupContainer* CPbk2TabGroupContainer::NewL(
        MPbk2ViewExplorer& aViewExplorer,
        MPbk2TabGroupContainerCallback& aCallback )
    {
    CPbk2TabGroupContainer* self =
        new ( ELeave ) CPbk2TabGroupContainer( aViewExplorer, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::TabGroupFromViewId
// --------------------------------------------------------------------------
//
EXPORT_C CAknNavigationDecorator* CPbk2TabGroupContainer::TabGroupFromViewId
        ( TInt aId ) const
    {
    CAknNavigationDecorator* tabGroupDecorator = NULL;

    // Loop through the view tab groups array
    const TInt tabGroupCount = iViewTabGroups.Count();
    for (TInt i = 0; i < tabGroupCount; ++i)
        {
        CAknTabGroup* tabGroup = iViewTabGroups[i]->TabGroup();
        if (tabGroup->TabIndexFromId(aId) >= 0)
            {
            CAknNavigationDecorator* decorator =
                    iViewTabGroups[i]->Decorator();
            tabGroupDecorator = decorator;
            break;
            }
        }

    return tabGroupDecorator;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::HandleNavigationKeyEventL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2TabGroupContainer::HandleNavigationKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool result = EFalse;
    CAknTabGroup* topTabGroup = GetTabGroupOnTop();

    if ( topTabGroup )
        {
        // Offer key event to be handled to top most tab
        result = ( topTabGroup->OfferKeyEventL
            ( aKeyEvent, aType ) == EKeyWasConsumed );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::TabIcons
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconInfoContainer& CPbk2TabGroupContainer::TabIcons() const
    {
    return *iTabIcons;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::TabChangedL
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainer::TabChangedL( TInt aIndex )
    {
    CAknTabGroup* topTabGroup = GetTabGroupOnTop();
    if ( topTabGroup )
        {
        // View id is stored in the tab
        const TInt viewId = topTabGroup->TabIdFromIndex( aIndex );

        MPbk2AppUi* appUi = Phonebook2::Pbk2AppUi();

        CPbk2ViewState* activeViewState =
            appUi->ActiveView()->ViewStateLC();
        iViewExplorer.ActivatePhonebook2ViewL( TUid::Uid( viewId ),
            activeViewState );
        CleanupStack::PopAndDestroy( activeViewState );
        }
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::HandleNaviDecoratorEventL
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainer::HandleNaviDecoratorEventL( TInt aEventID )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        CAknTabGroup* topTabGroup = GetTabGroupOnTop();
        if ( topTabGroup )
            {
            topTabGroup->HandleNaviDecoratorEventL( aEventID );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::DoCalculateTabGroupWidth
// --------------------------------------------------------------------------
//
TInt CPbk2TabGroupContainer::DoCalculateTabGroupWidth
        ( TInt aViewCount ) const
    {
    TInt width = EAknTabWidthWithOneTab;
    switch ( aViewCount )
        {
        case 1:
            width = EAknTabWidthWithOneTab;
            break;
        case 2:
            width = EAknTabWidthWithTwoTabs;
            break;
        case 3:
            width = EAknTabWidthWithThreeTabs;
            break;
        case 4:
            width = EAknTabWidthWithFourTabs;
            break;
        }
    if (aViewCount > KMaxTabsInTabGroup)
        {
        // n tabs is the maximum amount visible
        width = EAknTabWidthWithFourTabs;
        }

    return width;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::DoReadViewNodeTabResourcesL
// Reads a PBK2_VIEW_NODE_TAB structure, then based on the amount of
// views in the tab selects the correct substructure and adds an avkon
// tab from the TAB resource
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainer::DoReadViewNodeTabResourcesL
        ( const CPbk2ViewNode& aNode, CAknTabGroup& aAknTabGroup,
          TInt aViewCount )
    {
    // Reads PBK2_VIEW_NODE_TAB structure in the
    // view graph node
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( resReader, aNode.TabResourceId() );

    // Check how many PBK2_VIEW_NODE_TAB elements there are to read
    const TInt count = resReader.ReadInt16();
    TBool found = EFalse;
    // Loop through the resource elements,
    for ( TInt i = 0; i < count && !found; ++i )
        {
        resReader.ReadInt8(); // read version number
        TInt tabsInGroup = resReader.ReadInt8();
        TInt tabResourceId = resReader.ReadInt32();
        TInt iconResId = resReader.ReadInt32();
        // Check if this resource structure corresponds
        // to the amount of tabs previously found in the tab group
        if ( tabsInGroup == aViewCount ||
             (aViewCount >= KMaxTabsInTabGroup &&
              tabsInGroup == KMaxTabsInTabGroup))
            {
            // Read a TAB structure
            TResourceReader tabResReader;
            CCoeEnv::Static()->CreateResourceReaderLC
                ( tabResReader, tabResourceId );
            // Feed structure to Avkon tab group
            aAknTabGroup.AddTabL( tabResReader );
            CleanupStack::PopAndDestroy(); // tabResReader

            // Read the icons
            if ( iconResId != 0 )
                {
                // Add icon infos associated with the TAB to the
                // icon info container
                iTabIcons->AppendIconsFromResourceL(
                        iconResId );
                }
            }
        }
    CleanupStack::PopAndDestroy(); // resReader
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::CreateTabGroupsL
// Initializes the tab group from the view graph
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainer::CreateTabGroupsL()
    {
    RArray<TUid> viewIds;
    CleanupClosePushL( viewIds );

    // Get the number of nodes in the view graph
    const TInt nodeCount = iViewExplorer.ViewGraph().Count();

    // Travel through all the view graph nodes
    for ( TInt i = 0; i < nodeCount; ++i )
        {
        const CPbk2ViewNode& viewNode = iViewExplorer.ViewGraph()[i];

        // Check the view nodes tab group id and if it exists with
        // tab resource id, create tabs unless the view has already
        // been added to some tab group
        TPbk2TabGroupId tabGroupId = viewNode.TabGroupId();
        if ( tabGroupId != 0 &&
             viewNode.TabResourceId() != 0 &&
             viewIds.Find( viewNode.ViewId() ) == KErrNotFound )
            {
            // Get the view graph nodes in the same tab group
            RPointerArray<CPbk2ViewNode> viewsInTabGroup =
                    iViewExplorer.ViewGraph().FindViewsInTabGroupL(
                            tabGroupId );
            CleanupClosePushL( viewsInTabGroup );

            // Set up the tab group width
            const TInt viewsInTabGroupCount = viewsInTabGroup.Count();
            const TInt tabWidth =
                DoCalculateTabGroupWidth( viewsInTabGroupCount );

            // Create tab group if it does not already exist
            CAknTabGroup& aknTabGroup = FindTabGroupWithIdL( tabGroupId );
            aknTabGroup.SetTabFixedWidthL( tabWidth );

            // Loop through the tab groups' views
            for ( TInt view = 0; view < viewsInTabGroupCount; ++view )
                {
                // Get the tab group view node and add its view id to
                // the view id array
                const CPbk2ViewNode& node = *viewsInTabGroup[view];
                viewIds.AppendL( node.ViewId() );

                // If there is no tabs TabResourceId() returns 0.
                // tabs -resource defaults to 0. 
                if (node.TabResourceId() != 0)
                    {
                    // Read the view node tab resource structure and
                    // adds the tabs to the tab group
                    DoReadViewNodeTabResourcesL
                        ( node, aknTabGroup, viewsInTabGroupCount );
                    }
                else
                    {
                    // tab group resource doesnt exits, ask callback to create tab
                    iCallback.CreateViewNodeTabL(
                        node, aknTabGroup, *iTabIcons, viewsInTabGroupCount );
                    }

                }
            CleanupStack::PopAndDestroy(); // viewInTabGroup
            }
        }
    CleanupStack::PopAndDestroy(); // viewIds
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::FindTabGroupWithIdL
// --------------------------------------------------------------------------
//
CAknTabGroup& CPbk2TabGroupContainer::FindTabGroupWithIdL
        ( TPbk2TabGroupId aTabGroupId )
    {
    CAknTabGroup* result = NULL;

    // Find the tab group from the container
    const TInt tabGroupCount = iViewTabGroups.Count();
    for ( TInt i=0; i < tabGroupCount; ++i )
        {
        if ( iViewTabGroups[i]->TabGroupId() == aTabGroupId )
            {
            result = iViewTabGroups[i]->TabGroup();
            break;
            }
        }

    // If the tab group doesnt exists, it is created
    if ( !result )
        {
        CAknNavigationControlContainer* naviPane =
            static_cast<CAknNavigationControlContainer*>
                ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL
                    ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

        // Create the new tab group
        CAknNavigationDecorator* newTabGroup =
                naviPane->CreateTabGroupL( this );
        CleanupStack::PushL( newTabGroup );
        CPbk2TabGroupEntry* entry = CPbk2TabGroupEntry::NewL
            ( aTabGroupId, newTabGroup );
        CleanupStack::Pop( newTabGroup );
        CleanupStack::PushL(entry);
        iViewTabGroups.AppendL(entry);
        CleanupStack::Pop( entry );
        result = static_cast<CAknTabGroup*>
            ( newTabGroup->DecoratedControl() );
        }

    return *result;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainer::SkinTabGroupsL
// This function loops all the tabs and overrides then if theres a skin
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainer::SkinTabGroupsL()
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Travel all the tab groups that have been created
    const TInt count = iViewTabGroups.Count();
    for (TInt i = 0; i < count; ++i)
        {
        CAknTabGroup& tabGroup = *iViewTabGroups[i]->TabGroup();

        // loop through the tabs in the tab group
        const TInt tabCount = tabGroup.TabCount();
        for (TInt j = 0; j < tabCount; ++j)
            {
            TInt tabId = tabGroup.TabIdFromIndex(j);
            // tabId is used as icon id
            TPbk2IconId iconId( TUid::Uid(KPbk2UID3), tabId );
            // Find the icon info from the tab icon info container
            const CPbk2IconInfo* iconInfo = iTabIcons->Find(iconId);
            if (iconInfo)
                {
                CFbsBitmap* bitmap = NULL;
                CFbsBitmap* mask = NULL;
                CPbk2IconFactory* factory =
                    CPbk2IconFactory::NewLC( *iTabIcons );
                // Load the icon and mask
                factory->CreateIconLC(iconId, *skin, bitmap, mask);
                // Replace tab with new bitmaps, skin enabled
                tabGroup.ReplaceTabL(tabId, bitmap, mask);
                CleanupStack::Pop(2); // mask, bitmap
                CleanupStack::PopAndDestroy(factory);
                }
            }
        }
    }

// End of File
