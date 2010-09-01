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
*     Recent list reader
*
*/


#ifndef __Logs_Engine_CLogsRecentReader_H__
#define __Logs_Engine_CLogsRecentReader_H__

//  INCLUDES
#include "CLogsBaseReader.h"

// FORWARD DECLARATION

// CLASS DECLARATION
class CLogsClearDuplicates;
class CLogFilter;
class CLogViewRecent;
class CLogViewDuplicate;
class MLogsEventArray;

/**
 *  Recent list reader
 */
class CLogsRecentReader : public CLogsBaseReader
    {
    public:
        /**
         *  Two phase constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aEventArray  array of events
         *  @param aStrings     log client constants
         *  @param aModel       model id
         *  @param aObserver    observer
         */
        static CLogsRecentReader* NewL
                                (   RFs& aFsSession
                                ,   MLogsEventArray& aEventArray
                                ,   TLogsEventStrings& aStrings
                                ,   TLogsModel aModel
                                ,   MLogsObserver* aObserver
                                ,   CLogsEngine* aLogsEngineRef
                                );

       /**
        *   Destructor.
        */
        virtual ~CLogsRecentReader();

    private:
       /**
        *  Default constructor
        */
        CLogsRecentReader();

        /**
         *  Constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aEventArray  array of events
         *  @param aStrings     log client constants
         *  @param aModel       model id
         *  @param aObserver    observer
         */
        CLogsRecentReader(  RFs& aFsSession
                         ,  MLogsEventArray& aEventArray
                         ,  TLogsEventStrings& aStrings
                         ,  TLogsModel aModel
                         ,  MLogsObserver* aObserver
                         ,  CLogsEngine* aLogsEngineRef
                         );
                                    
        /**
         *  Second phase constructor
         */
        void ConstructL();

    private:  // from CLogsBaseReader 
        TInt ViewCountL() const;
        CLogEvent& Event() const;
        TBool DoNextL();        
        TInt DuplicateCountL() const;
        void ReadingFinishedL();
        void ConstructEventL( MLogsEvent& aDest, const CLogEvent& aSource );

    protected: // From CActive
        void DoCancel();

    public: // from MLogsReader
        void Stop();
        void StartL();
        void ConfigureL( const MLogsReaderConfig* aConfig );
        void ClearDuplicatesL();
        void ActivateL();
        void DeActivate();


    private: // data
        /// Own: View to filtered events.
        CLogViewRecent*         iLogViewRecent;

        /// Own: Recent duplicate view    
        CLogViewDuplicate*      iDuplicateView;

        /// Own: duplicate filter, excludes all "seen" events
        CLogFilter*             iDuplicateFilter;

        /// Own: clear duplicates flag for better performance
        TBool                   iDuplicates;

        /// Own:
        CLogsClearDuplicates*   iClearDuplicates;
        // TBool                   iSkipClearDuplicates;
    };

#endif      // __Logs_Engine_CLogsRecentReader_H__

// End of File
