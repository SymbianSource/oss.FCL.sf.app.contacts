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
#include "cntviewparameters.h"

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
class CntViewNavigator : public QObject
{
    Q_OBJECT
    
public:
    CntViewNavigator( QObject* aParent );
    ~CntViewNavigator();
    
public:
    /*!
     * Next view puts the given id as current view
     */
    void next( const CntViewParameters::ViewId& aId );
    
    /*!
     * Back view returns the previous view
     */
    const CntViewParameters::ViewId& back();
    
    /*!
     * Add exceptions to next/back mechanism. Function will use the aBack argument
     * for previous view when aCurrent is the active view.
     */
    void addException( const CntViewParameters::ViewId& aCurrent, const CntViewParameters::ViewId& aBack );
    void removeException( const CntViewParameters::ViewId& aCurrent );
    
private:
    QStack<CntViewParameters::ViewId> iViewStack;
    QMap< CntViewParameters::ViewId, CntViewParameters::ViewId> iExceptions;
    CntViewParameters::ViewId iTop;
};
#endif /* CNTVIEWNAVIGATOR_H_ */
