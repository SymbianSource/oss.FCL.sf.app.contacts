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
* Description:  Factory class for CCA Client
 *
*/


#ifndef T_CCAFACTORY_H
#define T_CCAFACTORY_H

#include <e32std.h>
#include <e32base.h>

class MCCAConnection;
class MCCAParameter;

/** @file ccafactory.h
 *  Factory class for creating CCA Client API objects.
 *
 *  Example usage of the whole API: 
 *  @code
 *   
 *   API objects creation with the factory class
 *   MCCAParameter* aParameter = TCCAFactory::NewParameterL()
 *   MCCAConnection* aConnection = TCCAFactory::NewConnectionL()
 *   
 *   Setting up the parameter
 *	 @see MCCAParameter
 *   aParameter->SetConnectionFlag(ENormal);
 *   aParameter->SetContactDataFlag(EContactId);
 *   aParameter->SetContactDataL(aString);
 *   aParameter->SetLaunchedViewUid(aUid);
 *
 *	 Launching the  CCA application.
 *   @see MCCAConnection
 *   aConnection = TCCAFactory::NewConnectionL()
 *   aConnection->LaunchApplicationL(*aParameter);
 *
 *   Clean up. 
 *   aParameter->Close();
 *   aConnection->Close();
 *
 *  @endcode
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class TCCAFactory
    {

public:

    /**
     * Create new CCA client parameter entity.
     * Ownership is transferred. 
     * @since S60 5.0
     * @return MCCAParameter, ownership is transferred.
     */
    IMPORT_C static MCCAParameter* NewParameterL( );

    /**
     * Create new CCA client connection entity.
     * Ownership is transferred. 
     * @since S60 5.0
     * @return MCCAConnection, ownership is transferred.
     */
    IMPORT_C static MCCAConnection* NewConnectionL( );

    };

#endif  // T_CCAFACTORY_H
// End of File
