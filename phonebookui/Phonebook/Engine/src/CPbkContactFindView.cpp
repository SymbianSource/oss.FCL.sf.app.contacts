/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*		Sub view to a CContactViewBase
*
*/


// INCLUDES
#include "CPbkContactFindView.h"
#include "CPbkContactFindViewImpl.h"
#include "CPbkContactEngine.h"
#include <PbkEngUtils.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkContactFindView");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkContactFindView* CPbkContactFindView::NewL
        (CPbkContactEngine& aEngine,
        CContactViewBase& aBaseView,
        MContactViewObserver& aObserver,
        MPbkFindPrimitives& aFindPrimitives)
	{
	CPbkContactFindView* self = 
        new(ELeave) CPbkContactFindView(aEngine,aBaseView,aFindPrimitives);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C TBool CPbkContactFindView::SetFindTextL
        (const TDesC& aFindText, 
        const CContactIdArray* aIncludeAlways/*=NULL*/)
    {
    return iCurrentState->SetFindTextL(aFindText, aIncludeAlways);
    }

EXPORT_C const TDesC& CPbkContactFindView::FindText() const
    {
    return iCurrentState->FindText();
    }

EXPORT_C TBool CPbkContactFindView::ResetFind()
    {
    return iCurrentState->ResetFind();
    }

EXPORT_C TInt CPbkContactFindView::IndexOfFirstFindMatchL() const
    {
    return iCurrentState->IndexOfFirstFindMatchL();
    }

TContactItemId CPbkContactFindView::AtL(TInt aIndex) const
	{
    return iCurrentState->AtL(aIndex);
	}

const CViewContact& CPbkContactFindView::ContactAtL(TInt aIndex) const
	{
    return iCurrentState->ContactAtL(aIndex);
	}

TInt CPbkContactFindView::CountL() const
	{
    return iCurrentState->CountL();
	}

TInt CPbkContactFindView::FindL(TContactItemId aId) const
	{
	return iCurrentState->FindL(aId);
	}

HBufC* CPbkContactFindView::AllFieldsLC
        (TInt aIndex,const TDesC& aSeparator) const
	{
    return iCurrentState->AllFieldsLC(aIndex,aSeparator);
	}

TContactViewPreferences CPbkContactFindView::ContactViewPreferences()
	{
    // No need to delegate to iCurrentState because this call is always
    // delegated to iBaseView in all states.
    return iBaseView.ContactViewPreferences();
	}

const RContactViewSortOrder& CPbkContactFindView::SortOrderL() const
    {
    // No need to delegate to iCurrentState because this call is always
    // delegated to iBaseView in all states.
    return iBaseView.SortOrderL();
    }

TAny* CPbkContactFindView::CContactViewBase_Reserved_1
        (TFunction aFunction, TAny* aParams)
	{
 	return iCurrentState->CContactViewBase_Reserved_1(aFunction,aParams);
	}

/**
 * Handles an event from iBaseView and synchronizes this view's state
 * accordingly.
 */
void CPbkContactFindView::HandleContactViewEvent
        (const CContactViewBase& aView, const TContactViewEvent& aEvent)
	{
    __ASSERT_DEBUG(&aView==&iBaseView, Panic(EPanicPreCond_HandleContactViewEvent));
    
    // First handle event self
    TRAPD(err, HandleContactViewEventL(aEvent));
    if (err != KErrNone)
        {
        NotifyObservers
            (TContactViewEvent(TContactViewEvent::ESortError,err,KNullContactId));
        }

    // Let the current state handle the event
    iCurrentState->HandleContactViewEvent(aView,aEvent);
	}

CPbkContactFindView::CPbkContactFindView
        (CPbkContactEngine& aEngine, 
        CContactViewBase& aView,
        MPbkFindPrimitives& aFindPrimitives) :
    CContactViewBase(aEngine.Database()), 
    iBaseView(aView),
    iNameFormatter(aEngine.ContactNameFormat()),
    iFindPrimitives(&aFindPrimitives)
	{
    // CBase::operator new(TLeave) will reset other members
	}
	
void CPbkContactFindView::ConstructL
        (MContactViewObserver& aObserver)
    {
	CContactViewBase::ConstructL();
    iCurrentState = MState::CreateInitialStateL(*this);
	OpenL(aObserver);
    iBaseView.OpenL(*this);
    iFindPrimitives->SetContactViewSortOrderL(SortOrderL());
	}

CPbkContactFindView::~CPbkContactFindView()
	{
    iBaseView.Close(*this);
    delete iCurrentState;
	}

void CPbkContactFindView::HandleContactViewEventL
        (const TContactViewEvent& aEvent)
    {
    if (aEvent.iEventType == TContactViewEvent::ESortOrderChanged)
        {
        // Sort order change needs to ne notified to iFindPrimitives
        const RContactViewSortOrder& sortOrder = SortOrderL();
        iFindPrimitives->SetContactViewSortOrderL(sortOrder);
        }
    }

// End of File
