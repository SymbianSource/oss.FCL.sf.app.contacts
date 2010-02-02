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
*       View activator - activates a tab view upon client request
*
*/


// INCLUDE FILES
#include "CxSPViewActivator.h"
#include <CxSPViewData.h>
#include "CxSPViewInfo.h"


// System includes
#include <aknViewAppUi.h>

// Unnamed namespace for local definitions
namespace
    {
    const TInt KDesAddressParam = 2; // Des address is in param ix 2
    _LIT( KPanicText, "xSPViewActivator" );
    
    enum TPanicCode
        {
        EBadDescriptorLength,
        EBadDescriptor
        };
    } // namespace

// ================= MEMBER FUNCTIONS =======================

CxSPViewActivator* CxSPViewActivator::NewL(CxSPViewIdChanger* aViewIdChanger)
    {
    CxSPViewActivator* self = new(ELeave) CxSPViewActivator(aViewIdChanger);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();    // self

    return self;
    }

CxSPViewActivator::CxSPViewActivator(CxSPViewIdChanger* aViewIdChanger) : 
                                                CServer2(EPriorityStandard)
    {
    iViewIdChanger = aViewIdChanger;
    }

void CxSPViewActivator::ConstructL()
    {
    StartL(KxSPServer);
    }

CxSPViewActivator::~CxSPViewActivator()
    {
    }

CSession2* CxSPViewActivator::NewSessionL(const TVersion& aVersion, const RMessage2&) const
    {
    TBool supported = User::QueryVersionSupported(TVersion(KxSPViewServerMajor,
                                                    KxSPViewServerMinor,
                                                    KxSPViewServerBuild),
                                                    aVersion);
    if(!supported)
        {
        User::Leave(KErrNotSupported);
        }

    return new (ELeave) CxSPViewActivatorSession();
    }

void CxSPViewActivator::ActivateView1L(const RMessage2 &aMessage)
    {
    TUint32 ecomID = aMessage.Int0();
    TUint32 viewID = aMessage.Int1();

    TInt err = KErrNone;
    TInt newViewId;
    if(ecomID)
        {
        err = iViewIdChanger->GetNewView(ecomID, viewID, newViewId);
        }
    else
        {
        newViewId = viewID;
        }

    aMessage.Complete(err);

    if(err == KErrNone)
        {
        // Activate the view
        static_cast<CAknViewAppUi*>(CCoeEnv::Static()->AppUi())->ActivateLocalViewL(
                                                                TUid::Uid((TInt)newViewId));
        }

    return;
    }

void CxSPViewActivator::ActivateView2L(const RMessage2 &aMessage)
    {
    TInt err = KErrNone;

    TUint32 ecomID = aMessage.Int0();
    TUint32 viewID = aMessage.Int1();
    TInt desLen = aMessage.GetDesLength(KDesAddressParam);
    if( desLen <= 0 )
        {
        aMessage.Panic( KPanicText, EBadDescriptorLength );
        }
    else
        {
        HBufC8* paramBuf = HBufC8::NewL(desLen);
        CleanupStack::PushL(paramBuf);
        TPtr8 ptr = paramBuf->Des();
        aMessage.ReadL(KDesAddressParam, ptr);
    
        TInt newViewId;
        if(ecomID)
            {
            err = iViewIdChanger->GetNewView(ecomID, viewID, newViewId);
            }
        else
            {
            newViewId = viewID;
            }
    
        aMessage.Complete(err);
    
        if(err == KErrNone)
            {
            // Make view id
            const TVwsViewId viewId(TUid::Uid(KUid), TUid::Uid(newViewId));
            // Activate the view
            static_cast<CAknViewAppUi*>(CCoeEnv::Static()->AppUi())->ActivateViewL(
                                                        viewId, CPbk2ViewState::Uid(), *paramBuf);
            }
    
        CleanupStack::PopAndDestroy();  // paramBuf;
        }
    }

