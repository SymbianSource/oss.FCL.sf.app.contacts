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
*     Observer for sim info
*
*/


#ifndef __MSimInfoObserver_H__
#define __MSimInfoObserver_H__

// FORWARD DECLARATIONS

// CLASS DECLARATION

class MSimInfoObserver
    {
    public: 
        virtual void SimInfoNotificationL() = 0;
    };

#endif // __MSimInfoObserver_H__

// End of File
