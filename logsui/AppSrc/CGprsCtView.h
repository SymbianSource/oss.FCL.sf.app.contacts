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
*     GPRS counters view
*
*/


#ifndef     __Logs_App_CGprsCtView_H__
#define     __Logs_App_CGprsCtView_H__


//  INCLUDES
#include "CLogsBaseView.h"  

//  FORWARD DECLARATIONS
class CGprsCtControlContainer;
class CAknView;
class MLogsAocUtil;


//  CLASS DECLARATIONS

/**
 *  GPRS counters view
 */
class   CGprsCtView :   public CLogsBaseView                    
    {
    public:  // interface
        /**
         *  Creates the new view
         *
         *  @return The new view object
         */
        static CGprsCtView* NewL();

        /**
         *  Destructor
         */
        ~CGprsCtView();

    private:
        /**
         *  2nd phase of construction
         */
        void ConstructL();

        /**
         *  Constructs and makes visible the Ok Option list
         */
        void OkOptionCtMenuL();


    public: // MLogsObserver
        void StateChangedL( MLogsStateHolder* aHolder );

    public: // from MLogsKeyProcessor 

 		/**
         * Overrides the baseclass handler
         * 
         * @param aIndex	The index of the item tapped
         */
        void ProcessPointerEventL( TInt aIndex);   

    public: // from MEikCommandObserver        
        void HandleCommandL( TInt aCommand );

        
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

    

    private:  // data
        /// Own: view's control container
        CGprsCtControlContainer* iContainer;

        /// Own: for security query
        MLogsAocUtil* iAocUtil;
    };

#endif  // __Logs_App_CGprsCtView_H__

// End of File
