/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Phonebook document class definition
*
*/


#ifndef __MenuFilteringFlags_H__
#define __MenuFilteringFlags_H__

//  INCLUDES
#include <e32base.h>

/**
 * Phonebook view menu filtering flags
 */

const TUint KPbkControlEmpty                = 0x00000001;
const TUint KPbkItemsMarked                 = 0x00000002;
const TUint KPbkCurrentFieldPhoneNumber     = 0x00000004;
const TUint KPbkContactListEmpty            = 0x00000008;
const TUint KPbkNoGroups                    = 0x00000010;
const TUint KPbkNoOpenOneTouch              = 0x00000020;
const TUint KPbkGroupsView                  = 0x00000040;
const TUint KPbkInfoView                    = 0x00000080;
const TUint KPbkCurrentFieldVoipAddress     = 0x00000100;
const TUint KPbkNoGroupSend                 = 0x00000200;
const TUint KPbkCurrentFieldPoc             = 0x00000400;

#endif  // __MenuFilteringFlags_H__
            
// End of File

