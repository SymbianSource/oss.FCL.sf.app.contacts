/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cntviewparameters.h"

CntViewParameters::CntViewParameters(CntViewParameters::ViewId activateView, CntViewParameters::ViewId previousViewId) :
    mActivateViewId(activateView),
    mPreviousViewId(previousViewId)
{
}

CntViewParameters::CntViewParameters( const CntViewParameters &viewParameters )
{
    mActivateViewId = viewParameters.nextViewId();
    mPreviousViewId = viewParameters.previousViewId();

    mContact        = viewParameters.selectedContact();
    mDetail         = viewParameters.selectedDetail();
    mGroupContact   = viewParameters.selectedGroupContact();
}

CntViewParameters::~CntViewParameters()
{
}

void CntViewParameters::setNextViewId(const ViewId activateViewId)
{
    mActivateViewId = activateViewId;
}

CntViewParameters::ViewId CntViewParameters::nextViewId() const
{
    return mActivateViewId;
}

void CntViewParameters::setPreviousViewId(const CntViewParameters::ViewId previousViewId)
{
    mPreviousViewId = previousViewId;
}

CntViewParameters::ViewId CntViewParameters::previousViewId() const
{
    return mPreviousViewId;
}

void CntViewParameters::setSelectedContact(const QContact &contact)
{
    mContact = contact;
}

QContact CntViewParameters::selectedContact() const
{
    return mContact;
}

void CntViewParameters::setSelectedDetail(const QContactDetail &detail)
{
    mDetail = detail;
}

QContactDetail CntViewParameters::selectedDetail() const
{
    return mDetail;
}

void CntViewParameters::setSelectedAction(const QString &action)
{
    mAction = action;
}

QString CntViewParameters::selectedAction() const
{
    return mAction;
}

void CntViewParameters::setSelectedGroupContact(const QContact &contact)
{
    mGroupContact = contact;
}

QContact CntViewParameters::selectedGroupContact() const
{
    return mGroupContact;
}
