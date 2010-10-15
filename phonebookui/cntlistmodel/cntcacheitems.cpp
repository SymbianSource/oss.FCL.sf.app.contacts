/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Classes for each of the items (name, info and icon) in cache.
*
*/

#include <cntcacheitems.h>

/*!
    \class CntNameCacheItem
    \brief Represents one name entry in cache.
    
    CntNameCacheItem wraps functionality for efficiently storing, serializing
    and changing the display format of one name in cache.

    \class CntInfoCacheItem
    \brief Holds data for info entry in cache.

    \class CntIconCacheItem
    \brief Holds data for icon entry in cache.
 */

/*!
    Creates a CntNameCacheItem object.
 */
CntNameCacheItem::CntNameCacheItem(QContactLocalId id, const QString& firstName, const QString& lastName, CntNameOrder nameFormat)
{
    mContactId = id;
    setFormattedName(firstName, lastName, nameFormat);
}

/*!
    Destroys the CntNameCacheItem object.
 */
CntNameCacheItem::~CntNameCacheItem()
{
}

/*!
    Changes the format used to present the name.
 */
void CntNameCacheItem::setNameFormat(CntNameOrder newFormat)
{
    QString firstName = mName.mid(mFirstNamePosition&0xffff, mFirstNamePosition>>16);
    QString lastName = mName.mid(mLastNamePosition&0xffff, mLastNamePosition>>16);
    setFormattedName(firstName, lastName, newFormat);
}

/*!
    Copies the contents of the other cache item to this one.
 */
void CntNameCacheItem::operator=(const CntNameCacheItem &other)
{
    mContactId = other.mContactId;
    mFirstNamePosition = other.mFirstNamePosition;
    mLastNamePosition = other.mLastNamePosition;
    mName = other.mName;
}

/*!
    Externalizes a CntNameCacheItem object.
 */
void CntNameCacheItem::externalize(QDataStream &stream)
{
    stream << mContactId;
    stream << mFirstNamePosition;
    stream << mLastNamePosition;
    stream << mName;
}

/*!
    Internalizes a CntNameCacheItem object.
 */
CntNameCacheItem* CntNameCacheItem::internalize(QDataStream &stream, CntNameOrder nameFormat)
{
    quint32 id;
    quint32 firstNamePosition;
    quint32 lastNamePosition;
    QString name;
    
    stream >> id;
    stream >> firstNamePosition;
    stream >> lastNamePosition;
    stream >> name;
    
    QString firstName = name.mid(firstNamePosition&0xffff, firstNamePosition>>16);
    QString lastName = name.mid(lastNamePosition&0xffff, lastNamePosition>>16);

    return new CntNameCacheItem(id, firstName, lastName, nameFormat);
}

/*!
    Sets the formatted name and positions of the first name and last name,
    according to the name format in the parameter.
 */
void CntNameCacheItem::setFormattedName(const QString& firstName, const QString& lastName, CntNameOrder nameFormat)
{
    int firstNameLength = firstName.length();
    int lastNameLength = lastName.length();

    if (lastNameLength == 0) {
        mName = firstName;
        mFirstNamePosition = firstNameLength << 16;
        mLastNamePosition = 0;
    } else if (firstNameLength == 0) {
        mName = lastName;
        mFirstNamePosition = 0;
        mLastNamePosition = lastNameLength << 16;
    } else {
        if (nameFormat == CntOrderLastFirst) {
            mName = lastName + " " + firstName;
            mFirstNamePosition = (firstNameLength << 16) | (lastNameLength + 1);
            mLastNamePosition = (lastNameLength << 16);
        } else if (nameFormat == CntOrderLastCommaFirst) {
            mName = lastName + ", " + firstName;
            mFirstNamePosition = (firstNameLength << 16) | (lastNameLength + 2);
            mLastNamePosition = (lastNameLength << 16);
        } else {
            mName = firstName + " " + lastName;
            mFirstNamePosition = (firstNameLength << 16);
            mLastNamePosition = (lastNameLength << 16) | (firstNameLength + 1);
        }
    }
}

QString CntNameCacheItem::firstName() const
{
    return mName.mid(mFirstNamePosition&0xffff, mFirstNamePosition>>16);
}

QString CntNameCacheItem::lastName() const
{
    return mName.mid(mLastNamePosition&0xffff, mLastNamePosition>>16);
}
