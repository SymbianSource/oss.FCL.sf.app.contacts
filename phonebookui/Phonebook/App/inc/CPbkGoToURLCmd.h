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
*      Go to URL command object.Opens browser to the URL address
*		saved in contact details.
*
*/


#ifndef __CPbkGoToURLCmd_H__
#define __CPbkGoToURLCmd_H__

// INCLUDES
#include <e32base.h>
#include <apparc.h>
#include <MPbkCommand.h>
#include <aknserverapp.h>

// FORWARD DECLARATIONS
class CPbkContactItem;
class TPbkContactItemField;
class CSchemeHandler;

// CLASS DECLARATION

/**
 * Call command object.
 */
class CPbkGoToURLCmd :
        public CBase,
		public MPbkCommand,
		public MAknServerAppExitObserver
    {
    public: // Interface
		/**
         * Static constructor.
         * @param aContact      contact to operate with
         * @param aFocusedField currently focused field. May be NULL if caller
         *                      doesn't have field-level focus
         * @return a new go to URL command object.
         */
		static CPbkGoToURLCmd* NewL(const CPbkContactItem& aContact,
			const TPbkContactItemField* aFocusedField);

        /**
         * Destructor.
         */
        ~CPbkGoToURLCmd();

    public:  // from MPbkCommand
        void ExecuteLD();

    private: //MAknServerAppExitObserver    
        void HandleServerAppExit(TInt aReason);

    private:  // Implementation
        CPbkGoToURLCmd(const CPbkContactItem& aContact,
			const TPbkContactItemField* aFocusedField);
        void DoGoToUrlL(const TDesC& aUrl);

    private:  // Data
		/// Ref: the contact to send message to
		const CPbkContactItem& iContact;
		/// Ref: the currently focused field
		const TPbkContactItemField* iFocusedField;
		/// Own: scheme handler for do the browser launch
		CSchemeHandler* iHandler;
        /// Ref: Referred boolean is set to ETrue when this object is destroyed.
        TBool* iDestroyedPtr;
   };

#endif // __CPbkGoToURLCmd_H__

// End of File
