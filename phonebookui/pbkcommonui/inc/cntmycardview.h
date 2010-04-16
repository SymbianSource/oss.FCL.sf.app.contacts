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

#include "cntabstractview.h"
#include "cntviewparameters.h"

class CntAbstractViewManager;
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
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters& aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    CntViewParameters::ViewId viewId() const { return CntViewParameters::myCardView; }
    
private slots:
    void showPreviousView();
    void openNameEditor();
    void openMyCardSelectionView(); 
    void setOrientation(Qt::Orientation orientation);
    
private:  
    QContact* mContact; // own
    CntAbstractViewManager* mViewManager;
    HbDocumentLoader mDocumentLoader;
    HbView* mView; // own
    HbAction* mSoftkey;
};

#endif /* CNTMYCARDVIEW_H_ */
