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
* Description:  An base class for multioperation implementations
*
*/


#include <CVPbkMultiContactOperationBase.h>

CVPbkMultiContactOperationBase::CVPbkMultiContactOperationBase()
    {
    }

CVPbkMultiContactOperationBase::~CVPbkMultiContactOperationBase()
    {
    Cancel();
    delete iIdle;
    iSubOperations.ResetAndDestroy();
    }

EXPORT_C void CVPbkMultiContactOperationBase::AddSubOperationL(MVPbkContactOperation* aOperation)
    {
    User::LeaveIfError(iSubOperations.Append(aOperation));
	}

EXPORT_C TInt CVPbkMultiContactOperationBase::SubOperationCount() const
    {
    return iSubOperations.Count();
	}

void CVPbkMultiContactOperationBase::StartL()
    {
    const TInt count = iSubOperations.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iSubOperations[i]->StartL();
        }
        
    if ( count == 0 )
        {
        if ( !iIdle )
            {
            iIdle = CIdle::NewL( CActive::EPriorityStandard );
            }
        iIdle->Start( TCallBack( &AsyncCallback, this ) );
        }
    }

void CVPbkMultiContactOperationBase::Cancel()
    {
    const TInt count = iSubOperations.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iSubOperations[i]->Cancel();
        }
    
    if ( iIdle )
        {
        iIdle->Cancel();
        }
    }

TInt CVPbkMultiContactOperationBase::AsyncCallback( TAny* aThis )
    {
    CVPbkMultiContactOperationBase* self = 
        static_cast<CVPbkMultiContactOperationBase*>( aThis );
    self->HandleZeroSuboperations();
    // Don't continue
    return 0;
    }
// End of File

