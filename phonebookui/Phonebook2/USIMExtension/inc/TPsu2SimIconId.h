/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for USIM extension defined icon identification
*
*/



#ifndef TPSU2SIMICONID_H
#define TPSU2SIMICONID_H

#include "Pbk2USIMUIUID.h"
#include <TPbk2IconId.h>
#include <Pbk2ExtensionUID.h>
#include <Pbk2USimUI.hrh>

/**
 *  A class for phonebook2 defined icon identification
 *
 */
class TPsu2SimIconId : public TPbk2IconId
    {
public:

    /**
     * C++ constructor.
     * @param aPbkCoreIconId the core phonebook2 icon id
     */
    inline TPsu2SimIconId( TPsu2IconId aPsu2IconId )
        :   TPbk2IconId( TUid::Uid( KPbk2USIMExtensionImplementationUID ), 
                aPsu2IconId )
        {
        }
    };

#endif // TPSU2SIMICONID_H
