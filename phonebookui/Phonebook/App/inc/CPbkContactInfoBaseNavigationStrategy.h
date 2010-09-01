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
*     Contact Info View common base navigation stategy.
*
*/


#ifndef __CPbkContactInfoBaseNavigationStrategy_H__
#define __CPbkContactInfoBaseNavigationStrategy_H__


//  INCLUDES
#include <e32base.h>
#include <cntviewbase.h>
#include "MPbkContactInfoNavigationStrategy.h"
#include <MPbkContactViewListControlObserver.h>


//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CAknNavigationDecorator;
class CContactGroupView;
class MPbkContactInfoNavigationCallback;


//  CLASS DECLARATION

/**
 * Contact Info View common base navigation stategy.
 */
class CPbkContactInfoBaseNavigationStrategy : 
        public CBase, 
        public MPbkContactInfoNavigationStrategy,
        public MContactViewObserver
    {
    protected:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         */
        CPbkContactInfoBaseNavigationStrategy(
                CPbkContactEngine& aEngine,
                MPbkContactInfoNavigationCallback& aCallback);

        /**
         * Destructor.
         */
        ~CPbkContactInfoBaseNavigationStrategy();

    private: // from MPbkContactInfoNavigationStrategy
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private: // from MContactViewObserver
        void HandleContactViewEvent(const CContactViewBase& aView,
			const TContactViewEvent& aEvent);

    private:  // from MPbkContactViewListControlObserver
        void HandleContactViewListControlEventL
            (CPbkContactViewListControl& aControl,
            const TPbkContactViewListControlEvent& aEvent);

    private: // Implementation
        void ConstructNaviIndicatorsL();
        void NavigateContactL(TInt aDir);
        void StatusPaneUpdateL();

    protected:  // data
        /// Ref: contact engine
        CPbkContactEngine& iEngine;
        /// Ref: contact navigation interface
        MPbkContactInfoNavigationCallback& iNavigationCallback;
        /// Own: navigation decorator
        CAknNavigationDecorator* iNaviDeco;
        /// Own: scrolling view
        CContactViewBase* iScrollView;
        /// Own: view load completed
        TBool iViewReady;
    };

#endif // __CPbkContactInfoBaseNavigationStrategy_H__
      
// End of File
