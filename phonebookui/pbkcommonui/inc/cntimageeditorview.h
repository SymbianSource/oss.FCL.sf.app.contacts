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
#include <hbdocumentloader.h>
#include <xqappmgr.h>

#include <cntabstractview.h>

class HbLabel;
class HbListView;
class XQAiwRequest;
class ThumbnailManager;
class HbAction;
class QStandardItemModel;
class QModelIndex;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactAvatar;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntImageEditorView : public QObject, public CntAbstractView
{
    Q_OBJECT

public:
    CntImageEditorView();
    ~CntImageEditorView();
    
public: // From CntAbstractView
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return imageEditorView; }
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void populateModel(QStandardItemModel *model);
    void openCamera();
    void openGallery();

#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif
    void showPreviousView();
    void removeImage();
    void handleImageChange(const QVariant &value);
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    void setOrientation(Qt::Orientation orientation);
    void listViewActivated(const QModelIndex &index);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QContact                *mContact; // own
    QContactAvatar          *mAvatar; // own
    HbLabel                 *mImageLabel; // owned by layout
    XQAiwRequest            *mRequest; // own
    XQApplicationManager     mAppManager;
    ThumbnailManager        *mThumbnailManager; // own
    HbView                  *mView; // own
    HbAction                *mSoftkey; // owned by view
    HbAction                *mRemoveImage; // own
    CntAbstractViewManager  *mViewManager;
    HbDocumentLoader         mDocumentLoader;
    HbListView              *mListView; // owned by layout
    QStandardItemModel      *mModel; // own
    
    CntViewParameters       mArgs;
};

#endif // CNTIMAGEEDITORVIEW_H
