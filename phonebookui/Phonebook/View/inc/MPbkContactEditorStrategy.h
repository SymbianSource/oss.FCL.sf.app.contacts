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
*     Phonebook contact editor contact strategy interface. 
*
*/


#ifndef __MPbkContactEditorStrategy_H__
#define __MPbkContactEditorStrategy_H__

//  INCLUDES
#include <e32std.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class TCoeHelpContext;
class CEikMenuPane;

// CLASS DECLARATION

/**
 * Phonebook Contact editor contact strategy interface. 
 */
class MPbkContactEditorStrategy
    {
	public: // interface
		/**
		 * Destructor.
		 */
		virtual ~MPbkContactEditorStrategy() = 0;

		/** 
		 * Save contact to database.
		 * @return Contact id if saving was succesful, KNullContactId if not.
		 */
		virtual TContactItemId SaveContactL() = 0;

		/** 
		 * Set help context to aContext.
		 */
		virtual void GetHelpContext(TCoeHelpContext& aContext) const = 0;
		
		/**
		 * Sets contact state to modified.
		 */
		virtual void SetStateModified() = 0;

        /**
         * Editor has been initialized.
         */
        virtual void SetStateInitialized() = 0;

		/**
		 * Delete the edited contact item.
		 */
		virtual void DeleteContactL() = 0;

		/**
		 * returns the default title of the editor.
		 */
		virtual const TDesC& DefaultTitle() const = 0;

        /**
         * initialize menu pane
         */
        virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane) = 0;
    
    };

inline MPbkContactEditorStrategy::~MPbkContactEditorStrategy()
    {
    }

#endif // __MPbkContactEditorStrategy_H__
            
// End of File
