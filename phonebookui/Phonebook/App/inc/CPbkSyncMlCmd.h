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
*      SyncML command object.
*
*/


#ifndef __CPbkSyncMlCmd_H__
#define __CPbkSyncMlCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>
#include <CPbkAddressSelect.h>
#include <AiwDialDataTypes.h>
#include <AiwPoCParameters.h>


// FORWARD DECLARATIONS
class CPbkContactItem;
class TPbkContactItemField;
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * SyncMl command object.
 */
class CPbkSyncMlCmd :
        public CBase,
        public MPbkCommand,
        private MPbkProcessObserver
    {
    public: // Interface
		/**
         * Static constructor.         
         * @param aCommandId command id
         * @param aServiceHandler service handler
         * @return a new call command object.
         */
		static CPbkSyncMlCmd* NewL (
            TInt aCommandId,
            CAiwServiceHandler& aServiceHandler);

        /**
         * Destructor.
         */
        ~CPbkSyncMlCmd();

    public:  // from MPbkCommand
        void ExecuteLD();

    public:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private:  // Implementation
        CPbkSyncMlCmd (            
            TInt aCommandId,
            CAiwServiceHandler& aServiceHandler);

    private:  // Data       
        /// Own: command id
        TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;        
    };

#endif // __CPbkSyncMlCmd_H__

// End of File
