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

#include <QGraphicsLinearLayout>
#include <qtcontacts.h>
#include <hbdocumentloader.h>
#include <hbabstractviewitem.h>
#include <hbscrollarea.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <xqaiwrequest.h>
#include <thumbnailmanager_qt.h>

const char *CNT_IMAGE_XML = ":/xml/contacts_if.docml";
const int BUTTON_HEIGHT = 50;

#define FETCHER_SERVICE "com.nokia.services.media"
#define FETCHER_INTERFACE "image"
#define FETCHER_OPERATION "fetch(QVariantMap,QVariant)"

/*!
Constructor
*/
CntImageEditorView::CntImageEditorView(CntViewManager *viewManager, QGraphicsItem *parent) :
        CntBaseView(viewManager, parent),
        mContact(0),
        mAvatar(0),
        mImageLabel(0),
        mScrollArea(0),
        mRequest(0),
        mThumbnailManager(0),
        mContainerWidget(0)
{
    bool ok = false;
    ok = loadDocument(CNT_IMAGE_XML);

    if (ok)
    {
        QGraphicsWidget *content = findWidget(QString("content"));
        setWidget(content);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_if.docml");
    }
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
	
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
        this, SLOT(thumbnailReady(QPixmap, void*, int, int)) );
}

void CntImageEditorView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    QIcon qicon(pixmap);
    HbIcon icon(qicon);
	mImageLabel->clear();
	mImageLabel->setIcon(icon);
}

/*!
Destructor
*/
CntImageEditorView::~CntImageEditorView()
{
    delete mImageLabel;
    delete mAvatar;
    delete mContact;
	delete mRequest;
}

void CntImageEditorView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (mScrollArea)
    {
        mContainerWidget->resize(mScrollArea->size().width(), 0);
    }
    CntBaseView::resizeEvent(event);
}

/*!
Called when user closes the view
*/
void CntImageEditorView::aboutToCloseView()
{
    mContact->saveDetail(mAvatar);

    if (mAvatar->avatar().isEmpty())
    {
        mContact->removeDetail(mAvatar);
    }
    viewManager()->previousViewParameters().setSelectedContact(*mContact);
    viewManager()->onActivatePreviousView();
}

/*!
Called when activating the view (create image label and scroll area with buttons)
*/
void CntImageEditorView::activateView(const CntViewParameters &viewParameters)
{
    mContact = new QContact(viewParameters.selectedContact());

    // set the correct image if the contact already has an image set
    mImageLabel = static_cast<HbLabel*>(findWidget(QString("image")));
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
    }

    QGraphicsWidget *w = findWidget(QString("content"));
    static_cast<QGraphicsLinearLayout*>(w->layout())->setAlignment(mImageLabel, Qt::AlignCenter);

    // construct listview and the model for it
    mScrollArea = static_cast<HbScrollArea*>(findWidget(QString("scrollArea")));

    mContainerWidget = new HbWidget();
    QGraphicsLinearLayout *buttonLayout = new QGraphicsLinearLayout(Qt::Vertical);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    HbPushButton *newPhoto = new HbPushButton(HbIcon(":/icons/qgn_indi_ai_nt_camera.svg"),
                                              hbTrId("txt_phob_list_take_a_new_photo"), Qt::Horizontal);
    newPhoto->setPreferredHeight(BUTTON_HEIGHT);
    newPhoto->setPreferredWidth(300);
    newPhoto->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(newPhoto, SIGNAL(clicked()), this, SLOT(openCamera()));
    buttonLayout->addItem(newPhoto);

    HbPushButton *gallery = new HbPushButton(HbIcon(":/icons/qgn_prop_tutor_gallery.svg"),
                                              hbTrId("Choose from Gallery"), Qt::Horizontal);
    gallery->setPreferredHeight(BUTTON_HEIGHT);
    gallery->setPreferredWidth(300);
    gallery->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(gallery, SIGNAL(clicked()), this, SLOT(openGallery()));
    buttonLayout->addItem(gallery);

    mContainerWidget->setLayout(buttonLayout);
    mContainerWidget->setPreferredWidth(mScrollArea->size().width());
    mScrollArea->setContentWidget(mContainerWidget);
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

    connect(mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleImageChange(const QVariant&)));
    mRequest->send();
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
    }
}