void CxSPViewActivator::ActivateView2AsyncL(const RMessage2 &aMessage)
    {
    TInt err = KErrNone;

    TUint32 ecomID = aMessage.Int0();
    TUint32 viewID = aMessage.Int1();
    TInt desLen = aMessage.GetDesLength(KDesAddressParam);
    if( desLen <= 0 )
        {
        aMessage.Panic( KPanicText, EBadDescriptorLength );
        }
    else
        {
        HBufC8* paramBuf = HBufC8::NewL(desLen);
        CleanupStack::PushL(paramBuf);
        TPtr8 ptr = paramBuf->Des();
        aMessage.ReadL(KDesAddressParam, ptr);
    
        TInt newViewId;
        if(ecomID)
            {
            err = iViewIdChanger->GetNewView(ecomID, viewID, newViewId);
            }
        else
            {
            newViewId = viewID;
            }
    
        aMessage.Complete(err);
    
        if(err == KErrNone)
            {
            // Make view id
            const TVwsViewId viewId(TUid::Uid(KUid), TUid::Uid(newViewId));
            // Activate the view
            static_cast<CAknViewAppUi*>(CCoeEnv::Static()->AppUi())->ActivateViewL(
                                                        viewId, CPbk2ViewState::Uid(), *paramBuf);
            }
    
        CleanupStack::PopAndDestroy();  // paramBuf;
        }

    }
    
void CxSPViewActivator::GetViewCountL(const RMessage2 &aMessage)
    {
    TInt tabsCount;
    TInt dummy;
    iViewIdChanger->GetViewCount( tabsCount, dummy );   
    TPckgC<TInt> tabsCountDes( tabsCount );
    TRAPD( err, aMessage.WriteL( 0, tabsCountDes ) );
    if( err != KErrNone )
        {
        aMessage.Panic( KPanicText, EBadDescriptor );
        User::Leave( err );
        }
    else
        {
        aMessage.Complete( KErrNone );
        }
    }
        
void CxSPViewActivator::GetViewDataPackLengthL(const RMessage2 &aMessage)
    {
    TInt index = aMessage.Int0();
    TInt tabsCount;
    TInt dummy;
    iViewIdChanger->GetViewCount( tabsCount, dummy );
    if( index < 0 )
        {
        aMessage.Complete( KErrUnderflow );
        }
    else if( index >= tabsCount )
        {
        aMessage.Complete( KErrOverflow );
        }
    else
        {
        RPointerArray<CxSPViewInfo> infoArray;
        CleanupClosePushL( infoArray );
        iViewIdChanger->GetTabViewInfoL( infoArray );
        const CxSPViewInfo& info = *infoArray[index];
        CxSPViewData* viewData = CxSPViewData::NewL();
        CleanupStack::PushL( viewData );
        viewData->SetEComId( info.Id() );
        viewData->SetOriginalViewId( info.OldViewId() );
        viewData->SetIconId( info.SortIconId() );
        viewData->SetMaskId( info.SortMaskId() );
        viewData->SetIconFileL( info.SortIconFile() );
        viewData->SetViewNameL( info.Name() );
        HBufC8* packed = viewData->PackL();
        TInt length = packed->Length();
        delete packed;                      
        CleanupStack::PopAndDestroy(); // viewData
        CleanupStack::PopAndDestroy(); // infoArray     
        TPckgC<TInt> lengthDes( length );
        TRAPD( err, aMessage.WriteL( 1, lengthDes ) );
        if( err != KErrNone )
            {
            aMessage.Panic( KPanicText, EBadDescriptor );
            User::Leave( err );
            }
        else
            {
            aMessage.Complete( KErrNone );
            }
        }   
    }
        
