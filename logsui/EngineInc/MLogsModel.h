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
*     Defines abstract interface for Logs model.
*
*/


#ifndef __MLogsModel_H_
#define __MLogsModel_H_

//  INCLUDES
#include <e32base.h>

#include "LogsEng.hrh"
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MLogsObserver;
class MLogsEventGetter;
class MLogsReaderConfig;
/**
 *	Defines abstract interface for Logs model
 */
class MLogsModel
    {
    public:
    
    enum TClearMissedCalls
            {
            ENormalOperation,
            ESkipClearing
            };

    enum TDoActivate
            {
            EActivateOnly,
            ERefresh,
            EResetAndRefresh
            };

    enum TDoDeactivate
        {
        EKeepDBConnection,
        ECloseDBConnection,
        ECloseDBConnectionAndResetArray,
        EResetOnlyArray,
        EResetOnlyArrayWithDirty 
        };
    
        virtual ~MLogsModel() {};

    public:

       /**
        *    Event deletion method
        *
        *    @param aIndex index array value of the event to be deleted
        */
        virtual void Delete( TInt aIndex ) = 0;

       /**
        *   Event count method.
        *
        *   @return count of events in this list.
        */
        virtual TInt Count() = 0;
    
       /**
        *   Event get method.
        *
        *   @param aIndex index array value.
        *   @return 
        */
        virtual const MLogsEventGetter* At( TInt aIndex ) = 0;

       /**
        *   Set UI observer for notifications
        * 
        *   @param aObserver pointer to observer.
        */
        virtual void SetObserver( MLogsObserver* aObserver ) = 0;

       /**
        *   Activate model.
        *   @param  aDoActivate  If event array is to be resetted or refreshed or model just activated.        
        */
        virtual void DoActivateL( TDoActivate aDoActivate ) = 0;

       /**
        *   Deactivate model.
        *   @param aClearMissedCalls For missed calls view the clearing of duplicates can be skipped
        *                            by setting this to ESkipClearing
        *   @param aDisconnectDb     Model disconnects from db if this is set to ETrue
        */
        virtual void DoDeactivate( TClearMissedCalls aClearMissedCalls,
                                   TDoDeactivate aDisconnectDb ) = 0;

       /**
        *   Reset model. Do not delete data from db.
        */
        virtual void Reset() = 0;
      
       /**
        *   Configure model
        *
        *   @param  aConfig  pointer to configure info   
        */
        virtual void ConfigureL( const MLogsReaderConfig* aConfig ) = 0;

        /**
        *   Start reading.
        *   @param  aReset  If event array is to be resetted.
        **/

        virtual void KickStartL( TBool aReset ) = 0;

    };

            
#endif

// End of File  __MLogsModel_H_
