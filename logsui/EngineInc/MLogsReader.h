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
*     Defines abstract interface for Logs reader
*
*/


#ifndef __MLogsReader_H_
#define __MLogsReader_H_

// INCLUDES
#include "LogsEng.hrh"
#include <CVPbkPhoneNumberMatchStrategy.h>
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsObserver;
class MLogsReaderConfig;

// CLASS DECLARATION

/**
 *  Defines abstract interface for Logs reader and contact updater
 */
class MLogsReader
    {
    public:
    
        virtual ~MLogsReader() {};


    public:
        enum TPreferredPbkStore
            {
            EPbkAndSim,
            EPbkOnly,
            ESimOnly,
            // ELdapOnly, 
            EAllStores
            };

    public:

       /**
        *   Stop reader
        */
        virtual void Stop() = 0;

       /**
        *   Interrupt reader
        */
        virtual void Interrupt() = 0;

       /**
        *   Is reader interrupted
        *
        *   @return ETrue if interrupted, otherwise EFalse
        */
        virtual TBool IsInterrupted() const = 0;

       /**
        *   Observer setter
        *
        *   @param  aObserver   pointer to observer
        */
        virtual void SetObserver( MLogsObserver* aObserver ) = 0;

       /**
        *   Start reader
        */
        virtual void StartL() = 0;

       /**
        *   Start contact updater
        */
        virtual void StartUpdaterL() {};

       /**
        *   Continue reading
        */
        virtual void ContinueL() = 0;


       /**
        *   Configure reader
        *
        *   @param  aConfig  pointer to configure info   
        */
        virtual void ConfigureL( const MLogsReaderConfig* aConfig ) = 0;

       /**
        *   Reader dirty flag getter
        *
        *   @return  ETrue if dirty, otherwise EFalse   
        */
        virtual TBool IsDirty() const = 0;

        
       /**
        *   Set the Dirty flag to ETrue  
        */
        virtual void SetDirty() = 0;
        
       /**
        *   Set reader activated 
        */
        virtual void ActivateL() = 0;
        
       /**
        *   Set reader deactivated 
        */
        //virtual void DeActivate( TBool aSkipClearDuplicates ) = 0;
        virtual void DeActivate() = 0;

       /**
        *   Clear duplicates - for use of Missed Calls model only. 
        */
        virtual void ClearDuplicatesL() {};
    };

            
#endif

// End of File  __MLogsReader_H_
