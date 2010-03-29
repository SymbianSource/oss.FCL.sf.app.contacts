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
* Description: Phone number parser
*
*/


#ifndef SPBPHONENUMBERPARSER_H_
#define SPBPHONENUMBERPARSER_H_

//  INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>

class CVPbkContactManager;
class CSpbContent;
class MVPbkContactLink;
class MVPbkContactOperationBase;

/**
 * Finds contacts's active phone number
 */
NONSHARABLE_CLASS( CSpbPhoneNumberParser ) :
    public CBase,
    public MVPbkSingleContactOperationObserver
    {
   
public: // Construction & destruction
    static CSpbPhoneNumberParser* NewL( 
            CVPbkContactManager& aContactManager,
            CSpbContent& aContent);
    ~CSpbPhoneNumberParser();

public: // Interface
    void FetchPhoneNumberL( const MVPbkContactLink& aLink);
    
private:    // from MVPbkSingleContactOperationObserver
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, 
            TInt aError );
    
private:	// new functions
    /*
     * Solves what to show on names list. 
	 * - If only one number, it's shown. 
	 * - If default number is set, it's shown. 
	 * - Otherwise nothing is shown.
     * @param aContact
     */
    void SolvePhoneNumberL( MVPbkStoreContact& aContact );
    
private:    // constructors
    inline CSpbPhoneNumberParser(
            CVPbkContactManager& aContactManager,
            CSpbContent& aContent);
    inline void ConstructL();
   
private: // data
    CVPbkContactManager& iContactManager;
    CSpbContent& iContent;
    MVPbkContactOperationBase* iOperation;
    };

#endif /*SPBPHONENUMBERPARSER_H_*/
