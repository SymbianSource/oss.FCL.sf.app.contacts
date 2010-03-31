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
* Description:  Phonebook 2 Group UI extension group view.
*
*/


#ifndef CPGUGROUPVIEW_H
#define CPGUGROUPVIEW_H

// INCLUDES
#include <e32base.h>
#include <MPbk2UIExtensionView.h>
#include <MPbk2ControlObserver.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MPbk2MenuCommandObserver.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionView;
class CPbk2NamesListControl;
template<class ControlType> class CPbk2ControlContainer;
class MPbk2CommandHandler;
class MPbk2PointerEventInspector;
class CPbk2StoreConfiguration;

// CLASS DECLARATION

/**
 * Phonebook 2 Group UI extension group view.
 */
class CPguGroupView : public CBase,
                      public MPbk2UIExtensionView,
                      private MPbk2ControlObserver,
                      private MPbk2StoreConfigurationObserver,
                      private MPbk2MenuCommandObserver,
                      public MEikListBoxObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aView             Phonebook 2 UI Extension view.
         * @return  A new instance of this class.
         */
        static CPguGroupView* NewL(
                CPbk2UIExtensionView& aView );

        /**
         * Destructor.
         */
        ~CPguGroupView();

    public:  // new function
        
        void CheckUiControlL();
        
    public: // From MPbk2UIExtensionView
        void HandleStatusPaneSizeChange();
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation);

    protected: 
        // from MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
   
   
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

    private: // Implementation
        CPguGroupView(
                CPbk2UIExtensionView& aView );
        void ConstructL();
        TBool CurrentStoreSupportsGroupsL() const;
        TBool CurrentStoreIsAvailableL() const;
        void CreateControlsL();
        void StoreStateL();
        void RestoreStateL();
        void UpdateCbasL();
        void UpdateListEmptyTextL();
        void HandleConfigurationChangeL();
        void ShowContextMenuL();

    private: // Types
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2NamesListControl> CContainer;

    private: // Data
        /// Ref: View that contains this extension view
        CPbk2UIExtensionView& iView;
        /// Own: This view's control container
        CContainer* iContainer;
        /// Ref: This view's control
        CPbk2NamesListControl* iControl;
        /// Ref: Pointer event inspector
        MPbk2PointerEventInspector* iPointerEventInspector;
        /// Own: Stored control state
        CPbk2ViewState* iControlState;
        /// Own: Control state passed as parameter
        CPbk2ViewState* iParamState;
        /// Ref: Command handler
        MPbk2CommandHandler* iCommandHandler;
        /// Own: Latest group name list control state
        TInt iListControlState;
        /// Ref: Phonebook store configuration
        CPbk2StoreConfiguration* iStoreConfiguration;
        /// Own: Stylus popup menu state indicator
        TBool iStylusPopupMenuLaunched;
    };

#endif // CPGUGROUPVIEW_H

// End of File
