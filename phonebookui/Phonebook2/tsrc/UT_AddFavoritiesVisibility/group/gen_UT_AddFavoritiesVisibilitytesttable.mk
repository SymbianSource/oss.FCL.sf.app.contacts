#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

SOURCEDIR=..\src

BLD:
	echo generating test table $(SOURCEDIR)\UT_AddFavoritiesVisibilitytesttable.cpp 
	perl sugentt.pl $(SOURCEDIR) $(SOURCEDIR)\UT_AddFavoritiesVisibilitytesttable.cpp CUT_AddFavoritiesVisibility
CLEAN:
	echo Removing test table
	if exist $(SOURCEDIR)\UT_AddFavoritiesVisibilitytesttable.cpp echo y|del $(SOURCEDIR)\UT_AddFavoritiesVisibilitytesttable.cpp

MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES SAVESPACE FINAL:
