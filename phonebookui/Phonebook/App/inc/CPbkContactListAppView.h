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
*     Base class for Contact List app views.
*
*/


#ifndef __CPbkContactListAppView_H__
#define __CPbkContactListAppView_H__

//  INCLUDES
#include "CPbkAppView.h"    // CPbkAppView
#include <MPbkAiwCommandObserver.h>
#include <TPbkSendingParams.h>

// FORWARD DECLARATIONS
class CPbkContactViewListControl;

// CLASS DECLARATION

/**
 *  Base class for Contact List app views.
 */
class CPbkContactListAppView :public CPbkAppView,
        public MPbkAiwCommandObserver
    {
    protected:  // Constructors and destructor
        /**
         * Default constructor.
         */
        CPbkContactListAppView();

        /**
         * Destructor.
         */
        ~CPbkContactListAppView();

        /**
         * Base class constructor.
         */
        void BaseConstructL(TInt aResId);

    protected: // Base class interface
        /**
         * Returns this view's UI control.
         */
        virtual CPbkContactViewListControl& Control() = 0;

    protected:  // Common command handlers shared by derived classes
        /**
         * Command handler: Goes into memory entry views of the focuced entry.
         */
        virtual void CmdOpenMeViewsL();

        /**
         * Command handler: Write SMS, MMS or email message to contact.
         * @param aParams, sending params
         */
        virtual void CmdWriteToContactsL(
            TPbkSendingParams aParams );

        /**
         * Command handler : send contact data via wanted media         
         * @param aParams, sending params
         */
        virtual void CmdSendContactsL(
            TPbkSendingParams aParams);

        /**
         * Common deletion focused item handler.
         */
        virtual void DeleteFocusedEntryL()=0;

        /**
         * Command handler: Belongs to groups.
         */
        virtual void CmdBelongsToGroupsL();

    protected: // shared methods
        /**
         * Handle send contacts functionality
         * @param aFlags
         */
        void HandleSendContactsL( TUint aFlags );
        
        /**
         * Create sending params
         * Push mtmfilter in CleanupStack       
         * @return Sending params
         */        
        TPbkSendingParams CreateWriteParamsLC();
        
        /**
         * Create sending params
         * Push mtmfilter in CleanupStack
         * @param aFlags, options menu flag
         * @return Sending params
         */        
        TPbkSendingParams CreateSendParamsLC(TUint aFlags);
        
    protected: // from MPbkAiwCommandObserver
        virtual void AiwCommandHandledL(TInt aMenuCommandId,
            TInt aServiceCommandId,
            TInt aErrorCode);

    protected:  // from CPbkAppView
        void HandleCommandL(TInt aCommandId);
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    protected:  // from MEikMenuObserver
	    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    };

#endif // __CPbkContactListAppView_H__
            
// End of File
