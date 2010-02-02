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
* Description:  The virtual phonebook Cntmodel remote view settings.
*
*/


#ifndef VPBKCNTMODELREMOTEVIEWPREFERENCES_H
#define VPBKCNTMODELREMOTEVIEWPREFERENCES_H


// INCLUDES
#include <cntviewbase.h>

// FORWARD DECLARATIONS
class CVPbkContactViewDefinition;

namespace VPbkCntModel {

// CONSTANTS

/**
 * Virtual Phonebook Cntmodel all contacts view remote view name.
 */
_LIT( KVPbkAllContactsViewName, "AllContacts" );

/**
 * Virtual Phonebook Cntmodel all contacts view preferences. 
 */
const TContactViewPreferences KVPbkDefaultContactViewPrefs =
    static_cast<TContactViewPreferences>
        ( EContactsOnly | EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField );

/**
 * Virtual Phonebook Cntmodel all groups view remote view name.
 */
_LIT(KVPbkAllGroupsViewName, "AllGroups");

/**
 * Virtual Phonebook Cntmodel all groups view preferences.
 */
const TContactViewPreferences KVPbkDefaultGroupsViewPrefs =
    static_cast<TContactViewPreferences>
        (EGroupsOnly | EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField);

// FUNCTIONS

/**
 * @return ETrue if aViewDefinition is a remote view definition.
 */
TBool RemoteViewDefinition( 
        const CVPbkContactViewDefinition& aViewDefinition );

/**
 * @return aViewDefinition.Name() if it's not empty, otherise
 *         the default name of the remove view.
 */
const TDesC& RemoteViewName( 
        const CVPbkContactViewDefinition& aViewDefinition );

}  // namespace VPbkCntModel

#endif  // VPBKCNTMODELREMOTEVIEWPREFERENCES_H

//End of file
