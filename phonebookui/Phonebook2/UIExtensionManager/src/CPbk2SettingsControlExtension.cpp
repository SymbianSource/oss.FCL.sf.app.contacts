/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 settings control composite UI extension.
*
*/


#include "CPbk2SettingsControlExtension.h"

// Phonebook 2
#include "CPbk2UIExtensionLoader.h"
#include "ForEachUtil.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionPlugin.h"
#include <MPbk2SettingsViewExtension.h>

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::CPbk2SettingsControlExtension
// --------------------------------------------------------------------------
//
CPbk2SettingsControlExtension::CPbk2SettingsControlExtension()
    {
    }

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::~CPbk2SettingsControlExtension
// --------------------------------------------------------------------------
//
CPbk2SettingsControlExtension::~CPbk2SettingsControlExtension()
    {
    iSettingExtensions.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SettingsControlExtension::ConstructL
        ( TArray<MPbk2SettingsViewExtension*> aSettingViewExtensions )
    {
    const TInt count = aSettingViewExtensions.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        MPbk2SettingsControlExtension* extension =
            aSettingViewExtensions[i]->CreatePbk2SettingsControlExtensionL();
        if ( extension )
            {
            CleanupDeletePushL( extension );
            iSettingExtensions.AppendL( extension );
            CleanupStack::Pop(); // extension
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::NewL
// --------------------------------------------------------------------------
//    
CPbk2SettingsControlExtension* CPbk2SettingsControlExtension::NewL
        ( TArray<MPbk2SettingsViewExtension*> aSettingViewExtensions )
    {
    CPbk2SettingsControlExtension* self = 
        new ( ELeave ) CPbk2SettingsControlExtension();
    CleanupStack::PushL( self );
    self->ConstructL( aSettingViewExtensions );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::DoRelease
// --------------------------------------------------------------------------
//
void CPbk2SettingsControlExtension::DoRelease()
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::ModifySettingItemListL
// --------------------------------------------------------------------------
//
void CPbk2SettingsControlExtension::ModifySettingItemListL
        ( CAknSettingItemList& aSettingItemList )
    {
    ForEachL( iSettingExtensions,
              VoidMemberFunction
              ( &MPbk2SettingsControlExtension::ModifySettingItemListL ),
                aSettingItemList );
    }

// --------------------------------------------------------------------------
// CPbk2SettingsControlExtension::CreateSettingItemL
// --------------------------------------------------------------------------
//
CAknSettingItem* CPbk2SettingsControlExtension::CreateSettingItemL
        ( TInt aSettingId )
    {
    CAknSettingItem* item = NULL;
    const TInt count = iSettingExtensions.Count();
    for ( TInt i = 0; i < count && !item; ++i )
        {
        item = iSettingExtensions[i]->CreateSettingItemL( aSettingId );
        }
    return item;
    }
    
// End of File
