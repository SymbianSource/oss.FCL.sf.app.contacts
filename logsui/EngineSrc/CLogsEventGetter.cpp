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
*     Implements interface for Logs event wrapper
*
*/


// INCLUDE FILES
#include "CLogsEventGetter.h"
#include "MLogsEvent.h"

// CONSTANTS

CLogsEventGetter* CLogsEventGetter::NewL( )
    {
    return new (ELeave) CLogsEventGetter();
    }

CLogsEventGetter::~CLogsEventGetter()
    {
    }

CLogsEventGetter::CLogsEventGetter( ) 
    {
    }

MLogsEvent* CLogsEventGetter::Event() const
    {
    return iWrappedObject;
    }

TInt8 CLogsEventGetter::Duplicates() const
    {
    return iWrappedObject->Duplicates();
    }

TLogsEventType CLogsEventGetter::EventType() const
    {
    return iWrappedObject->EventType();
    }

//TUid CLogsEventGetter::Uid() const
TUid CLogsEventGetter::EventUid() const
    {
    return iWrappedObject->EventUid();
    }

TLogsDirection CLogsEventGetter::Direction() const
    {
    return iWrappedObject->Direction();
    }

TTime CLogsEventGetter::Time() const
    {
    return iWrappedObject->Time();
    }

TLogId CLogsEventGetter::LogId() const
    {
    return iWrappedObject->LogId();
    }

HBufC* CLogsEventGetter::Number() const
    {
    return iWrappedObject->Number() ;
    }

HBufC* CLogsEventGetter::RemoteParty() const
    {
    return iWrappedObject->RemoteParty() ;
    }

const MLogsEventGetter* CLogsEventGetter::Wrap( MLogsEvent* aEvent )
    {
    iWrappedObject = aEvent;

    if( iWrappedObject )
        {
        return this;
        }
    else
        {
        return NULL;
        }
    }

MLogsEventData* CLogsEventGetter::LogsEventData() const
    {
    return iWrappedObject->LogsEventData();
    }

TBool CLogsEventGetter::ALS() const
    {
    return iWrappedObject->ALS();
    }

TBool CLogsEventGetter::CNAP() const
    {
    return iWrappedObject->CNAP();
    }
//For ring duation feature
TLogDuration CLogsEventGetter::RingDuration() const
    {
    return iWrappedObject->RingDuration();
    }

TInt CLogsEventGetter::NumberFieldType() const
    {
    return iWrappedObject->NumberFieldType();
    }

