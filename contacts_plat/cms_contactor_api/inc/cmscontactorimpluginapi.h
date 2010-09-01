/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Abstact class for IM Launcher plugin
 *
*/

#ifndef __CMSCONTACTORIMPLUGINAPI_H__
#define __CMSCONTACTORIMPLUGINAPI_H__

// INCLUDES
#include <e32base.h>


/**
 *  CmsContactor
 *
 *  Class for starting services of CMS contactor.
 *
 *  @since S60 v5.0
 */
class MCmsContactorImPluginApi
	{
public:

    /**
     * Start launching the IM application
     * @param aXspId URI (IMPP field in contact database) in format <service>:<userid>
     */
    virtual void ExecuteL( const TDesC& aXspId ) = 0;

	};

#endif     // __CMSCONTACTORIMPLUGINAPI_H__

