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

#include <hbapplication.h>
#include <hbmainwindow.h>
#include "feedview.h"

int main(int argc, char **argv)
{

    HbApplication a( argc, argv );
    
    HbMainWindow mainWindow(NULL, Hb::WindowFlagTransparent);
	FeedView feedView;
	mainWindow.addView(feedView.view());
	mainWindow.setCurrentView(feedView.view());

    mainWindow.show();

    return a.exec();
}

