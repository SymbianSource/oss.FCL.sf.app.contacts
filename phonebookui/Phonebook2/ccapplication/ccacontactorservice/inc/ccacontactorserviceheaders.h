/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ccacontactorservice's headers
 *
*/


#ifndef __CCACONTACTORSERVICEHEADERS_H__
#define __CCACONTACTORSERVICEHEADERS_H__

// Constants
#include <e32cons.h>
#define KCCAContactorServiceLoggerFile CCA_L("ccacontactorservice.txt")
_LIT( KCCAContactorServiceResourceFileName, "\\resource\\ccacontactorservicersc.rsc" );
_LIT( KColon, ":" );


// system
#include <e32base.h>
#include <bautils.h>
#include <coeutils.h>
#include <coemain.h>

//Phbk 
#include <VPbkFieldTypeSelectorFactory.h>

// AIW
#include <AiwCommon.h>
#include <AiwServiceHandler.h>
#include <AiwGenericParam.h>
#include <AiwContactSelectionDataTypes.h>

//SPSettings
#include <spsettings.h>
#include <spproperty.h>

//own
#include "ccacontactorservice.h"
#include "ccacontactorserviceoperator.h"
#include "ccacontactorpopuphandler.h"
#include <ccacontactorservicersc.rsg>

#include "ccacontactor.h"

//common logger
#include "ccalogger.h"

#endif // __CCACONTACTORSERVICEHEADERS_H__
