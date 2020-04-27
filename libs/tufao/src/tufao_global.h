/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

#ifndef TUFAO_GLOBAL_H
#define TUFAO_GLOBAL_H

#include <QtCore/qglobal.h>

#define TUFAO_LIBRARY

#if defined(TUFAO_LIBRARY)
#  define TUFAO_EXPORT Q_DECL_EXPORT
#else
#  define TUFAO_EXPORT Q_DECL_IMPORT
#endif

#ifndef TUFAO_VERSION_MAJOR
#  define TUFAO_VERSION_MAJOR 0
#endif

#ifndef TUFAO_VERSION_MINOR
#  define TUFAO_VERSION_MINOR 8
#endif

#ifdef Q_MOC_RUN
#define QT_VERSION_CHECK(major,minor,patch) ((major<<16)|(minor<<8)|(patch))
// QT_VERSION_* macros should be provided through -D when you call moc
#define QT_VERSION \
    QT_VERSION_CHECK(QT_VERSION_MAJOR,QT_VERSION_MINOR,QT_VERSION_PATCH)
#endif // Q_MOC_RUN

#endif // TUFAO_GLOBAL_H
