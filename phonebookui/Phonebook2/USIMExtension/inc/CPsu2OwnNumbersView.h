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
* Description:  Phonebook 2 USIM Own Numbers view.
*
*/


#ifndef CPSU2OWNNUMBERSVIEW_H
#define CPSU2OWNNUMBERSVIEW_H

//  INCLUDES
#include <CPbk2ControlContainer.h>
#include <MPbk2UIExtensionView.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionView;
class CPsu2ViewManager;
class CPsu2OwnNumberControl;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM Own Numbers view.
 */
class CPsu2OwnNumbersView : public CBase,
                            public MPbk2UIExtensionView
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aExtensionView    The view from the extension framework.
         * @param aViewManager      Rhe view manager of the USIM extension.
         * @return  A new instance of this class.
         */
        static CPsu2OwnNumbersView* NewL(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        
        /**
         * Destructor.
         */
        ~CPsu2OwnNumbersView();

    public: // Interface

        /**
         * Updates CBA buttons.
         */
        void UpdateCbasL();

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
    
    protected:  // Implementation
        void CreateControlsL();

    private: // From CPsu2NameListViewBase
        TInt NameListControlResourceId() const;

    private: // Implementation
        CPsu2OwnNumbersView(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );

    private: // Data
        /// Ref: The extension view instance
        CPbk2UIExtensionView& iExtensionView;
        /// Ref: The view manager of the extension
        CPsu2ViewManager& iViewManager;
        /// Ref: Control
        CPsu2OwnNumberControl* iControl;
        /// This view's control container type
        typedef CPbk2ControlContainer<CPsu2OwnNumberControl> CContainer;
        /// Own: This view's control container
        CContainer* iContainer;
    };

#endif // CPSU2OWNNUMBERSVIEW_H
            
// End of File
