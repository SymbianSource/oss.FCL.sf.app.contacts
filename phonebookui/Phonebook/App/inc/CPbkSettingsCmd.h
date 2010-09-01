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
*      Settings command object. Opens settings dialog.
*
*/


#ifndef __CPbkSettingsCmd_H__
#define __CPbkSettingsCmd_H__

// INCLUDES
#include <e32base.h>
#include <MPbkCommand.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class MObjectProvider;

// CLASS DECLARATION

/**
 * Settings command object.
 */
class CPbkSettingsCmd :
        public CBase,
        public MPbkCommand
    {
    public: // Interface
		/**
         * Static constructor.
		 * @param aEngine phonebook contact engine
         * @param aParent object provider parent
         * @return a new call command object.
         */
		static CPbkSettingsCmd* NewL (CPbkContactEngine& aEngine,
            MObjectProvider& aParent);

        /**
         * Destructor.
         */
        ~CPbkSettingsCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        
    private:  // Implementation
        CPbkSettingsCmd(CPbkContactEngine& aEngine, MObjectProvider& aParent);

    private:  // Data
		///Ref: phonebook contact engine
		CPbkContactEngine& iEngine;
        ///Ref: object provider parent
        MObjectProvider& iParent;
    };

#endif // __CPbkSettingsCmd_H__

// End of File
