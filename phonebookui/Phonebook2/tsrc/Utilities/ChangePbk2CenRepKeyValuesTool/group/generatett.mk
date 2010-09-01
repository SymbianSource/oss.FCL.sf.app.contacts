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
	echo generating test table $(SOURCEDIR)\ChangePbk2CenRepKeyValuesTooltesttable.cpp 
	perl sugentt.pl $(SOURCEDIR) $(SOURCEDIR)\ChangePbk2CenRepKeyValuesTooltesttable.cpp CChangePbk2CenRepKeyValuesTool
CLEAN:
	echo Removing test table
	if exist $(SOURCEDIR)\ChangePbk2CenRepKeyValuesTooltesttable.cpp echo y|del $(SOURCEDIR)\ChangePbk2CenRepKeyValuesTooltesttable.cpp

MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES SAVESPACE FINAL:
