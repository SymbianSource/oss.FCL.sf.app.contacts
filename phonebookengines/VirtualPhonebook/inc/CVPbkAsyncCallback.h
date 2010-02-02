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


#ifndef CVPBKASYNCCALLBACK_H
#define CVPBKASYNCCALLBACK_H

#include <e32base.h>
#include <CVPbkAsyncOperation.h>

namespace VPbkEngUtils {

/**
 * Asynchronous callback that uses no parameters in the operation complete 
 * function in addition to the observer parameter.
 */
template <typename NotifiedClass, typename Observer>
class CVPbkAsyncCallback : public CVPbkAsyncObjectCallback<Observer>
    {
    public:
        typedef void (NotifiedClass::*OperationCompleteFunc)(Observer& 
                                                             aObserver);
        typedef void (NotifiedClass::*OperationErrorFunc)(Observer& aObserver,
                                                          TInt aError);
        CVPbkAsyncCallback(
            NotifiedClass& aNotifiedClass, 
            OperationCompleteFunc aOperationCompleteFunc,
            OperationErrorFunc aOperationErrorFunc,
            Observer& aObserver);
    
    private: // From MAsyncCallback
        void operator()();
        void Error(TInt aError);

    public: // Data
        // Not owned: Class which function is to be called when async 
        // operation is complete
        NotifiedClass& iNotifiedClass;
        // Not owned: Function to be called when async event completes
        OperationCompleteFunc iOperationCompleteFunc;
        // Not owned: Function to be called if something went wrong
        OperationErrorFunc iOperationErrorFunc;
        // Not owned: Observer which is to be notified after async operation
        Observer& iObserver;
    };

/**
 * Asynchronous callback wrapper. When VPbk finishes its task, this callback 
 * executes callback.
 */
template <typename NotifiedClass, typename Observer>
CVPbkAsyncCallback<NotifiedClass, Observer>::CVPbkAsyncCallback(
        NotifiedClass& aNotifiedClass, 
        OperationCompleteFunc aOperationCompleteFunc,
        OperationErrorFunc aOperationErrorFunc,
        Observer& aObserver) :
    CVPbkAsyncObjectCallback<Observer>( aObserver ),
    iNotifiedClass(aNotifiedClass),
    iOperationCompleteFunc(aOperationCompleteFunc),
    iOperationErrorFunc(aOperationErrorFunc),
    iObserver(aObserver)
    {
    }

/**
 * Asynchronous callback that notifies the class that operation
 * has completed
 */
template <typename NotifiedClass, typename Observer>
void CVPbkAsyncCallback<NotifiedClass, Observer>::operator()()
    {
    (iNotifiedClass.*iOperationCompleteFunc)(iObserver);
    }

/**
 * Asynchronous callback that notifies the class that operation
 * failed with error code
 */
template <typename NotifiedClass, typename Observer>
void CVPbkAsyncCallback<NotifiedClass, Observer>::Error(TInt aError)
    {
    (iNotifiedClass.*iOperationErrorFunc)(iObserver, aError);
    }

/**
 * Asynchronous callback that uses one parameter in the operation complete 
 * function in addition to the observer parameter.
 */
template <typename NotifiedClass, typename CompleteOpParam, typename Observer>
class CVPbkAsyncCallbackOneParam : public CVPbkAsyncObjectCallback<Observer>
    {
    public:
        typedef void (NotifiedClass::*OperationCompleteFunc)( Observer& 
                                                              aObserver, 
                                                              CompleteOpParam 
                                                              aParam );
        typedef void (NotifiedClass::*OperationErrorFunc)( Observer& aObserver, 
                                                           TInt aError );
        CVPbkAsyncCallbackOneParam(
            NotifiedClass& aNotifiedClass, 
            OperationCompleteFunc aOperationCompleteFunc,
            CompleteOpParam aParam,
            OperationErrorFunc aOperationErrorFunc,
            Observer& aObserver);
    
    private: // From MAsyncCallback
        void operator()();
        void Error(TInt aError);

