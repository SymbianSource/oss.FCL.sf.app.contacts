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
* Description:  Helper class for asynchronous operations
*
*/


#ifndef VPBKENGUTILS_CVPBKASYNCOPERATION_H
#define VPBKENGUTILS_CVPBKASYNCOPERATION_H


// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
namespace VPbkEngUtils {

// CLASS DECLARATIONS

/**
 * Asynchronous callback interface
 */
class MAsyncCallback
    {
    public:
        /**
         * Destructor
         */
        virtual ~MAsyncCallback() {}
        
        /**
         * Calls the callback function.
         */
        virtual void operator()() = 0;
        
        /**
         * Called if operator()() leaves.
         */
        virtual void Error(TInt aError) = 0;
    };

/**
 * Contact store asynchrnous operations.
 */
class CVPbkAsyncOperation : public CBase
    {
    public:  // Constructor and destructor
        /**
         * Constructs the operation with priority CActive::EPriorityIdle.
         */
        IMPORT_C CVPbkAsyncOperation();
        
        /**
         * Constructs the operation with aPriority.
         * 
         * @param aPriority the priority for the active object.
         */
        IMPORT_C CVPbkAsyncOperation( CActive::TPriority aPriority );
        
        /**
         * Destructor.
         */
        IMPORT_C ~CVPbkAsyncOperation();

    public:  // New functions
        /**
         * Starts a new callback. Ownership is transferred in case
         * the function does not leave.
         *
         * @param aCallback A callback instance e.g. from CVPbkAsyncCallback.h.
         *                  Ownership is taken.
         */
        IMPORT_C void CallbackL(MAsyncCallback* aCallback);

        /**
         * Cancels and deletes all callbacks in this operation.
         */
        IMPORT_C void Purge();
    
        /**
         * Cancels and deletes the aCallback from this operation.
         * 
         * @param aCallback The callback to delete.
         */
        IMPORT_C void CancelCallback( const MAsyncCallback& aCallback );
        
        /**
         * Returns the number of callbacks in this operation.
         *
         * @return The number of callbacks in this operation.
         */
        IMPORT_C TInt CallbackCount() const;
        
        /**
         * Returns a reference to the callback at position aIndex.
         *
         * @param aIndex The position of the callback.
         * @return A reference to the callback at position aIndex.
         */
        IMPORT_C const MAsyncCallback& CallbackAt( TInt aIndex ) const;
        
    private:  // Implementation
        void IssueRequest();
	    TInt HandleIdleCallback();
        static TInt IdleCallback( TAny* aThis );
        
    private:  // Data
        /// The priority of this operation
        CActive::TPriority iPriority;
        // Owned: Array of callbacks
        RPointerArray<MAsyncCallback> iCallbacks;
        // Not owned: Set if this object is destroyed
        volatile TBool* iDestroyed;
        /// Own: An idle for making the operation asynchronous.
        CIdle* iIdle;
    };

} // namespace VPbkEngUtils


/**
 * A call back class that is identified by an object handle.
 * Can be used e.g. when a single call back must be possible
 * to cancel from CVPbkAsyncObjectOperation.
 */
template<class ObjectHandle>
NONSHARABLE_CLASS(CVPbkAsyncObjectCallback)
    :   public CBase, 
        public VPbkEngUtils::MAsyncCallback
    {
    public:
        /**
         * Constructor
         *
         * @param aObjectHandle This is a handle to the object that identifies
         *        the callback,
         */
        CVPbkAsyncObjectCallback( ObjectHandle& aObjectHandle );
        
        /**
         * Returns the ETrue if the callback is identified by aObjectHandle.
         *
         * @param aObjectHandle
         */
        TBool IsSame( ObjectHandle& aObjectHandle ) const;
        
    private:
        /// Not Own:
        ObjectHandle& iObjectHandle;
    };

/**
 * An operation class template for multiple callbacks that can be
 * identified by an object handle. This enables canceling a single 
 * callback.
 *
 * Object handle could be e.g. an observer.
 */
