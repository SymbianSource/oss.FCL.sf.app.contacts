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
* Description:  ccacontactor's headers
 *
*/


#ifndef __CCACONTACTORHEADERS_H__
#define __CCACONTACTORHEADERS_H__


// Constants
#include <e32cons.h>
_LIT( KColon, ":" );

// system
#include <e32base.h>
#include <bautils.h>
#include <coeutils.h>
#include <coemain.h>
#include <apgcli.h> 
#include <apgtask.h>
#include <eikenv.h>

//Phbk 
#include <VPbkFieldTypeSelectorFactory.h>

// AIW
#include <AiwCommon.h>
#include <AiwServiceHandler.h>
#include <AiwGenericParam.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwVariant.h>
#include <AiwGenericParam.hrh>
#include <aiwdialdataext.h>


//SPSettings
#include <spsettings.h>
#include <spproperty.h>

//Sendui
#include <SenduiMtmUids.h>
#include <sendui.h>
#include <CMessageData.h>
#include <commonphoneparser.h>
#include <CSendingServiceInfo.h>

//IM plugin
#include <cmscontactorimpluginbase.h>

//own
#include "ccacontactor.h"
#include "ccacontactoroperation.h"
#include "ccacontactorcalloperation.h"
#include "ccacontactorvideocalloperation.h"
#include "ccacontactorunieditoroperation.h"
#include "ccacontactoremailoperation.h"
#include "ccacontactorurloperation.h"
#include "ccacontactorvoipoperation.h"
#include "ccacontactorimoperation.h"
#include "ccamsgeditors.h"
#include "ccaphonecall.h"

#endif // __CCACONTACTORHEADERS_H__
