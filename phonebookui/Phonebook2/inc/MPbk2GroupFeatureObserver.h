/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 group feature observer interface.
*
*/


#ifndef MPBK2GROUPFEATUREOBSERVER_H
#define MPBK2GROUPFEATUREOBSERVER_H

// INLCUDES
#include <bamdesca.h>

// CLASS DECLARATION

/**
 * Phonebook 2 group feature observer interface.
 */
class MPbk2GroupFeatureObserver
    {
    public: // Interface

        /**
         * Returns groups where contact belongs to.
         * @see CPguGroupFeaturePlugin::JoinedGroups()
         * Array is quaranteed to be valid during this method call, but not
         * after it. If you need to store the content for later use,
         * make a copy of it!
         *
         * @param aGroups list of formatted group names
         */
        virtual void JoinedGroupsCallbackL(
                const MDesCArray& aGroups ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2GroupFeatureObserver() {}
    };

#endif // MPBK2GROUPFEATUREOBSERVER_H

// End of File
