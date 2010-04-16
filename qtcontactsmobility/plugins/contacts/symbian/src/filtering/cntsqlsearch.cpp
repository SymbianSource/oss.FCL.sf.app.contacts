/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QStringList>

#include "cntsqlsearch.h"

const char LimitLength = 15;
const char LowerLimitPadding = '0';
const char UpperLimitPadding = 'F';


CntSqlSearch::CntSqlSearch()
{


}

QString CntSqlSearch::CreatePredictiveSearch(const QString &pattern)
{  
    if (pattern.length() > 15)
        {
        return QString("");
        }
    else if (pattern.length() == 1)
        {
        return "SELECT contact_id FROM " + SelectTableView(pattern) + " ORDER BY first_name, last_name ASC;";
        }
    else
        {
        return CreateSubStringSearch(pattern);
        }
}

QString CntSqlSearch::SelectTableView(const QString &pattern)
{
    QString predictivesearch;
    int index;
    int num;
    if (pattern.contains("0"))
        {
        index = pattern.indexOf("0");
        if(index == pattern.length() - 1 )
            {
            num = 0;
            }
        else
            {
            num = pattern.at(index + 1).digitValue();
            }
        }
    else
        {
        num = pattern.at(0).digitValue();
        }

    switch (num)
        {

        case 0:
            {
            predictivesearch = QString("predictivesearch0");
            }
        break;
        case 1:
            {
            predictivesearch = QString("predictivesearch1");
            }
        break;
        case 2:
            {
            predictivesearch = QString("predictivesearch2");
            }
        break;
        case 3:
            {
            predictivesearch = QString("predictivesearch3");
            }
        break;
        case 4:
            {
            predictivesearch = QString("predictivesearch4");
            }
        break;
        case 5:
            {
            predictivesearch = QString("predictivesearch5");
            }
        break;
        case 6:
            {
            predictivesearch = QString("predictivesearch6");
            }
        break;
        case 7:
            {
            predictivesearch = QString("predictivesearch7");
            }
        break;
        case 8:
            {
            predictivesearch = QString("predictivesearch8");
            }
        break;
        case 9:
            {
            predictivesearch = QString("predictivesearch9");
            }
        break;
        }
return predictivesearch;
}

bool CntSqlSearch::IsSubStringSearch(const QString &pattern)
{
const QChar zero('0');
if (pattern.count( "0", Qt::CaseSensitive ) == pattern.count() )
    {
    return false;
    }
else if (pattern.contains(zero))
    {
    return true;
    }
else
    {
    return false;
    }
}

QStringList CntSqlSearch::GetNumber(const QString &pattern)
{
const QChar zero('0');
return pattern.split(zero, QString::SkipEmptyParts);
}
QString CntSqlSearch::CreateSubStringSearch(const QString &pattern)
{
QString queryString;
QStringList numbers;
numbers = GetNumber(pattern);

if (IsSubStringSearch(pattern) && numbers.count() > 1 )
    {
    //Case 203
    queryString = CreateSpaceStringSearch(numbers, pattern) + Order(numbers);
    }
else if (IsSubStringSearch(pattern) && numbers.count() < 1 )
    {
    //Case 01
    queryString = CreateStringSearch(pattern) + Order(numbers);
    }
else
    {
    //Case 33
    queryString = CreateStringSearch(pattern) + Order(numbers);
    }

return queryString;
}

QString CntSqlSearch::CreateStringSearch(const QString &pattern )
{
QString queryString;


return QString("SELECT contact_id FROM " + SelectTableView(pattern) +
              " WHERE " + CreateLimit(pattern));
}

QString CntSqlSearch::CreateSpaceStringSearch(QStringList numbers, const QString &pattern)
{
/*if(numbers.at(0) == numbers.at(1))
    {

    }
else*/
    {
    if((numbers.at(0).length() > 1 || numbers.at(1).length() > 1) &&
        (pattern.startsWith('0') || pattern.endsWith('0')))
        {
        return QString(CreateJoinTableSearch(numbers) +
                       " OR (" + CreateJoinTableLimit(lowerLimit(pattern), upperLimit(pattern), SelectTableView(numbers.at(0))) + ")" +
                       " OR (" + CreateJoinTableLimit(lowerLimit(pattern), upperLimit(pattern), SelectTableView(numbers.at(1)))) + ")";
        }
    else if(numbers.at(0).length() > 1 || numbers.at(1).length() > 1 )
        {
        return CreateJoinTableSearch(numbers);
        }
    else
        {
        return CreateSpaceSimpleSearch(numbers);
        }
    }
}

