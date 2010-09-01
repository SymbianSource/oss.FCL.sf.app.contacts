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
* Description:  Phonebook 2 contact info application view.
*
*/


#ifndef CPBK2CONTACTINFOAPPVIEW_H
#define CPBK2CONTACTINFOAPPVIEW_H

// INCLUDES
#include "CPbk2AppView.h"
#include <MPbk2MemoryEntryView.h>
#include <MPbk2ControlObserver.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MPbk2MenuCommandObserver.h>
#include <MPbk2NavigationObserver.h>
#include "MPbk2ViewLoaderObserver.h"

// FORWARD DECLARATIONS
class CPbk2ContactInfoControl;
class CPbk2ViewState;
class CPbk2MemoryEntryContactLoader;
template<class ControlType> class CPbk2ControlContainer;
class MPbk2ContactDetailsControl;
class MPbk2CommandHandler;
class MPbk2ViewActivationTransaction;
class MPbk2ContactNavigation;
class MPbk2PointerEventInspector;
class CPbk2ViewLoader;
class CVPbkContactManager;
class MVPbkContactStore;
class MVPbkContactLink;
class CPbk2StoreConfiguration;

// CLASS DECLARATION

/**
 * Phonebook 2 contact info application view.
 */
class CPbk2ContactInfoAppView :
        public CPbk2AppView,
        public MPbk2MemoryEntryView,
        public MPbk2ControlObserver,
        public MPbk2StoreConfigurationObserver,
        public MPbk2MenuCommandObserver,
        public MPbk2NavigationObserver,
        private MPbk2ViewLoaderObserver
    {
    public: // Construction and destruction

        /**
         * Creates and returns a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return A new instance of this class.
         */
        static CPbk2ContactInfoAppView* NewL(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ContactInfoAppView();

    private: // From CPbk2AppView
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        TUid Id() const;
        void HandleStatusPaneSizeChange();
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        void HandleForegroundEventL(
                TBool aForeground );
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation );

    private: // From MPbk2MemoryEntryView
        void PrepareForContactChangeL();
        void ContactChangedL(
                MVPbkStoreContact* aContact );
        void ContactChangeFailed();
        const MVPbkStoreContact* Contact() const;

    private: // From MPbk2ControlObserver
        void HandleControlEventL(
            MPbk2ContactUiControl& aControl,
            const TPbk2ControlEvent& aEvent );

    private: // From MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();

    private: // From MPbk2MenuCommandObserver
        void PreCommandExecutionL(
                const MPbk2Command& aCommand );
        void PostCommandExecutionL(
                const MPbk2Command& aCommand );

    private: // From MPbk2NavigationObserver
        void HandleNavigationEvent(
                const TEventType& aEventType );

    private: // From MPbk2ViewLoaderObserver
        void ViewLoaded( MVPbkContactViewBase& aView );
        void ViewError(
                MVPbkContactViewBase& aView,
                TInt aError );

    private: // Implementation
        CPbk2ContactInfoAppView(
                CVPbkContactManager& aContactManager );
        void ConstructL();
        void CreateControlsL();
        void UpdateViewStateL(
                const TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void SetNavigatorViewL();
        void StoreStateL();
        TBool IsFocusedFieldTypeL(
                TInt aSelectorResId );
        void UpdateCbasL();
        void AttachAIWProvidersL();
        MVPbkContactLink* CurrentContactInNavigatorLC();
        void ReturnToPreviousViewL(
                MVPbkContactLink* aFocusedContact ) const;
        void LoadContactViewL(
                const TVwsViewId& aPrevViewId );
        void HandleViewLoadedL();
        void LoadContactL();
        void ConstructNavigatorL();
        void UpdateViewGraphL(
                const TVwsViewId& aPrevViewId );
        void SetEmptyTextL(
        		TInt aResourceId );

    private: // Data
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2ContactInfoControl> CContainer;
        /// Own: This view's control container
        CContainer* iContainer;
        /// Own: Control state
        CPbk2ViewState* iControlState;
        /// Ref: This view's control
        MPbk2ContactDetailsControl* iControl;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Command handler
        MPbk2CommandHandler* iCommandHandler;
        /// Own: View activation transaction
        MPbk2ViewActivationTransaction* iViewActivationTransaction;
        /// Own: Navigation strategy
        MPbk2ContactNavigation* iNavigation;
        /// Own: If of the view that activated this view
        TVwsViewId iPreviousViewId;
        /// Own: Memory entry contact loader
        CPbk2MemoryEntryContactLoader* iContactLoader;
        /// Own: list of stores used
        RPointerArray<MVPbkContactStore> iStoreList;
        /// Ref: Stylus event inspector
        MPbk2PointerEventInspector* iPointerInspector;
        /// Ref: Contact view base
        MVPbkContactViewBase* iContactView;
        /// Own: Contact view loader
        CPbk2ViewLoader* iViewLoader;
        /// Ref: Phonebook store configuration
        CPbk2StoreConfiguration* iStoreConfiguration;
    };

#endif // CPBK2CONTACTINFOAPPVIEW_H

// End of File
