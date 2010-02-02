/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCH Handler Observer.
*
*/


#ifndef M_MLOGSEXTENSIONCCHHANDLEROBSERVER_H
#define M_MLOGSEXTENSIONCCHHANDLEROBSERVER_H

// INCLUDES
#include <e32base.h>
#include <cchtypes.h>

// CLASS DECLARATION

/**
 *  Logs Extension CCH Handler Observer
 *  This class defines CCH observing methods
 *
 *  @lib logsserviceextension.lib
 *  @since S60 3.2
 */
class MLogsExtensionCchHandlerObserver
    {
    public:
    
        /**
         * Informs observers that cch event has occured.
         *
         * @since S60 3.2
         * @param aServiceId, service id related to event
         * @param aState, service state
         * @param aServiceError, service error
         */    
        virtual void CchEventOccuredL( 
            TUint aServiceId, 
            TCCHSubserviceState aState, 
            TInt aServiceError ) = 0;
    };

#endif // M_MLOGSEXTENSIONCCHHANDLEROBSERVER_H

// End of file
