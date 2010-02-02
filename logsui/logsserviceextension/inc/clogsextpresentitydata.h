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
* Description:  Holdes data of a presentity.
*
*/


#ifndef C_CLOGSEXTPRESENTITYDATA_H
#define C_CLOGSEXTPRESENTITYDATA_H

#include <e32base.h>

#include "ximpcontextobserver.h"
#include "mlogsextservicehandlerobserver.h"


class MXIMPBase;
class TXIMPRequestId;
class CLogsExtServiceHandler;


/**
 * Presentity data holder.
 *
 * @since S60 v3.2
 */ 
NONSHARABLE_CLASS(CLogsExtPresentityData) : 
        public CBase     
    {
public: 
    
    enum TPresentityDataState 
            {        
            ESubscriptionOngoing = 0,
            ESubscribed,
            EUnsubscriptionOngoing,
            EUnsubscribed
            };
    
public:          
            
    /**
     * Creates a new CLogsExtPresentityData.
     *
     * @since S60 v3.2
     * @param aPresentityId a presentity id
     * @param aServiceHandler the service handler
     * @return a new CLogsExtPresentityData instance
     */
    static CLogsExtPresentityData* NewL(         
        const TDesC& aPresentityId,
        CLogsExtServiceHandler& aServiceHandler );
        
    /**
     * Creates a new CLogsExtPresentityData.
     *
     * @since S60 v3.2
     * @param aPresentityId a presentity id     
     * @param aServiceHandler the service handler
     * @return a new CLogsExtPresentityData instance
     */    
    static CLogsExtPresentityData* NewLC(         
        const TDesC& aPresentityId,
        CLogsExtServiceHandler& aServiceHandler );
        
    /**
     * Destructor.
     */
    ~CLogsExtPresentityData();

public: 

    /**
     * Returns the presentity id.
     *
     * @since S60 v3.2
     * @return the presentity id
     */ 
    const TDesC& PresentityId();   
    
    /**
     * Returns current subscription state.
     *
     * @since S60 v3.2
     * @return the subscription state.
     */
    TInt Status();

    /**
     * Sets subscription state.
     *
     * @since S60 v3.2
     * @param the subscription state.
     */    
    void SetStatus( TPresentityDataState aStatus );

    /**
     * Returns subscription request id.
     *
     * @since S60 v3.2
     * @return the subscription request id.
     */    
    TXIMPRequestId PresentitySubscriptionRequestId();

    /**
     * Sets subscription request id.
     *
     * @since S60 v3.2
     * @param the subscription state.
     */
    void SetPresentitySubscriptionRequestId( TXIMPRequestId aRequestId );
    
private:
    
    /**
     * Symbian second-phase constructor.
     */                    
    void ConstructL( const TDesC& aPresentityId );

    /**
     * Constructor.
     */
    CLogsExtPresentityData( CLogsExtServiceHandler& aServiceHandler ); 

    
private: // data
        
    /**
     * The request id of the subscription request.
     */
    TXIMPRequestId iPresentitySubscriptionRequestId;

    /**
     * Current subscription state.     
     */
    TPresentityDataState iState;
        
    /**
     * The presentity id
     * Own.
     */
    HBufC* iPresentityId;
        
    /**
     * Service handler     
     */
    CLogsExtServiceHandler& iServiceHandler;
        
    };

#endif  //  C_CLOGSEXTPRESENTITYDATA_H
