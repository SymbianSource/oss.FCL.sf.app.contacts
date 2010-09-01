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
*       Provides some additional common application view functionality. 
*
*/


#ifndef __CPbkAppView_H__
#define __CPbkAppView_H__


//  INCLUDES
#include <CPbkAppViewBase.h>    // CPbkAppViewBase

// FORWARD DECLARATIONS
class CPbkAppUi;

// CLASS DECLARATION

/**
 * Provides some additional common application view functionality. 
 */
class CPbkAppView : public CPbkAppViewBase
    {
    public:  // Constructors and destructor
		/**
		 * Destructor.
		 */
        ~CPbkAppView();

    public:  // Interface
        /**
         * Returns the Phonebook application UI object.
         */
        CPbkAppUi* PbkAppUi() const;

    protected: 
        /**
         * Constructor.
         */
        CPbkAppView();

    };

#endif  // __CPbkAppView_H__

// End of File
