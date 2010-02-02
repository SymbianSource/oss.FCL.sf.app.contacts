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
* Description:  ECom Predictive search interface definition
*
*/


// INCLUDE FILES
#include <ecom/ECom.h>

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CPcsPlugin::NewL
// Two phase construction
// ----------------------------------------------------------------------------
inline CPcsPlugin* CPcsPlugin::NewL( TUid aImpUid )
{
        
	CPcsPlugin* self = CPcsPlugin::NewLC( aImpUid );	
	CleanupStack::Pop();
		
	return self;
}

// ----------------------------------------------------------------------------
// CPcsPlugin::NewLC
// Two phase construction
// ----------------------------------------------------------------------------
inline CPcsPlugin* CPcsPlugin::NewLC( TUid aImpUid )
{
        
	TAny* implementation = REComSession::CreateImplementationL ( aImpUid,
	    _FOFF ( CPcsPlugin, iDtor_ID_Key ) );
	CPcsPlugin* self = REINTERPRET_CAST( CPcsPlugin*, implementation );
	self->iPluginId = aImpUid;
	CleanupStack::PushL( self );
	
	return self;
}

// ----------------------------------------------------------------------------
// CPcsPlugin::ConstructL
// Destructor
// ----------------------------------------------------------------------------
inline CPcsPlugin::~CPcsPlugin()
{
    REComSession::DestroyedImplementation ( iDtor_ID_Key );

}

// ----------------------------------------------------------------------------
// CPcsPlugin::PluginId
// Returns the plugin id
// ----------------------------------------------------------------------------
inline TUid CPcsPlugin::PluginId() const
{  
    return iPluginId;
}
 
