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

#include "cntimageeditorview.h"

#include <hblabel.h>
#include <xqaiwrequest.h>
#include <thumbnailmanager_qt.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbframebackground.h>

#include <QStandardItemModel>

const char *CNT_IMAGE_XML = ":/xml/contacts_if.docml";

#define FETCHER_SERVICE "com.nokia.services.media"
#define FETCHER_INTERFACE "image"
#define FETCHER_OPERATION "fetch(QVariantMap,QVariant)"

/*!
Constructor
*/
CntImageEditorView::CntImageEditorView() :
        mContact(0),
        mAvatar(0),
        mImageLabel(0),
        mRequest(0),
        mThumbnailManager(0),
        mView(0),
        mSoftkey(0),
        mRemoveImage(0),
        mViewManager(0),
        mListView(0),
        mModel(0)
{
    bool ok = false;
    mDocumentLoader.load(CNT_IMAGE_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_if.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    // menu action
    mRemoveImage = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:removeimage"));
    connect(mRemoveImage, SIGNAL(triggered()), this, SLOT(removeImage()));

    // thumbnail manager
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
	
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
        this, SLOT(thumbnailReady(QPixmap, void*, int, int)) );
}

/*!
Destructor
*/
CntImageEditorView::~CntImageEditorView()
{
    mView->deleteLater();

    delete mAvatar;
    mAvatar = 0;
    delete mContact;
    mContact = 0;
    delete mRequest;
    mRequest = 0;
    delete mRemoveImage;
    mRemoveImage = 0;
    delete mModel;
    mModel = 0;
}

/*!
Called when activating the view
*/
void CntImageEditorView::activate( CntAbstractViewManager* aMgr, const CntViewParameters& aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());    
    
    mContact = new QContact(aArgs.selectedContact());
    mViewManager = aMgr;

    // set the correct image if the contact already has an image set
    mImageLabel = static_cast<HbLabel*>(mDocumentLoader.findWidget(QString("cnt_image_label")));
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();

    if (details.count() > 0)
    {
        for (int i = 0;i < details.count();i++)
        {
            if (details.at(i).subType() == QContactAvatar::SubTypeImage)
            {
                mAvatar = new QContactAvatar(details.at(i));
                mThumbnailManager->getThumbnail(mAvatar->avatar());
                break;
            }
        }
    }
    else
    {
        mAvatar = new QContactAvatar();
        mAvatar->setSubType(QContactAvatar::SubTypeImage);
        mRemoveImage->setEnabled(false);
    }
    
    // set up the list
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("cnt_listview")));
    
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this,
        SLOT(listViewActivated(const QModelIndex&)));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    
    mModel = new QStandardItemModel();
    populateModel(mModel);
    mListView->setModel(mModel);
}

void CntImageEditorView::deactivate()
{

}

void CntImageEditorView::populateModel(QStandardItemModel *model)
{
    QStandardItem *newPhoto = new QStandardItem();
    newPhoto->setText(hbTrId("txt_phob_list_take_a_new_photo"));
    newPhoto->setData(HbIcon("qtg_large_camera"), Qt::DecorationRole);
    model->appendRow(newPhoto);
    
    QStandardItem *fromGallery = new QStandardItem();
    fromGallery->setText(hbTrId("txt_phob_list_chooce_from_gallery"));
    fromGallery->setData(HbIcon("qtg_large_photos"), Qt::DecorationRole);
    model->appendRow(fromGallery);
}

/*!
Open camera and get new image for contact
*/
void CntImageEditorView::openCamera()
{

}

/*!
Open gallery and select image for contact
*/
void CntImageEditorView::openGallery()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest = 0;
    }

    mRequest = mAppManager.create(FETCHER_SERVICE, FETCHER_INTERFACE, FETCHER_OPERATION, false);
    if ( mRequest ) 
    {
        connect(mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleImageChange(const QVariant&)));
        mRequest->send();
    }
}

/*!
Called when user closes the view
*/
void CntImageEditorView::showPreviousView()
{
    mContact->saveDetail(mAvatar);

    if (mAvatar->avatar().isEmpty())
    {
        mContact->removeDetail(mAvatar);
    }
    
    CntViewParameters args;
    args.setSelectedContact( *mContact );
    mViewManager->back( args );
}

void CntImageEditorView::removeImage()
{
    mAvatar->setAvatar(QString());
    mImageLabel->clear();
    mImageLabel->setIcon(HbIcon("qtg_large_avatar"));
    mRemoveImage->setEnabled(false);
}

/*!
Set the selected image as new QContactAvatar::SubTypeImage
*/
void CntImageEditorView::handleImageChange(const QVariant &value)
{
    if (value.canConvert<QString>())
    {
        mAvatar->setAvatar(value.toString());
        mThumbnailManager->getThumbnail(value.toString());
        mRemoveImage->setEnabled(true);
    }
}

void CntImageEditorView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    if (!error)
    {
        QIcon qicon(pixmap);
        HbIcon icon(qicon);
        mImageLabel->clear();
        mImageLabel->setIcon(icon);
    }
}

void CntImageEditorView::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocumentLoader.load(CNT_IMAGE_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        mDocumentLoader.load(CNT_IMAGE_XML, "landscape");
    }
}

/*!
Figure out which item was selected according to row number
*/
void CntImageEditorView::listViewActivated(const QModelIndex &index)
{
    if (index.isValid())
    {
        int row = index.row();
        switch(row)
        {
        case 0: // first item is always "take photo"
            openCamera();
            break;
        case 1: // and the second one is always "choose from gallery"
            openGallery();
            break;
        default:
            break;
        }
    }
}
