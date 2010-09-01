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
*     Logs application UI class
*
*/


#ifndef     __Logs_App_CLogsAppUi_H__
#define     __Logs_App_CLogsAppUi_H__

//  INCLUDES
#include <aknViewAppUi.h>
#include <AknTabObserver.h>

#include <akntoolbarobserver.h>    
#include <akntoolbar.h>  

#include "Logs.hrh"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CLogsEngine;
class CLogsDocument;
class CSendUi;
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Logs application UI class. 
 * An object of this class is created by the Symbian OS framework by a call to 
 * CLogsDocument::CreateAppUiL(). The application UI object creates and owns
 * the application's views and handles system commands selected from the menu.
 */

class   CLogsAppUi :    public CAknViewAppUi,
						public MAknTabObserver,          
                        public MAknToolbarObserver,
                        public MCoeControlObserver                      
    {
    public: 
        /**
         *  Destructor.
         */
        ~CLogsAppUi();

        /**
         *  Default constructor.
         */
        CLogsAppUi();

        /**
         *  Constructor.
         *
         *  @param aEngine Logs enegine
         */
        CLogsAppUi( CLogsEngine* aEngine );

    private: 
         //Constructor, second phase.
        void ConstructL();

        //Callback function for CIdle object that calls ConstructDelayedL()
        static TInt ConstructDelayedCallbackL( TAny* aContainer  );
        
        //Performs time consuming construction operations once. 
        void DoConstructDelayedL();
        
    public: // other functions

        /**
         *  HandleMessageL. Called by the framework. 
         */
        MCoeMessageObserver::TMessageResponse HandleMessageL(TUint32 aClientHandleOfTargetWindowGroup,
                                                             TUid aMessageUid,
                                                             const TDesC8& aMessageParameters);
        /**
         *  ProcessCommandParametersL. Called by the framework. 
         *
         *  IMPORTANT: This command line parameter interface is PRIVATE and may be replaced
         *  in future with more generic solution. So do not use this unless agreed with Logs
         *  project.
         */
        TBool ProcessCommandParametersL(TApaCommand aCommand,
                                        TFileName& aDocumentName,
                                        const TDesC8& aTail);

        /**
         * Handles menu commands. Called by the framework. Only application-
         * wide and system commands are handled here. Use 
         * CAknView::ProcessCommandL to handle view- and context-specific 
         * commands.
         *
         * @param aCommand  The command code. Codes are defined in Logs.hrh
         *                  and linked to UI controls (such as menus) in the 
         *                  resource file Logs.rss.
         *
         * @exception anything  thrown during the processing of the command. 
         *                      The application framework will by default trap
         *                      and report uncaught exceptions to the user 
         *                      with a simple pop-up dialog.
         */
        void HandleCommandL( TInt aCommand );

        /**
         *  Returns the pointer of the engine.
         *  @return iEngine
         */
        CLogsEngine* Engine();

        /**
         * TPbkIconId array getter
         * @return icon id array
         */
        //Not in use anymore, Phonebook icons replaced by own icons 
        //CArrayFix<TPbkIconId>* IconIdArray();

        /**
         * Icon info container getter
         * @return icon info container
         */
        //Not in use anymore, Phonebook icons replaced by own icons 
        //CPbkIconInfoContainer* IconInfoContainer();


        /*
         * Activate view
         * @param aView to activate, will create view if it 
         *              doesn't exist.
         */
        void ActivateLogsViewL( TLogsViewIds aView );

		/*
         * Activate view
         * @param aView to activate, will create view if it 
         *              doesn't exist.
		 * @param aCustomMessageId id of a custom message sent 
		 *                         with view activation
		 * @param aCustomMessage   a custom message sent
		 *                         with view activation
		 * @param aShowToolbar     show toolbar with
		 *                         view activation
         */
        void ActivateLogsViewL( TLogsViewIds aView,
								TLogsViewActivationMessages aCustomMessageId,  
								const TDesC8 & aCustomMessage );
								

        /**
         * Sets the previous view id.
         *
         * @param aView ID of the view.
         */
        void SetPreviousViewId( TUid aView );

        /**
         * Sets the current view id.
         *
         * @param aView id of the current view
         */
        void SetCurrentViewId( TUid aView );

        /**
         *  Command handler: exit
         */
        void CmdExit();
         
        /**
         *  Command handler: back
         */
        void CmdBackL();

        /**
         * Command OK key handler
         *
         * @param aIndex current item in the listbox
         */
        void CmdOkL( TInt aIndex ); 

        /**
         * Active view id
         * @return view id
         */
        TLogsViewIds ActiveViewId();

        /**
         * Returns the UId of the previous application
         *
         * @return  id of the previous application
         */
        TUid PreviousAppUid() const;
        
        /**
         * Sets the previous application UId.
         *
         * @param aUid ID of the application.
         */
        void SetPreviousAppUid( TUid aUid );

        /**
         * If dialled/missed view is called from another program
         *
         * @param aProvideOnlyRecentViews Is dialled/missed view called Idle etc so 
         *                                that not all views are provided
         */
        void SetProvideOnlyRecentViews( TBool aProvideOnlyRecentViews );

        /**
         * Is dialled/missed view is called from another program.
         *
         * @return Is dialled/missed view called from another program.
         */
        TBool ProvideOnlyRecentViews() const;

        /**
         * Log view current list position getter
         *
         * @return current position
         */
        TInt EventListViewCurrent() const;

        /**
         * Log view current list position setter
         *
         * @param aCurrent position
         */
        void SetEventListViewCurrent( TInt aCurrent );

		/**
		 * Gets the current execution mode for the app
		 *
		 * @return current execution mode
		 */
		TInt ExecutionMode() const;

		/**
		 * Sets the current execution mode for the app
		 *
		 * @param aMode the new execution mode
		 */
		void SetExecutionMode( TInt aMode );
		
        /**
         * When Logs regains foreground this checks wether Logs was actually
         * left to background (soft exit) and we should open Logs main view.
         *
         * Otherwise the dialled calls view would be opened as we activate
         * the dialled calls when  Logs is left to background.
         *
         * By default this is OFF:
         *   -> so normal foreground lost\regain not affected by this
         *
         * Set ON when user selects 'Exit':
         *   -> so when regaining fg without commandline parameter Logs main view is shown.
         *
         * Set OFF if commandline parameter received
         *
         * @return true/false open Logs main view
         */    
		TBool ResetViewOnFocusRegain() const;
		
		/**
         * Set the resetting to main ON\OFF after regaining foreground
         *
         * @param aReset true/false open Logs main view on foreground regain
         */    
		void SetResetViewOnFocusRegain(TBool aReset);
		
		void HideLogsToBackgroundL();

		/**
		 * Returns refrence to single instance of SendUi
		 * (ownership remains in ClogsAppUi)		 
		 */
		CSendUi* SendUiL();		
		
		/**
		 * Returns refrence to single instance of CallUi ServiceHandler for Phone calls 
		 * (ownership remains in ClogsAppUi)
         * @param ETrue If a special case of resetting callui needs to be done, use EFalse normally
		 */
        CAiwServiceHandler* CallUiL( TBool aReset = EFalse );
        
        /**
         * Returns wether Logs was started with Send key from idle
         *
         * @return true/false 
         */  
        TBool LogsOpenedWithSendKey();
        
        /**
         * Setter for iLogsOpenedWithSendKey
         *
         * @param true/false 
         */  
        void SetLogsOpenedWithSendKey(TBool aLogsOpenedWithSendKey);
		
        /**
         *  Returns whether Logs is background.
         */
        TBool IsBackground() const;

	public: // from MAknTabObserver
		void TabChangedL(TInt aIndex);

                //
        void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );
        
	public:        
		/**
		 * Sets the current execution mode for the app
		 *
		 * @param aUid      Uid of the app
		 * @param aTail		Command line parameter for the app
		 */
        TBool StartApplicationL( TUid aUid, const TDesC8& aTail );


	public:
	   /*************************************************************************
        FIXME: Toolbar is currently always on - keeping the toolbar visibility  *
               handling sources in comments for now - remove later.             */
        /**
         * Toolbar Visibility setting
         * @return Toolbar Visibility setting
         */
         // TBool ToolbarVisibility()  const;    
        
        /**
         * Records Toolbar visibility setting
         */
         // void RecordToolbarVisibility();
         
         /***********************************************************************/
      
    private: // From MAknToolbarObserver
        void DynInitToolbarL(TInt aResourceId,
                CAknToolbar* aToolbar );
        void OfferToolbarEventL(TInt aCommand);

        //from MCoeControlObserver        
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);        
       

    private: // data

        /// Own: id of the previous view
        TUid iPreviousViewId;

        /// Own: id of the current view
        TUid iCurrentViewId;

        /// Ref: Pointer to application model.
        CLogsEngine* iEngine;   
        
        /**********************************************************************
        Not in use anymore, Phonebook icons replaced by own icons 
        
        /// Own: icon id array
        CArrayFix<TPbkIconId>*   iIconIdArray;

        /// Own: icon info container
        CPbkIconInfoContainer*   iIconInfoContainer;
        **********************************************************************/        

        /// Own: resource file for recent view icons ( from phonebook )
        TBool               iResetToMain;
        
        /// Own id of the previous application
        TUid iPreviousAppUid;

        /// Own: Is dialled/missed view is called from another program.
        TBool iProvideOnlyRecentViews;
        
        TBool iLogsOpenedWithSendKey;

		/// Own: current execution mode
		TInt iExecutionMode;
		
        enum TExitEndPressed
            {
            ENone,
            EEndPressed,
            EExitPressed
            };

        TExitEndPressed		iExitOrEndPressed;	//Relates to KAknUidValueEndKeyCloseEvent		
        
        CSendUi*            iSendUi;            //Own: Instance of SendUi
        CAiwServiceHandler* iServiceHandler;    //own: ServiceHandler for CallUi, Pbk etc
        CIdle*              iConstructDelayed;  //own
        
        /*************************************************************************
        FIXME: Toolbar is currently always on - keeping the toolbar visibility   *
               handling sources in comments for now - remove later.              *
        TBool               iShowToolbar;        *********************************/
        
    };

#endif  // __Logs_App_CLogsAppUi_H__

        
// End of File
