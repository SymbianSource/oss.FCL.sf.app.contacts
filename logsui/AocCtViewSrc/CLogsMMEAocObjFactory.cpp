/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Object factory for Logs. Instantiates ETEL related objects, multi mode ETEL version.
*
*/


//  INCLUDE FILES  
#include "CLogsAocObjFactory.h"
#include "CLogsMMEAocUtil.h"
#include "CLogsMMECallStatus.h"


// ================= MEMBER FUNCTIONS =======================

//  Factory function for MAocUtil
EXPORT_C MLogsAocUtil* CLogsAocObjFactory::AocUtilL()
    {
    return CLogsAocUtil::NewL();
    }

//  Factory function for MAocUtil
EXPORT_C MLogsCallStatus* CLogsAocObjFactory::CallStatusLC()
    {
    return CLogsCallStatus::NewLC( CLogsCallStatus::ECallAndDataLines );
    }

// End of file
