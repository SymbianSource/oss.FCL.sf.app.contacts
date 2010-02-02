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
* Description:  Launches Phonebook2 with New Contact view.
*
*/

#ifndef PBK2LAUNCHNEWCONTACT_H
#define PBK2LAUNCHNEWCONTACT_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Class to launch Phonebook2 with New Contact view.
 *
 * Client application needs to link against Pbk2CommonUtility.lib
 */
class Pbk2LaunchNewContact
    {
    public: // Interface
          /**
           * Launches Phonebook2 in New Contact view.
           * Pressing Back from New Contact view would come to Phonebook2
           * NamesList view. 
           */
                IMPORT_C static void LaunchNewContactL();
    };

#endif // PBK2LAUNCHNEWCONTACT_H

// End of File
