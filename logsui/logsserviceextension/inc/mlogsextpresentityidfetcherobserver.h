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
* Description:  Observer for a presentity id fetcher.
*
*/



#ifndef M_MLOGSEXTPRESENTITYIDFETCHEROBSERVER_H
#define M_MLOGSEXTPRESENTITYIDFETCHEROBSERVER_H


#include "logwrap.h"

/**
 * Callback for the fetcher objects. Implemented by CLogsUiControlExtension
 *
 * @lib logsserviceextension.dll
 * @since S60 v3.2
 */
class MLogsExtPresentityIdFetcherObserver
    {
    public: 
        
        /**
         * Handles the presentity fetcher's callback.
         *
         * @since S60 v3.2
         * @param aServiceId a service id
         * @param aPresentityId the presentity id that was retrieved 
         *        by the fetcher
         */                                             
        virtual void PresentityIdFetchDoneL( const TUint32 aServiceId, 
                                             const TDesC& aPresentityId,
                                             const TLogId aLogEventId ) = 0;

    };

#endif // M_MLOGSEXTPRESENTITYIDFETCHEROBSERVER_H