void CxSPViewActivator::GetViewDataL(const RMessage2 &aMessage)
    {
    TInt index = aMessage.Int0();
    TInt tabsCount;
    TInt dummy;
    iViewIdChanger->GetViewCount( tabsCount, dummy );
    if( index < 0 )
        {
        aMessage.Complete( KErrUnderflow );
        }
    else if( index >= tabsCount )
        {
        aMessage.Complete( KErrOverflow );
        }
    else
        {
        RPointerArray<CxSPViewInfo> infoArray;
        CleanupClosePushL( infoArray );
        iViewIdChanger->GetTabViewInfoL( infoArray );
        const CxSPViewInfo& info = *infoArray[index];
        CxSPViewData* viewData = CxSPViewData::NewL();
        CleanupStack::PushL( viewData );
        viewData->SetEComId( info.Id() );
        viewData->SetOriginalViewId( info.OldViewId() );
        viewData->SetIconId( info.SortIconId() );
        viewData->SetMaskId( info.SortMaskId() );
        viewData->SetIconFileL( info.SortIconFile() );
        viewData->SetViewNameL( info.Name() );
        HBufC8* packed = viewData->PackL();
        CleanupStack::PushL( packed );      
        TRAPD( err, aMessage.WriteL( 1, *packed ) );
        if( err != KErrNone )
            {
            aMessage.Panic( KPanicText, EBadDescriptor );
            User::Leave( err );
            }
        else
            {
            aMessage.Complete( KErrNone );
            }
        CleanupStack::PopAndDestroy(); // packed                    
        CleanupStack::PopAndDestroy(); // viewData
        CleanupStack::PopAndDestroy(); // infoArray     
        }   
    }
           
void CxSPViewActivator::StartL(const TDesC &aName)
    {
    CServer2::StartL(aName);

    RSemaphore semaphore;
    if(semaphore.OpenGlobal(KxSPServer) == KErrNone)
        {
        semaphore.Signal();
        semaphore.Close();
        }
    }

void CxSPViewActivatorSession::ServiceL(const RMessage2 &aMessage)
    {
    // There are currently separate synchronous and asynchronous op codes for
    // some of the services. The server side implementation is however
    // synchronous and the opcodes are handled similarily whenever possible.
    // For example EGetViewCount and EGetViewCountAsync are treated identically
    // and the corresponding async cancel operation (ECancelGetViewCountAsync)
    // does nothing and completes immediately.

    switch(aMessage.Function())
        {
        case EActivateView1:
            ((CxSPViewActivator*)Server())->ActivateView1L(aMessage);
            break;
        case EActivateView2:
            ((CxSPViewActivator*)Server())->ActivateView2L(aMessage);
            break;
        case EActivateView2Async:
            ((CxSPViewActivator*)Server())->ActivateView2AsyncL( aMessage );
            break;            
        case EGetViewCount: // FALLTHROUGH
        case EGetViewCountAsync:
            ((CxSPViewActivator*)Server())->GetViewCountL(aMessage);
            break;
        case ECancelGetViewCountAsync:
            aMessage.Complete( KErrNone );
            break;
        case EGetViewDataPackLength:
            ((CxSPViewActivator*)Server())->GetViewDataPackLengthL(aMessage);
            break;
        case EGetViewDataPackLengthAsync:
            ((CxSPViewActivator*)Server())->GetViewDataPackLengthL( aMessage );
            break;
        case ECancelGetViewDataPackLengthAsync:
            aMessage.Complete( KErrNone );               
            break;                                                                            
        case EGetViewData:
            ((CxSPViewActivator*)Server())->GetViewDataL(aMessage);
            break;
        case EGetViewDataAsync:
            ((CxSPViewActivator*)Server())->GetViewDataL( aMessage );
            break;
        case ECancelActivateView2Async:
            aMessage.Complete( KErrNone );
            break;
        case ECancelGetViewDataAsync:
            aMessage.Complete( KErrNone );
            break;
        default:
            aMessage.Complete(KErrNotSupported);
        }
    }
        
// End of file.
