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
* Description:  This is a class for CMS contactor enum(s).
*
*/

#ifndef __CMSCONTACTORENUMS_H__
#define __CMSCONTACTORENUMS_H__


/**
 *  TCmsContactor
 *
 *  Class for CMS contactor enum(s).
 *
 *  @since S60 v5.0
 */
class TCmsContactor
    {
public:

    /** Communication launching related information. */
    enum TCMSCommLaunchFlag
        {
        ECMSContactorParamNotSet =       0x00000,
        ECMSContactorUseDefaultAddress = 0x00001
        };

    };

#endif      //__CMSCONTACTORENUMS_H__

