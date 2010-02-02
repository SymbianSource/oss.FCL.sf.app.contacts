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
*     Class which loads Phonebook's or empty icon into context pane.
*
*/


#ifndef __CPbkSwapContextPaneIcon_H__
#define __CPbkSwapContextPaneIcon_H__


//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CEikonEnv;
class CEikImage;
class CAknContextPane;
class CFbsBitmap;


// CLASS DECLARATION

/**
 * Class which loads Phonebook's or empty icon into context pane.
 * Class swap those icons, but don't restore previous icon.
 * Class start swappipng to phonebook's icon 
 *
*/
NONSHARABLE_CLASS(CPbkSwapContextPaneIcon) :
        public CBase
    {
    public:
        enum TSwapState
            {
            EEmptyIcon,
            EPhonebookIcon    
            };
    
    public:  // Constructors and destructor
        /**
         * Creates an instance of this class and puts Phonebook's icon into
         * current application's context pane.
         *
         * @param aEikonEnv EIKON environment where to fetch the status pane.
         */
        static CPbkSwapContextPaneIcon* NewL(CEikonEnv& aEikonEnv);

        /**
         * Destructor. Restores the previous icon into context pane.
         */
        ~CPbkSwapContextPaneIcon();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aEikonEnv
        */
        void ConstructL(CEikonEnv& aEikonEnv);

    public: //implementation        
        /**
         * Show loaded or previous icon
         * @param aState TSwapState
         */         
        void ShowContextPaneIcon(const TSwapState aState);

    private: // Implementation
        CPbkSwapContextPaneIcon();
        /**
         * Set phonebook's context pane icon
         */
        void SetPhonebookContextPaneIconL();

        /**
         * Load empty context pane icon
         */
        void LoadEmptyContextPaneIconL();

    private:  // data
        //Own: Own
        CEikImage* iContextPaneIcon;
        /// Own: TSwapState
        TSwapState iState;
        /// Ref: current application context pane
        CAknContextPane* iContextPane;
        
    };

#endif // __CPbkSwapContextPaneIcon_H__

// End of File
