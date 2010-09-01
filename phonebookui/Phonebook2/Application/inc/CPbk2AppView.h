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
* Description:  Phonebook 2 application view.
*
*/


#ifndef CPBK2APPVIEW_H
#define CPBK2APPVIEW_H

//  INCLUDES
#include <CPbk2AppViewBase.h>

// FORWARD DECLARATIONS
class CPbk2AppUi;

// CLASS DECLARATION

/**
 * Phonebook 2 application view.
 */
class CPbk2AppView : public CPbk2AppViewBase
    {
    public: // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPbk2AppView();

    public: // Interface

        /**
         * Returns the Phonebook 2 application UI object.
         *
         * @return  Phonebook 2 application UI.
         */
        CPbk2AppUi* Pbk2AppUi() const;

    protected: // Implementation

        /**
         * Constructor.
         */
        CPbk2AppView();
    };

#endif  // CPBK2APPVIEW_H

// End of File
