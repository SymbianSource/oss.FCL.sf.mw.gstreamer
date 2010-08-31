/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/

@echo off
echo ----------------------------
echo Configuring for RnD SIS
echo ----------------------------
echo Start of configure time:
time /t

:CREATE_DIR
echo.
echo - create Sis folder (created only if needed)
mkdir ..\..\SIS

:COPY_DLL
echo.
echo - Copy dlls [urel]:
copy %EPOCROOT%epoc32\release\armv5\urel\libgstadder.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstaudio.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstbase.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstcdda.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstcontroller.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstdataprotocol.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstdevsoundext.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstfft.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstgdp.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstinterfaces.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstnet.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstnetbuffer.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstpbutils.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstreamer.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstriff.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstsubparse.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgsttag.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgsttcp.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstaudiorate.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstaudioresample.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstaudiotestsrc.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstcoreelements.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstcoreindexers.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstdecodebin.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstdecodebin2.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstdevsoundsink.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstdevsoundsrc.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstplaybin.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgsttypefindfunctions.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstwavparse.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstwavenc.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstaudioconvert.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstqueue2.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstapp.dll .
copy %EPOCROOT%epoc32\release\armv5\urel\libgstautodetect.dll .

if errorlevel == 1 goto END_ERROR

:CALL_ELFTRAIN
echo - Updating Version No [10.1] (calling elftran)
call elftran.exe -version 10.1 libgstadder.dll
call elftran.exe -version 10.1 libgstaudio.dll 
call elftran.exe -version 10.1 libgstbase.dll 
call elftran.exe -version 10.1 libgstcdda.dll 
call elftran.exe -version 10.1 libgstcontroller.dll 
call elftran.exe -version 10.1 libgstdataprotocol.dll 
call elftran.exe -version 10.1 libgstdevsoundext.dll 
call elftran.exe -version 10.1 libgstfft.dll 
call elftran.exe -version 10.1 libgstgdp.dll 
call elftran.exe -version 10.1 libgstinterfaces.dll 
call elftran.exe -version 10.1 libgstnet.dll 
call elftran.exe -version 10.1 libgstnetbuffer.dll 
call elftran.exe -version 10.1 libgstpbutils.dll 
call elftran.exe -version 10.1 libgstreamer.dll 
call elftran.exe -version 10.1 libgstriff.dll 
call elftran.exe -version 10.1 libgstsubparse.dll 
call elftran.exe -version 10.1 libgsttag.dll 
call elftran.exe -version 10.1 libgsttcp.dll 
call elftran.exe -version 10.1 libgstaudiorate.dll 
call elftran.exe -version 10.1 libgstaudioresample.dll 
call elftran.exe -version 10.1 libgstaudiotestsrc.dll 
call elftran.exe -version 10.1 libgstcoreelements.dll 
call elftran.exe -version 10.1 libgstcoreindexers.dll 
call elftran.exe -version 10.1 libgstdecodebin.dll 
call elftran.exe -version 10.1 libgstdecodebin2.dll 
call elftran.exe -version 10.1 libgstdevsoundsink.dll 
call elftran.exe -version 10.1 libgstdevsoundsrc.dll 
call elftran.exe -version 10.1 libgstplaybin.dll 
call elftran.exe -version 10.1 libgsttypefindfunctions.dll 
call elftran.exe -version 10.1 libgstwavparse.dll 
call elftran.exe -version 10.1 libgstwavenc.dll 
call elftran.exe -version 10.1 libgstaudioconvert.dll 
call elftran.exe -version 10.1 libgstqueue2.dll 
call elftran.exe -version 10.1 libgstapp.dll 
call elftran.exe -version 10.1 libgstautodetect.dll 

:MAKESIS
makesis ..\..\SIS\gstreamer_urel.pkg ..\..\SIS\gstreamer_urel.sis
signsis ..\..\SIS\gstreamer_urel.sis ..\..\SIS\gstreamer_urel.sisx RD.crt RD.key
if errorlevel == 1 goto END_ERROR

:DELETE_DLL
echo.
echo - remove the copied dlls from current folder
del libgstadder.dll
del libgstaudio.dll 
del libgstbase.dll 
del libgstcdda.dll 
del libgstcontroller.dll 
del libgstdataprotocol.dll 
del libgstdevsoundext.dll 
del libgstfft.dll 
del libgstgdp.dll 
del libgstinterfaces.dll 
del libgstnet.dll 
del libgstnetbuffer.dll 
del libgstpbutils.dll 
del libgstreamer.dll 
del libgstriff.dll 
del libgstsubparse.dll 
del libgsttag.dll 
del libgsttcp.dll 
del libgstaudiorate.dll 
del libgstaudioresample.dll 
del libgstaudiotestsrc.dll 
del libgstcoreelements.dll 
del libgstcoreindexers.dll 
del libgstdecodebin.dll 
del libgstdecodebin2.dll 
del libgstdevsoundsink.dll 
del libgstdevsoundsrc.dll 
del libgstplaybin.dll 
del libgsttypefindfunctions.dll 
del libgstwavparse.dll 
del libgstwavenc.dll 
del libgstaudioconvert.dll 
del libgstqueue2.dll 
del libgstapp.dll 
del libgstautodetect.dll 

goto END

:END_ERROR
echo.
echo Sis creation failed.
goto FINAL_END

:END
echo.
echo Sis file created succesfully
echo ============================

:FINAL_END
echo End of configure time is:
time /t
