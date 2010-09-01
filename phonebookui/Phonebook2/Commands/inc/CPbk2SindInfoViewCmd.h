/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 SIND InfoView AIW interest item.
*
*/


#ifndef CPBK2SINDINFOVIEWCMD_H
#define CPBK2SINDINFOVIEWCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 SIND InfoView command object.
 */
NONSHARABLE_CLASS(CPbk2SindInfoViewCmd) :
        public CBase,
        public MPbk2Command
    {
    public: // Constructor and destructor

		/**
         * Creates a new instance of this class.
         *      
         * @param aCommandId command id
         * @param aServiceHandler service handler
         * @return a new instance of this class.
         */
		static CPbk2SindInfoViewCmd* NewL(
            TInt aCommandId,
            CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        ~CPbk2SindInfoViewCmd();

    public:  // From MPbk2Command
        void ExecuteLD();
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );
        void AddObserver(
                MPbk2CommandObserver& aObserver );
   
    public:
	    /**
	    * Sets packed contact link array
	    *
	    * @param aPackedLinkArray Packed contact link array for AIW transfer.
	    *        Array consists of the link for the contact whose SIND info
	    *        view is to be shown. Takes ownership of aPackedLinkArray.
	    */    
        void SetPackedLinkArray( TDesC8* aPackedLinkArray );

    private:  // Implementation
        CPbk2SindInfoViewCmd(            
            TInt aCommandId,
            CAiwServiceHandler& aServiceHandler );

    private:  // Data       
        /// Own: command id
        TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Own: Packed link
        TDesC8* iPackedLinkArray;
    };

#endif // CPBK2SINDINFOVIEWCMD_H

// End of File
