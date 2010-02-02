rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@echo off
set /p userin= What is your release [3.2/5.0]? :
set tmp_path=%PATH%
set path=\epoc32\tools\cenrep;%PATH%
FOR /R "." %%i IN (cenrep.bat) DO set curdrive=%%~di
\epoc32\tools\cenrep\generate_cenrep_inifile.pl -r %userin% -d ..\adapters\contacts\cenrep -f 4
\epoc32\tools\cenrep\generate_cenrep_inifile.pl -r %userin% -d ..\PcsAlgorithm\Algorithm1\cenrep -f 4

copy 2000B5C6.txt \epoc32\release\winscw\UDEB\Z\private\10202be9\2000B5C6.txt 
copy 2000B5C5.txt \epoc32\release\winscw\UDEB\Z\private\10202be9\2000B5C5.txt 

set path=%tmp_path%
