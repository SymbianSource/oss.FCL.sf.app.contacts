/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fetches a brand icon and its bitmask.
*
*/



#ifndef C_CLOGSEXTBRANDFETCHER_H
#define C_CLOGSEXTBRANDFETCHER_H


#include <e32base.h>

class MLogsExtBrandFetcherObserver;

/**
 * Branding icon fetcher class.
 *
 * @since S60 v3.2 
 */
NONSHARABLE_CLASS(CLogsExtBrandFetcher) : public CBase
    {
    
public:
    
        
    /**
     * Creates a new CLogsExtBrandFetcher.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aObserver the observer of this fetching process
     * @return a new CLogsExtBrandFetcher instance
     */       
    static CLogsExtBrandFetcher* NewL( 
        const TUint32 aServiceId, 
        MLogsExtBrandFetcherObserver& aObserver );

    /**
     * Creates a new CLogsExtBrandFetcher.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aObserver the observer of this fetching process
     * @return a new CLogsExtBrandFetcher instance
     */       
    static CLogsExtBrandFetcher* NewLC( 
        const TUint32 aServiceId, 
        MLogsExtBrandFetcherObserver& aObserver );
        
    /**
     * Destructor.
     */
    ~CLogsExtBrandFetcher();        

public: 
    
    /**
     * Initiates the fetching of the icon.
     *
     * @since S60 v3.2
     * @return KErrNone in case of no error, system error otherwise
     * e.g. KErrNotReady if the fetcher is in use already
     */           
    TInt Fetch();

    /**
     * Returns the service id.
     *
     * @since S60 v3.2
     * @return the service id
     */ 
    TUint32 ServiceId();

    
protected:

    /**
     * Symbian second-phase constructor
     */                    
    void ConstructL();


private:
    
    /**
     * Constructor.
     */
    CLogsExtBrandFetcher( const TUint32 aServiceId, 
                          MLogsExtBrandFetcherObserver& aObserver );
        
    /**
     * Retrieves the icon's bitmap and bitmask from the branding server
     * and completes its own request. RunL should call the callback 
     * afterwards.
     *
     * @since S60 v3.2
     */ 
    void GetBitmapsFromBrandServerL();
        
        
private: // data
        
    /**
     * Service id
     */
    TUint32 iServiceId;
        
    /**
     * The observer of this fetcher. Its callback function is called when
     * the bitmaps have been fetched successfully.
     * Not own.
     */
    MLogsExtBrandFetcherObserver* iObserver;
            
    };

#endif // C_CLOGSEXTBRANDFETCHER_H
