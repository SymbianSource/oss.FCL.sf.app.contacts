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
*       Contact manager.
*
*/


#ifndef __CXSPCONTACTMANAGER_H__
#define __CXSPCONTACTMANAGER_H__

// INCLUDES
#include <e32base.h>
#include "MxSPContactManager.h"

// FORWARD DECLARATIONS
class CPbk2ViewState;
class MxSPFactory;
class MVPbkContactStoreList;
class CVPbkContactManager;

/**
 * Contact map. Represents simply the mapping/linking between a single
 * contact link and a contact id.
 */
class CxSPPbkContactMap : public CBase
	{
	public:

		/**
         * Constructor.
         */
        CxSPPbkContactMap();

		/**
         * Destructor.
         */
        ~CxSPPbkContactMap();

		/**
		 * Internalize the class data
		 *
		 * @param aStream Stream for internalizing
		 * @param aContactStores
		 */
		void InternalizeL( RReadStream& aStream,
							const MVPbkContactStoreList& aContactStores );

		/**
		 * Externalize the class data
		 *
		 * @param aStream Stream for externalizing
		 */
		void ExternalizeL( RWriteStream& aStream ) const;

	public: // data
		/// Original contact id
		TUint32 iId;

		/// xSP contact id
		TInt32 ixSPContactId;

		/// Own: Contact link
		MVPbkContactLink* iPbkContactLink;
	};


/**
 * Contact manager.
 */
class CxSPContactManager : public CBase,
                               public MxSPContactManager
    {
    public:
        /**
		 * Static factory function that performs the 2-phased construction.
		 *
		 * @param aFactories Array of xSP factories
		 * @param aVPbkContactManager Virtual phonebook contact manager
		 *
         * @return new object of this class
         */
        static CxSPContactManager* NewL( CArrayPtrFlat<MxSPFactory>& aFactories,
        									CVPbkContactManager& aVPbkContactManager );

        /**
         * Destructor.
         */
        ~CxSPContactManager();

	public: // from MxSPContactManager

		void MapContactL( TUint32 aId,
						  const MVPbkContactLink& aPbkContactLink,
        				  TInt32 axSPContactId );
		void UnmapContactL( TUint32 aId,
						    const MVPbkContactLink& aPbkContactLink,
        					TInt32 axSPContactId );
		void GetMappedPbkContactsL( TUint32 aId,
									TInt32 axSPContactId,
        							RPointerArray<MVPbkContactLink>& aPbkContactLinks ) const;
		void GetMappedxSPContactsL( TUint32 aId,
									const MVPbkContactLink& aPbkContactLink,
        							RArray<TInt32>& axSPContactIds ) const;
		void ViewActivatedL( const TVwsViewId& aPrevViewId,
                         	 TUid aCustomMessageId,
                         	const TDesC8& aCustomMessage );
		const CPbk2ViewState* NamesListState() const;

	public:

		/**
		 * Converts the given contact link to buffer
		 *
		 * @param aLink contact link to be converted
		 *
		 * @return the converted link as buffer
		 */
		static HBufC8* LinkToBufferL( const MVPbkContactLink* aLink );

		/**
		 * Converts the given data buffer to contact link
		 *
		 * @param aBuffer data buffer to be converted
		 * @param aContactStores list of contact stores
		 *
		 * @return the converted data as contact link
		 */
		static MVPbkContactLink* BufferToLinkL( const TDesC8& aBuffer,
									const MVPbkContactStoreList& aContactStores );

    private: // Implementation

    	/**
         * Default constructor.
         */
        CxSPContactManager();

        /**
         * 2nd phase constructor.
         *
		 * @param aFactories Array of xSP factories
		 * @param aVPbkContactManager Virtual phonebook contadct manager
         */
        void ConstructL( CArrayPtrFlat<MxSPFactory>& aFactories,
        					CVPbkContactManager& aVPbkContactManager );

  	private:

  		/**
         * 	Searches for contact mapping
         *
         *	@param aId xSP implementation ID
         *	@param aPbkContactLink Phonebook contact link
         *	@param axSPContactId xSP contact ID
         *
         *	@return index of mapping or KErrNotFound
         */
  		TInt FindMap( TUint32 aId,
  					  const MVPbkContactLink& aPbkContactLink,
  					  TInt32 axSPContactId ) const;

  		/**
  		 * Creates a new folder
  		 *
  		 * @param aSession File server session
  		 * @param aPath path of the folder to be created
  		 * @param aDrive Drive name to which the folder is to be created
  		 *
  		 * @return error code in case of error
  		 */
		TInt CreatePrivateFolder( RFs& aSession, TDes& aPath, TDriveNumber aDrive ) const;

		/**
		 * Saves the contact map data to a file
		 *
		 * @param aFileName file name to be used
		 */
		void SaveArrayL( const TDesC& aFileName );

		/**
		 * Restores the contact map data from file
		 *
		 * @param aFileName file name to be used for reading data
		 * @param aFactories array of xSP factories available
		 * @param aContactStoreList list of contact stores available
		 */
		void RestoreArrayL( const TDesC& aFileName,
							CArrayPtrFlat<MxSPFactory>& aFactories,
							MVPbkContactStoreList& aContactStoreList );

    private: // data
		/// Contact map to be used
        RPointerArray<CxSPPbkContactMap> iContactMap;

        /// Own: Current view state
      	CPbk2ViewState* iState;
    };



#endif // __CXSPCONTACTMANAGER_H__

// End of File
