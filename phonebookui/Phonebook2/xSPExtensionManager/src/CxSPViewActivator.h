/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       View activator - activates a tab view upon client request
*
*/


#ifndef __XSPVIEWACTIVATOR_H__
#define __XSPVIEWACTIVATOR_H__

// INCLUDES
#include <e32svr.h>
#include <e32base.h>

#include <CPbk2ViewState.h>

#include "CxSPViewIdChanger.h"
#include "xSPViewServices_priv.h"


// CLASS DECLARATION
/**
 * This class is a server which listens to client application
 * requests and activates the requested tab view according
 * to the client request.
 */
class CxSPViewActivator : public CServer2
    {
    public:  // Construction and destruction

        /**
         * Static factory function for creating an instance of the class.
         *
         * @param aViewIdChanger view id changer
         * @return Newly created CxSPViewActivator server object.
         */
        static CxSPViewActivator* NewL(CxSPViewIdChanger* aViewIdChanger);

        /**
         * Standard C++ destructor.
         */
        virtual ~CxSPViewActivator();

    public: // Public methods

        /**
         * Executes client request to activate a given view. May leave on if the
         * request cannot be carried out.
         *
         * @param aMessage Client request
         */
        void ActivateView1L(const RMessage2 &aMessage);

        /**
         * Executes client request to activate a given view. May leave on if the
         * request cannot be carried out.
         *
         * @param aMessage Client request
         */
        void ActivateView2L(const RMessage2 &aMessage);

        /**
         * Executes client request to activate a given view. May leave on if the
         * request cannot be carried out. Asynchronous version.
         *
         * @param aMessage Client request
         */
        void ActivateView2AsyncL(const RMessage2 &aMessage);

        /**
         * Executes client request to get view count.
         *
         * @param aMessage Client request
         */
        void GetViewCountL(const RMessage2 &aMessage);

        /**
         * Executes client request to get view packed data length.
         *
         * @param aMessage Client request
         */
        void GetViewDataPackLengthL(const RMessage2 &aMessage);

        /**
         * Executes client request to get view data.
         *
         * @param aMessage Client request
         */
        void GetViewDataL(const RMessage2 &aMessage);

    private:

        /**
         * Performs the 2nd phase of the construction.
         */
        void ConstructL();

        /**
         * Standard C++ constructor.
         *
         * @param aViewIdChanger view id changer
         */
        CxSPViewActivator(CxSPViewIdChanger* aViewIdChanger);

        /**
         * Creates a new client session when a client contacts the server for the first time.
         * @param aVersion Client's SW version
         * @return A new session
         */
        CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& ) const;

        /**
         * Starts the server and signals the client application that the server
         * is ready to receive requests.
         *
         * @param aName Server name
         */
        void StartL(const TDesC &aName);

    private:    // Data

        ///Ref: view id changer
        CxSPViewIdChanger* iViewIdChanger;
    };

// CLASS DECLARATION
/**
 * This class is the server session class,
 * which dispatches client requests.
 */
class CxSPViewActivatorSession : public CSession2
    {
    public:

        /**
         * Called when the client makes a request to change the view.
         * @param aMessage Contains details about the view to bring in front.
         */
        void ServiceL(const RMessage2 &aMessage);
    };

#endif // __XSPVIEWACTIVATOR_H__

// End of file.
