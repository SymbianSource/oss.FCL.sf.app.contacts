/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook Cntmodel remote view settings.
*
*/


// INCLUDES
#include "VPbkCntModelRemoteViewPreferences.h"

#include <CVPbkContactViewDefinition.h>

namespace VPbkCntModel {

// FUNCTIONS

TBool RemoteViewDefinition( 
        const CVPbkContactViewDefinition& aViewDefinition )
    {
    TVPbkContactViewSharing sharing = aViewDefinition.Sharing();
    return ( sharing == EVPbkViewSharingUndefined ||
             sharing == EVPbkSharedView );
    }

const TDesC& RemoteViewName( 
        const CVPbkContactViewDefinition& aViewDefinition )
    {
    if ( aViewDefinition.Name().Length() > 0 )
        {
        return aViewDefinition.Name();
        }
    else if ( aViewDefinition.Type() == EVPbkGroupsView )
        {
        return KVPbkAllGroupsViewName;
        }
    else
        {
        return KVPbkAllContactsViewName;
        }
    }
}  // namespace VPbkCntModel

//End of file
