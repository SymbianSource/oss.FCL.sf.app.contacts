/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An internal interface for views that support view filtering
*
*/


#ifndef M_MPARENTVIEWFORFILTERING_H
#define M_MPARENTVIEWFORFILTERING_H

#include <MVPbkContactViewObserver.h>
#include <MVPbkContactView.h>

namespace VPbkSimStore {

class MParentViewForFiltering;

/**
 *  An observer interface for internal view events for filtering.
 *  Filtered views implements this.
 *
 */
NONSHARABLE_CLASS( MFilteredViewSupportObserver ) 
    :   public MVPbkContactViewObserver
    {
public:
    
    /**
     * Called when aView is ready for filtering. This is called before the
     * ContactViewReady.
     * Implementation of this function is NOT allowed to call 
     * MVPbkContactViewObserver functions to any external observer
     * but only update its own internal state. Except ContactViewError.
     *
     * @param aView the view that has been updated and is ready for filtering.
     */
    virtual void ContactViewReadyForFiltering( 
            MParentViewForFiltering& aView ) = 0;
    
    /**
     * Called when aView is unavailable for filtering. This is called before the
     * ContactViewUnavailable.
     * Implementation of this function is NOT allowed to call 
     * MVPbkContactViewObserver functions to any external observer
     * but only update its own internal state. Except ContactViewError.
     *
     * @param aView the view that has been updated and is ready for filtering.
     */
    virtual void ContactViewUnavailableForFiltering( 
            MParentViewForFiltering& aView ) = 0;

protected:
    ~MFilteredViewSupportObserver() {}
    };

/**
 *  An interface for views that supports filtering based on search
 *  string.
 *
 */
NONSHARABLE_CLASS( MParentViewForFiltering ) : public MVPbkContactView
    {
public:

    /**
     * A special version of AddObserver that overwrites the 
     * MVPbkContactViewBase::AddObserverL. 
     * Both must not be used for same user. 
     *
     * @param aObserver a new observer that needs view ready event before any
     *                  external MVPbkContactViewObserver observer.
     */
    virtual void AddFilteringObserverL( 
            MFilteredViewSupportObserver& aObserver ) = 0;

    /** 
     * Removes aObserver. MVPbkContactViewObserver::RemoveObserver is not
     *                    needed to call.
     *
     * @param aObserver the observer that doesn't need view events anymore.
     */
    virtual void RemoveFilteringObserver( 
            MFilteredViewSupportObserver& aObserver ) = 0;
    
    /**
     * @return ETrue if the native asynchrnous SIM store filtering
     *               is active.
     */
    virtual TBool IsNativeMatchingRequestActive() = 0;

protected:
    ~MParentViewForFiltering() {}
    };

} // namespace VPbkSimStore

#endif // M_MPARENTVIEWFORFILTERING_H
