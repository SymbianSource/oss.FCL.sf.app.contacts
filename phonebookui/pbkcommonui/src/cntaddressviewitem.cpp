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
#include "cntaddressviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"

#include <qcontactdetail.h>
#include <qcontactaddress.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbstyleloader.h>
#include <hbpushbutton.h>
#include <qgraphicslinearlayout.h>
#include <hbwidget.h>
#include <hbtextitem.h>

//#include "qlocationpickeritem_temp.h"
#include <xqaiwrequest.h>
#include <xqservicerequest.h>
#include <xqappmgr.h>
CntAddressViewItem::CntAddressViewItem(QGraphicsItem* aParent) :
    /*CntDetailViewItem(aParent),*/
    HbDataFormViewItem(aParent),
    mRequest(NULL),
    mAppManager(NULL), 
    mSenderButton(NULL)
{
}

CntAddressViewItem::~CntAddressViewItem()
{
    delete mRequest;
    delete mAppManager;
}

HbAbstractViewItem* CntAddressViewItem::createItem()
{
    return new CntAddressViewItem(*this);
}

HbWidget* CntAddressViewItem::createCustomWidget()
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Horizontal);

    HbWidget* widget = new HbWidget();
    widget->setLayout(layout);

    HbStyleLoader::registerFilePath(":/style/cntlocationbutton.css");
    HbStyleLoader::registerFilePath(":/style/cntlocationbutton.hbpushbutton.widgetml");
    HbDataForm* form = static_cast<HbDataForm*> (itemView());
    HbDataFormModel* model = static_cast<HbDataFormModel*> (form->model());

    HbPushButton* mLocationButton = new HbPushButton(this);
    mLocationButton->setObjectName("cntlocationbutton");
    mLocationButton->setIcon(HbIcon(":/icons/pickerIcon.jpg"));
    mLocationButton->setText(qtTrId("Select location from map"));
    mLocationButton->setOrientation(Qt::Horizontal);

    connect(mLocationButton, SIGNAL(clicked()), this, SLOT(launchLocationPicker()));

    layout->addItem(mLocationButton);
    return widget;
}

void CntAddressViewItem::launchLocationPicker()
{
    mAppManager = new XQApplicationManager();
    if (mRequest) {
        delete mRequest;
        mRequest = 0;
    }

    mSenderButton = sender();
    mRequest = mAppManager->create("com.nokia.symbian", "ILocationPick", "pick()", false);
    if (mRequest) {
        connect(mRequest, SIGNAL(requestOk(const QVariant&)), this,
            SLOT(handleLocationChange(const QVariant&)));
        mRequest->send();
    }
}

void CntAddressViewItem::handleLocationChange(const QVariant& aValue)
{
    Q_UNUSED( aValue );
}

//Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)

// End of File
