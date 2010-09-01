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
* Description:  The sim store subsession class
*
*/



#ifndef RVPBKSIMSTORE_H
#define RVPBKSIMSTORE_H

//  INCLUDES
#include <e32std.h>
#include <VPbkSimStoreCommon.h>
#include <VPbkSimServerCommon.h>
#include <MVPbkSimPhone.h>
#include <RVPbkStreamedIntArray.h>

// FORWARD DECLARATIONS
struct TVPbkGsmStoreProperty;
struct TVPbkUSimStoreProperty;

// CLASS DECLARATION

/**
*  The sim store subsession class
*
*/
class RVPbkSimStore : public RSessionBase
    {
    public: // Construction

        /**
        * C++ default constructor
        */
        IMPORT_C RVPbkSimStore();

    public: // New functions
        
        /**
        * Starts the server if not started and creates a connection to it.
        */
        IMPORT_C void ConnectToServerL();

        /**
        * Closes this session
        */
        IMPORT_C void Close();

        /**
        * Opens the store in the server side.
        * @param aSecurityInfo Security information of client.
        * @param aIdentifier one of the identifiers defined in
        *        VPbkSimStoreCommon.h
        */
        IMPORT_C void OpenL( const TSecurityInfo& aSecurityInfo, 
                             TVPbkSimStoreIdentifier aIdentifier );

        /**
        * Cancels asynchronous store request.
        * @param aReqToCancel an asynchronous operation code 
        *        from VPbkSimServerOpCodes.h
        */
        IMPORT_C void CancelAsyncRequest( TInt aReqToCancel );
            
        /**
        * Starts listening to store events. This should be called before OpenL
        * to get the EVPbkSimStoreOpen event. Client should keep this active
        * all the time to get EVPbkSimStoreNotAvailable events.
        * @param aStatus the status that is completed by the server
        *   after request is done.
        * @param aEvent the event that is updated by the server
        * @param aError KErrNone if event is EVPbkSimStoreOpen or 
        *               EVPbkSimStoreNotAvailable
        *               KErrNotSupported if the event is EVPbkSimStoreError and
        *               (U)SIM card doesn't support the store.
        *               Other system error like KErrNoMemory.
        * @exception KErrAlreadyExists if already called
        */
        IMPORT_C void ListenToStoreEvents( TRequestStatus& aStatus,
            TVPbkSimContactEventData& aEvent );

        /**
        * Returns the contact in the specified index in ETel format or NULL if
        * contact was not found
        * @param aSimIndex the sim index of the contact
        * @return a contact or NULL
        */
        IMPORT_C HBufC8* ReadLC( TInt aSimIndex );

        /**
        * Saves a contact to the (U)SIM.
        * @param aStatus the status that is completed after request is done.
        * @param aData the contact in ETel format
        * @param aSimIndex the sim index to save or 
        *       VPbkSimStoreImpl::KFirstFreeSimIndex
        */
        IMPORT_C void SaveL( TRequestStatus& aStatus, const TDesC8& aData, 
            TInt& aSimIndex );
        
        /**
        * Deletes many contacts from the (U)SIM
        * @param aStatus the status that is completed after request is done.
        * @param aSimIndexes array of indexes to be deleted
        */
        IMPORT_C void DeleteL( TRequestStatus& aStatus, 
                              RVPbkStreamedIntArray& aSimIndexes );

        /**
        * Fills the GSM store properties
        * @param aGSMProperties the GSM store properties 
        * @return KErrNone or system wide error code
        */
        IMPORT_C TInt GetGsmStoreProperties( TVPbkGsmStoreProperty& 
                                                    aGsmProperties ) const;

        /**
        * Fills the USIM store properties. If the store doesn't support
        * USIM properties then all the values are KVPbkSimStorePropertyUndefined
        * @param aUSimProperties the USIM store properties
        * @return KErrNone or system wide error code
        */
        IMPORT_C TInt GetUSimStoreProperties( TVPbkUSimStoreProperty& 
                                                    aUSimProperties ) const;

        /**
        * Finds a phone number match from the store
        * Completes with KErrNotFound if there was no match.
        * Completes with KErrOverflow if the result buffer was too small
        *
        * @param aStatus the status that is completed after request is done.
        * @param aPhoneNumber the phone number to match
        * @param aMaxMatchDigits the maximum digits from the end of to number
        *        to use in match
        * @param aSimIndexBuffer a buffer for results that are sim indexes
        */
        IMPORT_C void MatchPhoneNumber( TRequestStatus& aStatus,
            const TDesC& aPhoneNumber, TInt aMaxMatchDigits,
            TDes8& aSimIndexBuffer );
        
        /**
        * Finds a match for a text string.
        * Completes with KErrNotFound if there was no match.
        * Completes with KErrOverflow if the result buffer was too small
        * Completes with KErrArgument if invalid field types
        *
        * @param aStatus the status that is completed after request is done.
        * @param aStringToFind the text string to find. This must exist as long
        *        as the request is handled.
        * @param aFieldTypes a descriptor containing TVPbkSimCntFieldType
        *        RVPbkSimFieldTypeArray can be used to externalize types.
        * @param aSimIndexBuffer a buffer for results, the matched sim indexes
        *        will be appended to this buffer.
        *        RVPbkStreamedIntArray can be used to interalize indexes.
        */
        IMPORT_C void Find( TRequestStatus& aStatus,
            const TDesC& aStringToFind, const TDesC8& aFieldTypes,
            TDes8& aSimIndexBuffer );
            
        /**
         * Opens the phone in server side.
         */
        IMPORT_C void OpenPhoneL();

        /**
         * Closes the phone in server side.
         */
        IMPORT_C void ClosePhone();

        /**
         * Returns whether the usim access is supported or not.
         * @return ETrue if usim access is supported, otherwise EFalse.
         */
        IMPORT_C TBool USimAccessSupported() const;

        /**
         * Returns the service table
         * @return service table
         */
        IMPORT_C TUint32 ServiceTable() const;

        /**
         * Returns fixed dialling status
         * @return FDN status
         */
        IMPORT_C MVPbkSimPhone::TFDNStatus FixedDialingStatus() const;            

    private:    // Data
        /// A descriptor for the store event
        TPtr8 iEvent;
        /// A descriptor for the store error
        TPtr8 iStoreError;
        /// The sim index of the saved contact
        TPtr8 iSavedSimIndex;
        /// Sim Index buffer
        HBufC8* iIndexBuffer;
    };

#endif      // RVPBKSIMSTORE_H
            
// End of File
