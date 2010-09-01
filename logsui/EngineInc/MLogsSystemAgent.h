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
*     Defines abstract interface
*
*/


#ifndef __MLogsSystemAgent_H_
#define __MLogsSystemAgent_H_

//  INCLUDES

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsCallObserver;
class MLogsObserver;

// CLASS DECLARATION

/**
 *	
 */
class MLogsSystemAgent
    {
    public:
    
        virtual ~MLogsSystemAgent() {};

    public:

       /**
        *  Indicates current GPRS connection status
        *
        *  @return ETrue if active, EFalse if not
        */
        virtual TBool GprsConnectionActive() = 0;


       /**
        *  Indicates current voice, video, voip etc call status
        *
        *  @return KErrNone, if retrieving status was successful 
        */
        //TInt CallActive( TBool &aValue );  TERO


       /**
        *  Returns ALS support indication
        *
        *  @return ETrue if supported, EFalse if not
        */
        virtual TBool AlsSupport() = 0;

       /**
        *  Sets observer for Call Status changes.
        *
        */
        // virtual void SetCallObserver( MLogsCallObserver* aCallObserver ) = 0;

       /**
        *  Resets missed Poc call notification
        */
        virtual void ResetNewMissedPocCalls() = 0;

    };

            
#endif

// End of File  __MLogsSystemAgent_H_
