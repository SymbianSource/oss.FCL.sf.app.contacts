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
* Description:  Mapping that simply contains a unique log id and a 
                 presentity id.
*
*/



#ifndef C_CLOGSEXTLOGIDPRESIDMAPPING_H
#define C_CLOGSEXTLOGIDPRESIDMAPPING_H

#include <e32base.h>
#include <logwrap.h>

/**
 *  Encapsulates a log id and a presentity id.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsExtLogIdPresIdMapping) : public CBase
    {

public:    

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2     
     * @param aLogId a unique log id
     * @param aPresentityId a presentity id
     */
    static CLogsExtLogIdPresIdMapping* NewL( const TLogId aLogId, 
                                             const TDesC& aPresentityId );
        
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2     
     * @param aLogId a unique log id
     * @param aPresentityId a presentity id
     */
    static CLogsExtLogIdPresIdMapping* NewLC( const TLogId aLogId, 
                                              const TDesC& aPresentityId );


    /**
     * Destructor
     *
     * @since S60 v3.2     
     */    
    ~CLogsExtLogIdPresIdMapping();
    
    
    /**
     * Getter function for the log id of this mapping.
     *
     * @since S60 v3.2     
     * @return log id
     */
    TLogId LogId();
    
    /**
     * Getter function for the presentity id of this mapping.
     *
     * @since S60 v3.2
     * @return presentity id
     */
    const TDesC* PresentityId();
    
private:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aLogId a unique log id
     */
    CLogsExtLogIdPresIdMapping( TLogId aLogId );


    /**
     * Symbian Second Phase Constructor.
     *
     * @since S60 v3.2
     * @param aPresentityId a presentity id
     */       
    void ConstructL( const TDesC& aPresentityId );


private: // data
    
    /**
     * The unique log event id.
     */
    TLogId iLogId;
    
    /**
     * The presentity id.
     * Own.
     */
    HBufC* iPresentityId;

    };


#endif // C_CLOGSEXTLOGIDPRESIDMAPPING_H
