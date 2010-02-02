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
* Description:  CCApplication's headers
*
*/


#ifndef __CCAPPHEADERS_H__
#define __CCAPPHEADERS_H__

// Constants
#define KCCAppLogFile CCA_L("ccaapp.txt")

// system
#include <e32base.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <avkon.hrh>
#include <AknDef.h>
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <akntitle.h>
#include <centralrepository.h>
#include <bautils.h>
#include <aknViewAppUi.h>
#include <aknview.h>
#include <AknIconUtils.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <featmgr.h>
#include <hlplch.h>
#include <AknTabObserver.h>

//project inc
#include <mccapppluginparameter.h>
#include <mccaconnection.h>
#include <mccapluginfactory.h> 

// cms engine
#include "cmscontactfield.h"
#include "cmscontactfieldinfo.h"
#include "cmscontactfielditem.h"
#include "cmssession.h"
#include "cmscontact.h"
#include "cmspresencedata.h"

//own
#include <ccaapp.rsg>
#include <ccappviewpluginaknview.h>
#include "ccapppanic.h"
#include "ccappviewpluginbase.h"
#include "ccaparameter.h"
#include "ccacmscontactfetcherwrapper.h"
#include "ccappplugindata.h"
#include "ccapppluginloader.h"
#include "ccaappview.h"
#include "ccaappstatuspanehandler.h"
#include "ccaappappui.h"
#include "ccapppluginparameter.h"
#include "ccaappappui.h"
#include "ccaappdocument.h"
#include "ccaappservice.h"
#include "ccaappprivatecrkeys.h"


//common logger
#include "ccalogger.h"

#endif // __CCAPPHEADERS_H__
