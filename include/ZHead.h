#ifndef __ZHead__ 
#define __ZHead__
 
#pragma once
#include "stdafx.h"
/* 
功能：对指定的目录压缩
参数：
  1.LPSTR lpszDirectory 待压缩的目录
     "C:\\Program Files\\OfficeIM网络智能办公客户端\\我收到的文件\\SogouInput"
	 
  2.LPSTR lpszZipPathName 压缩文件名
     "d:\\aa.zip"
返回：0 成功 ，否则其他值
注意：指针由外界校验合法性
*/
DWORD  WINAPI ZipingDirectory( LPSTR lpszDirectory, LPSTR lpszZipPathName ); 


/*
功能：将压缩文件解压到指定目录
参数：
  1.LPSTR lpszZipPathName 压缩文件名
      "d:\\aa.zip"

  2.LPSTR lpszDestDirectory 解压到目标目录
      "f:\\aa\\cc"
返回：0 成功 ，否则其他值
注意：指针由外界校验合法性
*/
DWORD WINAPI UnZipToDirectory( LPSTR lpszZipPathName, LPSTR lpszDestDirectory );

#endif // __ZHead__  
