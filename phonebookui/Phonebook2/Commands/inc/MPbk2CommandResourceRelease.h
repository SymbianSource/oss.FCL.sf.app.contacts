/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A command extension interface for release resource.
*
*/

#ifndef MPBK2COMMANDRESOURCERELEASE_H_
#define MPBK2COMMANDRESOURCERELEASE_H_

/// Phonebook 2 Command MPbk2ResourceRelease interface UID
#define MPbk2ResourceReleaseUID              0xE9D61489

// CLASS DECLARATIONS

/**
 * Phonebook 2 command release resource interface.
 */
class MPbk2ResourceRelease
    {
    public:
        /**
         * Release resource.
         */
        virtual void ReleaseResource() = 0;
     
    };

#endif /* MPBK2COMMANDRESOURCERELEASE_H_ */
