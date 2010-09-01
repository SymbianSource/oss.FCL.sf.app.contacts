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
*     Defines abstract interface for Logs model observation
*
*/


#ifndef __MLogsCallObserver_H_
#define __MLogsCallObserver_H_

// INCLUDES

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *	Defines abstract interface for Logs model observation
 */
class MLogsCallObserver
    {
    public:

       /**
        *   Call state observation interface method. 
        */
        virtual void CallStateChangedL( TInt aCallState ) = 0;       
    };

            
#endif

// End of File  __MLogsCallObserver_H_
