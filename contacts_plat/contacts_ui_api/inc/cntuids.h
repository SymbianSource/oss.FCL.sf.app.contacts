/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTUIDS_H
#define CNTUIDS_H

#include <e32def.h>
#include <e32cmn.h>

/*
*   Contacts application's central repository Uid used for global settings
*/
const TUid KCRUiContacts = {0x2002FF54};

/*
*   Key value used to store the name ordering setting i.e
*   FirstName LastName, LastName FirstName etc
*/
const TUint32 KCntNameOrdering = 0x00000001;

// name order enumerations
enum CntNameOrder {
    CntOrderLastFirst = 0x0,
    CntOrderLastCommaFirst = 0x1,
    CntOrderFirstLast = 0x2
};

#endif
