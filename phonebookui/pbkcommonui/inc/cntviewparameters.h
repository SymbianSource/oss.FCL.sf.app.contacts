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

#ifndef CNTVIEWPARAMETERS_H
#define CNTVIEWPARAMETERS_H

#include <qtcontacts.h>
#include <QMap>
#include "qtpbkglobal.h"

QTM_USE_NAMESPACE

class QTPBK_EXPORT CntViewParameters
{
public:
    enum ViewId
        {
        noView=0, //exit
        defaultView, //first view
        namesView,
        commLauncherView,
        serviceContactCardView,
        serviceAssignContactCardView,
        myCardView,
        myCardSelectionView,
        serviceContactSelectionView,
        collectionView,
        collectionFavoritesView,
        FavoritesMemberView,
        editView,
        serviceEditView,
        serviceSubEditView,
        emailEditorView,
        namesEditorView,
        urlEditorView,
        companyEditorView,
        phoneNumberEditorView,
        onlineAccountEditorView,
        noteEditorView,
        familyDetailEditorView,
        addressEditorView,
        dateEditorView,
        imageEditorView,
        serviceContactFetchView,
        groupEditorView,
        groupMemberView,
        groupActionsView,
        historyView
        };
public:
    enum ParamId
        {
        Action,
        Filter,
        Title
        };

public:
    CntViewParameters(CntViewParameters::ViewId activateView = CntViewParameters::noView,
                        CntViewParameters::ViewId previousViewId = CntViewParameters::defaultView);

    CntViewParameters( const CntViewParameters &viewParameters );

    virtual ~CntViewParameters();

public:
    //view to be activated
    void setNextViewId(const CntViewParameters::ViewId& activateViewId);
    CntViewParameters::ViewId nextViewId() const;

    //view to be called when user clicks on back in the activated view
    void setPreviousViewId(const CntViewParameters::ViewId previousViewId);
    CntViewParameters::ViewId previousViewId() const;

    //selected contact
    void setSelectedContact(const QContact &contact);
    QContact selectedContact() const;
    
    //selected contactdetail
    void setSelectedDetail(const QContactDetail &detail);
    QContactDetail selectedDetail() const;
    
    //selected action (needed for popup display at least)
    void setSelectedAction(const QString &action);
    QString selectedAction() const;
    
    //selected group contact
    void setSelectedGroupContact(const QContact &contact);
    QContact selectedGroupContact() const;
    
    void setParameters(const QMap<int,QVariant>& aMap);
    QMap<int,QVariant> parameters() const;

private:
    CntViewParameters::ViewId   mActivateViewId;
    CntViewParameters::ViewId   mPreviousViewId;

    QContact                    mContact;
    QContactDetail              mDetail;
    QString                     mAction;
    QContact                    mGroupContact;
    QMap<int,QVariant>          mParams;

};
#endif //CNTVIEWPARAMETERS_H

// EOF
