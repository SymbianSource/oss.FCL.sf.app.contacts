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
* Description:  Message code for the virtual phonebook sim server
*
*/



#ifndef VPBKSIMSERVEROPCODES_H
#define VPBKSIMSERVEROPCODES_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// The amount of asynchronous operations
// Needed to reserve message slots for the server
const TInt KVPbkSimSrvAsyncOperations = 7;

// DATA TYPES
enum TVPbkSimServerOpCodes
    {
    /**
    * Adds the client to store event array. Clients
    * are notfied when the state of the store changes.
    * Asynchronous.
    */
    EVPbkSimSrvStoreEventNotification = 1,

    /**
    * Closes the store.
    * Synchronous
    */
    EVPbkSimSrvCloseStore = 2,

    /**
    * Cancels a request 
    * Synchronous
    */
    EVPbkSimSrvCancelAsyncRequest = 3,

    /**
    * Opens a store. After this is done the store
    * ready for other requests
    * Synchronous
    */
    EVPbkSimSrvOpenStore = 4,
    
    /**
    * Gets the size of the contact and loads it to server side
    * Synchronous
    */
    EVPbkSimSrvGetSizeAndLoadContact = 5,

    /**
    * Reads the last loaded contact from the store
    * Synchronous
    */
    EVPbkSimSrvGetContact = 6,

    /**
    * Saves a contact to the (U)SIM
    * Asynchronous
    */
    EVPbkSimSrvSaveContact = 7,

    /**
    * Deletes a contact from the (U)SIM
    * Asynchronous
    */
    EVPbkSimSrvDeleteContact = 8,

    /**
    * Gets the basic gsm store properties
    * Synchronous
    */
    EVPbkSimSrvGsmStoreProperties = 9,

    /**
    * Gets the usim store properties
    * Synchronous
    */
    EVPbkSimSrvUSimStoreProperties = 10,

    /**
    * Finds a phone number match from the store
    * Asynchronous
    */
    EVPbkSimSrvMatchPhoneNumber = 11,
    
    /**
    * Finds a match from fields with given field types
    * Asynchronous
    */
    EVPbkSimSrvFind = 12,
    
    // VIEW MESSAGES

    /**
    * Opens a view subsession
    * Synchronous
    */
    EVPbkSimSrvOpenView = 101,

    /**
    * Closes a view subsession
    * Synchronous
    */
    EVPbkSimSrvCloseView = 102,

    /**
    * Activate view event notification
    * Asynchronous
    */
    EVPbkSimSrvViewEventNotification = 103,

    /**
    * Gets the amount contacts in the view
    * Synchronous
    */
    EVPbkSimSrvViewCount = 104,

    /**
    * Returns a view contact
    * Synchronous
    */
    EVPbkSimSrvGetViewContact = 105,

    /**
    * Starts reordering of contacts in the view
    * Synchronous
    */
    EVPbkSimSrvChangeViewSortOrder = 106,

    /**
    * Finds the current view index related to sim index
    * Synchronous
    */
    EVPbkSimSrvFindViewIndex = 107,
    
    /**
    * Contact matching prefix    
    * Asynchronous
    */
    EVPbkSimSrvContactMatchingPrefix = 108,
    
    /**
    * Get contact matching results
    * Synchronous
    */
    EVPbkSimSrvContactMathingResult = 109,
    
    /**
    * Get extenrnalized size of the sort order
    * Synchronous
    */
    EVPbkSimSrvViewSortOrderSize = 110,
    
    /**
    * Get sort order
    * Synchronous
    */
    EVPbkSimSrvViewSortOrder = 111,
    
    /// PHONE MESSAGES
    /**
     * Opens the phone
     * Synchronous
     */
    EVPbkSimSrvOpenPhone = 401,

    /**
     * Closes the phone
     * Synchronous
     */
    EVPbkSimSrvClosePhone = 402,
    
    /**
     * Gets the usim access support data
     * Synchronous
     */
    EVPbkSimSrvUSimAccessSupported = 403,   

    /**
     * Gets the service table
     * Synchronous
     */
    EVPbkSimSrvServiceTable = 404,    

    /**
     * Gets the fdn status
     * Synchronous
     */
    EVPbkSimSrvFixedDiallingStatus = 405    
    };

#endif      // VPBKSIMSERVEROPCODES_H

// End of File
