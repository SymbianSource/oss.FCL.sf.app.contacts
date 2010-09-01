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
*     Call Timers view
*
*/


#ifndef     __Logs_App_CCtView_H__
#define     __Logs_App_CCtView_H__


//  INCLUDES
#include "CLogsBaseView.h" 
#include "LogsAlsEnum.hrh"

//  FORWARD DECLARATIONS
class CCtControlContainer;

//  CLASS DECLARATIONS

/**
 *  Call Timers view.
 */
class   CCtView :   public CLogsBaseView
    {
    private: 
        /**
         *  C++ Constructor
         */
        CCtView();
        /**
         *  Constructor
         */
        void ConstructL();


    public:  // interface
        /**
         *  Creates the new view
         *
         *  @return The new view object
         */
        static CCtView* NewL();

        /**
         *  Destructor
         */
        ~CCtView();

        /**
         *  Constructs and makes visible the Ok Option list
         */
        void OkOptionCtMenuL();

        /**
         * Selectec als line
         * @return als line
         */
        const TAlsEnum& AlsLine() const;
        
        /**
         * Set ALS line 
         */
        void SetAlsLine( TAlsEnum aTimerAls );
        

    public: // from MLogsModelObserver
        void StateChangedL( MLogsStateHolder* aHolder );

    public: // from MLogsKeyProcessor 

		/**
         * Overrides the baseclass handler
         * 
         * @param aIndex	The index of the item tapped
         */
        void ProcessPointerEventL( TInt aIndex);
    
    public: // from MEikCommandObserver
        /**
         *  Handles view-specific commands.
         *
         *  @param  aCommand    Command to be handled
         */
        void HandleCommandL( TInt aCommand );

    public:  // from CAknView
        /**
         *  Framework calls to init menu items.
         *
         *  @param aResourceId   Resource read.
         *  @param aMenuPane     Menu pane.
         */ 
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
    private:  // from CAknView
        /**
         *  Return the view's Id.
         *
         *  @return ECtViewId
         */
        TUid Id() const;

        /**
         *  Called by the framework when the view rectangle changes
         */
        void HandleClientRectChange();

        /**
         *  Called by the framework when the view is activated.
         *
         *  @param aPrevViewId  Previous view's id
         *  @param aCustonMessageId Custom Message id
         *  @param aCustomMessage   Custom message
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        /**
         *  Called by the framework when the view is deactivated.
         */
        void DoDeactivate();

    private:  // operations

        /**
         * Set als filter.
         * @return ETrue if als was changed, else return EFalse
         */
        TBool CmdSetAlsFilterL();

        /**
         * Add line to array
         * @param aItems array
         * @param resource text
         */
        void MakeOneLineL(  CDesCArrayFlat& aItems,
                            TInt aResourceText );


    private:  // data
        /// view's control container
        CCtControlContainer* iContainer;

        /// Own: Timers' ALS filter
        TAlsEnum iTimersAls;

    };

#endif  // __Logs_App_CCtView_H__

// End of File
