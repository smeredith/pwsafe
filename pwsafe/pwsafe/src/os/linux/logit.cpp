/*
* Copyright (c) 2003-2011 Rony Shapiro <ronys@users.sourceforge.net>.
* All rights reserved. Use of the code is allowed under the
* Artistic License 2.0 terms, as specified in the LICENSE file
* distributed with this code, or available from
* http://www.opensource.org/licenses/artistic-license-2.0.php
*/

#include "../logit.h"
#include "../../core/PWSLog.h"
#include "../../core/Util.h"

#include <stdio.h>
#include <stdarg.h>

// See discussion on CONVERT_GLIBC_FORMATSPECS in core/StringX.cpp
#if defined(__GNUC__)  && (defined(UNICODE) || defined(_UNICODE))
#define CONVERT_GLIBC_FORMATSPECS
#endif

void pws_os::Logit(LPCTSTR lpszFormat, ...)
{
  va_list args;
  va_start(args, lpszFormat);

  int num_required, num_written;

#ifdef UNICODE
#ifdef CONVERT_GLIBC_FORMATSPECS
  std::wstring fmt(lpszFormat);
  for (std::wstring::size_type pos = 0;
       (pos = fmt.find(L"%s", pos)) != std::wstring::npos; pos += 2)
    fmt.insert(pos + 1, 1, L'l');
  LPCTSTR format_str = fmt.c_str();
#else
  LPCTSTR format_str = lpszFormat;
#endif /* CONVERT_GLIBC_FORMATSPECS */

  num_required = GetStringBufSize(format_str, args);
  va_end(args);  // after using args we should reset list
  va_start(args, lpszFormat);

  wchar_t *szBuffer = new wchar_t[num_required];
  num_written = vswprintf(szBuffer, num_required, format_str, args);
  assert(num_required == num_written + 1);
  szBuffer[num_required - 1] = L'\0';
#else
  num_required = GetStringBufSize(lpszFormat, args);
  va_end(args);  // after using args we should reset list
  va_start(args, lpszFormat);

  char *szBuffer = new char[num_required];
  num_written = vsnprintf(szBuffer, num_required, lpszFormat, args);
  assert(num_required == num_written+1);
  szBuffer[num_required - 1] = '\0';
#endif /* UNICODE */

  PWSLog::GetLog()->Add(stringT(szBuffer));
  delete[] szBuffer;

  va_end(args);
}
