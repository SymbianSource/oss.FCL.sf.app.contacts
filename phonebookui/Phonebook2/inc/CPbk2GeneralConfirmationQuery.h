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
* Description:  Phonebook 2 general confirmation query.
*
*/


#ifndef CPBK2GENERALCONFIRMATIONQUERY_H
#define CPBK2GENERALCONFIRMATIONQUERY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAknQueryDialog;
class MVPbkBaseContact;

// CLASS DECLARATION

/**
 * Phonebook 2 general confirmation query.
 */
class CPbk2GeneralConfirmationQuery : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2GeneralConfirmationQuery* NewL();

        /**
         * Destructor.
         */
        ~CPbk2GeneralConfirmationQuery();

    public: // Interface

        /**
         * Executes the dialog.
         *
         * @param aContact  The contact whose name is used in the dialog.
         * @param aResId    Resource id for the text to be shown.
         * @return  Greater than zero if the user confirmed.
         *          Less than zero or zero in other cases.
         */
        IMPORT_C TInt ExecuteLD(
                const MVPbkBaseContact& aContact,
                TInt aResId );


        /**
         * Executes this dialog to confirm deletion of a multiple contacts.
         *
         * @param aAmountOfContacts     The amount of contacts to
         *                              operate with. Should be greater
         *                              than one.
         * @param aResId                Resource id for the text
         *                              to be shown.
         * @return  Greater than zero if the user confirmed.
         *          Less than zero or zero in other cases.
         */
        IMPORT_C TInt ExecuteLD(
                TInt aAmountOfContacts,
                TInt aResId );
        /**
         * Executes the dialog.
         *
         * @param aContact     The contact whose name is used in the dialog.
         * @param aResId       Resource id for the text to be shown.
         * @param aFormatFlag  Format flag info for string 
         * @return  Greater than zero if the user confirmed.
         * Less than zero or zero in other cases.
         */
        IMPORT_C TInt ExecuteLD
                ( const MVPbkBaseContact& aContact, TInt aResId, TInt aFormatFlag );

        /**
         * Force the query dialog to terminate as getting choice of 
         * right softkey
         */
        IMPORT_C void StopQuery();

    private: // Implementation
        CPbk2GeneralConfirmationQuery();

    private:
        CAknQueryDialog* iQueryDlg;
    };

#endif // CPBK2GENERALCONFIRMATIONQUERY_H

// End of File
