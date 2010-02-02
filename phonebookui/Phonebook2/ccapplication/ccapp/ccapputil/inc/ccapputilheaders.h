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
* Description:  Headers of ccapputil
*
*/


#ifndef __CCAPPUTILHEADERS_H__
#define __CCAPPUTILHEADERS_H__

//common logger
#include "ccalogger.h"
#define KCCAppUtilLogFile CCA_L("ccapputil.txt")

//own
#include "ccaparameter.h"

//CMS wrapper related
#include "cmscontactfield.h"
#include "cmscontactfieldinfo.h"
#include "cmscontactfielditem.h"
#include "cmssession.h"
#include "cmscontact.h"
#include "cmspresencedata.h"

// view / container baseclass related
#include <aknViewAppUi.h>
#include <AknUtils.h>

// own functionality
#include <ccappviewpluginakncontainer.h>
#include <ccappviewpluginaknview.h>
#include "ccacmscontactfetcherwrapper.h"

#include "../../inc/ccaappappui.h"

#endif // __CCAPPUTILHEADERS_H__
