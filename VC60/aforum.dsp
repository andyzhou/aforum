# Microsoft Developer Studio Project File - Name="aforum" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=aforum - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "aforum.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "aforum.mak" CFG="aforum - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "aforum - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "aforum - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "aforum - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "aforum - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "aforum - Win32 Release"
# Name "aforum - Win32 Debug"
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Group "template4c"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib20\template4c\advanced.c
# End Source File
# Begin Source File

SOURCE=..\lib20\template4c\simple.c
# End Source File
# Begin Source File

SOURCE=..\lib20\template4c\simple.cpp
# End Source File
# Begin Source File

SOURCE=..\lib20\template4c\snprintf.c
# End Source File
# Begin Source File

SOURCE=..\lib20\template4c\tpllib.c
# End Source File
# Begin Source File

SOURCE=..\lib20\template4c\tpllib.h
# End Source File
# End Group
# Begin Group "apreq"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib20\apreq\apache_cookie.c
# End Source File
# Begin Source File

SOURCE=..\lib20\apreq\apache_cookie.h
# End Source File
# Begin Source File

SOURCE=..\lib20\apreq\apache_multipart_buffer.c
# End Source File
# Begin Source File

SOURCE=..\lib20\apreq\apache_multipart_buffer.h
# End Source File
# Begin Source File

SOURCE=..\lib20\apreq\apache_request.c
# End Source File
# Begin Source File

SOURCE=..\lib20\apreq\apache_request.h
# End Source File
# Begin Source File

SOURCE=..\lib20\apreq\mod_testapreq.c
# End Source File
# End Group
# Begin Group "custom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib20\custom\api_cgi.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_cgi.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_cgi_old.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_cgi_old.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_flate.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_flate.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_keyword_split.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_keyword_split.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_ldap.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_ldap.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_memcache_hash.cpp
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_memcache_hash.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_mysql.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_mysql.h
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_util.c
# End Source File
# Begin Source File

SOURCE=..\lib20\custom\api_util.h
# End Source File
# End Group
# End Group
# Begin Group "public"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\public\fun_cfg.c
# End Source File
# Begin Source File

SOURCE=..\public\fun_cfg.h
# End Source File
# Begin Source File

SOURCE=..\public\fun_lan.h
# End Source File
# Begin Source File

SOURCE=..\public\fun_public.c
# End Source File
# Begin Source File

SOURCE=..\public\fun_public.h
# End Source File
# End Group
# Begin Group "union"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\union\fun_union_exit.c
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_exit.h
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_interface.c
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_interface.h
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_login.c
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_login.h
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_profile.c
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_profile.h
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_pwd.c
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_pwd.h
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_reg.c
# End Source File
# Begin Source File

SOURCE=..\union\fun_union_reg.h
# End Source File
# End Group
# Begin Group "sys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sys\fun_sys_cats.c
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_cats.h
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_editboard.c
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_editboard.h
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_interface.c
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_interface.h
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_moderators.c
# End Source File
# Begin Source File

SOURCE=..\sys\fun_sys_moderators.h
# End Source File
# End Group
# Begin Group "forum"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\forum\fun_forum_board.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_board.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_index.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_index.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_interface.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_interface.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_manage.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_manage.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_post.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_post.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_public.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_public.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_read.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_read.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_search.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_search.h
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_topic.c
# End Source File
# Begin Source File

SOURCE=..\forum\fun_forum_topic.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\mod_aforum.c
# End Source File
# End Target
# End Project
