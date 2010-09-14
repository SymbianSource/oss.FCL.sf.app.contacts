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
*       Performs menu pane filtering. Common filtering methods are in
*       app view classes, this class contains filtering methods for
*       commands that differ in different Series 60 releases.
*
*/


// INCLUDE FILES
#include "CPbkMenuFiltering.h"  // This class' declaration
#include <Phonebook.hrh>
#include <eikmenup.h>           // CEikMenuPane
#include <FeatMgr.h>
#include <bldvariant.hrh>
#include <MenuFilteringFlags.h>
#include <Stringloader.h>
#include <phonebook.rsg>

namespace {

// LOCAL CONSTANTS AND MACROS
_LIT(KSettingsVisible, "1");

} // namespace
// ================= MEMBER FUNCTIONS =======================

void CPbkMenuFiltering::SettingsPaneMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint /*aFlags = 0*/)
    {
    // settings menu item visibility is defined in phonebook.loc file
    // value "1" for visible and "0" for hidden.
    HBufC* visibility = StringLoader::LoadLC(R_QTN_PHOB_NAME_ORDERING_SETTINGS_VISIBILITY);

    if ( visibility->Compare(KSettingsVisible) != 0 )
        {
        // Filter out Settings
        aMenuPane.SetItemDimmed(EPbkCmdSettings, ETrue);
        }

    CleanupStack::PopAndDestroy(visibility);
    }

void CPbkMenuFiltering::GroupsListPaneMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags /*=0*/)
    {
    HBufC* visibility = StringLoader::LoadLC(R_QTN_PHOB_NAME_ORDERING_SETTINGS_VISIBILITY);

    if ( visibility->Compare(KSettingsVisible) != 0 )
        {
        // Filter out Settings
        aMenuPane.SetItemDimmed(EPbkCmdSettings, ETrue);
        }

    if (aFlags & KPbkControlEmpty)
        {
        // If the list is empty no options menu items
        aMenuPane.SetItemDimmed(EPbkCmdOpenGroup, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdRemoveGroup, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdRenameGroup, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdSetRingingTone, ETrue);
        aMenuPane.SetItemDimmed(EPbkPhonebookInfoSubMenu, ETrue);
        }

    if (aFlags & KPbkNoOpenOneTouch)
        {
        aMenuPane.SetItemDimmed(EPbkCmdOpenOneTouch, ETrue);
        }

    if ( aFlags & KPbkNoGroupSend )
        {
        aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
        }
    CleanupStack::PopAndDestroy(visibility);
    }

//  End of File
