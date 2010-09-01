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
* Description:  Implementation of the class Pbk2RemoteContactLookupFactory.
*
*/


#include <pbk2remotecontactlookupfactory.h>

#include "pbkxrclutils.h"
#include "cpbkxrclsettingitem.h"
#include "cpbkxrclserviceuicontextimpl.h"
#include "cpbkxrclprotocolenvimpl.h"
#include "cpbkxremotecontactlookupprotocoladapter.h"


// ======== MEMBER FUNCTIONS ========


EXPORT_C CPbkxRemoteContactLookupServiceUiContext* Pbk2RemoteContactLookupFactory::NewContextL( 
    CPbkxRemoteContactLookupServiceUiContext::TContextParams& aParams )
    {
    return CPbkxRclServiceUiContextImpl::NewL( aParams.iProtocolAccountId, aParams.iMode );
    }



EXPORT_C CAknSettingItem* Pbk2RemoteContactLookupFactory::NewDefaultProtocolAccountSelectorSettingItemL()
    {
    return CPbkxRclSettingItem::NewL();
    }

// end of file
