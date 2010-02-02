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
* Description:  Phonebook 2 server application title pane manager.
*
*/


#ifndef TPBK2TITLEPANEOPERATOR_H
#define TPBK2TITLEPANEOPERATOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Phonebook 2 server application title pane manager.
 */
class TPbk2TitlePaneOperator
    {
    public: // Construction

        /**
         * Constructor.
         */
        TPbk2TitlePaneOperator();

    public: // Interface

        /**
         * Sets the title pane.
         *
         * @param aTitlePaneText        Title pane text to set. If NULL,
         *                              Phonebook title is used.
         *                              Ownership of the text is taken!
         */
        void SetTitlePaneL(
                HBufC* aTitlePaneText );

    private: // Implementation
        void ChangeTitlePaneL(
                HBufC* aTitle );
        void ChangeContextIconL();
    };

#endif // TPBK2TITLEPANEOPERATOR_H

// End of File
