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
*     Factory for Phonebook's app views.
*
*/


#ifndef __PbkAppViewFactory_H__
#define __PbkAppViewFactory_H__

//  INCLUDES
#include <e32std.h>  // for TUid
#include <e32base.h>

//  FORWARD DECLARATIONS
class CAknView;
class MPbkKeyEventHandler;
class CPbkExtGlobals;

// CLASS DECLARATION

/**
 * Phonebook Application View factory class. CPbkAppUi uses this class to create the application
 * view objects. Removes direct dependencies from CPbkAppUi to application view classes.
 *
 * @see CPbkAppUi::ConstructL
*/
class CPbkAppViewFactory : public CBase
    {
    public: // Constructors and destructor
        /** 
         * Creates a CPbkAppViewFactory instance.
         */
        static CPbkAppViewFactory* NewL();

        /** 
         * Creates a CPbkAppViewFactory instance and leaves it to cleanup 
         * stack.
         */
        static CPbkAppViewFactory* NewLC();

        /**
         * Destructor.
         */
        ~CPbkAppViewFactory();

    public:  // interface
        /**
         * Creates and returns application view with the view's id.
         *
         * @param aId the view's ID
         * @return the created view object
         *
         * @exception KErrNoMemory  if out of memory.
         * @exception anything thrown during the intialisation of the view.
         */
        CAknView* CreateAppViewL(TUid aId);

        /**
         * As CreateAppViewL but leaves the created view to the cleanup stack.
         * @see CPbkAppViewFactory::CreateAppViewL
         */
        CAknView* CreateAppViewLC(TUid aId);

    private:  // implementation
        CPbkAppViewFactory();
        void ConstructL();
        
    private: // Data
        /// Own: for creating an extension view
        CPbkExtGlobals* iExtGlobal;
    };


#endif // __PbkAppViewFactory_H__

// End of File
