@echo off
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
@echo on

call del editview.xml
call del MON.sym
call del MON.dat
call qmake
call bldmake bldfiles
call abld clean
call abld reallyclean

IF ["%1"] EQU [""] GOTO :NORMAL

call ctcwrap -i d -C "EXCLUDE+*\moc*.cpp" -C "EXCLUDE+*\t_*.cpp" -C "EXCLUDE+*\stub_*.cpp" abld build winscw udeb
goto :FINAL

:NORMAL
call abld build winscw udeb

:FINAL
rem call shellstart
call \epoc32\release\winscw\udeb\ut_cnteditview.exe -xml -o c:\editview.xml
call copy R:\epoc32\winscw\c\editview.xml .
call type editview.xml
rem call fullstart