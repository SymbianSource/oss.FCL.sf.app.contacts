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
* Description: Private data for CntListModel.
*
*/

#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <qtcontacts.h>
#include <cntlistmodel_p.h>
#include <cntcache.h>
#include <cntdisplaytextformatter.h>
#include <cntdebug.h>

/*!
    \class CntListModel
    \brief Private data for CntListModel.
 */

/*!
    Initializes all data.
 */
CntListModelData::CntListModelData(QContactManager *manager, const QContactFilter &filter, bool showMyCard)
    : mContactManager(manager),
      mCache(CntCache::createSession(this, manager)),
      mGroupId(0),
      mCurrentContact(NULL),
      mCurrentRow(-1),
      mShowMyCard(showMyCard),
      mMyCardId(manager->selfContactId()),
      mSettings(NULL),
      mNameListRowSettingkey(NULL),
      mCurrentRowSetting(0),
      mFormat(new CntDummyDisplayTextFormatter),
      mDefaultIcon(HbIcon("qtg_large_avatar")),
      mDefaultMyCardIcon(HbIcon("qtg_large_avatar_mycard"))
{
    CNT_ENTRY

    setFilter(filter);

    CNT_EXIT
}

/*!
    Cleans up all data.
 */
CntListModelData::~CntListModelData()
{
    CNT_ENTRY

    mCache->closeSession(this);

    delete mSettings;
    delete mNameListRowSettingkey;
    delete mFormat;
    delete mCurrentContact;

    CNT_EXIT
}

/*!
    Sets the filter for selecting contacts in the list.
 */
void CntListModelData::setFilter(const QContactFilter& contactFilter)
{
    CNT_ENTRY_ARGS(contactFilter.type())

    mFilter = contactFilter;
    mCurrentRow = -1;
    if (contactFilter.type() == QContactFilter::RelationshipFilter) {
        QContactRelationshipFilter* relationshipFilter = static_cast<QContactRelationshipFilter*>(&mFilter);
        if (relationshipFilter->relationshipType() == QContactRelationship::HasMember &&
            relationshipFilter->relatedContactRole() == QContactRelationship::First) {
            mGroupId = relationshipFilter->relatedContactId().localId();
        }
    } else {
        mGroupId = 0;

        // set proper text formatter for the display name. 
        if (contactFilter.type() == QContactFilter::ContactDetailFilter) {
            delete mFormat;
            mFormat = NULL;

            QContactDetailFilter *detailFilter = static_cast<QContactDetailFilter*>(&mFilter);
            QStringList filter = detailFilter->value().toStringList();

            if (detailFilter->detailDefinitionName() == QContactDisplayLabel::DefinitionName
                && detailFilter->matchFlags() & QContactFilter::MatchStartsWith
                && !filter.isEmpty()) {
                mFormat = new CntHTMLDisplayTextFormatter();
            } else {
                mFormat = new CntDummyDisplayTextFormatter();
            }
        }
    }

    CNT_EXIT_ARGS(mGroupId)
}
