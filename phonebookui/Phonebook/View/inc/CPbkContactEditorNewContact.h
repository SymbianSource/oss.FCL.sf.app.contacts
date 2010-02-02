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
*     Phonebook contact editor new contact strategy.
*
*/


#ifndef __CPbkContactEditorNewContact_H__
#define __CPbkContactEditorNewContact_H__

//  INCLUDES
#include "MPbkContactEditorStrategy.h"
#include <babitflags.h>     // TBitFlags

// FORWARD DECLARATIONS
class CPbkFFSCheck;
class CPbkContactEngine;
class CPbkContactItem;

// CLASS DECLARATION

/**
 * Phonebook Contact editor new contact strategy. 
 */
NONSHARABLE_CLASS(CPbkContactEditorNewContact) :
        public CBase,
		public MPbkContactEditorStrategy
    {
	public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
		 * @param aEngine reference to phonebook engine
		 * @param aContactItem reference to the contact item
         */
		static CPbkContactEditorNewContact* NewL(
                CPbkContactEngine& aEngine,
		        CPbkContactItem& aContactItem);
		
        /**
         * Destructor.
         */
        ~CPbkContactEditorNewContact();

	public: // from MPbkContactEditorStrategy
		TContactItemId SaveContactL();
		void GetHelpContext(TCoeHelpContext& aContext) const;
		void SetStateModified();
        void SetStateInitialized();
		void DeleteContactL();
		TBool Saved();
		const TDesC& DefaultTitle() const;
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	private: // implementation
		CPbkContactEditorNewContact(
                CPbkContactEngine& aEngine,
        		CPbkContactItem& iContactItem);
		void ConstructL();

	private: // data members
		/// Ref: contact engine
		CPbkContactEngine& iEngine;
		/// Ref: Contact item
		CPbkContactItem& iContactItem;
        /// Own: FFS checker
        CPbkFFSCheck* iFFSCheck;
		/// Own: Default title
		HBufC* iDefaultTitle;
        /// Own: state flags
        mutable TBitFlags iStateFlags;
		/// Own: the contact id of the new contact
		TContactItemId iContactId;
    };

#endif // __CPbkContactEditorNewContact_H__
            
// End of File
