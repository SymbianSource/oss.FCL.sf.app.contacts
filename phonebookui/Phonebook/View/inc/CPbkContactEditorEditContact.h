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
*     Phonebook contact editor edit contact strategy.
*
*/


#ifndef __CPbkContactEditorEditContact_H__
#define __CPbkContactEditorEditContact_H__

//  INCLUDES
#include "MPbkContactEditorStrategy.h"
#include <e32base.h>
#include <babitflags.h>     // TBitFlags

// FORWARD DECLARATIONS
class CPbkFFSCheck;
class CPbkContactEngine;
class CPbkContactItem;

// CLASS DECLARATION

/**
 * Phonebook Contact editor new contact strategy. 
 */
NONSHARABLE_CLASS(CPbkContactEditorEditContact) :
		public CBase, 
		public MPbkContactEditorStrategy
    {
	public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
		 * @param aEngine reference to phonebook engine
		 * @param aContactItem contact item reference
         */
		static CPbkContactEditorEditContact* NewL(
				CPbkContactEngine& aEngine,
				CPbkContactItem& aContactItem);
        /**
         * Destructor.
         */
		~CPbkContactEditorEditContact();

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
		CPbkContactEditorEditContact(
				CPbkContactEngine& aEngine,
				CPbkContactItem& aContactItem);
		void ConstructL();
		
	private: // data members
		/// Ref: contact engine
		CPbkContactEngine& iEngine;
		/// Ref: Contact item
		CPbkContactItem& iContactItem;
        /// Own: FFS checker
        CPbkFFSCheck* iFFSCheck;
        /// Own: state flags
        mutable TBitFlags iStateFlags;
    };

#endif // __CPbkContactEditorEditContact_H__
            
// End of File
