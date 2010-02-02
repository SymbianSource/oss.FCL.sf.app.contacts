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
*     Prepend dialog.
*
*/


#ifndef __CPbkPrependDlg_H__
#define __CPbkPrependDlg_H__

//  INCLUDES
#include <AknQueryDialog.h>     // CAknTextQueryDialog

//  CLASS DECLARATION
/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook Prepend dialog. 
 */
class CPbkPrependDlg :
        public CAknTextQueryDialog
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aDataText data text
         */
        IMPORT_C static CPbkPrependDlg* NewL(TDes& aDataText);

        /**
         * Destructor.
         */
        ~CPbkPrependDlg();

    public: // from CAknTextQueryDialog
    	TBool OkToExitL(TInt aButtonId);
        TKeyResponse OfferKeyEventL
            (const TKeyEvent& aKeyEvent, TEventCode aType);

    private:  // implementation
        CPbkPrependDlg(TDes& aDataText);
    };

#endif // __CPbkPrependDlg_H__

// End of File
