/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook SIM remote view settings.
*
*/


#ifndef VPBKSIMSTORE_REMOTEVIEWPREFERENCES_H
#define VPBKSIMSTORE_REMOTEVIEWPREFERENCES_H


// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CVPbkContactViewDefinition;

namespace VPbkSimStore {

// CONSTANTS

/**
 * Virtual Phonebook SIM all contacts view remote view name.
 */
_LIT( KVPbkAllContactsViewName, "AllContacts" );

// FUNCTIONS

/**
 * @return ETrue if aViewDefinition is a remote view definition.
 */
TBool RemoteViewDefinition( 
        const CVPbkContactViewDefinition& aViewDefinition );

/**
 * @return aViewDefinition.Name() if it's not empty, otherwise
 *         the default name of the remove view.
 */
const TDesC& RemoteViewName( 
        const CVPbkContactViewDefinition& aViewDefinition );

}  // namespace VPbkSimStore

#endif  // VPBKSIMSTORE_REMOTEVIEWPREFERENCES_H

//End of file
