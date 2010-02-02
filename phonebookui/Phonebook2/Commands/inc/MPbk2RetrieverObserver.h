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
* Description:  An observer for retrieving contact
*
*/


#ifndef MPBK2RETRIEVEROBSERVER_H
#define MPBK2RETRIEVEROBSERVER_H

// FORWARD DECLARATIONS
class MVPbkStoreContact;

// CLASS DECLARATIONS
class MPbk2RetrieverObserver
	{
	public:
		/**
		 * Called when all contacts have been retrieved.
		 * @param aContacts Array of MVPbkStoreContacts. 
		 */
		virtual void ContactsRetrieved(
			RPointerArray<MVPbkStoreContact>& aContacts) = 0;
		
		/**
		 * Called when all contact have been committed.
		 */
		virtual void ContactCommitCompelete() = 0;
		
		/**
		 * Called when error occurs in the retrieving process.
		 * @param aError Error code.
		 */
		virtual void ContactsRetrieveError(TInt aError) = 0;
		
		/**
		 * Called when one contact retrieve has done.
		 * @param aStepsToGo Number of steps to go.
		 */
		virtual void ContactRetrievedStep(TInt aStepsToGo) = 0;
	};
	
#endif // MPBK2RETRIEVEROBSERVER_H
//End of file
