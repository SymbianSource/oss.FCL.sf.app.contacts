/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Error function and panic codes for sim server debug compilation
*
*/



#ifndef VPBKSIMSERVER_SIMSERVERERRORS_H
#define VPBKSIMSERVER_SIMSERVERERRORS_H

//  INCLUDES
#include <e32std.h>

/**
* VPbkSimStore internal error codes and functions for
* debug code
*/
namespace VPbkSimServer
    {
    /// Panic category for sim server
    _LIT( KPanicCategory, "VPbkSimServer" );

    /**
    * Panic codes
    */
    enum TPanicCode
        {
        ECommandObserverAlreadySetInSaveCmd,
        EIncorrectContactEventInSaveCmd,
        EInvalidHandleInSaveCmd,
        ECommandObserverAlreadySetInDeleteCmd,
        EIncorrectContactEventInDeleteCmd,
        EInvalidHandleInDeleteCmd,
        ESaveCmdAlreadyActive,
        EInvalidHandleInView,
        EMissingOpCodeDebugText
        };

    enum TClientPanicCode
        {
        // Server doesn't implement the service that client
        // is requesting
        EInvalidOpCode = 1,
        // Server doesn't implement the canceling service that client
        // is requesting
        EInvalidCancelOpCode,
        // Client hasn't open the store but makes a requests to service
        // that needs an open store.
        ECVPbkStoreSession_StoreServiceL,
        // Client has done a delete request and is trying to do another one
        // before the first delete request has completed.
        ECVPbkStoreSession_DeleteContactL,
        // Client has done a save request and is trying to do another one
        // before the first save request has completed.
        ECVPbkStoreSession_SaveContactL,
        // DEBUG time panic: view filtering using contact
        // matching prefix is in ivalid state. Client has
        // probably ignored an error and still using the view.
        ECVPbkViewSubSession_InvalidActiveMatchState,
        // DEBUG time panic: view filtering using contact
        // matching prefix is in ivalid state. Client has
        // probably ignored an error and still using the view.
        ECVPbkViewSubSession_InvalidNonActiveMatchState,
        // View is not in valid state. Client has probably
        // ignored an error and still using the view.
        ECVPbkViewSubSession_ReturnContactCountL,
        // View is not in valid state. Client has probably
        // ignored an error and still using the view.
        ECVPbkViewSubSession_GetViewContactL,
        // View is not in valid state. Client has probably
        // ignored an error and still using the view.
        ECVPbkViewSubSession_ChangeSortOrderL,
        // Client hasn't cancelled the view event message before
        // closing the view.
        ECVPbkViewSubSession_Destructor_EventMessage,
        // Client hasn't cancelled the contact match message before
        // closing the view.
        ECVPbkViewSubSession_Destructor_Match,
        // Client hasn't cancelled the store event message before
        // closing the store.
        ECVPbkStoreSession_CloseStore_EventMessage,
        // Client hasn't cancelled the async save before
        // closing the store
        ECVPbkStoreSession_CloseStore_Save,
        // Client hasn't cancelled the async delete before
        // closing the store
        ECVPbkStoreSession_CloseStore_Delete,
        // Client hasn't cancelled the async number match before
        // closing the store
        ECVPbkStoreSession_CloseStore_MatchNumber,
        // Client hasn't cancelled the async find before
        // closing the store
        ECVPbkStoreSession_CloseStore_Find
        };

    /**
     * Called if an unrecoverable error is detected.
     * Stops the thread with User::Panic.
     */
    inline void Panic( TPanicCode aPanicCode );

    /**
    * Panics client.
    * @param aMessage the message to use for panic
    * @param aReason one of the panic codes for the client
    */
    inline void PanicClient( const RMessage2& aMessage, 
        TClientPanicCode aReason );
    }

// INLINE FUNCTIONS
inline void VPbkSimServer::Panic( TPanicCode aPanicCode )
    {
    User::Panic( KPanicCategory, aPanicCode );
    }

inline void VPbkSimServer::PanicClient( const RMessage2& aMessage, 
    TClientPanicCode aPanicCode )
	{
	aMessage.Panic( KPanicCategory, aPanicCode );
	}

#endif      // VPBKSIMSERVER_SIMSERVERERRORS_H
            
// End of File
