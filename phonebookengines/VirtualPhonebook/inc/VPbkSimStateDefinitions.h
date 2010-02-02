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
* Description:  A P&S and CentRep UID/Flag variations
*
*/



#ifndef VPBKSIMSTATEDEFINITIONS_H
#define VPBKSIMSTATEDEFINITIONS_H

#include <VPbkConfig.hrh>

//  INCLUDES
#ifndef VPBK_TEST_BLUETOOTH_SIM_ACCESS_PROFILE
// The real Publish&Subcribe in use
#include <VPbkSimStateDefinitionsReal.h>
#else
// Own Publish&Subcribe in use
#include <vpbksimstatedefinitionstest.h>
#endif

#endif      // VPBKSIMSTATEDEFINITIONS_H
            
// End of File
