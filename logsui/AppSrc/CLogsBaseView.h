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
*     Logs Application view superclass
*
*/


#ifndef     __Logs_App_CLogsBaseView_H__
#define     __Logs_App_CLogsBaseView_H__

//  INCLUDES
#include <aknview.h>
#include <eiklbo.h>
#include <AiwServiceHandler.h>      //For Application InterWorking
#include <AiwPoCParameters.h>
#include <MPhCltEmergencyCallObserver.h>
#include <aiwdialdata.h>        //NEW, not yet available in wk46 bld
#include <akntoolbarobserver.h>    
#include <akntoolbar.h>    
#include <akninputblock.h>

#include "LogsEng.hrh"
#include "Logs.hrh"
#include "MLogsKeyProcessor.h"
#include "MLogsTimer.h"
#include "MLogsModel.h" 
#include "MLogsObserver.h"

#include <MVPbkContactStoreListObserver.h>

#include <eikmenub.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsAppUi;
class CLogsEngine;
class MLogsReaderConfig;
class MLogsEventGetter;
class MLogsModel;
class CLogsTimer;
class TAiwPocParameterData;
class CPhCltEmergencyCall;
class CSPEntry;

// CLASS DECLARATION

/**
 * Logs Application view superclass. Provides some common application view
 * functionality. Derive application views from this class to use the 
 * functionality.
 */
