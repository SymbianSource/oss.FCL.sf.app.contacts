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
*     Logs application "App List" view class implementation
*
*/


#ifndef     __Logs_App_CLogsAppListView_H__
#define     __Logs_App_CLogsAppListView_H__


//  INCLUDES

#include "CLogsBaseView.h"  // App view super class

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsAppListControlContainer;

// CLASS DECLARATION

/**
 * Logs application "App List" view class implementation.
 */
class   CLogsAppListView : public  CLogsBaseView
    {
    public:  // interface
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @return The new view object.
         */
        static CLogsAppListView* NewL();

        /**
         * Destructor.
         */
        ~CLogsAppListView();

    private:

        /**
         * Default constructor
         */
        CLogsAppListView();
        
        /**
         * Constructor, second phase. 
         */
        void ConstructL();
    
    private:  // from CAknView
        /**
         * Returns the ID of the view.
         * @return view id
         */
        TUid Id() const;

        /**
         *  HandleClientRectChange. Handles the view rectangle changes.
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

        /**
         * Early detection of view activation.
         * Overrides the baseclass implementation calls CAknView::ViewActivatedL().
         */
        void ViewActivatedL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage);


    public: // from MEikCommandObserver
        /**
         * HandleCommandL. Generic view commands.
         *
         * @param aCommandId This is an id for command.
         */
        void HandleCommandL(TInt aCommand);
    

    private:  // data

        /// Own: This view's control container
        CLogsAppListControlContainer* iContainer;

        /// Own: highlighted position
        TInt iSelectedLine;
    };


#endif  // __Logs_App_CLogsAppListView_H__


// End of File
