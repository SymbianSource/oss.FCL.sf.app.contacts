/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Headers of bpas
 *
*/


#ifndef __BPASHEADERS_H__
#define __BPASHEADERS_H__

// for constants
#include <coemain.h>

// Constants


// logging
#define KBPasLoggerFile CMS_L("bpas.txt")

// own
#include "bpaspresenceinfo.h"
#include "bpasserviceinfo.h"
#include "bpasiconinfo.h"
#include "bpasinfo.h"
#include "bpasobserver.h"
#include "bpas.h"

// service table
#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>

// presence cache
#include <presencecachereader2.h>
#include <mpresencebuddyinfo2.h>

// common logger
#include "cmslogger.h"

#endif // __BPASHEADERS_H__

