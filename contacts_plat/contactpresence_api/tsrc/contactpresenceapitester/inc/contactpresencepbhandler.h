/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __CONTACTPRESENCEPBHANDLER_H__
#define __CONTACTPRESENCEPBHANDLER_H__

// INCLUDE FILES
#include <e32base.h>
#include "contactpresencetesterdef.h"

class CContactPresenceApiTester;
class CContactPresencePhonebook;

class CContactPresencePbHandler : public CActive
    {
    public:

        static CContactPresencePbHandler* NewL( CContactPresenceApiTester& aTesterMain,
                                                CContactPresencePhonebook& aPhonebook );
        
        void Activate( TPbHandlerState aHandlerState );

        ~CContactPresencePbHandler();

    private:

        CContactPresencePbHandler( CContactPresenceApiTester& aTesterMain,
                                   CContactPresencePhonebook& aPhonebook );

        void ConstructL();
        
        void RunL();

        void DoCancel();
        
    private:
           
        TPbHandlerState                        iHandlerState;
        CContactPresenceApiTester&             iTesterMain;
        CContactPresencePhonebook&             iPhonebook;        
    };
    
#endif
