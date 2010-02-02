/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Implements interface for Logs reader configuration
*
*/


#ifndef __Logs_Engine_CLogsReaderConfig_H__
#define __Logs_Engine_CLogsReaderConfig_H__

//  INCLUDES
#include <e32base.h>
#include "MLogsReaderConfig.h"

// FORWARD DECLARATION

// CLASS DECLARATION

/**
 *  Implements interface for Logs reader configuration
 */
class CLogsReaderConfig :  public CBase, public MLogsReaderConfig
    {

    public:

       /**
        * Two-phased constructor.
        *
        * @param    aFilter filter
        * @param    aNumber number
        * @param    aRemoteParty remote party
        * @return CLogsReaderConfig object
        */
        static CLogsReaderConfig* NewLC( const TLogsFilter aFilter,
                                        const TDesC* aNumber,
                                        const TDesC* aRemoteParty );

       /**
        * Two-phased constructor.
        *
        * @param    aNumber number
        * @return CLogsReaderConfig object
        */
        static CLogsReaderConfig* NewLC( const TDesC* aNumber ); 

       /**
        * Two-phased constructor.
        *
        * @param    aNumber number
        * @param    aRemoteParty remote party
        * @param    aContackLink contact link
        * @return CLogsReaderConfig object
        */
        static CLogsReaderConfig* NewLC( const TDesC* aNumber,
                                        const TDesC* aRemoteParty, 
                                        const TDesC8* aContackLink ); 
    
       /**
        * Destructor.
        */
        virtual ~CLogsReaderConfig();

    private:

       /**
        * C++ default constructor.
        */
        CLogsReaderConfig();

       /**
        * Constructor.
        *
        * @param aFilter filter
        */
        CLogsReaderConfig( const TLogsFilter aFilter );
              
       /**
        * Second phase of construction
        *
        * @param    aNumber number
        * @param    aRemoteParty remote party
        */
        void ConstructL( const TDesC* aNumber, const TDesC* aRemoteParty, const TDesC8* aContactLink );


    public: // from MLogsReaderConfig

        TLogsFilter Filter() const;      
        TDesC& Number() const;       
        TDesC& RemoteParty() const; 
        TDesC8& ContactLink() const;
       
    private:    // data

        /// own: filter
        TLogsFilter     iFilter;

        /// own: number
        HBufC*  iNumber;

        /// own: remote party
        HBufC*  iRemoteParty;
        
        /// own: Contack Link
        HBufC8*  iContactLink;
    };

#endif      // __Logs_Engine_CLogsReaderConfig_H__



// End of File
