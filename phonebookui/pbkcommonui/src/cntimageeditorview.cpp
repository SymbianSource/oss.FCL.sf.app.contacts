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
#include "cntimageutility.h"

#include <hblabel.h>
#include <xqaiwrequest.h>
#include <xqaiwdecl.h>

#include "cntdebug.h"

#include <thumbnailmanager_qt.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbframebackground.h>

#include <QStandardItemModel>

const char *CNT_IMAGE_XML = ":/xml/contacts_if.docml";

/*!
Constructor
*/
CntImageEditorView::CntImageEditorView() :
        mContact(NULL),
        mAvatar(NULL),
        mImageLabel(NULL),
        mRequest(NULL),
        mViewManager(NULL),
        mListView(NULL),
        mModel(NULL)
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
    CNT_ENTRY
    
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
    
    CNT_EXIT
}

/*!
Called when activating the view
*/
void CntImageEditorView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mViewManager = aMgr;
    mArgs = aArgs;
    
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    if ( window )
    {
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    }
    
    mContact = new QContact(mArgs.value(ESelectedContact).value<QContact>());
    
    // set the correct image if the contact already has an image set
    mImageLabel = static_cast<HbLabel*>(mDocumentLoader.findWidget(QString("cnt_image_label")));
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();
    if (details.count() > 0)
        {
        for (int i = 0;i < details.count();i++)
            {
                if (details.at(i).imageUrl().isValid())
                    {
                    mAvatar = new QContactAvatar(details.at(i));
                    mThumbnailManager->getThumbnail(mAvatar->imageUrl().toString());
                    break;
                    }
            }
        }
    else
        {
        mAvatar = new QContactAvatar();
        mRemoveImage->setEnabled(false);
        if (mContact->type() == QContactType::TypeGroup)
            {
            mImageLabel->setIcon(HbIcon("qtg_large_add_group_picture"));
            }
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
    mListView->setUniformItemSizes(true);
    
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
    CNT_ENTRY
    
    if (mRequest)
    {
        delete mRequest;
        mRequest = 0;
    }
    
    mRequest = mAppManager.create(XQI_CAMERA_CAPTURE, "capture(int,QVariantMap)", false);
    if ( mRequest ) 
    {
        int mode = 0; //image mode
        
        QVariantMap map;
        map.insert(XQCAMERA_INDEX, 0);
        map.insert(XQCAMERA_QUALITY, 0);
        map.insert(XQCAMERA_MODE_SWITCH, false);
        map.insert(XQCAMERA_INDEX_SWITCH, false);
        map.insert(XQCAMERA_QUALITY_CHANGE, true);
        
        // Set function parameters
        QList<QVariant> args;
        args << mode;
        args << map;
        mRequest->setArguments(args);
        
        connect(mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleImageChange(const QVariant&)));
        connect(mRequest, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

        mRequest->send();
    }
    
    CNT_EXIT
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
    
    mRequest = mAppManager.create(XQI_IMAGE_FETCH, XQOP_IMAGE_FETCH, true);
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

    if ( mAvatar->imageUrl().isEmpty())
    {
        mContact->removeDetail(mAvatar);
    }
    
    QVariant var;
    var.setValue(*mContact);
    
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(ESelectedGroupContact, var);
    mArgs.insert(ECustomParam, viewId());
    mViewManager->back( mArgs );
}

void CntImageEditorView::removeImage()
{
    QString filePath=mAvatar->imageUrl().toString();
    if(!filePath.isEmpty())
    {
        // Check if image removable.
        CntImageUtility imageUtility;
        if(imageUtility.isImageRemovable(filePath))
        {
            imageUtility.removeImage(filePath);
        }
        
        mAvatar->setImageUrl(QUrl());
        mImageLabel->clear();
        if (mContact->type() == QContactType::TypeGroup)
            mImageLabel->setIcon(HbIcon("qtg_large_add_group_picture"));
        else
            mImageLabel->setIcon(HbIcon("qtg_large_add_contact_picture"));
        mRemoveImage->setEnabled(false);
    }
}

/*!
Set the selected image as new QContactAvatar::SubTypeImage
*/
void CntImageEditorView::handleImageChange(const QVariant &value)
{
    CNT_ENTRY_ARGS("image path = " << value.toString())
    
    if(value.canConvert<QString>())
    {
        CntImageUtility imageUtility;
        if(imageUtility.isMassStorageAvailable())
        {
            CNT_LOG_ARGS("mass storage available")
            /* Copy image and create thumbnail
             * When contact image removed only copy is deleted
             */
            QString imagepath;
            if(imageUtility.createImage(value.toString(),imagepath))
            {
                
                // If image already assigned, delete
                QString filePath=mAvatar->imageUrl().toString();
                if(!filePath.isEmpty()
                   && imageUtility.isImageRemovable(filePath))
                    imageUtility.removeImage(filePath);
            
                CNT_LOG_ARGS("image created, image = " << filePath)
                
                mAvatar->setImageUrl(QUrl(imagepath));
                mThumbnailManager->getThumbnail(imagepath);
                mRemoveImage->setEnabled(true);
            }
        }
        else
        {
            // No mass memory, use the orginal image
            mAvatar->setImageUrl(QUrl(value.toString()));
            mThumbnailManager->getThumbnail(value.toString());
            mRemoveImage->setEnabled(true);
        }
    }
    
    CNT_EXIT
}

void CntImageEditorView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    CNT_ENTRY_ARGS("error code = " << error)
    
    Q_UNUSED(data);
    Q_UNUSED(id);
    if (!error)
    {
        QIcon qicon(pixmap);
        HbIcon icon(qicon);
        mImageLabel->clear();
        mImageLabel->setIcon(icon);
    }
    
    CNT_EXIT
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

void CntImageEditorView::handleError(int errorCode, const QString& errorMessage)
{
    CNT_LOG_ARGS("error code = " << errorCode << "errorMessage=" << errorMessage)
}

