/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 prepend dialog.
*
*/


#ifndef CPBK2PREPENDDLG_H
#define CPBK2PREPENDDLG_H

// INCLUDES
#include <AknQueryDialog.h>

// CLASS DECLARATION

/**
 * Phonebook 2 prepend dialog.
 */
class CPbk2PrependDlg : public CAknTextQueryDialog
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aDataText     Data text.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2PrependDlg* NewL(
                TDes& aDataText );

        /**
         * Destructor.
         */
        ~CPbk2PrependDlg();

    public: // From CAknTextQueryDialog
        TBool OkToExitL(
                TInt aButtonId );
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2PrependDlg(
                TDes& aDataText );
    };

#endif // CPBK2PREPENDDLG_H

// End of File
