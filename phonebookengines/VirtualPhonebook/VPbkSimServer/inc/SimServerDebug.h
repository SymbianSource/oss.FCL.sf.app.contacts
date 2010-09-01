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
* Description:  A header for debug helpers
*
*/



#ifndef VPBKSIMSERVER_SIMSERVERDEBUG_H
#define VPBKSIMSERVER_SIMSERVERDEBUG_H

//  INCLUDES
#include <e32std.h>
#include "../../inc/VPbkConfig.hrh"

#if defined(_DEBUG) || defined(VPBK_ENABLE_DEBUG_PRINT)

namespace VPbkSimServer {

/**
 * Returns the message id text for the opCode
 */
const TDesC& MessageInTextFormat( TInt aOpCode );

} // namespace
#endif // _DEBUG
#endif      // VPBKSIMSERVER_SIMSERVERDEBUG_H

// End of File
