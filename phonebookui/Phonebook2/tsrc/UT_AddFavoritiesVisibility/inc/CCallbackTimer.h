/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UT_AddFavoritiesVisibility class member functions
*
*/


#ifndef  CCALLBACKTIMER_H
#define  CCALLBACKTIMER_H

#include <e32base.h> 

/*
* Implements:
*
* Given a 100 mSec, it stops the AS loop owned by 
* CActiveSchedulerWait obj if CActiveSchedulerWait is started
*
*/
NONSHARABLE_CLASS(CCallbackTimer) : public CTimer
    {
    public: 
            static CCallbackTimer* NewL();
            ~CCallbackTimer();
            void StartWaitingTillCallbackOrTimeOutL();
            void CallbackReceived();
    private:

            void RunL();
            CCallbackTimer();
            void ConstructL();
    private:
    
           CActiveSchedulerWait iWait;        
    
	};

#endif /* CCALLBACKTIMER_H */
