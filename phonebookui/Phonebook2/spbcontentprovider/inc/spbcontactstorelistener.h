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
* Description:  .
*
*/

#ifndef SPBCONTACTSTORELISTENER_H_
#define SPBCONTACTSTORELISTENER_H_

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATION
class CSpbContent;
class CPbk2StoreManager;

/*
 * Listener of contact stores to observe phonenumber changes
 */
NONSHARABLE_CLASS( CSpbContactStoreListener ) : 
    public CBase,
    public MVPbkContactStoreObserver
	{
	
public: // Construction & destruction
	
    static CSpbContactStoreListener* NewL(
            CPbk2StoreManager& aStoreManager,
            RPointerArray<CSpbContent>& aContentCache );
    
    ~CSpbContactStoreListener();
    
    
public:	//from MVPbkContactStoreObserver
	
	void StoreReady(MVPbkContactStore& aContactStore);
	
	void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason );
	
	void HandleStoreEventL(
        MVPbkContactStore& aContactStore, 
        TVPbkContactStoreEvent aStoreEvent);
    
private:	// new functions
	
	/*
	 * Returns content corresponding to the link
	 */
	CSpbContent* ContentByLink( const MVPbkContactLink& aLink );
    
	/**
	 * Delete content from cache corresponding to the link
	 * @param aLink contact link to be matched with cache
	 */
	void DeleteContentByLink( const MVPbkContactLink& aLink );
	
private:    // constructors
    inline CSpbContactStoreListener(
        CPbk2StoreManager& aStoreManager,
        RPointerArray<CSpbContent>& aContentCache );
    
    inline void ConstructL();
    
private: // data
   
    /// Ref. Store Manager
    CPbk2StoreManager& iStoreManager;

    /// Ref. Cached content
    RPointerArray<CSpbContent>& iContentCache;
	};


#endif /* SPBCONTACTSTORELISTENER_H_ */
