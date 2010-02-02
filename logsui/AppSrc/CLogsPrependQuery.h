/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Prepend query definition
*
*/


#ifndef     __LOGS_APP_CLOGSPREPENDQUERY_H__
#define     __LOGS_APP_CLOGSPREPENDQUERY_H__

//  INCLUDES
#include <AknQueryDialog.h>

//  CLASS DECLARATIONS

/*
 * Prepend query
 * @since 2.7
 */
class CLogsPrependQuery : public CAknTextQueryDialog
    {
    public:
        /**
         * Instantiate prepend query
         * @param aResource : query resource
         */
        static CLogsPrependQuery* NewL( TDes& aResource );

        /**
         * Destructor
         */
        virtual ~CLogsPrependQuery();

    private:
        /**
         * C++ constructor
         * @param aResource : query resource
         */
        CLogsPrependQuery( TDes& aResource );

        /**
         * From CCoeControl
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );
    };

#endif  // __LOGS_APP_CLOGSPREPENDQUERY_H__

// end of file