QString CntSqlSearch::CreateSpaceSimpleSearch(QStringList numbers)
{
QString firstTable = SelectTableView(numbers.at(0));
QString secondTable = SelectTableView(numbers.at(1));
QString queryString;

queryString ="SELECT " + firstTable + ".contact_id FROM " + firstTable + " WHERE EXISTS (SELECT contact_id FROM " + secondTable +
" WHERE " + firstTable + ".contact_id = " + secondTable + ".contact_id)";
return queryString;
}

QString CntSqlSearch::CreateLimit(QString pattern)
{
QString low = lowerLimit(pattern);
QString upp = upperLimit(pattern);
/*return QString("(nbr>" +low + " AND nbr<" + upp +
               ") OR (nbr2>" +low + " AND nbr2<" + upp +
               ") OR (nbr3>" +low + " AND nbr3<" + upp +
               ") OR (nbr4>" +low + " AND nbr4<" + upp + ")");*/

return "NOT((NOT (nbr >= " + low + " AND nbr <= " + upp +
              ")) AND (NOT (nbr2 >= " + low + " AND nbr2 <= " + upp +
              ")) AND (NOT (nbr3 >= " + low + " AND nbr3 <= " + upp +
              ")) AND (NOT (nbr4 >= " + low + " AND nbr4 <= " + upp + ")))";
}

QString CntSqlSearch::CreateJoinTableSearch(QStringList numbers)
{
QString firstTable = SelectTableView(numbers.at(0));
QString secondTable = SelectTableView(numbers.at(1));
QString queryString;

queryString = QString("SELECT " + firstTable + ".contact_id FROM " + firstTable + " JOIN " + secondTable + " ON " + firstTable +".contact_id = " + secondTable + ".contact_id WHERE");


if (numbers.at(0).length() > 1 && numbers.at(1).length() > 1 )
    {
    queryString += "(" + CreateJoinTableLimit(lowerLimit(numbers.at(0)), upperLimit(numbers.at(0)), SelectTableView(numbers.at(0))) +
                   ") AND (" + CreateJoinTableLimit(lowerLimit(numbers.at(1)), upperLimit(numbers.at(1)), SelectTableView(numbers.at(1))) + ")";
    }
else if (numbers.at(0).length() > 1 )
    {
    queryString += CreateJoinTableLimit(lowerLimit(numbers.at(0)), upperLimit(numbers.at(0)), SelectTableView(numbers.at(0)));
    }
else
    {
    queryString += CreateJoinTableLimit(lowerLimit(numbers.at(1)), upperLimit(numbers.at(1)), SelectTableView(numbers.at(1)));
    }

return queryString;
}

QString CntSqlSearch::CreateJoinTableLimit(QString low, QString upp, QString table )
{
table += ".";
return QString("(" + table + "nbr>" + low +
              " AND " + table + "nbr<" + upp +
              ") OR (" + table + "nbr2>" + low +
              " AND " + table + "nbr2<" + upp +
              ") OR (" + table + "nbr3>" + low +
              " AND " + table + "nbr3<" + upp +
              ") OR (" + table + "nbr4>" + low +
              " AND " + table + "nbr4<" + upp + ")");
}

QString CntSqlSearch::Order(QStringList numbers)
{
if (numbers.count() > 1 )
    {
    if( numbers.at(0).length() > numbers.at(1).length() || numbers.at(0).length() == numbers.at(1).length() )
        {
        return QString(" ORDER BY " + SelectTableView(numbers.at(0)) + ".first_name, " + SelectTableView(numbers.at(0)) + ".last_name ASC;");
        }
    else
        {
        return QString(" ORDER BY " + SelectTableView(numbers.at(1)) + ".first_name, " + SelectTableView(numbers.at(1)) + ".last_name ASC;");
        }
    }
else
    {
    return QString(" ORDER BY first_name, last_name ASC;");
    }
}

QString CntSqlSearch::pad( const QString &pattern, char padChar ) const
    {
    QString des;
    int padCount = LimitLength-pattern.length();
    padCount = padCount < 0 ? 0 : padCount;
    
    QString result; //("0x");
    
    if ( LimitLength-pattern.length() < 0 ) {
        result = result + pattern.left( LimitLength );
    } else {
        result = result + pattern;
        for( int i=0;i<padCount;i++) {
            result.append( padChar );
        }
    }
    bool ok;
    quint64 hex = result.toULongLong(&ok, 16);
    QString str = QString::number(hex, 10);
    return (str);
    }

QString CntSqlSearch::upperLimit( const QString &pattern ) const
    {
    return pad( pattern, UpperLimitPadding );
    }

QString CntSqlSearch::lowerLimit( const QString &pattern ) const
    {
    return pad( pattern, LowerLimitPadding );
    }



