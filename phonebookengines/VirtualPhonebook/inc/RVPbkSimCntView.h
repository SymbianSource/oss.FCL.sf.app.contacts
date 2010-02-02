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
* Description:  A SIM contact view subsession.
*
*/


#ifndef RVPBKSIMCNTVIEW_H
#define RVPBKSIMCNTVIEW_H

//  INCLUDES
#include <e32std.h>
#include <VPbkSimStoreCommon.h>
#include <VPbkSimServerCommon.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class RVPbkSimStore;
class RVPbkSimFieldTypeArray;
class CVPbkSimFieldTypeFilter;
struct TVPbkSimContactEventData;


// CLASS DECLARATION

/**
 *  A SIM contact view subsession.
 *
 */
class RVPbkSimCntView : public RSubSessionBase
    {
    public:  // Constructors and destructor
        
        /**
         * C++ constructor.
         */
        IMPORT_C RVPbkSimCntView();
        
    public: // New functions
        
        /**
         * Opens a view subsession.
         *
         * @param aStore                An open parent store session.
         * @param aSortOrder            The sortorder for the view.
         * @param aConstructionPolicy   Defines the way the view
         *                              is constructed.
         * @param aViewName             The name of the view for the
         *                              shared view or KNullDesC.
         * @param aFilter               Field type filter or NULL
         * @exception KErrArgument      If invalid sortorder or
         *                              store is not ready.
         */
        IMPORT_C void OpenL(
                RVPbkSimStore& aStore, 
                const RVPbkSimFieldTypeArray& aSortOrder,
                TVPbkSimViewConstructionPolicy aConstructionPolicy,
                const TDesC& aViewName,
                CVPbkSimFieldTypeFilter* aFilter );

        /**
         * Closes the view subsession
         */
        IMPORT_C void Close();

        /**
         * Cancels asynchronous store request.
         *
         * @param aReqToCancel  An asynchronous operation code
         *                      from VPbkSimServerOpCodes.h.
         */
        IMPORT_C void CancelAsyncRequest(
                TInt aReqToCancel );

        /**
         * Starts listening to view events.
         *
         * @param aStatus   The request status completed by the server.
         * @param aEvent    The view event data structure filled by the server.
         */
        IMPORT_C void ListenToViewEvents(
                TRequestStatus& aStatus,
                TVPbkSimContactEventData& aEvent );

        /**
         * Returns the number of contacts in the view.
         *
         * @return The number of contacts in the view.
         */
        IMPORT_C TInt CountL() const;

        /**
         * Returns a view contact in specified index.
         *
         * @param aIndex            The view index of the contact.
         * @exception KErrArgument  If invalid index.
         * @return  A pointer to view contact in ETel format. The contact.
         *          is valid until next call to this function.
         */
        IMPORT_C TPtr8 ContactAtL(
                TInt aIndex );

        /**
         * Starts reordering of the contacts in the view.
         *
         * @param aSortOrder    The new sort order.
         */
        IMPORT_C void ChangeSortOrderL(
                const RVPbkSimFieldTypeArray& aSortOrder );

        /**
         * Finds the view index of the contact whose sim index is given.
         *
         * @param aSimIndex     The SIM index of the contact.
         * @return  The view index or KErrNotFound.
         */
        IMPORT_C TInt FindViewIndexL(
                TInt aSimIndex );
                
        /**
        * Contact matching prefix
        *
        * @param aFindStrings
        * @param aResultBufferSize, contact matching result buffer's size
        * @param aStatus The request status completed by the server.
        */
        IMPORT_C void ContactMatchingPrefixL(
                const MDesCArray& aFindStrings,   
                TInt& aResultBufferSize,
                TRequestStatus& aStatus );
                
        /**
        * Get contact matching result, synchronous function                                       
        *
        * @param aSimMatchResultBufPtr
        */
        IMPORT_C void ContactMatchingResultL( 
                TPtr8& aSimMatchResultBufPtr );
        
        /**
        * Gets the sort order of the view.
        *
        * It can be internalized using RVPbkSimFieldTypeArray
        *
        * @return A packed sort order. Ownership is given.
        */
        IMPORT_C HBufC8* SortOrderL() const;
        
    private: // Implementation
        /// Double the size of the buffer or leave if max size achieved
        void EnlargeBufferOrLeaveL();
        // Prohibit copy constructor if not deriving from CBase.
        RVPbkSimCntView(
                const RVPbkSimCntView& );
        // Prohibit assigment operator if not deriving from CBase.
        RVPbkSimCntView& operator=(
                const RVPbkSimCntView& );
        HBufC8* SerializeParametersL(
                const TDesC* aViewName,
                const HBufC8* aFilterBuffer ) const;

    private: // Data
        /// Own: Descriptor pointer for the view event
        TPtr8 iViewEvent;
        /// Own: Buffer for ETel contact
        HBufC8* iContactBuf;
        /// Own: Pointer for the ETel contact
        TPtr8 iContactPtr;
        /// Own: 
        HBufC8* iMatchingBuffer;
        /// The sim index of the saved contact
        TPtr8 iMatchingBufferSize;
    };

#endif      // RVPBKSIMCNTVIEW_H
            
// End of File
