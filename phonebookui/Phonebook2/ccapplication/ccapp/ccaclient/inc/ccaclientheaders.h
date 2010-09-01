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
* Description:  Headers of ccaclient
*
*/


#ifndef __CCACLIENTHEADERS_H__
#define __CCACLIENTHEADERS_H__

// for constants
#include <coemain.h>

// Constants
_LIT8( KAiwContentTypeAll,                           "*" );

#define KCCAClientLoggerFile CCA_L("ccaclient.txt")

// system
#include <StringLoader.h>
#include <eikenv.h>
#include <bautils.h>
#include <coeutils.h>
#include <e32std.h>
#include <e32base.h>
#include <AknServerApp.h>
#include <apparc.h>

// avkon
#include <avkon.rsg>

//project inc
#include "ccauids.h"

//own
#include "ccaclientuiservice.h"
#include "ccaparameter.h"
#include "ccafactory.h"
#include "ccaconnection.h"
#include "mccaconnection.h"
#include "mccaconnectionext.h"
#include "ccaclientserverappipc.h"

//CMS wrapper related
#include "cmscontactfield.h"
#include "cmscontactfieldinfo.h"
#include "cmscontactfielditem.h"
#include "cmssession.h"
#include "cmscontact.h"
#include "cmspresencedata.h"

//common logger
#include "ccalogger.h"


#endif // __CCACLIENTHEADERS_H__
