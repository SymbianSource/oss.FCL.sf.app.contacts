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
* Description:  Phonebook 2 memory entry contact loader.
*
*/


#ifndef MPBK2VIEWLOADEROBSERVER_H
#define MPBK2VIEWLOADEROBSERVER_H

//  INCLUDES

// FORWARD DECLARATIONS
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 view loader
 * Responsible for:
 *  
 */
class MPbk2ViewLoaderObserver

    {
    public: 
        /**
         * Destructor.
         */
        virtual ~MPbk2ViewLoaderObserver(){}
    
    public: // Interface
        
        /**
         * Called when a loaded view is ready to use.
         *
         * @param aView loaded view
         */
        virtual void ViewLoaded( MVPbkContactViewBase& aView ) = 0;
        
        /**
         * Called when an error occurs in the loading
         *
         * @param aView loaded view
         * @param aError An error code of the failure.
         */
        virtual void ViewError( 
                MVPbkContactViewBase& aView, 
                TInt aError ) = 0;
        

    };

#endif // MPBK2VIEWLOADEROBSERVER_H

// End of File
