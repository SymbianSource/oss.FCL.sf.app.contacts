/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A contact editor strategy interface. 
*
*/



#ifndef MPBK2CONTACTEDITORSTRATEGY_H
#define MPBK2CONTACTEDITORSTRATEGY_H

// INCLUDES
#include "MPbk2ContactEditorEventObserver.h"

// FORWARD DECLARATIONS
class CPbk2PresentationContact;
class TCoeHelpContext;

// CLASS DECLARATION

/**
 *  A contact editor strategy interface.
 *
 */
class MPbk2ContactEditorStrategy
    {
    public:  // Constructors and destructor
        
        /**
         * Destructor.
         */
        virtual ~MPbk2ContactEditorStrategy() {}

    public: // Interface
        
        /**
         * Saves contact to the store.
         *
         * @param aObserver For notifying after save.
         * @param aParams   Parameters for the observer.
         */
		virtual void SaveContactL(
            MPbk2ContactEditorEventObserver& aObserver,
		    MPbk2ContactEditorEventObserver::TParams& aParams) = 0;
        
        /**
		 * Deletes the edited contact.
		 *
		 * @param aObserver For notifying after delete.
         * @param aParams   Parameters for the observer.
		 */
		virtual void DeleteContactL(
            MPbk2ContactEditorEventObserver& aObserver,
		    MPbk2ContactEditorEventObserver::TParams& aParams) = 0;
		
		/**
		 * Returns the default title of the editor.
         *
		 * @return The default title of the editor.
		 */
		virtual const TDesC& DefaultTitle() const = 0;
		
		/**
		 * Needed for changing the edited contact on fly.
		 *
		 * @param aContact  The new contact.
		 */
		virtual void SwitchContactL(
            CPbk2PresentationContact* aContact) = 0;

		/**
		 * Get current editor help context
		 *
		 * @param aContext  current editor help context
		 */
        virtual void GetHelpContext(TCoeHelpContext &aContext) const = 0;

        /**
         * Force the query dialog to terminate as getting choice of 
         * right softkey
         *
         * @return True if a query is stopped.
         */
        virtual TBool StopQuery() = 0;
    };

#endif // MPBK2CONTACTEDITORSTRATEGY_H
            
// End of File
