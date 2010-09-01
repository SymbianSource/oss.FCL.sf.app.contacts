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
*     Logs application "Sub application list" view class implementation
*
*/


#ifndef     __Logs_App_CLogsSubAppListView_H__
#define     __Logs_App_CLogsSubAppListView_H__

//  INCLUDES

#include "CLogsBaseView.h"   // App view super class

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CLogsSubAppListControlContainer;

// CLASS DECLARATION

/**
 * Logs application "Sub application list" view class implementation.
 * From this view user can navigate to any sub application.
 */

class   CLogsSubAppListView :   public CLogsBaseView
    {
    public:  // interface
        /**
         * Standard creation function. Creates and returns a new object of 
         * this class.
         * @return The new view object.
         */
        static CLogsSubAppListView* NewL();

        /**
         * Destructor.
         */
        ~CLogsSubAppListView();

    private:

        /**
         * Default constructor
         */
        CLogsSubAppListView();

        /**
         * Constructor, second phase. 
         */
        void ConstructL();

        /**
         *  For clearing all the recent lists via query
         */
        void CmdClearRecentListsL();
    

    private:  // from CAknView
        /**
         * Returns the ID of the view.
         * @return view id
         */
        TUid Id() const;
        /**
         *  Handles the view rectangle changes.
         */
        void HandleClientRectChange();
        /**
         * Called by the framework when this view is activated.
         *
         * @param aPrevViewId      This is ID for previous view.
         * @param aCustomMessageId ID of the custom message. Not used.
         * @param aCustomMessage   custom message. Not used.
         */
        void DoActivateL(const TVwsViewId& aPrevViewId,
                         TUid aCustomMessageId,
                         const TDesC8& aCustomMessage);
        /**
         * Called by the framework when this view is deactivated.
         */
        void DoDeactivate();

    public: // from MEikCommandObserver
        /**
         * Handles the view specific commands.
         *
         * @param aCommandId This is an id for command.
         *                   Most of the commands are forwarded to LogAppUI.
         */
        void HandleCommandL(TInt aCommand);
   
    public: // MLogsObserver
        void StateChangedL( MLogsStateHolder* aHolder );
    
    private:  // data

        /// Own: This view's control container
        CLogsSubAppListControlContainer* iContainer;

        /// Own: highlighted position
        TInt iSelectedLine;
    };


#endif  // __Logs_App_CLogsSubAppListView_H__


// End of File