    public: // Data
        // Not owned: Class which function is to be called when async 
        // operation is complete
        NotifiedClass& iNotifiedClass;
        // Not owned: Function to be called when async event completes
        OperationCompleteFunc iOperationCompleteFunc;
        // Not owned: Parameters to be passed for complete callback
        CompleteOpParam iParam;
        // Not owned: Function to be called if something went wrong
        OperationErrorFunc iOperationErrorFunc;
        // Not owned: Observer which is to be notified after async operation
        Observer& iObserver;
    };

/**
 * Asynchronous callback wrapper. When VPbk finishes its task, this callback 
 * executes callback with one parameter to caller in addition to the observer 
 * parameter
 */
template <typename NotifiedClass, typename CompleteOpParam, typename Observer>
CVPbkAsyncCallbackOneParam<NotifiedClass, CompleteOpParam, Observer>::
  CVPbkAsyncCallbackOneParam(
        NotifiedClass& aNotifiedClass, 
        OperationCompleteFunc aOperationCompleteFunc,
        CompleteOpParam aParam,
        OperationErrorFunc aOperationErrorFunc,
        Observer& aObserver) :
    CVPbkAsyncObjectCallback<Observer>( aObserver ),
    iNotifiedClass(aNotifiedClass),
    iOperationCompleteFunc(aOperationCompleteFunc),
    iParam(aParam),
    iOperationErrorFunc(aOperationErrorFunc),
    iObserver(aObserver)
    {
    }

/**
 * Asynchronous callback that notifies the class that operation
 * has completed with one parameter to caller in addition to the 
 * observer parameter
 */
template <typename NotifiedClass, typename CompleteOpParam, typename Observer>
void CVPbkAsyncCallbackOneParam<NotifiedClass, CompleteOpParam, Observer>::operator()()
    {
    (iNotifiedClass.*iOperationCompleteFunc)(iObserver, iParam);
    }

/**
 * Asynchronous callback that notifies the class that operation
 * failed with error code in addition to the observer parameter
 */
template <typename NotifiedClass, typename CompleteOpParam, typename Observer>
void CVPbkAsyncCallbackOneParam<NotifiedClass, CompleteOpParam, Observer>::
    Error(TInt aError)
    {
    (iNotifiedClass.*iOperationErrorFunc)(iObserver, aError);
    }

/**
 * Creates asynchronous callback that is left to cleanup stack
 */
template <typename NotifiedClass, typename Observer>
CVPbkAsyncObjectCallback<Observer>* CreateAsyncObjectCallbackLC(
        NotifiedClass& aNotifiedClass,
        void (NotifiedClass::*aOperationCompleteFunc)(Observer&),
        void (NotifiedClass::*aOperationErrorFunc)(Observer&, TInt),
        Observer& aObserver)
    {
    CVPbkAsyncObjectCallback<Observer>* asyncOp = 
        new(ELeave) CVPbkAsyncCallback<NotifiedClass, Observer>(
            aNotifiedClass,
            aOperationCompleteFunc,
            aOperationErrorFunc,
            aObserver);
    CleanupStack::PushL(asyncOp);
    return asyncOp;
    }

/**
 * Creates asynchronous callback with observer parameter that is left to 
 * cleanup stack
 */
template <typename NotifiedClass, typename CompleteOpParam, typename Observer>
CVPbkAsyncObjectCallback<Observer>* CreateAsyncObjectCallbackLC(
        NotifiedClass& aNotifiedClass,
        void (NotifiedClass::*aOperationCompleteFunc)(Observer&, CompleteOpParam),
        CompleteOpParam aParam,
        void (NotifiedClass::*aOperationErrorFunc)(Observer&, TInt),
        Observer& aObserver)
    {
    CVPbkAsyncObjectCallback<Observer>* asyncOp = 
       new(ELeave) 
        CVPbkAsyncCallbackOneParam<NotifiedClass, CompleteOpParam, Observer>(
                aNotifiedClass,
                aOperationCompleteFunc,
                aParam,
                aOperationErrorFunc,
                aObserver);
    CleanupStack::PushL(asyncOp);
    return asyncOp;
    }

/**
 * Creates asynchronous callback that is left to cleanup stack
 */
template <typename NotifiedClass, typename Observer>
MAsyncCallback* CreateAsyncCallbackLC(
        NotifiedClass& aNotifiedClass,
        void (NotifiedClass::*aOperationCompleteFunc)(Observer&),
        void (NotifiedClass::*aOperationErrorFunc)(Observer&, TInt),
        Observer& aObserver)
    {
    MAsyncCallback* asyncOp = 
        new(ELeave) CVPbkAsyncCallback<NotifiedClass, Observer>(
            aNotifiedClass,
            aOperationCompleteFunc,
            aOperationErrorFunc,
            aObserver);
    CleanupDeletePushL(asyncOp);
    return asyncOp;
    }

/**
 * Creates asynchronous callback with observer parameter that is left to 
 * cleanup stack
 */
template <typename NotifiedClass, typename CompleteOpParam, typename Observer>
MAsyncCallback* CreateAsyncCallbackLC(
        NotifiedClass& aNotifiedClass,
        void (NotifiedClass::*aOperationCompleteFunc)(Observer&, CompleteOpParam),
        CompleteOpParam aParam,
        void (NotifiedClass::*aOperationErrorFunc)(Observer&, TInt),
        Observer& aObserver)
    {
    MAsyncCallback* asyncOp = 
        new(ELeave) CVPbkAsyncCallbackOneParam<NotifiedClass, 
            CompleteOpParam, Observer>( aNotifiedClass, 
                aOperationCompleteFunc, aParam, aOperationErrorFunc,
                aObserver);
    CleanupDeletePushL(asyncOp);
    return asyncOp;
    }

} // namespace VPbkEngUtils

#endif // CVPBKASYNCCALLBACK_H
// End of file
