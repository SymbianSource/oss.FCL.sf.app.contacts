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
* Description:  Encapsulates log event information
*
*/



// INCLUDE FILES
#include "tlogseventinfo.h"
#include "logsextconsts.h"
#include "simpledebug.h"

// ----------------------------------------------------------------------------
// TLogsEventInfo::TLogsEventInfo
// ----------------------------------------------------------------------------
//
TLogsEventInfo::TLogsEventInfo() : 
    iServiceId( 0 ),
    iBrandingIconIndex( KInitialIndexValue )
    {
    }


// ----------------------------------------------------------------------------
// TLogsEventInfo::TLogsEventInfo
// ----------------------------------------------------------------------------
//
TLogsEventInfo::TLogsEventInfo( TUint32 aServiceId ) : 
    iServiceId( aServiceId ),
    iBrandingIconIndex( KInitialIndexValue )
    {
    }


// ----------------------------------------------------------------------------
// TLogsEventInfo::SetBrandingIconIndex
// ----------------------------------------------------------------------------
//
void TLogsEventInfo::SetBrandingIconIndex( TInt aBrandingIconIndex )
    {
    _LOGP("TLogsEventInfo::SetBrandingIconIndex =%d", 
        aBrandingIconIndex )
    iBrandingIconIndex = aBrandingIconIndex;
    _LOG("TLogsEventInfo::SetBrandingIconIndex end" )
    }

// ----------------------------------------------------------------------------
// TLogsEventInfo::ServiceId
// ----------------------------------------------------------------------------
//
TUint32 TLogsEventInfo::ServiceId()
    {
    _LOGP("TLogsEventInfo::ServiceId() serviceid=%d", iServiceId )
    return iServiceId;
    }


// ----------------------------------------------------------------------------
// TLogsEventInfo::BrandingIconIndex
// ----------------------------------------------------------------------------
//
TInt TLogsEventInfo::BrandingIconIndex()
    {
    _LOGP("TLogsEventInfo::BrandingIconIndex() =%d" , 
        iBrandingIconIndex )
    return iBrandingIconIndex;
    }

