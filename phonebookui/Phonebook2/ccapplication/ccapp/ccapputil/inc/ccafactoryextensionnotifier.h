/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CCCaFactoryExtensionNotifier
*
*/

#ifndef CCAFACTORYEXTENSIONOBSERVER_H
#define CCAFACTORYEXTENSIONOBSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecom.h>

// CLASS DECLARATION
class CCCAExtensionFactory;

NONSHARABLE_CLASS(CCCaFactoryExtensionNotifier) : public CActive
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CCCaFactoryExtensionNotifier();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCCaFactoryExtensionNotifier* NewL();
    
    /**
     * Sets the callback
     * 
     * @since TB 9.2
     */
    IMPORT_C void ObserveExtensionFactory( TCallBack& aCallBack );
    
    /**
     * Returns CCCAExtensionFactory pointer
     *
     * @return CCCAExtensionFactory
     * @since TB 9.2
     */
    IMPORT_C CCCAExtensionFactory* ExtensionFactory();
    
protected:  // From CActive
    void DoCancel();
    void RunL();
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    inline CCCaFactoryExtensionNotifier();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    inline void ConstructL();
    
    /**
    * Checks if implementation plugins are available, creates/deletes 
    * extension factory accordingly and notifies clients.
    *
    * @since TB 9.2
    */
    void CreateExtensionFactoryL();
    
private:
    TCallBack iCallBack;
    CCCAExtensionFactory* iExtensionFactory;
    REComSession* iEComSession;
    
    };

#endif // CCAFACTORYEXTENSIONOBSERVER_H
