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

#include "cntdisplaytextformatter.h"
#include <qcontactdetailfilter.h>
#include <qcontactdisplaylabel.h>
#include <hbcolorscheme.h>
#include <cntdebug.h>
#include <QStringList>

CntHTMLDisplayTextFormatter::CntHTMLDisplayTextFormatter()
{
}

CntHTMLDisplayTextFormatter::~CntHTMLDisplayTextFormatter()
{
}
   
QString CntHTMLDisplayTextFormatter::formattedText( const QString aText, const QContactFilter& aCriteria )
{
    CNT_LOG_ARGS( "filter:" << aText )
    
    QString foundPattern;
    QString pattern;
    QString tempDash;
    QString tempSpace;
    const QChar dash = '-';
    const QChar space = ' ';
    
    if ( aCriteria.type() == QContactFilter::ContactDetailFilter ) 
    {
        const QContactDetailFilter& filter = static_cast<const QContactDetailFilter&>( aCriteria );
        if ( filter.detailDefinitionName() == QContactDisplayLabel::DefinitionName && 
             filter.matchFlags() & QContactFilter::MatchStartsWith )
        {
            QString formattedText;
            // go through the words (e.g. Lastname, Firstname) and apply list of pattern to them.
            foreach ( QString text, aText.split(QRegExp("\\s+"), QString::SkipEmptyParts) )
            {
                foundPattern = "";
                bool match( false );
                // go through every search criteria word
                foreach (pattern, filter.value().toStringList())
                {
                    tempDash = pattern;
                    tempSpace = pattern;
                    tempDash.insert(0, dash);
                    tempSpace.insert(0, space);
                    
                    if ( text.startsWith(pattern, Qt::CaseInsensitive) )
                    {
                        match = true;
                        if (pattern.length() > foundPattern.length())
                            foundPattern = pattern;
                    }
                }
                
                // if no match found, original text is returned
                if ( !match )
                    formattedText.append( text );
                // if match is found then the longest variation of the pattern is high lighted, e.g. "a ab" 
                else
                {
                    insertTag( text, foundPattern.length() );
                    formattedText.append( text );
                }
                
                // put spaces back between words (split() looses them)
                formattedText.append( " " );
            }
            return formattedText.trimmed();
        }
    }
    return aText;
}

void CntHTMLDisplayTextFormatter::insertTag( QString& aText, int aChars )
{
    QColor highlight = HbColorScheme::color("qtc_lineedit_marker_normal");
    QColor color = HbColorScheme::color("qtc_lineedit_selected");
    
    QString start = QString(TAG_START).arg( highlight.name().toUpper() ).arg(color.name().toUpper());
    aText.prepend( start );
    aText.insert( start.length() + aChars, TAG_END );
}
// End of File
