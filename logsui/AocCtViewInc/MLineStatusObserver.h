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
*     Observer for line status.
*
*/


#ifndef __MLineStatusObserver_H__
#define __MLineStatusObserver_H__

// FORWARD DECLARATIONS

// CLASS DECLARATION

class MLineStatusObserver
    {
    public: 
        virtual void LineStatusChangedL() = 0;
    };

#endif // __MLineStatusObserver_H__

// End of File
