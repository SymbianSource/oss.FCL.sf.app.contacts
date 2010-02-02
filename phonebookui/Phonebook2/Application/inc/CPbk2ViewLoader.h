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
* Description:  Phonebook 2 view loader
*
*/


#ifndef CPBK2VIEWLOADER_H
#define CPBK2VIEWLOADER_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactViewObserver.h>


// FORWARD DECLARATIONS
class MPbk2ViewLoaderObserver;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 view loader
 * Responsible for:
 *  - Load given view and inform when it is ready to use.
 */
NONSHARABLE_CLASS( CPbk2ViewLoader ): public CBase,
                                      public MVPbkContactViewObserver

    {    
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aObserver view loader's observer
         * @return  A new instance of this class.
         */
        static CPbk2ViewLoader* NewL(
                MVPbkContactViewBase& aView,
                MPbk2ViewLoaderObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ViewLoader();
        
    private: // From MVPbkContactViewObserver
        /**
         * If subclass overwrites this it must first call the
         * base class version.
         */
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        /**
         * If subclass overwrites this it must first call the
         * base class version.
         */
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        /**
         * If subclass overwrites this it must first call the
         * base class version.
         */
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        /**
         * If subclass overwrites this it must first call the
         * base class version.
         */
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        /**
         * If subclass overwrites this it must first call the
         * base class version.
         */
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );        

    private: // Implementation
        CPbk2ViewLoader( 
                MVPbkContactViewBase& aView,
                MPbk2ViewLoaderObserver& aObserver );
        void ConstructL();

    private: // Data       
        /// Ref: Loaded view
        MVPbkContactViewBase& iView;
        /// Ref: Observer view loading
        MPbk2ViewLoaderObserver& iObserver;
    };

#endif // CPBK2VIEWLOADER_H

// End of File
