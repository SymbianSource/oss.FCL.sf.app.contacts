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
#include "cntviewnavigator.h"
#include <QDebug>

CntViewNavigator::CntViewNavigator( QObject* aParent ) : QObject( aParent ),
iTop( CntViewParameters::noView )
{
}

CntViewNavigator::~CntViewNavigator()
{   
}

void CntViewNavigator::next( const CntViewParameters::ViewId& aId )
{
    iViewStack.push( aId );
}

const CntViewParameters::ViewId& CntViewNavigator::back()
{
    qDebug() << "CntViewParameters::back() - IN";
    iTop = CntViewParameters::noView;
    // Check if exception is set for current view item. Exception
    // means that instead of one step back, we go back until that 
    // execption value is located. So all items that are jumped over,
    // their history will be eared.
    if ( !iViewStack.isEmpty() ) 
    {
        CntViewParameters::ViewId top = iViewStack.top();
        
        // If any exception defined for the current (top) view
        if ( iExceptions.contains( top ) ) 
        {
            iTop = iExceptions.value( top );
            // cleanup the history until the exception value is found
            while ( !iViewStack.isEmpty() ) 
            {
                if ( iTop == iViewStack.top() ) 
                {
                    break; // don't pop the exception it self.
                }
                iViewStack.pop();
            }
        }
        // No exceptions defined, go one step back
        else 
        {
            iViewStack.pop();
            if ( !iViewStack.isEmpty() )
            {
                iTop = iViewStack.top();
            }
        }
    } 
    qDebug() << "CntViewParameters::back() - OUT";
    return iTop;
}

void CntViewNavigator::addException( const CntViewParameters::ViewId& aCurrent, const CntViewParameters::ViewId& aBack )
{
    iExceptions.insert( aCurrent, aBack );
}

void CntViewNavigator::removeException( const CntViewParameters::ViewId& aCurrent )
{
    if ( iExceptions.contains(aCurrent) )
    {
        iExceptions.remove( aCurrent );
    }
}
    
// End of File
