/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface for import operation implementations
*
*/


#ifndef MVPBKIMPORTOPERATIONIMPL_H
#define MVPBKIMPORTOPERATIONIMPL_H


#include <e32def.h>

enum TVPbkImportCardType
    {
    EVCard,
    ECompactBusinessCard
    };
        
/**
 *  An observer for import operation implementation
 */
NONSHARABLE_CLASS( MVPbkImportOperationObserver )
    {
public:

    /**
     * Called after the card have been imported to contacts.
     */
    virtual void ContactsImported() = 0;

    /**
     * 
     */
    virtual void ContactImportingFailed( TInt aError ) = 0;
    
    /**
     * Called when no more contacts exist to be imported and reahced EOF.
     */
    virtual void ContactsImportingCompleted() = 0;
    };

/**
 * An interface for import operation implementations
 */
NONSHARABLE_CLASS( MVPbkImportOperationImpl )
    {
public:
    virtual ~MVPbkImportOperationImpl() {}

    virtual void StartL() = 0;
    virtual void SetObserver( MVPbkImportOperationObserver& aObserver ) = 0;
    virtual	TBool IsOwncard() = 0;
    };

#endif // MVPBKIMPORTOPERATIONIMPL_H
