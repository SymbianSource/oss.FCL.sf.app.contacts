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
*    Abstract interface for observing extensions.
*
*/


#ifndef __MLogsExtObserver_H
#define __MLogsExtObserver_H


//  INCLUDES

//  FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * @internal This interface is internal to Logs.
 *
 * Abstract interface for observing extensions.
 */
class MLogsExtObserver
    {
    public: // Interface
        
        /**
         * Extension state change callback
         */
        virtual void ExtStateChangedL() = 0;                    
    };

#endif
            

// End of File
