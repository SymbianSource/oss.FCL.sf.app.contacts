/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 Nameslist UI control UI extension.
*
*/


#ifndef MPbk2EcePresenceEngineObserver_H
#define MPbk2EcePresenceEngineObserver_H

//  INCLUDES
#include <e32base.h>

#include <TPbk2IconId.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
// CLASS DECLARATION

/**
 * 
 */
class MPbk2EcePresenceEngineObserver
    {
    public:

        /**
         * Informs observer that the presence information of a contact has changed.
         */
        virtual void ContactPresenceChanged( const MVPbkContactLink& aLink ) = 0;

    };

#endif // MPbk2EcePresenceEngineObserver_H
           
// End of File
