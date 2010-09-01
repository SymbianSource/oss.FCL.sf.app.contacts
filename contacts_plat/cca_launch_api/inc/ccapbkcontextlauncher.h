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


#ifndef CCAPBKCONTEXTLAUNCHER_H
#define CCAPBKCONTEXTLAUNCHER_H

#include <e32base.h>

class MVPbkContactLink;
class TUid;
class CCCAPbkContextLauncherPrivate;


/**
 * Launches Contact Card and MyCard views in Phonebook context.
 *
 *  @lib ccaclient.lib
 */
NONSHARABLE_CLASS( CCCAPbkContextLauncher ) : public CBase
    {
public: // constructor and destructor

    IMPORT_C static CCCAPbkContextLauncher* NewL();
    ~CCCAPbkContextLauncher();

public: // New functions

    /**
     * Activates MyCard view in Phonebook context.
     * This function leaves with KErrNotSupported if MyCard is not supported
     * MyCard is supported if feature flag FF_CONTACTS_MYCARD is defined.
     *
     * @param aUid CCA view plugin UID, if UID is NULL then default view for
     *        the MyCard is activated
     */
    IMPORT_C void LaunchMyCardViewL(
            const TUid* aUid = NULL );

    /**
     * Activates Contact Card view in Phonebook context
     *
     * @param aPackedContactLink packed link to contact that is opened in
     *        Contact Card view. @see MVPbkContactLink
     * @param aUid CCA view plugin UID, if UID is NULL then
     *        default view for the contact is activated.
     */
    IMPORT_C void LaunchCCAViewL(
            const TDesC8& aPackedContactLink,
            const TUid* aUid = NULL );

private:
    inline CCCAPbkContextLauncher();
    inline void ConstructL();

private:
    CCCAPbkContextLauncherPrivate* iImpl;

    };

#endif // CCAPBKCONTEXTLAUNCHER_H

// End of File
