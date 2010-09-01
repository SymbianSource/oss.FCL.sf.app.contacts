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
* Description:  Phonebook 2 names list state factory.
*
*/


#ifndef CPBK2NAMESLISTSTATEFACTORY_H
#define CPBK2NAMESLISTSTATEFACTORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbk2NamesListState;
class CCoeControl;
class MPbk2FilteredViewStack;
class CPbk2ContactViewListBox;
class MVPbkContactViewBase;
class CAknSearchField;
class MPbk2UiControlEventSender;
class CPbk2ThumbnailLoader;
class MPbk2ContactNameFormatter;
class MPbk2UiControlCmdItem;
class CPbk2PredictiveSearchFilter;

/**
 * Phonebook 2 names list state factory.
 * Responsible for:
 * - creating and owning names list control state objects
 */
NONSHARABLE_CLASS(CPbk2NamesListStateFactory) : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContainer        The window owning control.
         * @param aListBox          Contact view list box.
         * @param aViewStack        A view stack for states that needs
         *                          a reference to the contact view and/or
         *                          states who have a find box.
         * @param aFindBox          Find box.
         * @param aThumbnailLoader  Thumbnail loader.
         * @param aEventSender      UI control event sender.
         * aSearchFilter            Ref to the Search filter
         * @return  A new instance of this class.
         */
        static CPbk2NamesListStateFactory* NewL(
                const CCoeControl& aContainer,
                CPbk2ContactViewListBox& aListBox,
                MPbk2FilteredViewStack& aViewStack,
                CAknSearchField* aFindBox,
                CPbk2ThumbnailLoader& aThumbnailLoader,
                MPbk2UiControlEventSender& aEventSender,
                MPbk2ContactNameFormatter& aNameFormatter,
                RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
                CPbk2PredictiveSearchFilter& aSearchFilter );

        /**
         * Destructor.
         */
        ~CPbk2NamesListStateFactory();

    public: // Factory interface
    
        /**
         * Creates the initial state instance of MPbk2NamesListState
         * that can be used when names list is initializing.
         *
         * @param aPreviousState The previous state, can be NULL.
         * @return  A reference to the not ready state instance
         *          of MPbk2NamesListState.
         */
        MPbk2NamesListState& ActivateStartupStateL(
                MPbk2NamesListState* aPreviousState );
                
        /**
         * Creates the not ready state instance of MPbk2NamesListState.
         *
         * @param aPreviousState The previous state, can be NULL.
         * @return  A reference to the not ready state instance
         *          of MPbk2NamesListState.
         */
        MPbk2NamesListState& ActivateNotReadyStateL(
                MPbk2NamesListState* aPreviousState );

        /**
         * Creates the empty state instance of MPbk2NamesListState.
         *
         * @param aPreviousState The previous state, can be NULL.
         * @return  A reference to the empty state instance
         *          of MPbk2NamesListState.
         */
        MPbk2NamesListState& ActivateEmptyStateL(
                MPbk2NamesListState* aPreviousState );

        /**
         * Creates the ready state instance of MPbk2NamesListState.
         *
         * @param aPreviousState The previous state, can be NULL.
         * @return  A reference to the ready state instance
         *          of MPbk2NamesListState.
         */
        MPbk2NamesListState& ActivateReadyStateL(
                MPbk2NamesListState* aPreviousState );

        /**
         * Creates the ready state instance of MPbk2NamesListState.
         *
         * @param aPreviousState The previous state, can be NULL.
         * @return  A reference to the ready state instance
         *          of MPbk2NamesListState.
         */
        MPbk2NamesListState& ActivateFilteredStateL(
                MPbk2NamesListState* aPreviousState );
        
        /**
         * Creates the hidden state instance of MPbk2NamesListState.
         *
         * @param aPreviousState The previous state, can be NULL.
         * @return  A reference to the ready state instance
         *          of MPbk2NamesListState.
         */
        MPbk2NamesListState& ActivateHiddenStateL(
                MPbk2NamesListState* aPreviousState );


    private: // Implementation
        CPbk2NamesListStateFactory(
                const CCoeControl& aContainer,
                CPbk2ContactViewListBox& aListBox,
                MPbk2FilteredViewStack& aViewStack,
                CAknSearchField* aFindBox,
                CPbk2ThumbnailLoader& aThumbnailLoader,
                MPbk2UiControlEventSender& aEventSender,
                MPbk2ContactNameFormatter& aNameFormatter,
                RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
                CPbk2PredictiveSearchFilter& aSearchFilter );
        MPbk2NamesListState& ActivateStateL(
                TInt aState,
                MPbk2NamesListState* aPreviousState );
        void ConstructL();
        static TInt SendEventToObservers( TAny* aSelf );
        void DoSendEventToObservers();

    private: // Data
        /// Ref: Control container
        const CCoeControl& iContainer;
        /// Ref: Control container
        CPbk2ContactViewListBox& iListBox;
        /// Ref: A view stack for states that use find box
        MPbk2FilteredViewStack& iViewStack;
        /// Ref: Find box control
        CAknSearchField* iFindBox;
        /// Ref: Thumbnail loader
        CPbk2ThumbnailLoader& iThumbnailLoader;
        /// Own: Array of states, owns the state objects
        RPointerArray<MPbk2NamesListState> iStateArray;
        /// Ref: UI control event sender
        MPbk2UiControlEventSender& iEventSender;
        /// Own: Control event sending
        CIdle* iSendControlEvent;
        /// Own: Current names list state
        TInt iNamesListState;
        /// Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: Command items
        RPointerArray<MPbk2UiControlCmdItem>& iCommandItems;
        ///Ref: Search filter
        CPbk2PredictiveSearchFilter& iSearchFilter; 
    };

#endif // CPBK2NAMESLISTSTATEFACTORY_H

// End of File
