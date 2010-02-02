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
* Description:  An API for SIM stores
*
*/



#ifndef MVPBKSIMCNTSTORE_H
#define MVPBKSIMCNTSTORE_H

//  INCLUDES
#include <e32std.h>
#include "VPbkSimStoreCommon.h"

// FORWARD DECLARATIONS
class MVPbkSimContactObserver;
class MVPbkSimStoreObserver;
class RVPbkSimFieldTypeArray;
class MVPbkSimCntView;
class MVPbkSimFindObserver;
class MVPbkSimContact;
class CVPbkETelCntConverter;
class MVPbkSimStoreOperation;
struct TVPbkGsmStoreProperty;
struct TVPbkUSimStoreProperty;
class CVPbkSimFieldTypeFilter;
class RVPbkStreamedIntArray;

// CLASS DECLARATION

/**
 *  An API for SIM contact stores.
 *
 */
class MVPbkSimCntStore
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MVPbkSimCntStore() {}

        /**
         * Returns the store identifier
         *
         * @return The store identifier.
         */
        virtual TVPbkSimStoreIdentifier Identifier() const = 0;

        /**
         * Opens the store asynchronously.
         * Calls back the observers attached to this contact store when the
         * opening completes.
         *
         * @param aObserver Observer.
         */
        virtual void OpenL(
                MVPbkSimStoreObserver& aObserver ) = 0;

        /**
         * Closes this contact store from a single observer. The observer will
         * no longer receive events from this store.
         *
         * @param aObserver Observer.
         */
        virtual void Close(
                MVPbkSimStoreObserver& aObserver ) = 0;

        /**
         * Creates a new contact view that contains the contacts of this 
         * store.
         *
         * If the view is a shared view and it has already been created
         * then aSortOrder and aConstructionPolicy will be ignored.
         *
         * @param aSortOrder             The sort order for the view.
         * @param aConstructionPolicy    Defines the way the view
         *                               is constructed.
         * @param aFilter                Field type filter. The view
         *                               will contain only contacts
         *                               having the field types of the filter.
         *                               Ownership of the filter must be taken.
         * @return   A new contact view.
         */
        virtual MVPbkSimCntView* CreateViewL( 
                const RVPbkSimFieldTypeArray& aSortOrder,
                TVPbkSimViewConstructionPolicy aConstructionPolicy,
                const TDesC& aViewName,
                CVPbkSimFieldTypeFilter* aFilter ) = 0;
        
        /**
         * Returns a reference to the contact in ETel format or NULL.
         *
         * Client can not save the reference for later use because
         * next call to ContactAtL can invalidate the saved reference.
         *
         * @param aSimIndex The index of the contact in the store.
         * @return The ETel contact or NULL if the given index is empty
         */
        virtual const TDesC8* AtL( TInt aSimIndex ) = 0;
        
        /**
         * Returns a reference to the contact or NULL.
         *
         * Client can not save the reference for later use because
         * next call to ContactAtL can invalidate the saved reference.
         *
         * @param aSimIndex The index of the contact in the store.
         * @return The contact or NULL if the given index is empty
         */
        virtual const MVPbkSimContact* ContactAtL( TInt aSimIndex ) = 0;

        /**
         * Saves a contact asynchronously into the store.
         * It's not possible to call this if saving is already in progress.
         *
         * @param aData     The contact data in the format defined
         *                  by the Symbian MM ETel (CPhoneBookBuffer).
         * @param aSimIndex The index for the contact or 
         *                  VPbkSimStoreImpl::KFirstFreeSimIndex if
         *                  new contact (after saving the index
         *                  contains the new index).
         * @param aObserver The observer that is notified after saving.
         * @return a handle to an asynchrnous operation. Client
         *         has the ownership and can cancel the operation
         *         by deleting the operation
         */
        virtual MVPbkSimStoreOperation* SaveL(
                const TDesC8& aData,
                TInt& aSimIndex, 
                MVPbkSimContactObserver& aObserver ) = 0;

        /**
        * Deletes a contacts asynchronously from the store.
        * It's not possible to call this if deleting is already in progress.
        *
        * @param aSimIndexes array of contacts to be delete
        * @param aObserver the observer that is notified after deleting.
        * @return a handle to an asynchrnous operation. Client
        *         has the ownership and can cancel the operation
        *         by deleting the operation
        * @exception operation completes with KErrArgument if aSimIndexes
        *            is empty or contains only invalid SIM indexes.
        */
        virtual MVPbkSimStoreOperation* DeleteL( 
                RVPbkStreamedIntArray& aSimIndexes,
                MVPbkSimContactObserver& aObserver ) = 0;

        /**
         * Fills the GSM store properties
         *
         * @param aGsmProperties The GSM store properties.
         * @return  KErrNone if properties were successfully set. General error code.
         */
        virtual TInt GetGsmStoreProperties( 
                TVPbkGsmStoreProperty& aGsmProperties ) const = 0;

        /**
         * Fills the USIM store properties. If the store doesn't support
         * USIM properties then all the values are KVPbkSimStorePropertyUndefined
         *
         * @param aUSimProperties The USIM store properties.
         * @return  KErrNone if properties were successfully set. General error code.
         */
        virtual TInt GetUSimStoreProperties( 
                TVPbkUSimStoreProperty& aUSimProperties ) const = 0 ;
        
        /**
         * Returns the contact converter that is used to convert
         * native contacts to ETel contacts an vice versa.
         *
         * @return  A contact converter for native contact versus
         *          ETel contact conversion.
         */
        virtual const CVPbkETelCntConverter& ContactConverter() const = 0;
        
        /**
         * Finds a contact match for the phone number.
         *
         * @param aPhoneNumber       The phone number to look for.
         * @param aMaxMatchDigits    The maximum amount of digits to compare
         *                           from the end of the number.
         * @param aObserver          The observer that is notified
         *                           with results.
         * @return   Match phone number operation.
        */
        virtual MVPbkSimStoreOperation* CreateMatchPhoneNumberOperationL(
                const TDesC& aPhoneNumber,
                TInt aMaxMatchDigits, 
                MVPbkSimFindObserver& aObserver ) = 0;
        
        /**
         * Finds a contact match for a string.
         *
         * @param aStringToFind The string to look for.
         * @param aFieldTypes   The types of the fields to find from.
         * @param aObserver     The observer that is notified with results.
         * @return  Find operation.
         */
        virtual MVPbkSimStoreOperation* CreateFindOperationL( 
                const TDesC& aStringToFind,
                RVPbkSimFieldTypeArray& aFieldTypes,
                MVPbkSimFindObserver& aObserver ) = 0;
        
        /**
         * Returns the maximum phonenumber length defined in Virtual Phonebook
         * Central Repository settings
         *
         * @return the system maximum length for phonenumber
         */
        virtual TInt SystemPhoneNumberMaxLength() const = 0;
    };

#endif      // MVPBKSIMCNTSTORE_H
            
// End of File
