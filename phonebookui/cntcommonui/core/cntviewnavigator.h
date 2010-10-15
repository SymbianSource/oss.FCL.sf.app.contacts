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

#ifndef CNTVIEWNAVIGATOR_H_
#define CNTVIEWNAVIGATOR_H_
#include <QObject>
#include <QStack>
#include <hbnamespace.h>
#include "cntglobal.h"
/*!
 * Navigator keeps track on view history, the path that user has
 * gone throug. If back() is called, one step back view id
 * given, unless exception is set to current view id. In case of
 * exception, navigator goes back in history until the exception 
 * value is found. Note the difference between "back with exeception" vs.
 * "launching given view id" (next()). 
 *
 * Next goes one step forward and stores history. 
 * Navigator is implemented as stack of view ids.
 *
 */
class QTPBK_EXPORT CntViewNavigator : public QObject
{
    Q_OBJECT
    friend class T_NavigatorTest;
    
public:
    CntViewNavigator( QObject* aParent );
    ~CntViewNavigator();
    
public:
    /*!
     * Next view puts the given id as current view
     */
    void next( const int& aId, QFlags<Hb::ViewSwitchFlag> &flags );
    
    /*!
     * Back view returns the previous view
     */
    const int& back( QFlags<Hb::ViewSwitchFlag> &flags, bool toRoot );
    
    /*!
     * Add exceptions to next/back mechanism. Function will use the aBack argument
     * for previous view when aCurrent is the active view.
     */
    void addException( const int& aCurrent, const int& aBack );
    void removeException( const int& aCurrent );
    
    /*!
     * Add special view switching effects for some of the views. 
     */
    void addEffect( const int& aCurrent, const int& aBack );
    void removeEffect( const int& aCurrent );
    
    /*!
     * Set a view as a root view.
     */
    void addRoot( const int& aCurrent );
        
    /*!
     * Internalize view data. Returns the previous view id.
     */
    int internalize(QDataStream &stream);
    
    /*!
     * Externalize view data.
     */
    void externalize(QDataStream &stream);
    
    /*!
     * Clear view stack.
     */
    void clearViewStack();
    
private:
    QStack<int> mViewStack;
    QMap< int, int > mExceptions;
    QMap< int, int > mEffects;
    QList<int> mRoots;
    int mTop;
    int mPreviousViewId;
};
#endif /* CNTVIEWNAVIGATOR_H_ */
