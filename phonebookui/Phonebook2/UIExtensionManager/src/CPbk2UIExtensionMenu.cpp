/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 UI Extension menu information.
*
*/


#include "CPbk2UIExtensionMenu.h"

// System includes
#include <barsread.h>

#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::CPbk2UIExtensionMenu
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenu::CPbk2UIExtensionMenu()
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::CPbk2UIExtensionMenu
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenu::CPbk2UIExtensionMenu(        
        TUid aViewId, 
        TUint32 aMenuFilteringFlags,
        TInt aParentMenuResourceId, 
        TInt aPreviousCommandId, 
        TInt aMenuResourceId, 
        TInt aCascadingMenuCmd ) :
    iViewId ( aViewId ), 
    iMenuFilteringFlags ( aMenuFilteringFlags ),
    iParentMenuResId ( aParentMenuResourceId ), 
    iPreviousCommand ( aPreviousCommandId ), 
    iMenuResId ( aMenuResourceId ), 
    iCascadingMenuCmd ( aCascadingMenuCmd )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::~CPbk2UIExtensionMenu
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenu::~CPbk2UIExtensionMenu()
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::NewLC
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenu* CPbk2UIExtensionMenu::NewLC
        ( TResourceReader& aReader )
    {
    CPbk2UIExtensionMenu* self = new ( ELeave ) CPbk2UIExtensionMenu;
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2UIExtensionMenu* CPbk2UIExtensionMenu::NewLC( 
        TUid aViewId, 
        TUint32 aMenuFilteringFlags,
        TInt aParentMenuResourceId, 
        TInt aPreviousCommandId, 
        TInt aMenuResourceId, 
        TInt aCascadingMenuCmd)
    {
    CPbk2UIExtensionMenu* self = new ( ELeave ) CPbk2UIExtensionMenu(
            aViewId, aMenuFilteringFlags, aParentMenuResourceId, 
            aPreviousCommandId, aMenuResourceId, aCascadingMenuCmd);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionMenu::ConstructL( TResourceReader& aReader )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionMenu::ConstructL(0x%x) START"), this );
        
    aReader.ReadInt8(); // read version number
    iViewId = TUid::Uid( aReader.ReadInt32() );
    iMenuFilteringFlags = aReader.ReadUint32();
    iParentMenuResId = aReader.ReadInt32();
    iPreviousCommand = aReader.ReadInt32();
    iMenuResId = aReader.ReadInt32();
    iCascadingMenuCmd = aReader.ReadInt32();
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2UIExtensionMenu::ConstructL(0x%x) END"), this );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionMenu::ConstructL( )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::ViewId
// --------------------------------------------------------------------------
//
TUid CPbk2UIExtensionMenu::ViewId() const
    {
    return iViewId;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::ParentMenuId
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionMenu::ParentMenuId() const
    {
    return iParentMenuResId;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::PreviousCommand
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionMenu::PreviousCommand() const
    {
    return iPreviousCommand;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::MenuResId
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionMenu::MenuResId() const
    {
    return iMenuResId;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::CascadingMenuCmd
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionMenu::CascadingMenuCmd() const
    {
    return iCascadingMenuCmd;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionMenu::IsVisible
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionMenu::IsVisible
        ( TUid aViewId, TInt aParentMenuId, TUint32 aFilteringFlags) const
    {
    // Menu item is visible if:
    // 1) It belongs to the same view (aViewId == iViewId)
    // 2) It belongs to the same menu (aParentMenuId == iParentMenuResId)
    // 3) All menu filtering flags defined in the item are on in aFilteringFlags
    return
        ( aViewId == iViewId && aParentMenuId == iParentMenuResId && (
        ( ( iMenuFilteringFlags & aFilteringFlags ) ^ iMenuFilteringFlags )
         == 0 ) );
    }

// End of File
