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
*     Logs application "Event list" view class implementation
*
*/


#ifndef     __Logs_App_CLogsEventListView_H__
#define     __Logs_App_CLogsEventListView_H__

//  INCLUDES
#include "CLogsBaseView.h"
#include "LogsEng.hrh"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEikMenuPane;
class CLogsEventListControlContainer;
class MLogsStateHolder;

// CLASS DECLARATION

/**
 * Logs application "Event list" view class implementation.
 */
class   CLogsEventListView :    public  CLogsBaseView
    {
    public:  // interface
        /**
         *  Standard creation function. Creates and returns a new object
         *  of this class.
         *
         *  @return The new view object.
         */
        static CLogsEventListView* NewL();

        /**
         *  Destructor.
         */
        ~CLogsEventListView();

     private:

        /**
         *  Default constructor
         */
        CLogsEventListView();

        /**
         *   Constructor, second phase. 
         */
        void ConstructL();

        
    public:  // from CAknView
        /**
         *  Framework calls to init menu items.
         *
         *  @param aResourceId   resource read
         *  @param aMenuPane     menu pane
         */ 
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:  // from CAknView
        /**
         * Returns the ID of the view.
         * @return view id
         */
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
         *  Called by the framework when this view is activated.
         *
         *  @param aPrevViewId      This is ID for previous view.
         *  @param aCustomMessageId ID of the custom message. Not used.
         *  @param aCustomMessage   custom message. Not used.
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         *  Called by the framework when this view is deactivated.
         */
        void DoDeactivate();
        

    public:

        /**
         *  Currently used filter
         *
         *  @return current filter
         */

        TLogsFilter CurrentFilter() const;

        /**
         *  State of underlying model
         *
         *  @return state of model
         */
        TLogsState State() const;
    
    public: // from MEikCommandObserver
        /**
         *  Handles the view specific commands.
         *
         *  @param aCommandId This is an id for command.
         *                    Most of the commands are forwarded to LogAppUI.
         */
        void HandleCommandL( TInt aCommand );
        
        /**
         * Overrides the baseclass handler
         * 
         * @param aCommand	The id of the command
         */   
        void ProcessCommandL( TInt aCommand ); 
        
        /**
         * Restore original Logs title pane text according to current view.
         * Calls CLogsBaseControlContainer::SetTitlePaneTextToDefaultL which
         * does the actual work.
         */
        void ChangeTitlePaneTextToDefaultL();
               
    public: // from MLogsKeyProcessor
        TBool ProcessKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    public: // from MLogsModelObserver
        void StateChangedL( MLogsStateHolder* aHolder ); 
           
    private:

        /**
         *  Initialize filter popup view.
         *
         *  @return Index of selected filter.
         */
        TLogsFilter InitFilterPopupListL();

        /**
         *  Makes filter list's one line with the current information
         *
         *  @param  aItems              Array where to put the line information
         *  @param  aFirstResourceText  First line's resource id
         */
        void AddOneListBoxTextLineL( CDesCArrayFlat* aItems,
                                     TInt aResourceText );

        /**
         *  Function clears all the events from log db.
         */
        void CmdClearListL();

        /**
         *  Function filters the events from log db.
         */
        void CmdFilterL();

    private:  // data

        /// Own:  control container
        CLogsEventListControlContainer* iContainer;
        
        /// Own:
        TLogsFilter iCurrentFilter;

        /// Own:
        TLogsState iState;
        
        /// Own:
        TBool iClearListInProgress;

#ifdef _DEBUG // for performance evaluation
        /// Own: view construction time
        TTime iCreationTime;
        /// Own: construction time calculated
        TBool iCreationTimeCalculated;
#endif

    };

#endif  // __Logs_App_CLogsEventListView_H__

// End of File
