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
* Description:  Phonebook 2 Group Feature ECom plug-in interface inline impl.
*
*/


/// Unused namespace for local definitions
namespace {

// Group feature plugin implementation
const TUid KGroupFeatureImplementationUid = { 0x102830CF };

}  /// namespace


// --------------------------------------------------------------------------
// CPbk2GroupFeaturePlugin::~CPbk2GroupFeaturePlugin
// --------------------------------------------------------------------------
//
inline CPbk2GroupFeaturePlugin::~CPbk2GroupFeaturePlugin()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

// --------------------------------------------------------------------------
// CPbk2GroupFeaturePlugin::NewL
// --------------------------------------------------------------------------
//
inline CPbk2GroupFeaturePlugin* CPbk2GroupFeaturePlugin::NewL()
    {
    TAny* ptr = REComSession::CreateImplementationL
        ( KGroupFeatureImplementationUid,
          _FOFF( CPbk2GroupFeaturePlugin, iDtorIDKey ) );
    return REINTERPRET_CAST( CPbk2GroupFeaturePlugin*, ptr );
    }

// End of File