class   CLogsBaseView:  public CAknView,
                        public MEikListBoxObserver,
                        public MLogsKeyProcessor,
                        public MAiwNotifyCallback,
                        public MLogsTimer,
                        public MAknToolbarObserver,
                        public MCoeControlObserver,
                        public MPhCltEmergencyCallObserver,
                        public MAknInputBlockCancelHandler,
                        public MCoeViewActivationObserver,
                        public MLogsObserver
    {
    public:
        /**
         * Destructor
         */
        virtual ~CLogsBaseView();

    protected:
        /**
         * Default constructor
         */
        CLogsBaseView();
        
        /**
         * Delayed constructor
         *
         * Performs time consuming construction operations once. Calling this is needed before anything
         * needing objects constructed here can be used (e.g. pressing Send key or launching a command from
         * ui's menu.        
         * @param aIdle EFalse: perform immediately, ETrue: perform using idle time
         */
        void ConstructDelayedL( TBool aIdle );

        /**
         * Hook to catch BaseConstructL calls from derived classes before/after call to super 
         * class BaseConstructL.
         *
         * @param aResId ResourceId of the view to be constructed.
         */
        void BaseConstructL(TInt aResId );
        
    private:  
    
        //Callback function for CIdle object that calls ConstructDelayedL()
        static TInt ConstructDelayedCallbackL( TAny* aContainer  );
        
        
    public: // From MLogsTimer
        //Callback function for CLogsTimer 
        void HandleLogsTimerL( TAny* aPtr );
            
    public: // From MAknInputBlockCancelHandler        
        void AknInputBlockCancel();
   
    public: // From MCoeViewActivationObserver
        
       /**
        * The MCoeViewActivationObserver is set when save to pbk AiW command is called.
        * If then another Logs view is activated while the AiW call is still active, we can 
        * cancel the AiW call here. 
        * 
        * @param aNewlyActivatedViewId      the view that is being activated
        * @param aViewIdToBeDeactivated     the view that is being deactivated
        */
        void HandleViewActivation(const TVwsViewId& aNewlyActivatedViewId,
                                  const TVwsViewId& aViewIdToBeDeactivated);
     
    public: // from MLogsModelObserver
        void StateChangedL( MLogsStateHolder* aHolder );
    
    protected:     
        
       /**
        * Sets the avkon provided input blocker
        */
        void SetInputBlockerL();
        
       /**
        * Removes the avkon provided input blocker
        */
        void RemoveInputBlocker();
               
        enum TLogsPbkFieldType  //Enum used to determine what kind of field to add to Phonebook
            {
            ELogsPbkNumberField,
            ELogsPbkVoipField,
            ELogsPbkPocField
            };

    public:       
// Safwish VoIP changes  >>>
        /**
         * Returns the SendUi command text.         
         *
         * @return SendUi command text.         
         */
        const TDesC& SendUiTextL();
// <<<  Safwish VoIP changes
        
        /**
         * Returns the Logs Application UI object.
         *
         * @return The Logs Application UI
         */
        CLogsAppUi* LogsAppUi();
   
        /**
         *  Returns the Logs application's model.
         *
         *  @return The current application model.
         */
        CLogsEngine* Engine();

        /**
         *  Creates a pop up menubar of aResourceId. Runs DoLaunchPopupL
         *  inside a TRAP.
         *
         *  @param aResourceId Resource ID.
         */
        void LaunchPopupMenuL( TInt aResourceId );
        

        /**
         * List current index getter
         *
         * @return current index
         */
        TInt EventListCurrent() const;

        /**
         * List top index getter
         *
         * @return top index
         */
        TInt EventListTop() const;

        /**
         * List current index setter
         *
         * @param aIndex current index
         */
        void SetEventListCurrent( TInt aIndex );

        /**
         * List top index setter
         *
         * @param aIndex top index
         */
        void SetEventListTop( TInt aIndex );
        
	    /**
         * Getter for recent model
         *
         * @return pointer to model
         */ 
        MLogsModel* CurrentModel();
        
        /**
          * Checks whether phone number and sip of the log event are equal
          * 
          * @return true/false of the above conditional
          * @param aEvent   
          */ 
        TBool PhoneNumberAndSipAreEqual(const MLogsEventGetter* aEvent);
        
	    /**
         * Checks whether phone number is available
         *
         * @param aEvent   
         */ 
        TBool PhoneNumberAvailable( const MLogsEventGetter* aEvent );

	    /**
         * Checks whether sip uri is available
         *
         * @param aEvent   
         */ 
        TBool SipUriAvailable( const MLogsEventGetter* aEvent );
        
        /**
         * Checks wether the options menu needs to be refreshed.
         */ 
        void HandleMenuRefreshL(const MLogsEventGetter* aCurrentEvent);
        
        /**
         *  Setter for iRefreshMenubarOnUpdate
         *
         *  @param aRefreshMenubar wether the options menu should be refreshed
         */
        void SetRefreshMenuOnUpdate(const MLogsEventGetter* aCurrentEvent );
        
        /**
         *  Getter for iRefreshMenubarOnUpdate
         *
         *  @return iRefreshMenubarOnUpdate 
         */
        TBool RefreshMenuOnUpdate(const MLogsEventGetter* aCurrentEvent);
        
       /**
        *  Change title pane text back to what it was. Base class has no implementation
        *  child classes override this.
        */
        virtual void ChangeTitlePaneTextToDefaultL();

    public: // From MAiwNotifyCallback
        TInt HandleNotifyL(
		        TInt aCmdId,
		        TInt aEventId,
		        CAiwGenericParamList& aEventParamList,
		        const CAiwGenericParamList& aInParamList);

    public: // from CAknView
        void ProcessCommandL( TInt aCommand );
        void HandleCommandL( TInt aCommandId );
        
        /**
         *  Process send key press when menu is open. For call commands, send key works as 
         *  if the menu option would have been selected. Default behaviour is not to consume the
         *  key which causes send key to open dialler. 
         *
         *  @param aType         type of key event
         *  @param aEvent        current event
         *  
         *  return ETrue\EFalse  wether the key event was consumed
         */
        TBool ProcessSendKeyEventInMenuL(TEventCode aType, const MLogsEventGetter* aEvent);
        
    protected: 
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage);
        void DoDeactivate();

        // for subclasses to handle commands needing LogEvent data
        void HandleCommandEventL( TInt aCommandId, const MLogsEventGetter* aEvent );

    public: // from MLogsKeyProcessor
        TBool ProcessKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        MAknTabObserver* TabObserver();
        
    public: // from MCoeForegroundObserver
		void HandleGainingForeground();
		void HandleLosingForeground();
		
	public: // from MPhCltEmergencyCallObserver
        /**
        * This is called whenever client's dial request is completed.
        *
        * @param aStatus error code of the success of the operation.
        */       
        void HandleEmergencyDialL( const TInt aStatus );
        
		/**
         * Default pointer event handler. Called when the current 
         * focused listbox item is tapped or after a double tap. 
         * Subclasses override this if needed.
         *
         * @param aIndex to the current listbox item  
         */ 
        virtual void ProcessPointerEventL( TInt aIndex);
        // void FocusChangedL();
        
    protected: // From MAknToolbarObserver
        void DynInitToolbarL(TInt aResourceId,
                CAknToolbar* aToolbar );
        void OfferToolbarEventL(TInt aCommand);
       
       /**
        * Wrapper for the leaving version of this function
        * 
        * @param aShowToolbar toolbar on/off.
        */
        void SetToolbarState(TLogsShowToolbar aShowToolbar, TBool aSetRect);
        
       /**
        * Called from recent lists, details and event list views to
        * switch toolbar on/off.
        * 
        * @param aShowToolbar toolbar on/off.
        */  
        void SetToolbarStateL(TLogsShowToolbar aShowToolbar, TBool aSetRect);
        
    protected:         
        void SetToolbarItems( const MLogsEventGetter* aEvent );
                
        //from MCoeControlObserver        
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);        
        
    protected: // for subclasses to handle key events needing LogEvent data
        TBool ProcessKeyEventEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType, 
                                     const MLogsEventGetter* aEvent );    

    public: // from MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
    protected: // for subclasses to call initing parts of menus needing LogEvent data
        void DynInitMenuPaneEventL( TInt aResourceId,CEikMenuPane* aMenuPane, const MLogsEventGetter* aEvent );

    protected: 
        // from MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
       
    protected: 
        TBool IsOkToShowContextMenu( const MLogsEventGetter* aEvent );
        
       /**
        * Update the view rect. Base class implementation empty.
        */
        virtual void HandleClientRectChange();
        
       /**
        * Draw the components. Base class implementation empty.
        */
        virtual void DrawComponents();
        
    private:  // functions
    
        //(NEW IMPLEMENTATION) To save some data to Phonebook
        
        /**
         * Save to phonebook function using AIW
         * 
         * @param  aCommandId
         * Command to execute
         *
         * @param  aCalledForMenu   
         * ETrue, if we are ececuting call based on Logs 
         * menu option. EFalse, if we want the CallUI itself to
         * display a query (pop-up menu) to user and then act 
         * accordingly.
         * 
         * @param aEvent 
         * The corresponding log event for the call. 
         * Provides access to the event data.                  
         */
        void CmdSaveToVirtualPhonebookL( TInt aCommandId, 
                                         TBool aCalledForMenu, 
                                         const MLogsEventGetter* aEvent );
    
        /**
         * Launch the popup.
         *
         * @param aResourceId Resource ID.
         */
        void DoLaunchPopupL( TInt aResourceId );
        
        /**
         * Checks if valid service settings for the service id 
         * are stored in the service table 
         *
         * @param aEvent the current logs event
         */
        CSPEntry* FindXSPServiceSettingsLC(const MLogsEventGetter* aEvent);
        
        
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


    protected:
        
        /**
         * Clears missed call notifications (Voice, poc...)
         */
        void ClearMissedCallNotifications();   //non-leaving wrapper
        void ClearMissedCallNotificationsL();        
        
        void DynInitAiwSaveToPbkMenuPaneL(TInt aResourceId, 
                                          CEikMenuPane* aMenuPane, 
                                          const MLogsEventGetter* aEvent,
                                          TBool aPhoneNumberAvailable,
                                          TBool aSipUriAvailable );

    private:          
        /**
         * Save AiW call params. Needed if the AiW call is later cancelled, see CancelSaveToPbkL.
         */
        void SaveAiwCallParamsL( TInt aCommandId,
                                 TBool  aCalledForMenu,
                                 CAiwGenericParamList*  aInParamList);
        /**
         * Clear the stored AiW call params when AiW call is finished or cancelled.
         */
        void ClearAiwCallParams();
        
        /**
         * Cancel the save to pbk command. Called for example when another Logs view is activated.
         */
        void CancelSaveToPbkL();
                
        /**
         * Initialisation of CallUI menupane (AIW) 
         * 
         * @param aResourceId   As in DynInitMenuPaneL
         * @param aMenuPane     As in DynInitMenuPaneL
         * @param aVideo,       ETrue: video call selectable, EFalse: voice call only
         */
        void DynInitAiwCallUiMenuPaneL( TInt aResourceId, 
                                        CEikMenuPane* aMenuPane, 
                                        TBool aVideo,
                                        const MLogsEventGetter* aEvent );
                                        
       /**
         * Handles runtime variation of __VIDEOCALL_MENU_VISIBILITY feature flag.
         * Disables or keeps the cascade feature from the menu pane. 
         * 
         * @param aMenuPane     As in DynInitMenuPaneL
         */                                
        void VideoCallMenuVisibilityL ( CEikMenuPane& aMenuPane ) const;

        /**
         * Initialisation of AIW menu contents to menupanes (PoC items to menus) 
         * 
         * @param aResourceId   As in DynInitMenuPaneL
         * @param aMenuPane     As in DynInitMenuPaneL
         * @param aEvent        Event data         
         */
        void DynInitAiwPocMenuPaneL( TInt aResourceId, 
                                     CEikMenuPane* aMenuPane, 
                                     const MLogsEventGetter* aEvent );
                                     
        /**
         * Send message 
         * 
         * @param aCommandId, sms or mms
         * @param aEvent
         */
        void SendMessageCmdHandlerL( TInt aCommandId, const MLogsEventGetter* aEvent  );

        /**
         * Performs Aiw menu command 
         * 
         * @param aCommandId     AIW provider's menu command id. 
         * @param aEvent         
         */
        void CmdAiwMenuCmdL( TInt aCommandId, const MLogsEventGetter* aEvent );
        
        /**
         * Performs Aiw service command 
         * 
         * @param aCommandId     Logs application's command id.
         * @param aEvent         
         */
        void CmdAiwSrvCmdL( TInt aCommandId, const MLogsEventGetter* aEvent );        

        /**
         * Prepares for a PoC call.
         * 
         * @param aCommandId 
         * The selected PoC command from PoC AIW menu (see: AiwPoCParameters.hrh). 
         * If the call is initiated with PTT-key, KErrNotFound can be used; the 
         * correct PoC call type is checked automatically from log event data.
         * (see: MLogsEventData::Type()).
         *
         * @param aCalledFromMenu 
         * ETrue when the PoC command is selected from PoC AIW menu. In case 
         * the call is made with PTT-key use EFalse and the AIW request is 
         * made as direct request.
         *
         * @param aEvent 
         * The corresponding log event for the call. 
         * Provides access to the event data.                  
         */
        void PreparePocCallL( TInt aCommandId, 
                              TBool aCalledForMenu,         
                              const MLogsEventGetter* aEvent );                  

        /**
         * Initialises TAiwPocParameterData
         */
        TAiwPocParameterData& InitPocParameterL( TAiwPocParameterData& aPocParameter, 
                                                 const MLogsEventGetter* aEvent );
            
        /**
         * Performs PoC call.
         * @param aCommandId 
         * @param aCalledFromMenu 
         * @param aParamList 
         */
        void CmdPocCallL( TInt aCommandId, 
                          TBool aCalledForMenu,
                          CAiwGenericParamList& aParamList );
            
        /**
         * Call function. Performs call using AIW
         * 
         * @param  aCommandId       Command to execute
         * 
         * @param  aCalledForMenu   
         * ETrue, if we are executing a call based on Logs 
         * menu option. EFalse, if we want the CallUI itself to
         * display a query (pop-up menu, e.g. when Send key pressed) 
         * to user and then act accordingly.
         * @param aEvent 
         * The corresponding log event for the call. 
         * Provides access to the event data.                  
         * @param aForceVideo
         * Forces video call for non-voip events
         */
        void PrepareCallL( TInt aCommandId, 
                           TBool aCalledForMenu,
                           const MLogsEventGetter* aEvent,
                           TBool aForceVideo = EFalse );                  

        /**
         * Call function. Performs call using AIW
         * 
         * @param  aCommandId
         * Command to execute
         *
         * @param  aCalledForMenu   
         * ETrue, if we are ececuting call based on Logs 
         * menu option. EFalse, if we want the CallUI itself to
         * display a query (pop-up menu) to user and then act 
         * accordingly.
         * 
         * @param  aNumberOrUrl 
         * MSISDN or SIP URI to make call to.
         * 
         * @param  aCallType 
         * Default calltype (voice, video or voip; however 
         * CallUI can change this).
         *
         * @param aEvent 
         * Used to retrieve the service id from the data field
         * of the log event.
         */
        void CmdCallL( TInt aCommandId, 
                       TBool aCalledForMenu, 
                       TPtrC aNumberOrUrl, 
                       CAiwDialData::TCallType aCallType,                        
                       const MLogsEventGetter* aEvent );

        /**
         * Attempts to initialise AiwGenericParamList for Poc menu and Poc call functions
         *
         * @param aEvent,                        In:  Event containing source data
         * @param aPocParameter,                 In:  PocParameter data 
         * @param aIsParamListForPocOk,          Out: Retuns ETrue if parameter can be used    
         * @param aOverrideUsingThis = KNullDesC In:  Use this number/uri preferred instead of what is in aEvent
         * @param aOverrideIsNumber = EFalse     In:  Type of string in aOverrideUsingThis
         *
         * @return CAiwGenericParamList& 
         */
        CAiwGenericParamList& GetAiwGenericParamListForPocL(
            const MLogsEventGetter* aEvent,             //In:  Event containing source data
            TAiwPocParameterData& aPocParameter,        //In:  PocParameter data 
            TBool& aIsParamListForPocOk,                //Out: Retuns ETrue if parameter can be used    
        	const TDesC& aOverrideUsingThis = KNullDesC,//In:  Use this number/uri preferred instead of what is in aEvent
        	TBool aOverrideIsNumber = EFalse );         //In:  Type of string in aOverrideUsingThis
                       
        /**
         * Copy number / address to clipboard.
         */
        void CmdCopyNumberL( const MLogsEventGetter* aEvent );  
        void CmdCopyAddressL( const MLogsEventGetter* aEvent );        

        /**
         * Show sip address
         */
        void CmdShowMyAddressL( const MLogsEventGetter* aEvent );
        
        /**
         * Use number / address
         *  @param    aPreferNumber: If there are both sip-uri and phone number available, prefer number 
         *                           instead of sip-uri
         */
        void CmdPrependCallL( TBool aPreferNumber, const MLogsEventGetter* aEvent );
	
        /**
         * Performs time consuming construction operations once. 
         */
        void DoConstructDelayedL();
        
         /**
         * Get the current menu type 
         *
         * @return  the menu type, CEikMenuBar::EMenuOptions or CEikMenuBar::EMenuContext
         */      
        CEikMenuBar::TMenuType CurrentMenuType() const;
        
         /**
         * Set the current menu type
         *
         * @param   aMenuType the current menu type
         */
        void SetCurrentMenuType(CEikMenuBar::TMenuType aMenuType);
        
    public:    
    	/**
    	 * Get whether the original CallMenu is visible
    	 *
    	 * @return EFalse\ETrue whether the original CallMenu is visible
    	 */  
        virtual TBool OriginalCallMenuIsVisible();
            
    private:    // data

        CEikMenuBar* iPopup;
        
        /// Ref: reference to singleton instance of CallUI and PoC provider owned by LogsAppUi
        CAiwServiceHandler* iServHandlerRef;
        
        CIdle*              iConstructDelayed;
        HBufC*              iNumberInPbkProcessing;
        HBufC*              iUriInPbkProcessing;    
        TBool               iConstructDelayedOk;   
        HBufC*              iSendUiText;
        HBufC*              iAddress;
        CLogsTimer*         iLogsTimer;

    protected:  // data

        enum TLogsFocusChangeControl  
            {
            ELogsOkToChange,             //Ok to change focus.
            ELogsNoChange,               //Do not change focus
            ELogsNoChange_PbkUpdPending, //Special case as resetting back to normal cannot be indicated reliably (no 
                                         //way of finding out when reading(re-reading of db is really finished. Works 
                                         //differently in emulator and hardware as order of needed notifications cannot 
                                         //be preempted, e.g. HandleGainingForeground may or may not be received before 
                                         //CLogsBaseView::HandleNotifyL is received. Also the reading of db because of the
                                         //above may be finished or interrupted depending on the timing of the above)
            };
        TLogsFocusChangeControl iFocusChangeControl;                  
        
        TLogsModel  iModel;
        TInt        iEventListCurrent; //Points to current event in normal Logs processing when Logs has not
                                       //lost foreground and hence has not lost currency in listboxes.
        TInt        iEventListTop;
        TBool       iRefreshMenuOnUpdate; 
        CPhCltEmergencyCall* iEmergencyCallAPI; 
        CEikMenuBar::TMenuType iMenuType;
        TInt iPreviousCommand;        
        TInt iEventUidWhenMenuOpened;
        CAknInputBlock* iInputBlocker;
        
        // AiW call params are needed for possible cancel of the save to pbk command
        TInt iAiwCommandId;
        TBool  iAiwCalledFromMenu;
        TBool  iIsCheckedCntLinkInvaild;
        CAiwGenericParamList*  iAiwInParamList;
        // ---------------------------------------------------------------------
        
        TLogsShowToolbar iToolBarState;
        
        MLogsModel::TDoActivate iResetAndRefreshOnHandleNotify;
        
        // EJZO-7RJB3V,When send AIW menu call command with contact link,
        //ignore the view deactivate and lost foreground operations
        TBool iIgnoreViewDeactivatedHandling;
        TBool iSemiFinishViewDeactivatedOperation;
        
    };

#endif  // __Logs_App_CLogsBaseView_H__

// End of File

