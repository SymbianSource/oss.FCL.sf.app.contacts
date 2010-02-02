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
* Description:  A operation class for setting/getting speeddial information
*
*/



#ifndef CSPEEDDIALATTRIBUTEOPERATION_H
#define CSPEEDDIALATTRIBUTEOPERATION_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkBatchOperationObserver.h>

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CVPbkSpeedDialAttribute;
class MVPbkSetAttributeObserver;
class MVPbkContactFindObserver;
class CVPbkBatchOperation;
class CVPbkContactLinkArray;

namespace VPbkEngUtils { class CVPbkAsyncOperation; }

namespace VPbkCntModel {

class CContactStore;
class TContactField;
class CContact;
class CContactFilter;
class CContactLink;

/**
 *  A operation class for setting/getting speed dial information
 */
NONSHARABLE_CLASS( CSpeedDialAttributeOperation ): 
        public CBase,
        public MVPbkContactOperation,
		private MVPbkBatchOperationObserver
    {
public:
    /**
     * Creates operation that sets the speed dial
     * @param aContactStore Store which is used
     * @param aField The contact field where the speed dial is set
     * @param aAttribute Speed dial attribute 
     * @param aObserver Observer that is notified when finished
     * @return asynchronous operation
     */ 
    static CSpeedDialAttributeOperation* NewSetLC(
            CContactStore& aContactStore,
            MVPbkStoreContactField& aField, 
            const CVPbkSpeedDialAttribute& aAttribute,
            MVPbkSetAttributeObserver& aObserver);
    
    /**
     * Creates operation that removes the speed dial
     * @param aContactStore Store which is used
     * @param aField The contact field where the speed dial is removed
     * @param aAttribute Speed dial attribute 
     * @param aObserver Observer that is notified when finished
     * @return asynchronous operation
     */             
    static CSpeedDialAttributeOperation* NewRemoveLC(
            CContactStore& aContactStore,
            MVPbkStoreContactField& aField, 
            const CVPbkSpeedDialAttribute& aAttribute,
            MVPbkSetAttributeObserver& aObserver);
    
    /**
     * Creates operation that lists all speed dials
     * @param aContactStore Store which is usedt
     * @param aAttribute Speed dial attribute 
     * @param aObserver Observer that is notified when finished
     * @return asynchronous operation
     */         
    static CSpeedDialAttributeOperation* NewListLC(
            CContactStore& aContactStore,
            const CVPbkSpeedDialAttribute& aAttribute,
            MVPbkContactFindObserver& aObserver);
            
    ~CSpeedDialAttributeOperation();

public: // from base class MVPbkContactOperation

    void StartL();
    void Cancel();



private: // from base class MVPbkBatchOperationObserver
    void StepComplete(MVPbkContactOperationBase& aOperation,
                              TInt aStepSize);
    TBool StepFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aStepSize,
            TInt aError);
    void OperationComplete(MVPbkContactOperationBase& aOperation);

// Implementation

private:
    enum TOperationMode { ESet, ERemove, EList };
    
    CSpeedDialAttributeOperation(
            TOperationMode aOperationMode,
            CContactStore& aContactStore,
            MVPbkStoreContactField& aField, 
            MVPbkSetAttributeObserver& aObserver);
    CSpeedDialAttributeOperation(
            TOperationMode aOperationMode,
            CContactStore& aContactStore,
            MVPbkContactFindObserver& aObserver);
    void ConstructL(const CVPbkSpeedDialAttribute& aAttribute);
    
    void HandleOperationCompleteL();
    void DoSetOperationL(MVPbkSetAttributeObserver& aObserver);
    void SetOperationError(MVPbkSetAttributeObserver& aObserver, TInt aError);
    void DoListOperationL(MVPbkContactFindObserver& aObserver);
    void ListOperationError(MVPbkContactFindObserver& aObserver, TInt aError);
    TInt NativeFieldIndex(CContact& aContact, TContactField& aField) const;
    void FindSpeedDialsL();
    void FindSpeedDialL();
    CContactLink* DoFindSpeedDialLC( TInt aIndex );
    
private: // Data
    /// Ref: operation mode
    TOperationMode iOperationMode;
    /// Ref: contact store
    CContactStore& iContactStore;
    /// Ref: store contact field
    MVPbkStoreContactField* iField;
    /// Own: speed dial attribute
    CVPbkSpeedDialAttribute* iAttribute;
    /// Ref: set attribute observer
    MVPbkSetAttributeObserver* iSetObserver;
    /// Ref: contact find observer
    MVPbkContactFindObserver* iFindObserver;
    /// Own: VPbk asynchronous operation
    VPbkEngUtils::CVPbkAsyncOperation* iAsyncOperation;
    /// Own: empty operation
    CVPbkBatchOperation* iEmptyOperation;
    /// Own: contact link array
    CVPbkContactLinkArray* iLinkArray;
    };
} // namespace VPbkCntModel

#endif // CSPEEDDIALATTRIBUTEOPERATION_H

// End of File
