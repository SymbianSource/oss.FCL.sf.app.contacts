/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      An interface for the CLogsNaviDecoratorWrapper. This is basically needed 
*      to hide implementation from Logs Plugin wich inherits CLogsBaseControlContainer.
*      Instead of using the singleton directly in CLogsBaseControlContainer we pass an 
*      interface object and then we dont have to add CLogsNaviDecoratorWrapper.cpp to the
*      Logs Plugin project file.
*      
*
*/


#ifndef MLogsNaviDecoratorWrapper_H
#define MLogsNaviDecoratorWrapper_H

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class MAknTabObserver;

// CLASS DECLARATION

/**
 * An abstract interface for CLogsNaviDecoratorWrapper
 */
class MLogsNaviDecoratorWrapper 
    {
    
    public:    
    /**
     * Returns the wrapped iNaviDecorator instance. If no instance
	 * exists yet a new one is created. If there is an instance but it is
	 * of wrong resource type, a new instance is created and the old deleted. 
	 * 
	 * Called from CLogsBaseControlContainer::NavigationTabGroupL
	 *
	 * @param aNaviPane, the calling views status pane's navigation pane control
	 * @param aTabObserver, an interface to the LogsAppUi::TabChangedL
	 * @param aResource, the resource id of the 
	 *
	 * @return CAknNavigationDecorator object for the view's tab group
     */
    virtual CAknNavigationDecorator* GetNaviDecoratorL(CAknNavigationControlContainer* aNaviPane,
                                                      MAknTabObserver* aTabObserver,
                                                      TInt aResource) =0;
    };
                                              
#endif //MLogsNaviDecoratorWrapper_H
            

// End of File
