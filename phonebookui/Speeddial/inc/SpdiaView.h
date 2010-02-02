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
* Description:     Declares view for application.
*
*/






#ifndef SPDIALVIEW_H
#define SPDIALVIEW_H

// INCLUDES
#include <aknview.h>

        #include <sendui.h>



#include <aiwdialdataext.h>


#include <AiwServiceHandler.h>      //For Application InterWorking

#include "SpdiaBaseView.h"

// FORWARD DECLARATIONS
class CEikImage;
class CSpdiaContainer;


// CLASS DECLARATION

/**
*  Speed dial Application view class. Provides some common application 
*  view functionality. 
* 
*  @since 
*/
class CSpdiaView : public CSpdiaBaseView,public MAiwNotifyCallback
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return GS connection view.
        */
        static CSpdiaView* NewLC(CSpeedDialPrivate* aSpeedPrivate);

        /**
        * Destructor.
        */
        virtual ~CSpdiaView();

    public: // New functions
        /**
        * The CBA is set to menu from resourceID
        * @param aResourceID
        */
        void LoadCba(TInt aResourceID);

        /**
        * Performs the call operation on EspdiaCmdNumberCall event.
        * @param aNumber        A phone number.
        * @param aNumberType    Type of the number.
        * @param aVmbxNumber    Boolean indicating whether this is vmbx number.
        */
        void NumberCallL( const TDesC& aNumber, TInt& aNumberType, 
            TBool aVmbxNumber = EFalse );
        
        /* Set iSpeedPrivate
        * Only invoked by CSpdiaAppUi::~CSpdiaAppUi()
        * @param aSpeedPrivate
        */
        void SetSpeedPrivate(CSpeedDialPrivate* aSpeedPrivate);
        
          
        /**
        * Sets the cba buttons for middle softkey
        * @param Resource id
        */
        void SetCba(TInt aResourceId);


    public: // Functions from base classes
        /**
        * Returns this view's id.
        */
        TUid Id() const;

        /**
        * Processes Speed Dial View's commands.
        */ 
        void HandleCommandL(TInt aCommand);

        /**
        * Called by the view framework when the client rectangle of this 
        * view changes.
        */
        void HandleClientRectChange();
        
        /** 
        * Event handler for status pane size changes.
        */
        void HandleStatusPaneSizeChange();

        /**
        * Command handler: Assigns number to, or changes number in a dial 
        * grid entry.
        */
        TBool CmdAssignNumberL();

        /**
        * Default constructor.
        */
        void ConstructL(CSpeedDialPrivate* aSpeedPrivate);

    private:

        /**
        * Called by the view framework when this view is activated
        */
        void DoActivateL(const TVwsViewId& aPrevViewId, 
                TUid aCustomMessageId, const TDesC8& aCustomMessage);

        /**
        * Called by the view framework when this view is deactivated.
        */
        void DoDeactivate();

        /**
        * From MEikMenuObserver, foreground event handler
        */
        void HandleForegroundEventL(TBool aForeground);

        /** 
        * From MEikMenuObserver, Delete meaningless menu item.  
        * @param aResourceId : resource ID of menu pane
        * @param aMenuPane : Pointer of menu pane object
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

        /**
         * Initialisation of CallUI menupane (AIW) 
         * 
         * @param aResourceId   As in DynInitMenuPaneL
         * @param aMenuPane     As in DynInitMenuPaneL
         * @param aVideo,       ETrue: video call selectable, EFalse: voice call only
         */
        void DynInitAiwMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane, TBool aVideo );



   public:  // Command handlers
        /**
        *  Command handler: Performs call using AIW
        *  @param  aCommandId Command to execute
        *  @param  aShowQuery False, if we are ececuting call based on Logs 
        *                     menu option. True, if we want the CallUI itself to
        *                     display a query to user and then act accordingly.
        */
        void CmdNumberCallL( TInt aCommandId, TBool aShowQuery );
        
       /**
        *  Command handler: Performs call using AIW
        *  @return  ETrue if is lanuched from general settings
        */
        TBool IsSettingType( );
        
        /**
         *  If msg dialog is showng.
         *  @return  ETrue if msg dialog is showng.
         */
        TBool IsShowMsgDlg();
        
        /**
         *  If Vmbx dialog is showng.
         *  @return  ETrue if msg dialog is showng.
         */
        TBool IsShowVmbxDlg();
        
        /* Get the iAssignCompletedFlag 
        * @return iAssignCompletedFlag 
        */
        TBool IsAssignCompleted();
        
        /* Check whether there has been a focus when clicking MSK 
        * @return ETrue for focus exists, or EFalse 
        */
        TBool IsFocused();
   private:
        /**
        * pop up the enable speeddial dialog.
        *
        * @return  the keyid which key pressed. 
        */
        TInt PopUpEnableSpDiaDlgL( );

        /**
        * Call function. Performs call using AIW
        *  @param  aCommandId Command to execute
        *  @param  aShowQuery False, if we are ececuting call based on Logs 
        *                     menu option. True, if we want the CallUI itself to
        *                     display a query to user and then act accordingly.
        *  @param  aNumber    Number to make call to
        *  @param  aCallType  Default calltype (However CallUI can change this) 
        */
        void CmdNumberCallL ( TInt aCommandId,
                              TBool aShowQuery, 
                              TPtrC aNumber, 
							  CAiwDialData::TCallType aCallType);		

        /**
        * Command handler: Release
        */
        void CmdReleaseNumberL();

        /**
        * Command handler: ViewNumber
        */
        void CmdViewNumberL();


    	void CmdSendMessageL();

    private: // functions
        /**
        * Undo layout and user leave.
        */
        void UndoLayoutAndLeaveL(TInt aError, TBool aUpdateSp = ETrue);

        /**
        * Make the container to be invisible.
        *
        * @param aContainer CSpdiaContainer object
        * @return FALSE
        */
        static TInt MakeInvisibleCallBack(TAny *aContainer);
  	    /*
  		  * MAiwNotifyCallback APIs
  		  */
        TInt HandleNotifyL(
 		        TInt aCmdId,
 		        TInt aEventId,
 		        CAiwGenericParamList& aEventParamList,
 		        const CAiwGenericParamList& aInParamList);
         /**
         * ProcessCommandL
         */
         void ProcessCommandL(TInt aCommand);
    private: // Data
        CSpdiaContainer* iContainer;            // owned by this
        CSpeedDialPrivate* iSpeedPrivate;		//owned by application
        TInt iCbaID;
        CAiwServiceHandler* iServiceHandler;    /// Own: AIW
        TBool iClientRectChanged;
		/// Own: AIW
		TBool iSkinChange;// for build ver 2.6
		TBool iReleasenumberFlag;

		CSendUi* iSendUi;

		TBool iAssignNumberFlag;
		TInt vmbxsupported;
		TBool iIsShowMsgDlg;
		TBool iIsShowVmbxDlg;
		TBool iAIWRunFlag;
		TBool iAssignCompletedFlag;
    };

#endif        // SPDIALVIEW_H

// End of File
