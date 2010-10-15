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
#include <cntviewparams.h>
#include <QDebug>

CntViewNavigator::CntViewNavigator( QObject* aParent ) : 
    QObject(aParent),
    mTop(noView),
    mPreviousViewId(noView)
{
    
}

CntViewNavigator::~CntViewNavigator()
{
    
}

void CntViewNavigator::next( const int& aId, QFlags<Hb::ViewSwitchFlag> &flags )
{
    if ( !mViewStack.isEmpty() )
    {
        int top = mViewStack.top();
        
        // If any special effects are defined for the current (top) view and associated with the next view
        if ( mEffects.contains( top ) && mEffects.value( top ) == aId ) 
        {
            flags = Hb::ViewSwitchUseNormalAnim | Hb::ViewSwitchUseAltEvent;
        }
        else
        {
            flags = Hb::ViewSwitchUseNormalAnim;
        }
    }
    mViewStack.push( aId );
}

const int& CntViewNavigator::back( QFlags<Hb::ViewSwitchFlag> &flags, bool toRoot )
{
    mTop = noView;
    // Check if exception is set for current view item. Exception
    // means that instead of one step back, we go back until that 
    // execption value is located. So all items that are jumped over,
    // their history will be eared.
    if ( !mViewStack.isEmpty() ) 
    {
        int top = mViewStack.top();
        
        // If we need to go to the latest root view
        if ( toRoot )
        {
            while ( !mViewStack.isEmpty() ) 
            {
                int current = mViewStack.top();
                
                if ( mRoots.contains( current ) )
                {
                    mTop = current;
                    break;
                }
                mPreviousViewId = mViewStack.pop();
            }
        }
        // If any exception defined for the current (top) view
        else if ( mExceptions.contains( top ) ) 
        {
            mTop = mExceptions.value( top );
            // cleanup the history until the exception value is found
            while ( !mViewStack.isEmpty() ) 
            {
                if ( mTop == mViewStack.top() ) 
                {
                    break; // don't pop the exception it self.
                }
                mPreviousViewId = mViewStack.pop();
            }
        }
        // No exceptions defined, go one step back
        else 
        {
            mPreviousViewId = mViewStack.pop();
            if ( !mViewStack.isEmpty() )
            {
                mTop = mViewStack.top();
            }
        }
        
        // If any special effects are defined for the current (top) view and associated with the previous view
        if ( mEffects.contains( top ) && mEffects.value( top ) == mTop ) 
        {
            flags = Hb::ViewSwitchUseBackAnim | Hb::ViewSwitchUseAltEvent;
        }
        else
        {
            flags = Hb::ViewSwitchUseBackAnim;
        }
    } 

    return mTop;
}

void CntViewNavigator::addException( const int& aCurrent, const int& aBack )
{
    mExceptions.insert( aCurrent, aBack );
}

void CntViewNavigator::removeException( const int& aCurrent )
{
    if ( mExceptions.contains(aCurrent) )
    {
        mExceptions.remove( aCurrent );
    }
}

void CntViewNavigator::addEffect( const int& aCurrent, const int& aBack )
{
    mEffects.insert( aCurrent, aBack );
}

void CntViewNavigator::removeEffect( const int& aCurrent )
{
    if ( mEffects.contains(aCurrent) )
    {
        mEffects.remove( aCurrent );
    }
}

void CntViewNavigator::addRoot( const int& aCurrent )
{
    mRoots.append( aCurrent );
}
 
int CntViewNavigator::internalize(QDataStream &stream)
{
    int viewId = noView;
    //read the view id and a view stack from stream.
    stream >> viewId >> mViewStack;
    
    return viewId;
}
    
void CntViewNavigator::externalize(QDataStream &stream)
{
    int viewId = noView;
        
    //remove top view id of stack.
    if (!mViewStack.isEmpty()) {
        viewId = mViewStack.pop();
    }
    
    if (viewId == noView) {
        viewId = mPreviousViewId;
    }
    //stream the view id and a view stack
    stream << viewId << mViewStack;
}

void CntViewNavigator::clearViewStack()
{
    mViewStack.clear();
}

// End of File
