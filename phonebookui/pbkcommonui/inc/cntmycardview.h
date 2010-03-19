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
#include "cntbaseview.h"
#include "cntviewparameters.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntMyCardView : public CntBaseView
{

Q_OBJECT

public slots:

    void aboutToCloseView();
    void openNameEditor();
    void openMyCardSelectionView();

public: // from CntBaseView
    virtual void setOrientation(Qt::Orientation orientation);
    
public:

    CntMyCardView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntMyCardView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::myCardView; }
    void activateView(const CntViewParameters &aViewParameters);

private:
    
    QContact* mContact;

};

#endif /* CNTMYCARDVIEW_H_ */
