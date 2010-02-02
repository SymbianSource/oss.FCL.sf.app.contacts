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
* Description:  Headers of plugin view implementation to help unit testing
*
*/


#ifndef __CCAPPCOMMLAUNCHERHEADERS_H__
#define __CCAPPCOMMLAUNCHERHEADERS_H__

// logfile definition
#define KCommLauncherLogFile CCA_L("commlauncherplugin.txt")
#include "ccalogger.h"

// common
#include <s32mem.h>
#include <imageconversion.h>
#include <bautils.h>
#include <StringLoader.h>

// avkon
#include <AknsUtils.h>
#include <aknnotewrappers.h>
#include <eikmenup.h>
#include <coemain.h>
#include <aknlists.h>
#include <AknIconArray.h>
#include <AknIconUtils.h>
#include <aknconsts.h>
#include <gulicon.h>
#include <avkon.mbg>
#include <eikimage.h>
#include <AknsBasicBackgroundControlContext.h>
#include <aknnotewrappers.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <eikclbd.h>
#include <eiklbo.h>
#include <AknPreviewPopUpController.h>
#include <AknUtils.h>
#include <aknViewAppUi.h>

// phonebook2/inc
#include <Phonebook2PrivateCRKeys.h>
#include <pbk2nameordercenrep.h>
#include <pbk2mapcommands.hrh>

// aiw
#include <AiwServiceHandler.h>
#include <AiwContactAssignDataTypes.h>

// cca client
#include "ccaparameter.h"
#include <mccapppluginparameter.h>

// cms engine
#include "cmscontactfield.h"
#include "cmscontactfieldinfo.h"
#include "cmscontactfielditem.h"
#include "cmssession.h"
#include "cmscontact.h"
#include "ccacontactorservice.h"
#include <VPbkFieldTypeSelectorFactory.h>
#include "cmspresencedata.h"

#include "ccacmscontactfetcherwrapper.h"

// help
#include <csxhelp/phob.hlp.hrh>
#include <Pbk2UID.h>

// own
#include <ccappcommlauncherpluginrsc.rsg>
#include <ccappviewpluginakncontainer.h>

#include "ccappcommlauncherlpadmodel.h"
#include "ccappcommlaunchercontacthandler.h"
#include "ccappcommlauncherheadercontrol.h"
#include "ccappcommlauncherheadertextorder.h"
#include "ccappcommlaunchercontainer.h"
#include "ccappviewpluginbase.h"
#include "ccappcommlauncherplugin.h"
#include "ccappcommlauncherpluginuids.hrh"
#include "ccappcommlauncher.hrh"
#include "CCCAppCommLauncherPbkCmd.h"
#include "ccappcommlaunchermenuhandler.h"
#include "ccappcommlauncherimagedecoding.h"
#include "ccappcommlauncherprivatecrkeys.h"

#endif // __CCAPPCOMMLAUNCHERHEADERS_H__
