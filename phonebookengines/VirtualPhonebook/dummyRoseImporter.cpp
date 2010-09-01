/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <cvpbkasynccallback.h>
#include <cvpbkasyncoperation.h>
#include <cvpbkbatchoperation.h>
#include <cvpbkcontactfindoperation.h>
#include <cvpbkcontactmanager.h>
#include <cvpbkcontactstoredomain.h>
#include <cvpbkcontactstoreuri.h>
#include <cvpbkcontactviewdefinition.h>
#include <cvpbkcontactviewsortpolicy.h>
#include <cvpbkfieldfilter.h>
#include <cvpbkfieldtypeselector.h>
#include <cvpbkmulticontactoperationbase.h>
#include <cvpbkphonenumbermatchstrategy.h>
#include <cvpbksortorder.h>
#include <mvpbkbasecontact.h>
#include <mvpbkbasecontactfield.h>
#include <mvpbkbasecontactfieldcollection.h>
#include <mvpbkbatchoperationobserver.h>
#include <mvpbkcontactfieldbinarydata.h>
#include <mvpbkcontactfielddata.h>
#include <mvpbkcontactfielddatetimedata.h>
#include <mvpbkcontactfieldordering.h>
#include <mvpbkcontactfieldselector.h>
#include <mvpbkcontactfieldtextdata.h>
#include <mvpbkcontactfindobserver.h>
#include <mvpbkcontactlink.h>
#include <mvpbkcontactobserver.h>
#include <mvpbkcontactoperation.h>
#include <mvpbkcontactoperationbase.h>
#include <mvpbkcontactoperationfactory.h>
#include <mvpbkcontactstore.h>
#include <mvpbkcontactstorelist.h>
#include <mvpbkcontactstorelistobserver.h>
#include <mvpbkcontactstoreobserver.h>
#include <mvpbkcontactview.h>
#include <mvpbkcontactviewbase.h>
#include <mvpbkcontactviewobserver.h>
#include <mvpbkcontactviewsortpolicy.h>
#include <mvpbkfieldtype.h>
#include <mvpbkfieldtypeselector.h>
#include <mvpbkmulticontactoperation.h>
#include <mvpbkobjecthierarchy.h>
#include <mvpbksinglecontactoperationobserver.h>
#include <mvpbkstorecontact.h>
#include <mvpbkstorecontactfield.h>
#include <mvpbkstorecontactfieldcollection.h>
#include <mvpbkstreamable.h>
#include <mvpbkviewcontact.h>
#include <mvpbkviewcontactfield.h>
#include <mvpbkviewcontactfieldcollection.h>
#include <rdigitstring.h>
#include <rlocalizedresourcefile.h>
#include <tfixeddigitstring.h>
#include <tvpbkcontactstoreproperties.h>
#include <tvpbkcontactstoreuriptr.h>
#include <tvpbkfieldtypemapping.h>
#include <tvpbkfieldtypeparameters.h>
#include <tvpbkfieldversitproperty.h>
#include <vpbkcontactview.hrh>
#include <vpbkcontactview.rh>
#include <vpbkdatacaging.hrh>
#include <vpbkdebug.h>
#include <vpbkerror.h>
#include <vpbkfieldtype.hrh>
#include <vpbkfieldtype.rh>
#include <vpbkphonenumberindex.h>
#include <vpbkstoreuridefinitions.hrh>
#include <vpbkstoreuriliterals.h>
#include <vpbkprivateuid.h>
