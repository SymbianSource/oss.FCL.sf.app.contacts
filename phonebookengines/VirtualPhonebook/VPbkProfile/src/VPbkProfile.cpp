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
* Description:  VirtualPhonebook profiling support.
*
*/


#include <vpbkprofile.h>
#include <e32debug.h>

namespace VPbkProfile {

// --------------------------------------------------------------------------
// TVPbkProfiling::GlobalL
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkProfiling* TVPbkProfiling::GlobalL()
    {
    if ( !Dll::Tls() )
        {
        TVPbkProfiling* profiling = new(ELeave)TVPbkProfiling;
        Dll::SetTls( profiling );
        }
    return reinterpret_cast<TVPbkProfiling*>( Dll::Tls() );
    }

// --------------------------------------------------------------------------
// TVPbkProfiling::~TVPbkProfiling
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkProfiling::~TVPbkProfiling()
    {
    }

// --------------------------------------------------------------------------
// TVPbkProfiling::StartProfile
// --------------------------------------------------------------------------
//
EXPORT_C void TVPbkProfiling::StartProfile( TInt aBin )
    {
    TTime startTime;
    startTime.HomeTime();
    iProfileArray[aBin] = startTime;
    }

// --------------------------------------------------------------------------
// TVPbkProfiling::EndProfile
// --------------------------------------------------------------------------
//
EXPORT_C void TVPbkProfiling::EndProfile( TInt aBin ) const
    {
    TTime endTime;
    endTime.HomeTime();
    RDebug::Print( _L( "VPbk Profile;%d; Time;%d;\n" ),
        aBin, endTime.MicroSecondsFrom( iProfileArray[aBin] ) );
    }

// --------------------------------------------------------------------------
// TVPbkProfiling::TVPbkProfiling
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkProfiling::TVPbkProfiling()
    {
    }

}  // namespace VPbkProfile

// End of File
