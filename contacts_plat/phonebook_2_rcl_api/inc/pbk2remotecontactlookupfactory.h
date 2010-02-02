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
* Description:
*
*/

#ifndef PBK2REMOTECONTACTLOOKUPFACTORY_H
#define PBK2REMOTECONTACTLOOKUPFACTORY_H

//  INCLUDES
#include <e32base.h>

#include <tpbkxremotecontactlookupprotocolaccountid.h>
#include <cpbkxremotecontactlookupserviceuicontext.h>


// FORWARD DECLARATIONS
class CAknSettingItem;
class CPbkxRemoteContactLookupProtocolAccount;

// CLASS DECLARATION

/**
 * A factory for creating objects for Remote Contact Lookup.
 */
class Pbk2RemoteContactLookupFactory
    {
    public:

    /**
     * Creates a new context which is used for searching remote contacts.
     * Loads a protocol plugin.
     *
     * @return The new context. Ownership is given.
     */
    IMPORT_C static CPbkxRemoteContactLookupServiceUiContext* NewContextL( 
        CPbkxRemoteContactLookupServiceUiContext::TContextParams& aParams );


    /**
     * Creates new Avkon UI setting item for selecting protocol account.
     */    
    IMPORT_C static CAknSettingItem* NewDefaultProtocolAccountSelectorSettingItemL();

    };

#endif // PBK2REMOTECONTACTLOOKUPFACTORY_H

// End of File
