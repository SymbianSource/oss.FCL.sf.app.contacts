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
*     Logs application "Detail" view
*
*/


#ifndef     __Logs_App_CLogsDetailView_H__
#define     __Logs_App_CLogsDetailView_H__

//  INCLUDES
#include "CLogsBaseView.h"
#include "MLogsObserver.h"
#include "LogsEng.hrh"

#include <aknnavide.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsDetailControlContainer;
class CEikListBox;
class TListBoxEvent;

// CLASS DECLARATION

/**
 * Logs application "Detail" view class implementation.
 * This view show the details of the selected event.
 */
class   CLogsDetailView :   public CLogsBaseView,
                            public MAknNaviDecoratorObserver
    {
    public:  // interface
        /**
         *  Standard creation function. Creates and returns a new object
         *  of this class.
         *
         *  @return The new view object.
         */
        static CLogsDetailView* NewL();
        /**
         *  Destructor.
         */
        ~CLogsDetailView();
    
    public:  // from CAknView
        /**
         *  Framework calls to init menu items.
         *
         *  @param aResourceId   resource read
         *  @param aMenuPane     menu pane
         */ 
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
         *  If touch support is on, we handle the touch related events here.
         *  
         *  @param aListBox    the listbox reference
         *  @param aEventType  the listbox event 
         */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);     

    private:  // from CAknView
        /**
         * Returns the ID of the view.
         * @return view id
         */
        TUid Id() const;

        /**
         *  HandleClientRectChange. Called by the framework the view rectangle
         *  changes
         */
        void HandleClientRectChange();

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

       /**
        * Early detection of view activation.
        * Overrides the baseclass implementation calls CAknView::ViewActivatedL().
        * 
        * @param aPrevViewId Specifies the view previously active.
        * @param aCustomMessageId Specifies the message type.
        * @param aCustomMessage Activation message.
        */ 
        void ViewActivatedL(const TVwsViewId& aPrevViewId,
                                   TUid aCustomMessageId,
                                   const TDesC8& aCustomMessage);
               
       /**
        * Early detection of view activation.
        * Overrides the baseclass implementation calls CAknView::ViewDeactivated().
        */
        void ViewDeactivated();


    public: // from MEikCommandObserver
        /**
         *  View specific command handler.
         *
         *  @param aCommandId This is an id for command.
         *                    Most of the commands are forwarded to LogAppUI.
         */
        void HandleCommandL( TInt aCommand );


    public: // from MLogsKeyProcessor
        TBool ProcessKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
 		/**
         * Overrides the baseclass handler
         * 
         * @param aIndex	The index of the item tapped
         */ 
        void ProcessPointerEventL(TInt aIndex);
        
    public: // from MAknNaviDecoratorObserver     
        /**
         * Handler for pointer events, when the Navi Pane arrows have been tapped
         *
         * @param aEventID  The enum indicating wether right or left arrow was tapped
         */
        void HandleNaviDecoratorEventL( TInt aEventID ); 

    public: // from MLogsModelObserver
        void StateChangedL( MLogsStateHolder* aHolder ); 

    
    public: // Command handlers (use "Cmd" prefix)

        /**
         *  Command handler: Open the context sensive menu.
         */
        void CmdContextMenuL();

        /**
         * Show the next event's details.
         */
        void CmdNextEventL();

        /**
         * Show the previous event's details.
         */
        void CmdPreviousEventL();
        
        /**
         * Restore original Logs title pane text according to current view.
         */
        void ChangeTitlePaneTextToDefaultL();


    private:  // operations
        /**
         *  Constructor, second phase. 
         */
        void ConstructL();

        /**
         *  Show 1.5 second long confirmation note to the user.
         */
        void PopUpNoteL();
        
       /**
        * Draws the listbox.
        */
        void DrawComponents();

        /**
         * Sets all menu items item specific property.
         * 
         * @param aMenuPane Pointer to menu pane.
         * @param aItemSpecific ETrue if command should be item specific.
         * EFalse otherwise.
         */
        void SetMenuItemsItemSpecificProperty(
                CEikMenuPane* aMenuPane, TBool aItemSpecific );

    private:  // data

        /// Own: This view's control container.
        CLogsDetailControlContainer* iContainer;

        /// Own:
        TLogsState iState;   
    };


#endif  // __Logs_App_CLogsDetailView_H__


// End of File

