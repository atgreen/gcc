// natThrowable.cc - Superclass for all exceptions.

/* Copyright (C) 2000  Red Hat Inc

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

/**
 * @author Andrew Haley <aph@cygnus.com>
 * @date Jan 6  2000
 */

#include <config.h>

#include <string.h>

#include <gcj/cni.h>
#include <jvm.h>
#include <java/lang/Object.h>
#include <java-threads.h>
#include <java/lang/Throwable.h>
#include <java/io/PrintStream.h>
#include <java/io/PrintWriter.h>
#include <java/io/IOException.h>

#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#include <name-finder.h>

#ifdef __ia64__
extern "C" int _Jv_ia64_backtrace (void **array, int size);
#endif

/* FIXME: size of the stack trace is limited to 128 elements.  It's
   undoubtedly sensible to limit the stack trace, but 128 is rather
   arbitrary.  It may be better to configure this.  */

java::lang::Throwable *
java::lang::Throwable::fillInStackTrace (void)
{
#if defined (HAVE_BACKTRACE) || defined (__ia64__)
  void *p[128];
  
  // We subtract 1 from the number of elements because we don't want
  // to include the call to fillInStackTrace in the trace.
#if defined (__ia64__)
  int n = _Jv_ia64_backtrace (p, 128) - 1;  
#else
  int n = backtrace (p, 128) - 1;  
#endif

  // ???  Might this cause a problem if the byte array isn't aligned?
  stackTrace = JvNewByteArray (n * sizeof p[0]);
  memcpy (elements (stackTrace), p+1, (n * sizeof p[0]));

#endif

  return this;
}

void 
java::lang::Throwable::printRawStackTrace (java::io::PrintWriter *wr)
{
  wr->println (toString ());
#ifdef HAVE_BACKTRACE
  if (!stackTrace)
    return;

  void **p = (void **)elements (stackTrace);
  int depth = stackTrace->length / sizeof p[0];

  _Jv_name_finder finder (_Jv_ThisExecutable ());

  for (int i = 0; i < depth; i++)
    {
      bool found = finder.lookup (p[i]);
      wr->print (JvNewStringLatin1 ("   at "));
      wr->print (JvNewStringLatin1 (finder.hex));
      if (found)
	{
	  wr->print (JvNewStringLatin1 (": "));
	  wr->print (JvNewStringLatin1 (finder.method_name));
	  if (finder.file_name[0])
	    {
	      wr->print (JvNewStringLatin1 (" ("));
	      wr->print (JvNewStringLatin1 (finder.file_name));
	      wr->print (JvNewStringLatin1 (")"));
	    }
	}
      wr->println ();
    }
#endif /* HAVE_BACKTRACE */
  wr->flush ();
}
