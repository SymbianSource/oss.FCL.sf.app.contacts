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
* Description:  The virtual phonebook contact filter observer
*
*/



#ifndef CCONTACTFILTER_H
#define CCONTACTFILTER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CContactIdArray;
class MVPbkStoreContact;
class CVPbkContactLinkArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS	
class CContactStore;
class MContactFilterObserver;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CContactFilter ):	public CActive
	{
	public: 
	    /**
         * Creates a new instance of this class.
         *
         * @param aObserver, Observer of the filter 
         * @param aStore, store where to retrieve contacts  
         *
         * @return a new instance of this class             
         */
        static CContactFilter* NewL(
		 	MContactFilterObserver& aObserver,
		 	CContactStore& aStore );
		 	
		 ~CContactFilter();
		 
	public: // interface
		void StartL();
		
	private: // from CActive		
		void RunL();
		void DoCancel();
		TInt RunError( TInt aError );

	private: // Implementation
		CContactFilter(
		 	MContactFilterObserver& aObserver,
		 	CContactStore& aStore );
		void ConstructL();
		void NextContactL();			
		void HandleError( TInt aError );
		void IssueRequest();
		void IssueStopRequest();

	private: // Data
		/// Ref: Observer of the filter	
		MContactFilterObserver& iObserver;
		/// Ref: store where to retrieve contacts
		CContactStore& iStore;
		/// Own: contact link array
		CVPbkContactLinkArray* iLinkArray;
		/// Own: control's state
		enum TState
			{
			EGettingContactIds,
			EFilteringContact,
			EStopping
			} iState;
		/// Own: contact ids
		CContactIdArray* iContactIdArray;
	};

} // namespace VPbkCntModel

#endif // CCONTACTFILTER_H

// End of file
