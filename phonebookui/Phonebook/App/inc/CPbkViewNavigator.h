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
*       Phonebook view navigation base class. 
*
*/


#ifndef __CPbkViewNavigator_H__
#define __CPbkViewNavigator_H__


//  INCLUDES
#include <e32base.h>    // CBase
#include <w32std.h>     // TKeyEvent, TEventCode

// FORWARD DECLARATIONS
class CDigViewGraph;
class CPbkAppUiBase;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class TResourceReader;
class CAknTabGroup;
class CPbkExtGlobals;
class CPbkIconInfoContainer;

// CLASS DECLARATION

/**
 * Phonebook view navigation base class.
 */
class CPbkViewNavigator : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class. Derived class implements this
         * function to return the platform-specific implementation.
         *
         * @param aViewGraph Phonebook view navigation graph.
         * @param aAppUI     Phonebook app UI.
         */
        static CPbkViewNavigator* NewL
            (const CDigViewGraph& aViewGraph,
            CPbkAppUiBase& aAppUi);

        /**
         * Destructor.
         */
        ~CPbkViewNavigator();

    public:  // Interface
        /**
         * Handles a view navigation key event.
         *
         * @param aKeyEvent @see CCoeControl::OfferKeyEventL.
         * @param aType     @see CCoeControl::OfferKeyEventL.
         * @return true if the key event was processed.
         */
        virtual TBool HandleNavigationKeyEventL
            (const TKeyEvent& aKeyEvent, TEventCode aType) =0;

        /**
         * Handles resource change event.
         *
         * @param aType		@see CEikAppUi::HandleResourceChangeL
         */
        void HandleResourceChange(TInt aType);

        /**
         * Returns the app navi pane.
         */
        CAknNavigationControlContainer& NaviPane() const;

        /**
         * Returns a tab group that contains a tab with aTabId.
         *
         * @param aTabId    tab id to search for.
         * @return  the matching tab group or NULL if not found.
         */
        CAknNavigationDecorator* TabGroupFromId(TInt aTabId) const;

    protected:  // Interface for derived classes
        /**
         * Creates and returns a tab group from a resource.
         */
        virtual CAknNavigationDecorator* CreateTabGroupL(TResourceReader& aResReader) =0;

    protected:  // Constructors
        /**
         * C++ constructor.
         */
        CPbkViewNavigator(const CDigViewGraph& aViewGraph, CPbkAppUiBase& aAppUi);

        /**
         * Call from derived class ConstructL().
         */
        void BaseConstructL();

	private: // Implementation
		CAknTabGroup* TabGroupFromIndex (TInt aIndex) const;
        void SkinTabBitmaps();
        void DoSkinTabBitmapsL();
        void SkinTabGroupL(CAknTabGroup& aTabGroup, 
                           CPbkIconInfoContainer& aIconInfoContainer);

    protected:  // Data
        /// Ref: Phonebook view navigation graph
        const CDigViewGraph& iViewGraph;
        /// Ref: Phonebook app UI. Used for view switching
        CPbkAppUiBase& iAppUi;
        /// Ref: app navi pane
        CAknNavigationControlContainer* iNaviPane;
        /// Own: view navigation tab groups
        RPointerArray<CAknNavigationDecorator> iViewTabGroups;
        /// Ref: Extension globals singleton
        CPbkExtGlobals* iExtGlobals;
    };


// INLINE FUNCTIONS

inline CPbkViewNavigator::CPbkViewNavigator
        (const CDigViewGraph& aViewGraph, CPbkAppUiBase& aAppUi) :
    iViewGraph(aViewGraph), iAppUi(aAppUi)
    {
    // CBase::operator new(TLeave) resets other member data
    }

inline CAknNavigationControlContainer& CPbkViewNavigator::NaviPane() const
    {
    return *iNaviPane;
    }


#endif  // __CPbkViewNavigator_H__

// End of File
