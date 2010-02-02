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
*     Class which loads Phonebook's icon into context pane.
*
*/


#ifndef __CPbkContextPaneIcon_H__
#define __CPbkContextPaneIcon_H__


//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CEikonEnv;
class CEikImage;
class CAknContextPane;


// CLASS DECLARATION

/**
 * Class which loads Phonebook's icon into context pane. 
*/
NONSHARABLE_CLASS(CPbkContextPaneIcon) : 
        public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates an instance of this class and puts Phonebook's icon into
         * current application's context pane.
         *
         * @param aEikonEnv EIKON environment where to fetch the status pane.
         */
        static CPbkContextPaneIcon* NewL(CEikonEnv& aEikonEnv);

        /**
         * Destructor. Restores the previous icon into context pane.
         */ 
        ~CPbkContextPaneIcon();

    private: // Implementation
        CPbkContextPaneIcon();
        void StoreContextPaneIconL(CEikonEnv& aEikonEnv);
        void SetPhonebookContextPaneIconL();
        void RestoreContextPaneIcon();

    private:  // data
        /// Ref: current application context pane
        CAknContextPane* iContextPane;
        /// Own: previous context pane icon
        CEikImage* iPreviousContextPaneIcon;
    };

#endif // __CPbkContextPaneIcon_H__

// End of File
