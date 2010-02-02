/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for timer.
*
*/


#ifndef CLOGSTIMER_H
#define CLOGSTIMER_H

//  INCLUDES
#include <e32base.h>
#include "MLogsTimer.h"

// CLASS DECLARATION

/**
*  Utility class for timer 
*
* @since 3.1
*/
class CLogsTimer : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aPriority priority of this active object.
        */
        static CLogsTimer* NewL( 
            TInt aPriority = CActive::EPriorityStandard );

        /**
        * Destructor.
        */
        virtual ~CLogsTimer();

    public: 
        
        /**
        * The Callback function is called after the interval.
        * All former request will be canceled first.
        *
        * @param anInterval interval.
        * @param aCallBack callback to be called after 
        *                  interval.
        */
        void After( 
            TTimeIntervalMicroSeconds32 anInterval,
            TCallBack aCallBack );

        /**
        * Use the mixin class to notify the timer.
        * All former request will be canceled first.
        *
        * @param anInterval interval.
        * @param aObserver callback to be called after
        *                  interval.
        * @param  aPtr pointer to additional parameter object.
        */
        void After( 
            TTimeIntervalMicroSeconds32 anInterval, 
            MLogsTimer* aObserver,
            TAny* aPtr );

    protected:  // Functions from base classes
        
        /**
        * From CActive::RunL()
        */
        virtual void RunL();

    private:
        
        /**
        * C++ constructor.
        */
        CLogsTimer( TInt aPriority );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private:    // Data

        // Timer callback.
        TCallBack iCallBack;

        // Timer observer.
        MLogsTimer* iTimerObserver;
        TAny* iTimerObserverParam;

    };

#endif      // CLOGSTIMER_H 
            
// End of File
