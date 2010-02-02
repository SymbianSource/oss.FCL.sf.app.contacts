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

#include <ccapbkcontextlauncher.h>
#include "ccapbkcontextlauncherprivate.h"

EXPORT_C CCCAPbkContextLauncher* CCCAPbkContextLauncher::NewL()
    {
    CCCAPbkContextLauncher* self = new (ELeave) CCCAPbkContextLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCCAPbkContextLauncher::~CCCAPbkContextLauncher()
    {
    delete iImpl;
    }

inline CCCAPbkContextLauncher::CCCAPbkContextLauncher()
    {
    }

inline void CCCAPbkContextLauncher::ConstructL()
    {
    iImpl = CCCAPbkContextLauncherPrivate::NewL();
    }

EXPORT_C void CCCAPbkContextLauncher::LaunchMyCardViewL(
    const TUid* aUid )
    {
    iImpl->LaunchMyCardViewL( aUid );
    }

EXPORT_C void CCCAPbkContextLauncher::LaunchCCAViewL(
    const TDesC8& aPackedContactLink,
    const TUid* aUid )
    {
    iImpl->LaunchCCAViewL( aPackedContactLink, aUid );
    }

// End of File
