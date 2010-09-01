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


#ifndef CCAPBKCONTEXTLAUNCHERPRIVATE_H
#define CCAPBKCONTEXTLAUNCHERPRIVATE_H

#include <e32base.h>

class CVwsSessionWrapper;
class MVPbkContactLink;
class TUid;
class CBufBase;

/**
 *
 */
NONSHARABLE_CLASS( CCCAPbkContextLauncherPrivate ) : public CBase
    {
public: // constructor and destructor

    static CCCAPbkContextLauncherPrivate* NewL();
    ~CCCAPbkContextLauncherPrivate();

public: // New functions

    void LaunchMyCardViewL(
            const TUid* aUid );

    void LaunchCCAViewL(
            const TDesC8& aPackedContactLink,
            const TUid* aUid );

private:
    void DoLaunchCCAViewL(
            const TDesC8* aPackedContactLink,
            const TUid* aUid );
    
private:
    inline CCCAPbkContextLauncherPrivate();
    inline void ConstructL();

private:
    CVwsSessionWrapper* iVwsSessionWrapper;
    CBufBase* iBuffer;
    };

#endif // CCAPBKCONTEXTLAUNCHERPRIVATE_H

// End of File
