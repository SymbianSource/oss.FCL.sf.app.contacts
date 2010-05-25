/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is the client side internal file to handle
*               cache update notifications sent by the server using
*               Publish and Subscribe framework.
*
*/

#ifndef CPSUPDATEHANDLER_H
#define CPSUPDATEHANDLER_H

#include <e32base.h>

class CPSRequestHandler;


class CPsUpdateHandler : public CActive
    {
public:
    // Cancel and destroy
    ~CPsUpdateHandler();

    // Two-phased constructor.
    static CPsUpdateHandler* NewL( CPSRequestHandler& aRequestHandler, TInt aPsKey );

    // Two-phased constructor.
    static CPsUpdateHandler* NewLC( CPSRequestHandler& aRequestHandler, TInt aPsKey );

private:
    // C++ constructor
    CPsUpdateHandler( CPSRequestHandler& aRequestHandler, TInt aPsKey );

    // Second-phase constructor
    void ConstructL();

private: // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

private:
    
    /**
    * Handle to RequestHandler through which observers will be notified
    */
    CPSRequestHandler& iRequestHandler;
    
    /**
    * P&S property handle to be observed
    */
    RProperty iProperty;
    
    /**
    * The P&S key observed by this handler object
    */
    TInt iPsKey;
    };

#endif // CPSUPDATEHANDLER_H
