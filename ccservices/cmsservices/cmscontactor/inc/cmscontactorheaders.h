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
* Description:  Headers of cmscontactor
 *
*/


#ifndef __CMSCONTACTORHEADERS_H__
#define __CMSCONTACTORHEADERS_H__

// for constants
#include <coemain.h>

// Constants
_LIT8( KAiwContentTypeAll, "*");
//_LIT( KLoggerFile, "cmscontactor.txt" );

_LIT( KCMSContactorEmptyLit, " ");
_LIT( KCMSContactorTabLit, "\t");

#define KCMSContactorLoggerFile CMS_L("cmscontactor.txt")

// aiw for phone call
#include <AiwCommon.h>
#include <AiwVariant.h>
#include <AiwCommon.hrh>
#include <AiwGenericParam.h>
#include <AiwGenericParam.hrh>
#include <AiwServiceHandler.h>
#include <commonphoneparser.h>

#include <apgcli.h>
#include <apgtask.h>

// system
#include <StringLoader.h>
#include <eikenv.h>
#include <bautils.h>
#include <coeutils.h>

// avkon
#include <avkon.rsg>

// sendui
#include <sendui.h>
#include <CMessageData.h>
#include <SenduiMtmUids.h>

// own
#include "cmsphonecall.h"
#include "cmsmsgeditors.h"
#include "cmscontactorenums.h"
#include "cmsoperationhandlerbase.h"
#include "cmsunieditoroperation.h"
#include "cmscalloperation.h"
#include "cmsvoipcalloperation.h"
#include "cmsemailoperation.h"
#include "cmsselectiondialog.h"
#include <cmscontactor.rsg>
#include "cmsimoperation.h"
#include "cmsopenurloperation.h"
#include "cmsvideocalloperation.h"

// common logger
#include "cmslogger.h"

#endif // __CMSCONTACTORHEADERS_H__

