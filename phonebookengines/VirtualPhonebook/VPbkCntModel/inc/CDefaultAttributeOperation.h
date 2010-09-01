/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook default attribute operation
*
*/


#ifndef CDEFAULTATTRIBUTEOPERATION_H
#define CDEFAULTATTRIBUTEOPERATION_H

#include <e32base.h>
#include <MVPbkContactOperation.h>
#include "MContactFilterObserver.h"

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CVPbkDefaultAttribute;
class MVPbkSetAttributeObserver;
class MVPbkContactFindObserver;

namespace VPbkEngUtils { class CVPbkAsyncOperation; }

namespace VPbkCntModel {

class CContactStore;
class CContactFilter;

NONSHARABLE_CLASS( CDefaultAttributeOperation )
	: 	public CBase,
		public MVPbkContactOperation,
		public MContactFilterObserver
    {
    public:
        static CDefaultAttributeOperation* NewSetLC(
                CContactStore& aContactStore,
                MVPbkStoreContactField& aField, 
                const CVPbkDefaultAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
                
        static CDefaultAttributeOperation* NewRemoveLC(
                CContactStore& aContactStore,
                MVPbkStoreContactField& aField, 
                const CVPbkDefaultAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
                
        static CDefaultAttributeOperation* NewListLC(
                CContactStore& aContactStore,
                const CVPbkDefaultAttribute& aAttribute,
                MVPbkContactFindObserver& aObserver);
                
        ~CDefaultAttributeOperation();
        
    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();
        
    public: // from MContactFilterObserver
    	TBool IsIncluded(MVPbkStoreContact& aContact);
    	void FilteringDoneL(MVPbkContactLinkArray* aLinkArray);
    	void FilteringError(TInt aError);
        
    private: // Implementation
        enum TOperationMode { ESet, ERemove, EList };
        
        CDefaultAttributeOperation(
                TOperationMode aOperationMode,
                CContactStore& aContactStore,
                MVPbkStoreContactField& aField, 
                MVPbkSetAttributeObserver& aObserver);
        CDefaultAttributeOperation(
                TOperationMode aOperationMode,
                CContactStore& aContactStore,
                MVPbkContactFindObserver& aObserver);
        void ConstructL(const CVPbkDefaultAttribute& aAttribute);
        void DoSetOperationL(MVPbkSetAttributeObserver& aObserver);
        void SetOperationError(MVPbkSetAttributeObserver& aObserver, TInt aError);
        void DoListOperationL(MVPbkContactFindObserver& aObserver);
        void ListOperationError(MVPbkContactFindObserver& aObserver, TInt aError);
        
    private: // Data
        TOperationMode iOperationMode;
        CContactStore& iContactStore;
        MVPbkStoreContactField* iField;
        CVPbkDefaultAttribute* iAttribute;
        MVPbkSetAttributeObserver* iSetObserver;
        MVPbkContactFindObserver* iFindObserver;
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOperation;
        CContactFilter* iContactFilter;
    };
    
} // namespace VPbkCntModel

#endif // CDEFAULTATTRIBUTEOPERATION_H

// End of File
