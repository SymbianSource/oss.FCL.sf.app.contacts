/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Call Vmbx for softnotification.
*
*/





#ifndef SPDIACALLINGVMBXVIEW_H
#define SPDIACALLINGVMBXVIEW_H

// INCLUDES
#include<RSSSettings.h>  
#include "SpdiaBaseView.h"
#include <aiwdialdataext.h>
#include <AiwServiceHandler.h>      //For Application InterWorking

// FORWARD DECLARATIONS
class CSpdiaAppUi;
class CSpdiaCallingVmbxContainer;
class CSpdVmbxAsync;

const TInt KCustomMessageSize(30);
// CLASS DECLARATION
/**
*  Speed dial view class. Provides a call to vmbx 
*  view functionality. 
*/
class CSpdiaCallingVmbxView : public CSpdiaBaseView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor.
        */
        static CSpdiaCallingVmbxView* NewLC();

        /**
        * Destructor.
        */
        ~CSpdiaCallingVmbxView();

    public: // Functions from base classes

        /**
        * Returns this view's id.
        */
        TUid Id() const;

        /**
        * From phoneclient
        */
        void HandleDialL( const TInt /*iStatus*/ );

        /**
        * Default constructor.
        */
        void ConstructL();

    public: // New functions

        /**
        * Performs the call operation.
        * @param aNumber  call number
        */
        void NumberCallL( const TDesC& aNumber );
        void NumberCallL( const TDesC& aNumber, 
                          CAiwDialData::TCallType aCallType );		
		/**
        * Exits speed dial Application..
        */
		void ExitSpdApp();
		/**
        * Displays VMBX text quary dialog.
        */   
		void DispalyVmbxDialogL() ;                      
		/**
        * set the AO exit flag;
        */ 
		void SetSureToExit( TBool aValue );
		/**
        * get the AO exit flag;
        */ 	
		TBool GetSureToExit();
    private:

        /**
        * Default constructor.
        */
        CSpdiaCallingVmbxView();

        /**
        * Called by the view framework when this view is activated
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid /*aCustomMessageId*/,
                          const TDesC8& aCustomMessage );

        /**
        * Called by the view framework when this view is deactivated.
        */
        void DoDeactivate();

        /**
        * From MEikMenuObserver, foreground event handler
        */
        void HandleForegroundEventL( TBool /*aForeground*/ );

		 // Internal functions for ALS-support
        TBool DoWeCallToDifferentLineL();
        TBool IsLineBlocked(RSSSettings& aSettings);
        TBool ToggleLine(RSSSettings& aSettings);


    private:   // Data
        CSpdiaAppUi*                    iAppUi;
        CSpdiaCallingVmbxContainer*     iContainer;
		CAiwServiceHandler*             iServiceHandler;    /// Own: AIW
		TVwsViewId						iPrevViewId;
		TUid 							iCustomMessageId;
		TBuf<KCustomMessageSize> 		iCustomMessage;
		CSpdVmbxAsync* 					iSpdVmbxAsync; 
        TBool                           iVoIPSupported;
        TBool                           iSureToExit;
    };

/**
*  Asynchronously call to vmbx 
*/
class CSpdVmbxAsync : public CActive
   {
     public: 
       /**
       * Symbian OS two-phased constructor.
       */
       static CSpdVmbxAsync* NewL(CSpdiaCallingVmbxView* aSpdiaCallingVmbxView);
       
       /**
        * Destructor.
        */
       ~CSpdVmbxAsync();

	   /**
       * Sets Active objects.
       */
       void SetVmbxActiveL();
       
       private:
       /**
       * Default constructor.
       */      
       CSpdVmbxAsync(CSpdiaCallingVmbxView* aSpdiaCallingVmbxView);

	   /**
       * Default constructor.
       */
       void ConstructL();
       
       /**
       * Cancels any outstanding requests.
       */
       void DoCancel();
	    
	   /**
       * Respond to an Asynchronous event.
       */
       void RunL();
       
       private: // Data        
       CSpdiaCallingVmbxView* iSpdiaCallingVmbxView; //Speed dial view class
            
    };
#endif        // SPDIACALLINGVMBXVIEW_H

// end of file
