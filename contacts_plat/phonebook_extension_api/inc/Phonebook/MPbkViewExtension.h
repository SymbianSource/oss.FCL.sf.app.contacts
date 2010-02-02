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
*    Abstract interface for making phone book view extensions.
*
*/


#ifndef __MPbkViewExtension_H__
#define __MPbkViewExtension_H__


//  INCLUDES
#include <MPbkUiExtensionBase.h>
#include <MPbkMenuCommandObserver.h>


//  FORWARD DECLARATIONS
class CEikMenuPane;
class MPbkContactUiControl;
class MPbkMenuCommandObserver;


// CLASS DECLARATION

/**
 * Abstract interface for phone book view extensions.
 */
class MPbkViewExtension : public MPbkUiExtensionBase
    {
    public: // Interface
        /**
         * Filters the menu pane of the Phonebook view connect to this 
         * extension.
         *
         * @param aResourceId menu resource id.
         * @param aMenuPane menu pane which will be filtered.
         */
        virtual void DynInitMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane) = 0;

        /**
         * Handles phone book extension commands.
         *
         * @param aCommandId command id.
         * @return ETrue if command was handled.
         */
        virtual TBool HandleCommandL(TInt aCommandId) = 0;

        /**
         * Handles phone book extension commands.
         *
         * @param aCommandId command id.
         * @param aObserver command execution observer
         * @return ETrue if command was handled.
         */
        virtual TBool HandleCommandL
            (TInt aCommandId, MPbkMenuCommandObserver& aObserver);
        
        /**
         * Sets ui contact control for this view extension. 
         *
         * @param aContactControl for requesting data from the 
         *                        contact control; if NULL, association 
         *                        is removed.
         * @see MPbkContactUiControl
         */
        virtual void SetContactUiControl
            (MPbkContactUiControl* aContactControl) = 0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MPbkViewExtension() { }
    };

inline TBool MPbkViewExtension::HandleCommandL
        (TInt aCommandId, 
        MPbkMenuCommandObserver& aObserver)
    {
    aObserver.PreCommandExecutionL();
    TBool result = HandleCommandL(aCommandId);
    TPbkCommandStatus status = 
		(result ? EPbkCommandExecuted : EPbkCommandNotHandled);
    aObserver.PostCommandExecutionL(status);
    return result;
    }

#endif // __MPbkViewExtension_H__

// End of File
