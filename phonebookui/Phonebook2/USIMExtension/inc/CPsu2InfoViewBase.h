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
* Description:  Phonebook 2 USIM contact info view base class.
*
*/


#ifndef CPSU2INFOVIEWBASE_H
#define CPSU2INFOVIEWBASE_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2UIExtensionView.h>
#include <MPbk2ControlObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2MemoryEntryView.h>
#include <vwsdef.h>
#include <MPbk2NavigationObserver.h>

// FORWARD DECLARATIONS
class CPsu2ViewManager;
class CPbk2UIExtensionView;
template<class Control> class CPbk2ControlContainer;
class CPbk2ContactInfoControl;
class MPbk2ContactUiControl;
class MVPbkStoreContact;
class CPbk2MemoryEntryContactLoader;
class MPbk2ViewActivationTransaction;
class MVPbkContactStore;
class MPbk2ContactNavigation;
class CAknNavigationDecorator;
class MVPbkContactLink;
class MPbk2PointerEventInspector;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM contact info view base class.
 * Responsible for providing common functionality for USIM info views.
 */
class CPsu2InfoViewBase : public CBase,
                          public MPbk2UIExtensionView,
                          public MPbk2ControlObserver,
                          public MPbk2MemoryEntryView,
                          public MPbk2NavigationObserver
    {
    public: // Construction and destruction

        /**
         * Destructor.
         */
        ~CPsu2InfoViewBase();

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
                const TPoint& aPenEventScreenLocation );

    public: // From MPbk2ControlObserver
        void HandleControlEventL(
                MPbk2ContactUiControl& aControl,
                const TPbk2ControlEvent& aEvent );

    private: // From MPbk2MemoryEntryView
        void PrepareForContactChangeL();
        void ContactChangedL(
                MVPbkStoreContact* aContact );
        void ContactChangeFailed();
        const MVPbkStoreContact* Contact() const;

    protected: // From MPbk2NavigationObserver
        void HandleNavigationEvent(
                const TEventType& aEventType );

    protected: // Implementation
        CPsu2InfoViewBase(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        void BaseConstructL();

    private: // Base class implementation interface
        virtual TInt NameListControlResourceId() const = 0;
        virtual HBufC* NaviPaneLabelL() const = 0;
        virtual void UpdateCbasL() = 0;

    private: // Implementation
        void CreateControlsL();
        void UpdateViewStateL(
                const TDesC8& aCustomMessage );
        void StoreStateL();
        void RestoreStateL();
        MVPbkContactLink* CurrentContactInNavigatorLC();
        void ReturnToPreviousViewL( MVPbkContactLink* aFocusedContact ) const;
        

    protected: // Data
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2ContactInfoControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Ref: The extension view instance
        CPbk2UIExtensionView& iExtensionView;
        /// Ref: The view manager of the extension
        CPsu2ViewManager& iViewManager;
        /// Ref: This view's control
        MPbk2ContactUiControl* iControl;
        /// Own: Contact loader
        CPbk2MemoryEntryContactLoader* iContactLoader;
        /// Own: View activation transaction object
        MPbk2ViewActivationTransaction* iViewActivationTransaction;
        /// Ref: Pointer event inspector
        MPbk2PointerEventInspector* iPointerEventInspector;

    private: // Data
        /// Own: Stored control state
        CPbk2ViewState* iControlState;
        /// Ref: Register for events
        MVPbkContactStore* iStore;
        /// Own: The view which this view was activated from
        TVwsViewId iPreviousViewId;
        /// Own: Navigation strategy
        MPbk2ContactNavigation* iNavigation;
        /// Own: Store list
        RPointerArray<MVPbkContactStore> iStoreList;
        /// Own: Decorator for the navipane
        CAknNavigationDecorator* iNaviDecorator;
    };

#endif // CPSU2INFOVIEWBASE_H

// End of File
