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
*     View for recent lists
*
*/


#ifndef __CLogsRecentListView_H__
#define __CLogsRecentListView_H__

//  INCLUDES
#include <coemain.h>
#include "CLogsBaseView.h"
#include "MLogsExtObserver.h"
#include "MLogsModel.h"

#include "LogsEng.hrh"
#include "Logs.hrh"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsRecentListControlContainer;
class CLogsViewGlobals;
class MLogsViewExtension;
class MLogsEventGetter;

// CLASS DECLARATION

/**
*	Abstract base class for recent views
*/
class CLogsRecentListView :     public CLogsBaseView,
                                public MLogsExtObserver,
                                public MCoeForegroundObserver
    {
    public:  // Constructors and destructor
        
        enum TLogsDrawNow
            {
            ELogsDrawNow,
            ELogsDontDraw
            };
        /**
         * Symbian OS constructor
         * @param recentlist type
         * @return new object
         */

        static CLogsRecentListView* NewL( TLogsModel aModel );

        /**
         * Destructor.
         */
        virtual ~CLogsRecentListView();
        
        /**
         * Recentlist state
         */
        TLogsState State() const;

        /**
         * Getter for recent type
         *
         * @return type of recent list (model)
         */
        TLogsModel RecentListType() const;

    public: // from MLogsObserver
        void StateChangedL( MLogsStateHolder* aHolder ); 

    public: // from MLogsExtObserver
        void ExtStateChangedL();

    public: // from MLogsKeyProcessor
        TBool ProcessKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

       	/**
         * Overrides the baseclass handler
         * 
         * @param aIndex	The index of the item tapped
         */ 
		void ProcessPointerEventL(TInt aIndex);

    public: // CAknView
        void HandleCommandL( TInt aCommandId );
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
    
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage);
        void DoDeactivate();
        TUid Id() const;
        
       /**
        *  Called when the client rectangle of this view changes
        */
        void HandleClientRectChange();
        
       /**
        * Draws the listbox.
        */
        void DrawComponents();
        
       /**
        * Restore original Logs title pane text according to current view.
        * Calls CLogsBaseControlContainer::SetTitlePaneTextToDefaultL which
        * does the actual work.
        */
        void ChangeTitlePaneTextToDefaultL();
        
       /**
        * Store the event list current focused item information
        */
        void StoreEvenListCurrentFocus();
        
        /**
         * Early detection of view activation.
         * Overrides the baseclass implementation calls CAknView::ViewDeactivated().
         */
        void ViewDeactivated();

        /**
         * Overrides the baseclass handler
         * 
         * @param aCommand	The id of the command
         */ 
        void ProcessCommandL( TInt aCommand );
    

    public: // from MCoeForegroundObserver
		void HandleGainingForeground();
		void HandleLosingForeground();
		
    public: // From MAiwNotifyCallback
        TInt HandleNotifyL( TInt aCmdId,
            		        TInt aEventId,
            		        CAiwGenericParamList& aEventParamList,
            		        const CAiwGenericParamList& aInParamList);
		
    private:

        /**
        * C++ default constructor.
        * @param recentlist type
        */
        CLogsRecentListView( TLogsModel aModel );

        /**
        * By default Symbian OS constructor is private.
        * Calls AknViews BaseConstruct
        */
        void ConstructL();
        
        /**
         * Delayed constructor
         *
         * Performs time consuming construction operations once. Calling this is needed before anything
         * needing objects constructed here can be used (e.g. pressing Send key or launching a command from
         * ui's menu.        
         */
        // void ConstructDelayedL();        

        /**
         * Getter for view id
         *
         * @return Return view id
         */
        TLogsViewIds LogsCurrentRecentViewId() const;

        /**
         * Retrieves event relating current selection of listbox.
         * @return Current event or NULL
         */
        const MLogsEventGetter* CurrentEvent();

        /**
         * Clear listbox contents
         */
        void CmdClearRecentListL();

        /**
         * Delete selected recent event from recent list
         */
        void CmdDeleteEventL();

        /**
         *  Overwrites Option_Exit CBA with Option_Back CBA 
         *  @return ETrue if cba changed else return EFalse
         */
        TBool ChangeCba2ToBackL();

        /**
         *  Overwrites Option_Back CBA with Option_Exit CBA
         *
         *  @param  aDrawNow draw now or just change command
         *  @return ETrue if cba changed else return EFalse
         */
        TBool ChangeCba2ToExitL(TLogsDrawNow aDrawNow);
        
        /**
        *   Helper function to check wether event reading has
        *   finished.
        */
        TBool ReadingFinished();

        /**
        *   Helper function to check wether view deactivated operation may be
        *   ignored to aviod the list flicking, see EJZO-7RJB3V
        */
        TBool IgnoreViewDeactivatedOperation();
    
        /**
         * Early detection of view activation.
         * Overrides the baseclass implementation calls CAknView::ViewActivatedL().
         */
        void ViewActivatedL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage);
    
    private:    // Data
        /// Own: view's control container
        CLogsRecentListControlContainer* iContainer;

        /// Own: exit  cba text
        HBufC* iExitCbaButton;

         /// Own: back cba text
        HBufC* iBackCbaButton;  
        
        /// Own:
        TLogsState iState;        

#ifdef _DEBUG // for performance evaluation
        /// Own: view construction time
        TTime iCreationTime;
        /// Own: construction time calculated
        TBool iCreationTimeCalculated;
#endif

        /// Own:
        CLogsViewGlobals* iViewGlobal;

        /// Own:
        MLogsViewExtension* iViewExtension;
        
        
        /// Own: text for send SMS menu item
        //HBufC* iSendSMSText;
        /// Own: text for send MMS menu item
        //HBufC* iSendMMSText;
        
        TInt        iEventListCurrentNoChange;//Points to current event in special Logs processing when Logs has lost 
                                              //foreground and hence iEventListCurrent (in base class) may already 
                                              //have been set to KErrNotFound. Typical case is adding entry to phonebook.
        MLogsModel::TDoActivate iResetAndRefreshOnGainingForeground;
        
    };

#endif // __CLogsRecentListView_H__
            
// End of File
