/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A dialog for asking fixed dialling phone number.
*
*/



#ifndef CPSU2NUMBERQUERYDLG_H
#define CPSU2NUMBERQUERYDLG_H

//  INCLUDES
#include <AknQueryDialog.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  A dialog for asking phone number.
*
*/
class CPsu2NumberQueryDlg : public CAknTextQueryDialog
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aSourceNumber A source number
        * @param aNumberBuffer an enough big buffer for number
        * @return a new CPsu2NumberQueryDlg instance.
        */
        static CPsu2NumberQueryDlg* NewL( const TDesC& aSourceNumber,
            TDes& aNumberBuffer );

        /**
        * Destructor.
        */
        virtual ~CPsu2NumberQueryDlg();

        TBool ExecuteLD();

    protected: // Functions from base classes

        /**
        * From CAknTextQueryDialog
        */
        void UpdateLeftSoftKeyL();

        /**
        * From CAknTextQueryDialog
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
            TEventCode aType );


    private:

        /**
        * C++ default constructor.
        */
        CPsu2NumberQueryDlg( TDes& aNumberBuffer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aSourceNumber );
        
        void MakeMiddleSoftkeyVisibleL( TBool aVisible );

    private:    // Data
        // ETrue if number is ok.
        TBool iNumberAccepted;
    };

#endif      // CPSU2NUMBERQUERYDLG_H

// End of File
