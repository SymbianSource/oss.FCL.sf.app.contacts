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
*       Factory to create instances of MPbkGlobalSetting.
*
*/


#include <PbkGlobalSettingFactory.h>
#include <e32std.h>
#include "CPbkCenRepSetting.h"

EXPORT_C MPbkGlobalSetting* PbkGlobalSettingFactory::CreatePersistentSettingL()
    {
    return CPbkCenRepSetting::NewL();
    }

// End of File
