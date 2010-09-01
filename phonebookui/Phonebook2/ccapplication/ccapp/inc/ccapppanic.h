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
* Description:  Defines CCApp panic codes
*
*/


#ifndef _CCAPPPANIC_H
#define _CCAPPPANIC_H

// panic codes
enum TCCAppPanic
    {
    ECCAppPanicNullPointer = 1,
    ECCAppPanicObjectCountMismatch = 2
    };

inline void CCAppPanic( TInt aReason )
    {
    _LIT( applicationName,"CCApp" );
    User::Panic( applicationName, aReason );
    }

#endif// _CCAPPPANIC_H

// End of File
