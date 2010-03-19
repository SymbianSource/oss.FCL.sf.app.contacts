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

#ifndef CNTIMAGEEDITORVIEW_H
#define CNTIMAGEEDITORVIEW_H

#include <QObject>
#include "cntbaseview.h"
#include <xqappmgr.h>

class HbLabel;
class HbScrollArea;
class XQAiwRequest;
class ThumbnailManager;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactAvatar;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntImageEditorView : public CntBaseView
{
    Q_OBJECT

public:
    CntImageEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntImageEditorView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::imageEditorView; }
    void activateView(const CntViewParameters &viewParameters);

public slots:
    void aboutToCloseView();
    void openCamera();
    void openGallery();
    void handleImageChange(const QVariant &value);
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QContact             *mContact;
    QContactAvatar       *mAvatar;
    HbLabel              *mImageLabel;
    HbScrollArea         *mScrollArea;
    XQAiwRequest         *mRequest;
    XQApplicationManager  mAppManager;
    ThumbnailManager     *mThumbnailManager;
    HbWidget             *mContainerWidget;
};

#endif // CNTIMAGEEDITORVIEW_H
