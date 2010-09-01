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
*       xSP View client-side API.
*
*/


#ifndef __XSPVIEWSERVICES_H__
#define __XSPVIEWSERVICES_H__

// INCLUDES
#include <e32cmn.h>
#include <e32svr.h>

#include <MVPbkContactLink.h>


// CLASS DECLARATION
/**
 * The xSP View API, which is used to bring a certain
 * view in front and get tab-view information.
 *
 * There is an issue with KErrServerTerminated error code
 * sometimes returned by this API functions. This situation
 * may arise when server object is terminated with the
 * Phonebook process but clients outside the Phonebook
 * are not informed and their handles remain open. A way to
 * deal with this KErrServerTerminated error is to close
 * the handle first by calling Close() and then reopen it
 * by calling Open() or Activate() methods which do not
 * require explicit Open() calls.
 * 
 * The following panic codes are possible in case the API is abused:
 * 
 * Panic category xSPViewActivator. These panics represent programming errors
 * which are detected by the view activator service that activates a tab view.
 * Possible panic codes for this category are: EBadDescriptorLength = 0
 * (descriptor length was illegal), EBadDescriptor = 1 (descriptor type was
 * not correct; could not write to client side descriptor buffer).
 */
class RxSPViewServices : public RSessionBase
    {

    public: // Implementation

        /**
         * Opens a session to the server. Launches Phonebook in the background.
         * @return Error code
         */
        IMPORT_C TInt Open();

        /**
         * Closes the session.
         */
        IMPORT_C void Close();

        /**
         *  Triggers xSP extension view activation. Starts the Phonebook
         *      if it was not running already. Designed to be used by
         *      the xSP extension view activator. If the session was not
         *		open, this method will open one and close it afterwards.
         *		If the session was open, this method will not close it
         *		afterwards. 
         *  @param aEComID xSP extension ECom implementation ID
         *  @param aOriginalViewID view original / old ID
         *  @return KErrNone if successful, error code otherwise
         */
        IMPORT_C TInt Activate( TUint32 aEComID, 
                                TInt aOriginalViewID);

        /**
         *  Triggers xSP extension view activation. Starts the Phonebook
         *      if it was not running already. Designed to be used by
         *      the xSP extension view activator. If the session was not
         *		open, this method will open one and close it afterwards.
         *		If the session was open, this method will not close it
         *		afterwards.
         *  @param aEComID xSP extension ECom implementation ID
         *  @param aOriginalViewID view original / old ID
         *  @param aContactLink which contact item to show
         *  @param aFocusedField which field gets the focus
         */
        IMPORT_C void ActivateL( TUint32 aEComID, 
                                 TInt aOriginalViewID, 
                                 MVPbkContactLink* aContactLink, 
                                 TInt aFocusedField );

        /**
         *  Triggers xSP extension view activation. Starts the Phonebook
         *      if it was not running already. Designed to be used by
         *      the xSP extension view activator. If the session was not
         *		open, this method will open one and close it afterwards.
         *		If the session was open, this method will not close it
         *		afterwards.
         *  @param aEComID xSP extension ECom implementation ID
         *  @param aOriginalViewID view original / old ID
         *  @param aParamBuf specifies which contact to show (see CPbk2ViewState.h or
         *                   CPbkViewState.h for details)
         *  @return KErrNone if successful, error code otherwise
         */
        IMPORT_C TInt Activate( TUint32 aEComID, 
                                TInt aOriginalViewID, 
                                TPtr8& aParamBuf);

        /**
         *  Triggers view activation asynchronously.
         *
         *  @param aEComID xSP extension ECom implementation ID
         *  @param aOriginalViewID view original / old ID
         *  @param aParamBuf specifies which contact to show (see CPbk2ViewState.h or
         *                   CPbkViewState.h for details)
         *  @param aStatus contains the status of the request on completion
         */
        IMPORT_C void Activate( TUint32 aEComID, 
                                TInt aOriginalViewID, 
                                TPtr8& aParamBuf, 
                                TRequestStatus& aStatus );
         
        /**
         *  Cancel activate
         *  @return KErrNone if successful, error code otherwise
         */                       
      	IMPORT_C TInt CancelActivate() const;
      	
        /**
         *  Constructor
         */                       
      	IMPORT_C RxSPViewServices();
      	
      	/**
         *	Opens a session to the server. Async version.
         *	Launches Phonebook in the background.
         *	@param aAsyncMessageSlots The number of message slots available to session.
         *	@param aType type of session to create.
         *	@param aStatus request status 
         *	@return Error code
         */
        IMPORT_C TInt Open( TInt aAsyncMessageSlots,
        					TIpcSessionType aType,
        					TRequestStatus& aStatus );
                 
        /**
         *  Get view count
         *	@param aCount placeholder for view count
         *  @return KErrNone if successful, error code otherwise
         */                       
		IMPORT_C TInt GetViewCount( TInt& aCount ) const;
		
		/**
         *  Get view count. Async version.
         *	@param aCount placeholder for view count
         *	@param aStatus request status 
         *  @return KErrNone if successful, error code otherwise
         */                       
		IMPORT_C TInt GetViewCount( TInt& aCount,
									TRequestStatus& aStatus );
		
		/**
         *  Cancel async get view count
         *  @return KErrNone if successful, error code otherwise
         */ 									
		IMPORT_C TInt CancelGetViewCount() const;
		
		/**
         *  Get packed view data buffer length
         *	@param aIndex view index
         *	@param aLength placeholder for buffer length
         *  @return KErrNone if successful, error code otherwise
         */
		IMPORT_C TInt GetPackedViewDataBufferLength( TInt aIndex, 
														TInt& aLength ) const;
		
		/**
         *  Get packed view data buffer length. Async version.
         *	@param aIndex view index
         *	@param aLength placeholder for buffer length
         *	@param aStatus request status 
         *  @return KErrNone if successful, error code otherwise
         */
		IMPORT_C TInt GetPackedViewDataBufferLength( TInt aIndex,
														TInt& aLength,
														TRequestStatus& aStatus );
		
		
		/**
         *  Cancel async get packed view data buffer length
         *  @return KErrNone if successful, error code otherwise
         */ 									
		IMPORT_C TInt CancelGetPackedViewDataBufferLength() const;
														
		/**
         *  Get packed view data
         *	@param aIndex view index
         *	@param aPackedViewData reference to allocated buffer.
         *		The buffer will contain packed data on successful return      
         *  @return KErrNone if successful, error code otherwise
         */
		IMPORT_C TInt GetPackedViewData( TInt aIndex, TDes8& aPackedViewData ) const;
		
		/**
         *  Get packed view data. Async version.
         *	@param aIndex view index
         *	@param aPackedViewData reference to allocated buffer.
         *		The buffer will contain packed data on successful completion
         *	@param aStatus request status     
         *  @return KErrNone if successful, error code otherwise
         */
		IMPORT_C TInt GetPackedViewData( TInt aIndex,
											TDes8& aPackedViewData,
											TRequestStatus& aStatus );
		
		/**
         *  Cancel get packet view data
         *  @return KErrNone if successful, error code otherwise
         */ 									
		IMPORT_C TInt CancelGetPackedViewData() const;

    private:    // Private methods

        /**
         * Returns the client side API version.
         */
        TVersion Version() const;

        /**
         * Starts the Phonebook if it is not already running.
         */
        void StartPhonebookL();
        
    private: // data
    
    	/// Own: pointer to view count
    	TPtr8 iViewCount;
    	/// Own: pointer to packed view data buffer length
    	TPtr8 iPackedViewDataBufferLength;
    };


#endif // __XSPVIEWSERVICES_H__

// End of file.
