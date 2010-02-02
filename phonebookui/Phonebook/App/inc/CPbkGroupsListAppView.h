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
*     Groups list View controller (CAknView).
*
*/


#ifndef __CPbkGroupsListAppView_H__
#define __CPbkGroupsListAppView_H__


//  INCLUDES
#include <cntdef.h>			// TContactItemId
#include "CPbkAppView.h"	// Application views super class
#include <MPbkAiwCommandObserver.h>
#include <MPbkContactViewListControlObserver.h>

//  FORWARD DECLARATIONS
class CPbkContactViewListControl;
template<class ControlType> class CPbkControlContainer;
class CPbkViewState;
class MPbkViewExtension;
class CPbkExtGlobals;
class TPbkSendingParams;


//  CLASS DECLARATION

/**
 * Phonebook "Groups List View" application view class. 
 */
class   CPbkGroupsListAppView :
        public CPbkAppView, public MPbkContactViewListControlObserver,
        public MPbkAiwCommandObserver

    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         */
        static CPbkGroupsListAppView* NewL();

        /**
         * Destructor.
         */
        ~CPbkGroupsListAppView();

    private:  // Command handlers (use "Cmd" prefix)
        void CmdOpenGroupL();
        void CmdCreateNewGroupL();
        void CmdRemoveGroupL();
        void CmdRenameGroupL();
        void CmdSetRingingToneL();
        void CmdContextMenuL();
		void CmdOpenSettingsL();
        void CmdPhonebookInfoL();
        void CmdWriteToGroupL();

    public:  // from CPbkAppView
        CPbkViewState* GetViewStateLC() const;

    private:  // from CPbkAppView
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private:  // from CAknView
        void HandleCommandL(TInt aCommand);
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	    TUid Id() const;
	    void HandleStatusPaneSizeChange();
	    void DoActivateL
            (const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
	    void DoDeactivate();

    private:  // from MPbkContactViewListControlObserver
        void HandleContactViewListControlEventL
            (CPbkContactViewListControl& aControl,
            const TPbkContactViewListControlEvent& aEvent);

    private:  // Implementation
        CPbkGroupsListAppView();
        void ConstructL();
        void DoShowPhonebookInfoL();
        TPbkSendingParams CreateWriteParamsLC();
        
    private: // from MPbkAiwCommandObserver
        virtual void AiwCommandHandledL(TInt aMenuCommandId,
            TInt aServiceCommandId,
            TInt aErrorCode);

    private:  // data
        /// This view's control container type
        typedef CPbkControlContainer<CPbkContactViewListControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Own: Saved view-side UI control state
        CPbkViewState* iControlState;
        /// Own: parameter view state
        CPbkViewState* iParamState;
        /// Own: Group to focus
        TContactItemId iGroupToFocus;
        /// Own: for extension menu filtering and command handling
        MPbkViewExtension* iViewExtension;
        /// Own: for accessing extension factory
        CPbkExtGlobals* iExtGlobal;
    };

#endif  // __CPbkGroupsListAppView_H__
            

// End of File
