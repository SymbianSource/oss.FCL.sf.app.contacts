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
/*
 * ccappcommlaunchercustomlistbox.cpp
 *
 *  Created on: 2009-10-30
 *      Author: dev
 */

// System includes
#include <barsread.h>
#include <eikclbd.h>

#include "ccappcommlaunchercustomlistbox.h"
#include "ccappcommlaunchercustomlistboxitemdrawer.h"
#include "ccappcommlaunchercustomlistboxdata.h"

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::CCCAppCommLauncherCustomListBox
// --------------------------------------------------------------------------
//
inline CCCAppCommLauncherCustomListBox::CCCAppCommLauncherCustomListBox
            () 
    {
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::~CCCAppCommLauncherCustomListBox
// --------------------------------------------------------------------------
//
CCCAppCommLauncherCustomListBox::~CCCAppCommLauncherCustomListBox()
    {
    
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::NewL
// --------------------------------------------------------------------------
//
CCCAppCommLauncherCustomListBox* CCCAppCommLauncherCustomListBox::NewL()
    {
    CCCAppCommLauncherCustomListBox* self =
        new ( ELeave ) CCCAppCommLauncherCustomListBox();
    
    return self;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBox::CreateItemDrawerL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBox::CreateItemDrawerL()
    {    
    CCCAppCommLauncherCustomListBoxData* columnData = CCCAppCommLauncherCustomListBoxData::NewL();

    CleanupStack::PushL( columnData );  
    
    iItemDrawer = new (ELeave) CCCAppCommLauncherCustomListBoxItemDrawer(
            static_cast<MTextListBoxModel*>(Model()), iCoeEnv->NormalFont(), 
            columnData);
    CleanupStack::Pop( columnData );
    
    //Ownership has been transferred to iItemDrawer
    iColumnData = columnData;
    }
void CCCAppCommLauncherCustomListBox::SizeChanged()
	{
	CAknDoubleLargeStyleListBox::SizeChanged();
	}

