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
* Description:  Phonebook 2 application server.
*
*/


#ifndef CPBK2SERVERAPP_H
#define CPBK2SERVERAPP_H

//  INCLUDES
#include <AknServerApp.h>

// CLASS DECLARATION

class CPbk2ServerApp : public CAknAppServer
    {   
    public:
        /**
         * Creates a new instance of this class.
         *
         * @return A new instance of this class.
         */
        static CPbk2ServerApp* NewL();
        
    private: // From CAknAppServer
        CApaAppServiceBase* CreateServiceL(
                TUid aServiceType ) const;
        CPolicyServer::TCustomResult CreateServiceSecurityCheckL(
                TUid aServiceType,
                const RMessage2& aMsg,
                TInt& aAction,
                TSecurityInfo& aMissing );
        void HandleAllClientsClosed();
    };

#endif // CPBK2SERVERAPP_H
            
// End of File
