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

#ifndef CntFetchContacts_H_
#define CntFetchContacts_H_

#include <hbabstractitemview.h>
#include <hbwidget.h>
#include <hbtextitem.h>
#include <cntviewparams.h>
#include <QSet>

class HbDialog;
class HbSearchPanel;
class HbAction;
class HbListView;
class HbLabel;
class HbStaticVkbHost;
class HbIndexFeedback;

class CntListModel;
class QGraphicsLinearLayout;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntFetchContacts : public QObject
    {
    Q_OBJECT

public:
    CntFetchContacts(QContactManager &aManager);
    ~CntFetchContacts();
    
private slots:
    void memberSelectionChanged(const QModelIndex &index);
    
    void setFilter(const QString &filterString);
    
    void handleKeypadOpen();
    void handleKeypadClose();
    
    void handleUserResponse(HbAction* action);
    
public:
    void displayContacts(HbAbstractItemView::SelectionMode aMode,
                                     QSet<QContactLocalId> aContacts);
    
    QSet<QContactLocalId> getSelectedContacts() const;
    bool wasCanceled() const;
    void setDetails(QString aTitle, QString aButtonText);

private:
    void doInitialize(HbAbstractItemView::SelectionMode aMode,
                                  QSet<QContactLocalId> aContacts);
    void showPopup();
    void markMembersInView();
    
signals:
    void clicked();
    
private:
    HbDialog*                          mPopup;
    HbSearchPanel*                     mSearchPanel;
    CntListModel*                      mCntModel;
    HbListView*                        mListView;
    QGraphicsLinearLayout*             mLayout;
    HbTextItem*                        mEmptyListLabel;
    HbWidget*                          mContainerWidget;
    HbAbstractItemView::SelectionMode  mSelectionMode;
    QSet<QContactLocalId>              mCurrentlySelected;
    QContactManager*                   mManager;
    bool                               mWasCanceled;
    HbLabel*                           mLabel;
    HbStaticVkbHost*                   mVirtualKeyboard;
    QString                            mButtonText;
    HbAction*                          mPrimaryAction;
    HbAction*                          mSecondaryAction;
    HbIndexFeedback*                   mIndexFeedback;
    
    friend class TestCntFetchUtility;
    friend class TestCntMyCardView;
    };

#endif /* CntFetchContacts_H_ */
