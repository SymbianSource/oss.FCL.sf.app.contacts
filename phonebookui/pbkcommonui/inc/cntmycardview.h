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

#ifndef CNTMYCARDVIEW_H_
#define CNTMYCARDVIEW_H_

#include <QObject>
#include <hbdocumentloader.h>

#include <cntabstractview.h>

class CntAbstractViewManager;
class CntFetchContacts;
class HbView;
class HbAction;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntMyCardView : public QObject, public CntAbstractView
{

Q_OBJECT

public:
    CntMyCardView();
    ~CntMyCardView();

public: // From CntAbstractView
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return myCardView; }
    
#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif
    void showPreviousView();
    void openNameEditor();
    void openMyCardSelectionView(); 
    void handleMultiCardSelection();
    void setOrientation(Qt::Orientation orientation);

private: 
    
    void removeFromGroup(const QContact* contact);
    
private:  
    QContact*                 mContact; // own
    CntAbstractViewManager*   mViewManager;
    HbDocumentLoader          mDocumentLoader;
    HbView*                   mView; // own
    HbAction*                 mSoftkey;
    CntFetchContacts*         mFetchView;
};

#endif /* CNTMYCARDVIEW_H_ */
