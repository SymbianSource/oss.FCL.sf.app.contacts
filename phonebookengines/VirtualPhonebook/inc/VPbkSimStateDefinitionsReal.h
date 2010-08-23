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
* Description:  A P&S and CentRep UID/Flag variations
*
*/



#ifndef VPBKSIMSTATEDEFINITIONSREAL_H
#define VPBKSIMSTATEDEFINITIONSREAL_H

//  INCLUDES
#include <e32property.h>
#include <startupdomainpskeys.h>
#include <PSVariables.h>
#include <BTSapDomainPSKeys.h>

namespace VPbkSimStoreImpl
{
// MACROS
/// The P&S category for SIM status
#define KVPbkSimStatusPSCategory KPSUidStartup
/// The P&S category for BT Sap status
#define KVPbkBTSapStatusPSCategory KPSUidStartup

// CONSTANTS
/// The P&S key for SIM status
const TUint SimStatusPSKey = KPSSimStatus;
/// The value of the SimStatusPSKey key that indicates that sim is usable
enum TSimStatus
    {
    SimUninitializedStatus  = ESimStatusUninitialized,
    SimOkStatus             = ESimUsable
    };

/// The P&S key for SIM owned status
const TUint SimOwnedStatusPSKey = KPSSimOwned;
/// The value of the SimOwnedStatusPSKey that indicates that
/// current sim is not owned.
const TInt SimNotOwnedStatus = ESimNotOwned;

/// The P&S key for BT Sap status
const TUint BTSapStatusPSKey = KPSGlobalSystemState;
/// The value of the BTSapStatusPSKey key that indicates that bt sap is active
const TInt BTSapOkStatus = ESwStateNormalBTSap;

/// COMMENT: these are defined in Phonebook1&2) centrep sheet.
/// There is no own centrep sheet in VPbk for this purpose
/// because Phonebook1 needs the same flag anyway and
/// it's not wanted to keep the same flag in two centrep sheets.

/// Phonebook2 local variation UID
const TUid KLocalVaritationNameSpaceUid = { 0x101f8794 };
/// Phonebook2 local variation flags key
const TUint32 KLocalVariationFlagsKey = 0x00000002;
/// A local variation value
const TInt KEnableAdnWhenFdnOn = 0x00000400;

} // namespace VPbkSimStoreImpl

#endif      // VPBKSIMSTATEDEFINITIONSREAL_H

// End of File