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
* Description:  The virtual phonebook contact filter observer interface
*
*/


#ifndef MCONTACTFILTEROBSERVER_H
#define MCONTACTFILTEROBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactLinkArray;

namespace VPbkCntModel 	{

/**
 * The virtual phonebook contact filter observer interface
 */
NONSHARABLE_CLASS( MContactFilterObserver )
	{
	public: 
		/**
		 * Called when one contact has retrieved.
		 * If aContact meets requirements of the client returns ETrue,
		 * otherwise EFalse.
		 * @param aContact candidate of filtering
		 */
		virtual TBool IsIncluded(MVPbkStoreContact& aContact) = 0;
		
		/**
		 * Called when filtering process is done.
		 * Link array of filtered contacts.
		 * If function leaves the FilteringError will
		 * be called with propriate error code.
		 * The ownership of the array is transferred to client.
		 * @param aLinkArray Link array of filtered contacts.
		 */
		virtual void FilteringDoneL(MVPbkContactLinkArray* aLinkArray) = 0;
		
		/**
		 * Called when error occurs in filtering process.
		 * @param aError error code
		 */
		virtual void FilteringError(TInt aError) = 0;			 
	};

} // namespace VPbkCntModel

#endif // MCONTACTFILTEROBSERVER_H

// End of file
