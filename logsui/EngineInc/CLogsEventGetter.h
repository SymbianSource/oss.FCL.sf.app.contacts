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
*     Implements wrapper for Logs event object
*
*/


#ifndef __Logs_Engine_CLogsEventGetter_H__
#define __Logs_Engine_CLogsEventGetter_H__

//  INCLUDES
#include <e32base.h>
#include "MLogsEventWrapper.h"

// FORWARD DECLARATION

// CLASS DECLARATION

/**
 *  Implements interface for Logs event getter
 */
class CLogsEventGetter :    public CBase,
                            public MLogsEventWrapper
    {

    public:
       /**
        * Two-phased constructor.
        *
        * @return CLogsEventGetter object
        */
        static CLogsEventGetter* NewL();
        
       /**
        * Destructor.
        */
        virtual ~CLogsEventGetter();

    private:
      /**
        * C++ constructor.
        *
        */
        CLogsEventGetter();

    public: // from MLogsEventGetter
    
        MLogsEvent* Event() const;
        
        TInt8 Duplicates() const;

        TLogsEventType EventType() const;

        TUint8 FieldId() const;

        TUid EventUid() const;

        TLogsDirection Direction() const;

        TTime Time() const;

        TLogId LogId() const;

        HBufC*  Number() const;

        HBufC* RemoteParty() const;

        MLogsEventData* LogsEventData() const;

        TBool ALS() const;

        TBool CNAP() const;
//For ring duation feature
        TLogDuration RingDuration() const;
        
        TInt NumberFieldType() const;

    public: // from MLogsEventWrapper

        //MLogsEventGetter* Wrap( MLogsEvent* aEvent );
        const MLogsEventGetter* Wrap( MLogsEvent* aEvent );
             
    private:    // data

        /// Ref: object to wrapper.
        MLogsEvent* iWrappedObject;

    };
#endif      // __Logs_Engine_CLogsEventGetter_H__



// End of File
