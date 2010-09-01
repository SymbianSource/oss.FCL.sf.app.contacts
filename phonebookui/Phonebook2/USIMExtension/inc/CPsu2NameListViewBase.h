/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A base class for sim name list views
*
*/



#ifndef CPSU2NAMELISTVIEWBASE_H
#define CPSU2NAMELISTVIEWBASE_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2UIExtensionView.h>
#include <MPbk2ControlObserver.h>
#include "TPsu2SimIconId.h"
#include "Pbk2USimUI.hrh"
#include <MPbk2MenuCommandObserver.h>

// FORWARD DECLARATIONS

class CEikImage;
class CPsu2ViewManager;
class CPbk2UIExtensionView;
template<class Control> class CPbk2ControlContainer;
class CPbk2NamesListControl;
class MPbk2ContactUiControl;
class MPbk2ContactNameFormatter;
class CPbk2SortOrderManager;
class MPbk2PointerEventInspector;

// CLASS DECLARATION

/**
 *  A base class for SIM name list views
 */
class CPsu2NameListViewBase : 
        public CBase,
        public MPbk2UIExtensionView,
        public MPbk2ControlObserver,
        public MPbk2MenuCommandObserver
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CPsu2NameListViewBase();

    public: // From MPbk2UIExtensionView
        void HandleStatusPaneSizeChange();
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent,
            TEventCode aType);
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void DoActivateL(const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId, const TDesC8& aCustomMessage);
        void DoDeactivate();
        void HandleCommandL(TInt aCommand);
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation);

    public: // From MPbk2ControlObserver
        void HandleControlEventL(MPbk2ContactUiControl& aControl,
            const TPbk2ControlEvent& aEvent);

    public: // From MPbk2MenuCommandObserver
        void PreCommandExecutionL(
                const MPbk2Command& aCommand );
        void PostCommandExecutionL(
                const MPbk2Command& aCommand );

    protected:  // New functions
        virtual void UpdateCbasL() { };

    protected: // Construction
        CPsu2NameListViewBase(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        virtual void UpdateListEmptyTextL(
                TInt aListState );

    private:  // Implementation
        /// Sub class offers the resource id for the name list control
        virtual TInt NameListControlResourceId() const = 0;
        void CreateControlsL();
        void StoreStateL();
        void RestoreStateL();

    protected:  // Data
        /// Ref: The extension view instance
        CPbk2UIExtensionView& iExtensionView;
        /// Ref: The view manager of the extension
        CPsu2ViewManager& iViewManager;
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2NamesListControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Ref: This view's control
        CPbk2NamesListControl* iControl;
        /// Ref: Pointer event inspector
        MPbk2PointerEventInspector* iPointerEventInspector;
        class TContextPaneIcon;
        /// Own: ETrue, if any selection modifier key is depressed
        TBool iSelectionModifierUsed;
        /// Own: ETrue after PreCommandExecutionL() was called, and EFalse
        /// after PostCommandExecutionL().
        TBool iCommandBetweenPreAndPostExecution;

    private:    // Data
        /// Own: stored control state
        CPbk2ViewState* iControlState;
        /// Own: Own sort order manager for fdn
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: own formatter is needed for fdn
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: Focus index
        TInt iFocusIndex;
    };

/**
 * Phonebook 2 USIM extension context pane icon.
 * Responsible for loading  the application icon for the context pane.
 */
class CPsu2NameListViewBase::TContextPaneIcon
    {
    public: // Construction
     
        /**
         * Constructor.
         *
         * @param aIconId       Icon id.
         */
        TContextPaneIcon(
                TPsu2SimIconId aIconId );

    public: // Interface

        /**
         * Creates a context pane icon.
         *
         * @return  Image for the context pane.
         */
        CEikImage* CreateLC();

    private: // Implementation
        void SetSize(
                CFbsBitmap& aBitmap,
                CFbsBitmap& aMask );

    private: // Data
        /// Own: Icon id
        TPsu2SimIconId iIconId;
    };

#endif      // CPSU2NAMELISTVIEWBASE_H

// End of File