template<class ObjectHandle>
NONSHARABLE_CLASS(CVPbkAsyncObjectOperation) : public CBase
    {
    public:  // Constructor and destructor
        
        /**
         * Constructs the operation with priority CActive::EPriorityStandard.
         *
         * @return A new instance of this class.
         */
        static CVPbkAsyncObjectOperation* NewL();
        
        /**
         * Destructor
         */
        ~CVPbkAsyncObjectOperation();

    public:  // New functions
        /**
         * Starts a new callback. Ownership is transferred in case 
         * the function does not leave.
         *
         * @param aCallback A callback instance e.g. from CVPbkAsyncCallback.h.
         *                  Ownership is taken.
         */
        void CallbackL( CVPbkAsyncObjectCallback<ObjectHandle>* aCallback );
        
        /**
         * Cancels and deletes all the callbacks in this operation.
         */
        void Purge();
        
        /** 
         * Cancels and deletes a single callback identified by aObjectHandle.
         *
         * @param aObjectHandle A callback identifier.
         */
        void CancelCallback( ObjectHandle* aObjectHandle );
        
    private:
        CVPbkAsyncObjectOperation();
        void ConstructL();
        
    private:  // Data
        /// Own: The async operation that implements most of the functionality.
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOp;
    };

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectCallback::CVPbkAsyncObjectCallback
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline CVPbkAsyncObjectCallback<ObjectHandle>::CVPbkAsyncObjectCallback( 
        ObjectHandle& aObjectHandle )
        :   iObjectHandle( aObjectHandle )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectCallback::IsSame
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline TBool CVPbkAsyncObjectCallback<ObjectHandle>::IsSame( 
        ObjectHandle& aObjectHandle ) const
    {
    return &aObjectHandle == &iObjectHandle;
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::CVPbkAsyncObjectOperation
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline CVPbkAsyncObjectOperation<ObjectHandle>::CVPbkAsyncObjectOperation()
    {
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::ConstructL
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline void CVPbkAsyncObjectOperation<ObjectHandle>::ConstructL()
    {
    iAsyncOp = new( ELeave ) VPbkEngUtils::CVPbkAsyncOperation( 
        CActive::EPriorityStandard );
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::NewL
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline CVPbkAsyncObjectOperation<ObjectHandle>* 
        CVPbkAsyncObjectOperation<ObjectHandle>::NewL()
    {
    CVPbkAsyncObjectOperation* self = new( ELeave ) CVPbkAsyncObjectOperation;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::~CVPbkAsyncObjectOperation
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
CVPbkAsyncObjectOperation<ObjectHandle>::~CVPbkAsyncObjectOperation()
    {
    delete iAsyncOp;
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::CallbackL
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline void CVPbkAsyncObjectOperation<ObjectHandle>::CallbackL(
        CVPbkAsyncObjectCallback<ObjectHandle>* aCallback)
    {
    iAsyncOp->CallbackL( aCallback );
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::Purge
// -----------------------------------------------------------------------------
//
template<typename ObjectHandle>
inline void CVPbkAsyncObjectOperation<ObjectHandle>::Purge()
    {
    iAsyncOp->Purge();
    }
    
// -----------------------------------------------------------------------------
// CVPbkAsyncObjectOperation::CancelCallback
// -----------------------------------------------------------------------------
//
template<class ObjectHandle>
inline void CVPbkAsyncObjectOperation<ObjectHandle>::CancelCallback(
        ObjectHandle* aObjectHandle)
    {
    const TInt count = iAsyncOp->CallbackCount();
    for ( TInt i = 0; i < count && aObjectHandle; ++i )
        {
        // MAsyncCallback instances can be safely casted to
        // CVPbkAsyncObjectCallback because there can not be any other types
        // in iAsyncOp. CallbackL takes only CVPbkAsyncObjectCallback types 
        // as input.
        const CVPbkAsyncObjectCallback<ObjectHandle>& callback = 
            static_cast<const CVPbkAsyncObjectCallback<ObjectHandle>&>( 
                iAsyncOp->CallbackAt( i ) );
        if ( callback.IsSame( *aObjectHandle ) )
            {
            iAsyncOp->CancelCallback( callback );
            return;
            }
        }
    }

#endif  // VPBKENGUTILS_CVPBKASYNCOPERATION_H
// End of file


