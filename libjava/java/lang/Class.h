// Class.h - Header file for java.lang.Class.  -*- c++ -*-

/* Copyright (C) 1998, 1999, 2000  Free Software Foundation

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

// Written primary using compiler source and Class.java as guides.
#ifndef __JAVA_LANG_CLASS_H__
#define __JAVA_LANG_CLASS_H__

#pragma interface

#include <java/lang/Object.h>
#include <java/lang/String.h>
#include <java/net/URL.h>

// We declare these here to avoid including gcj/cni.h.
extern "C" void _Jv_InitClass (jclass klass);
extern "C" void _Jv_RegisterClasses (jclass *classes);

// This must be predefined with "C" linkage.
extern "C" void *_Jv_LookupInterfaceMethodIdx (jclass klass, jclass iface, 
                                               int meth_idx);

// These are the possible values for the `state' field of the class
// structure.  Note that ordering is important here.  Whenever the
// state changes, one should notify all waiters of this class.
enum
{
  JV_STATE_NOTHING = 0,		// Set by compiler.

  JV_STATE_PRELOADING = 1,	// Can do _Jv_FindClass.
  JV_STATE_LOADING = 3,		// Has super installed.
  JV_STATE_LOADED = 5,		// Is complete.
    
  JV_STATE_COMPILED = 6,	// This was a compiled class.

  JV_STATE_PREPARED = 7,	// Layout & static init done.
  JV_STATE_LINKED = 9,		// Strings interned.

  JV_STATE_IN_PROGRESS = 10,	// <Clinit> running.
  JV_STATE_DONE = 12,		// 

  JV_STATE_ERROR = 14		// must be last.
};

struct _Jv_Field;
struct _Jv_VTable;
union _Jv_word;

struct _Jv_Constants
{
  jint size;
  jbyte *tags;
  _Jv_word *data;
};

struct _Jv_Method
{
  _Jv_Utf8Const *name;
  _Jv_Utf8Const *signature;
  _Jv_ushort accflags;
  void *ncode;
  _Jv_Method *getNextMethod ()
  { return this + 1; }
};

// Interface Dispatch Tables 
union _Jv_IDispatchTable
{
  struct
  {
    // Index into interface's ioffsets.
    jshort iindex;
    jshort itable_length;
    // Class Interface dispatch table.
    void **itable;
  } cls;

  struct
  {
    // Offsets into implementation class itables.
    jshort *ioffsets;
  } iface;
};

// Used by _Jv_GetInterfaces ()
struct _Jv_ifaces
{
  jclass *list;
  jshort len;
  jshort count;
};

#define JV_PRIMITIVE_VTABLE ((_Jv_VTable *) -1)

#define JV_CLASS(Obj) ((jclass) (*(_Jv_VTable **) Obj)->clas)

class java::lang::Class : public java::lang::Object
{
public:
  static jclass forName (jstring className);
  JArray<jclass> *getClasses (void);

  java::lang::ClassLoader *getClassLoader (void)
    {
      return loader;
    }

  java::lang::reflect::Constructor *getConstructor (JArray<jclass> *);
  JArray<java::lang::reflect::Constructor *> *getConstructors (void);
  java::lang::reflect::Constructor *getDeclaredConstructor (JArray<jclass> *);
  JArray<java::lang::reflect::Constructor *> *getDeclaredConstructors (void);
  java::lang::reflect::Field *getDeclaredField (jstring);
  JArray<java::lang::reflect::Field *> *getDeclaredFields (void);
  java::lang::reflect::Method *getDeclaredMethod (jstring, JArray<jclass> *);
  JArray<java::lang::reflect::Method *> *getDeclaredMethods (void);

  JArray<jclass> *getDeclaredClasses (void);
  jclass getDeclaringClass (void);

  java::lang::reflect::Field *getField (jstring);
private:
  jint _getFields (JArray<java::lang::reflect::Field *> *result, jint offset);
  JArray<java::lang::reflect::Constructor *> *_getConstructors (jboolean);
  java::lang::reflect::Field *getField (jstring, jint);
  jint _getMethods (JArray<java::lang::reflect::Method *> *result,
		    jint offset);
  java::lang::reflect::Field *getPrivateField (jstring);
  java::lang::reflect::Method *getPrivateMethod (jstring, JArray<jclass> *);

public:
  JArray<java::lang::reflect::Field *> *getFields (void);

  JArray<jclass> *getInterfaces (void);

  void getSignature (java::lang::StringBuffer *buffer);
  static jstring getSignature (JArray<jclass> *, jboolean is_constructor);
  java::lang::reflect::Method *getMethod (jstring, JArray<jclass> *);
  JArray<java::lang::reflect::Method *> *getMethods (void);

  inline jint getModifiers (void)
    {
      return accflags;
    }

  jstring getName (void);

  java::net::URL        *getResource (jstring resourceName);
  java::io::InputStream *getResourceAsStream (jstring resourceName);
  JArray<jobject> *getSigners (void);

  inline jclass getSuperclass (void)
    {
      return superclass;
    }

  inline jboolean isArray (void)
    {
      return name->data[0] == '[';
    }

  inline jclass getComponentType (void)
    {
      return isArray () ? (* (jclass *) &methods) : 0;
    }

  jboolean isAssignableFrom (jclass cls);
  jboolean isInstance (jobject obj);
  jboolean isInterface (void);
  
  inline jboolean isPrimitive (void)
    {
      return vtable == JV_PRIMITIVE_VTABLE;
    }

  jobject newInstance (void);
  jstring toString (void);

  // FIXME: this probably shouldn't be public.
  jint size (void)
    {
      return size_in_bytes;
    }
    
  // finalization
  void finalize ();

private:   

  void checkMemberAccess (jint flags);

  void initializeClass (void);

  // Friend functions implemented in natClass.cc.
  friend _Jv_Method *_Jv_GetMethodLocal (jclass klass, _Jv_Utf8Const *name,
					 _Jv_Utf8Const *signature);
  friend jboolean _Jv_IsAssignableFrom(jclass, jclass);
  friend void *_Jv_LookupInterfaceMethodIdx (jclass klass, jclass iface, 
					     int method_idx);

  inline friend void 
  _Jv_InitClass (jclass klass)
  {
    if (__builtin_expect (klass->state != JV_STATE_DONE, false))
      klass->initializeClass ();
  }

  friend _Jv_Method* _Jv_LookupDeclaredMethod (jclass, _Jv_Utf8Const *, 
					       _Jv_Utf8Const*);
  friend jfieldID JvGetFirstInstanceField (jclass);
  friend jint JvNumInstanceFields (jclass);
  friend jfieldID JvGetFirstStaticField (jclass);
  friend jint JvNumStaticFields (jclass);

  friend jobject _Jv_AllocObject (jclass, jint);
  friend jobjectArray _Jv_NewObjectArray (jsize, jclass, jobject);
  friend jobject _Jv_NewPrimArray (jclass, jint);

  friend jobject _Jv_JNI_ToReflectedField (_Jv_JNIEnv *, jclass, jfieldID,
					   jboolean);
  friend jobject _Jv_JNI_ToReflectedMethod (_Jv_JNIEnv *, jclass, jmethodID,
					    jboolean);
  friend jfieldID _Jv_FromReflectedField (java::lang::reflect::Field *);

  friend jmethodID _Jv_FromReflectedMethod (java::lang::reflect::Method *);
  friend jmethodID _Jv_FromReflectedConstructor (java::lang::reflect::Constructor *);
  friend jint JvNumMethods (jclass);
  friend jmethodID JvGetFirstMethod (jclass);

  friend class _Jv_PrimClass;

  // Friends classes and functions to implement the ClassLoader
  friend class java::lang::ClassLoader;

  friend class java::io::ObjectOutputStream;
  friend class java::io::ObjectInputStream;
  friend class java::io::ObjectStreamClass;

  friend void _Jv_WaitForState (jclass, int);
  friend void _Jv_RegisterClasses (jclass *classes);
  friend void _Jv_RegisterInitiatingLoader (jclass,java::lang::ClassLoader*);
  friend void _Jv_UnregisterClass (jclass);
  friend jclass _Jv_FindClass (_Jv_Utf8Const *name,
			       java::lang::ClassLoader *loader);
  friend jclass _Jv_FindClassInCache (_Jv_Utf8Const *name,
				      java::lang::ClassLoader *loader);
  friend jclass _Jv_FindArrayClass (jclass element,
				    java::lang::ClassLoader *loader);
  friend jclass _Jv_NewClass (_Jv_Utf8Const *name, jclass superclass,
			      java::lang::ClassLoader *loader);

  friend void _Jv_PrepareCompiledClass (jclass);
  friend void _Jv_PrepareConstantTimeTables (jclass);
  friend jshort _Jv_GetInterfaces (jclass, _Jv_ifaces *);
  friend void _Jv_GenerateITable (jclass, _Jv_ifaces *, jshort *);
  friend jstring _Jv_GetMethodString(jclass, _Jv_Utf8Const *);
  friend jshort _Jv_AppendPartialITable (jclass, jclass, void **, jshort);
  friend jshort _Jv_FindIIndex (jclass *, jshort *, jshort);

#ifdef INTERPRETER
  friend jboolean _Jv_IsInterpretedClass (jclass);
  friend void _Jv_InitField (jobject, jclass, _Jv_Field*);
  friend int _Jv_DetermineVTableIndex (jclass, _Jv_Utf8Const *, 
				       _Jv_Utf8Const*);
  friend void _Jv_InitField (jobject, jclass, int);
  friend _Jv_word _Jv_ResolvePoolEntry (jclass, int);
  friend _Jv_Method *_Jv_SearchMethodInClass (jclass cls, jclass klass, 
                        		      _Jv_Utf8Const *method_name, 
					      _Jv_Utf8Const *method_signature);

  friend void _Jv_PrepareClass (jclass);

  friend class _Jv_ClassReader;	
  friend class _Jv_InterpClass;
  friend class _Jv_InterpMethod;
  friend class _Jv_InterpMethodInvocation;
#endif

#ifdef JV_MARKOBJ_DECL
  friend JV_MARKOBJ_DECL;
#endif

  // Chain for class pool.
  jclass next;
  // Name of class.
  _Jv_Utf8Const *name;
  // Access flags for class.
  _Jv_ushort accflags;
  // The superclass, or null for Object.
  jclass superclass;
  // Class constants.
  _Jv_Constants constants;
  // Methods.  If this is an array class, then this field holds a
  // pointer to the element type.  If this is a primitive class, this
  // is used to cache a pointer to the appropriate array type.
  _Jv_Method *methods;
  // Number of methods.  If this class is primitive, this holds the
  // character used to represent this type in a signature.
  jshort method_count;
  // Number of methods in the vtable.
  jshort vtable_method_count;
  // The fields.
  _Jv_Field *fields;
  // Size of instance fields, in bytes.
  jint size_in_bytes;
  // Total number of fields (instance and static).
  jshort field_count;
  // Number of static fields.
  jshort static_field_count;
  // The vtbl for all objects of this class.
  _Jv_VTable *vtable;
  // Interfaces implemented by this class.
  jclass *interfaces;
  // The class loader for this class.
  java::lang::ClassLoader *loader;
  // Number of interfaces.
  jshort interface_count;
  // State of this class.
  jbyte state;
  // The thread which has locked this class.  Used during class
  // initialization.
  java::lang::Thread *thread;
  // How many levels of "extends" this class is removed from Object.
  jshort depth;
  // Vector of this class's superclasses, ordered by decreasing depth.
  jclass *ancestors;
  // Interface Dispatch Table.
  _Jv_IDispatchTable *idt;
};

#endif /* __JAVA_LANG_CLASS_H__ */
