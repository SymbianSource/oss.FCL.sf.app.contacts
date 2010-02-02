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
* Description:  
*
*/


#ifndef CSDNDVOICETAGATTRIBUTEOPERATION_H
#define CSDNDVOICETAGATTRIBUTEOPERATION_H

// INCLUDES
#include <e32base.h>
#include <mvpbkcontactoperation.h>
#include "mcontactfilterobserver.h"

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CVPbkVoiceTagAttribute;
class MVPbkSetAttributeObserver;
class MVPbkContactFindObserver;

namespace VPbkEngUtils { class CVPbkAsyncOperation; }

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;
class CContactFilter;

/**
 * Contact model store Voice tag attribute operation.
 */
NONSHARABLE_CLASS( CSdndVoiceTagAttributeOperation ): 	
        public CBase,
		public MVPbkContactOperation,
		public MContactFilterObserver
    {
    public:
        /**
         * Creates operation that sets the voice tag
         * @param aContactStore Store which is used
         * @param aField The contact field where the voice tag is set
         * @param aAttribute Voice tag attribute 
         * @param aObserver Observer that is notified when finished
         * @return Asynchronous operation
         */
        static CSdndVoiceTagAttributeOperation* NewSetLC(
                CContactStore& aContactStore,
                MVPbkStoreContactField& aField, 
                const CVPbkVoiceTagAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
                
        /**
         * Creates operation that removes the voice tag
         * @param aContactStore Store which is used
         * @param aField The contact field where the voice tag is removed
         * @param aAttribute Voice tag attribute 
         * @param aObserver Observer that is notified when finished
         * @return Asynchronous operation
         */
        static CSdndVoiceTagAttributeOperation* NewRemoveLC(
                CContactStore& aContactStore,
                MVPbkStoreContactField& aField, 
                const CVPbkVoiceTagAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
        
        /**
         * Creates operation that lists all voice tags
         * @param aContactStore Store which is used
         * @param aAttribute Voice tag attribute 
         * @param aObserver Observer that is notified when finished
         * @return Asynchronous operation
         */
        static CSdndVoiceTagAttributeOperation* NewListLC(
                CContactStore& aContactStore,
                const CVPbkVoiceTagAttribute& aAttribute,
                MVPbkContactFindObserver& aObserver);
                
        ~CSdndVoiceTagAttributeOperation();
        
    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();
        
    public: // from MContactFilterObserver
    	TBool IsIncluded(MVPbkStoreContact& aContact);
    	void FilteringDoneL(MVPbkContactLinkArray* aLinkArray);
    	void FilteringError(TInt aError);
        
    private: // Implementation
        enum TOperationMode { ESet, ERemove, EList };
        
        CSdndVoiceTagAttributeOperation(
                TOperationMode aOperationMode,
                CContactStore& aContactStore,
                MVPbkStoreContactField& aField, 
                MVPbkSetAttributeObserver& aObserver);
        CSdndVoiceTagAttributeOperation(
                TOperationMode aOperationMode,
                CContactStore& aContactStore,
                MVPbkContactFindObserver& aObserver);
        void ConstructL(const CVPbkVoiceTagAttribute& aAttribute);
        void DoSetOperationL(MVPbkSetAttributeObserver& aObserver);
        void SetOperationError(MVPbkSetAttributeObserver& aObserver, TInt aError);
        void DoListOperationL(MVPbkContactFindObserver& aObserver);
        void ListOperationError(MVPbkContactFindObserver& aObserver, TInt aError);
        
    private: // Data
        ///Own: Attribute operation mode
        TOperationMode iOperationMode;
        ///Own: Contact store
        CContactStore& iContactStore;
        ///Own: contact field
        MVPbkStoreContactField* iField;
        ///Own: Voice tag attribute
        CVPbkVoiceTagAttribute* iAttribute;
        ///Own: Setting attribute observer
        MVPbkSetAttributeObserver* iSetObserver;
        ///Own: Find observer
        MVPbkContactFindObserver* iFindObserver;
        ///Own: Asynchronous opereration
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOperation;
        ///Own: Contact filter
        CContactFilter* iContactFilter;
    };
    
} // namespace VPbkCntModel

#endif // CSDNDVOICETAGATTRIBUTEOPERATION_H

// End of File
