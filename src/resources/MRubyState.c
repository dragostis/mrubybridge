#include <stdlib.h>
#include <string.h>

#include <jni.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/string.h>

#include "MRubyState.h"
#include "JavaClass.h"

JNIEnv* jenv;
jobject this;

mrb_value pointerToFloat(mrb_state* mrb, long pointer) {
    return mrb_float_value(mrb, (mrb_float) pointer);
}

long floatToPointer(mrb_state* mrb, mrb_value flt) {
    return mrb_float(flt);
}

jarray getArray(mrb_state* mrb, char* signature, mrb_value arg) {
    jarray result = NULL;
    mrb_int i;
    mrb_int length = mrb_ary_len(mrb, arg);

    if (signature[0] == 'Z') {
        result = (*jenv)->NewBooleanArray(jenv, length);

        jboolean array[length];

        for (i = 0; i < length; i++) {
            if (mrb_type(mrb_ary_ref(mrb, arg, i)) == MRB_TT_TRUE) {
                array[i] = (jboolean) 1;
            } else {
                array[i] = (jboolean) 0;
            }
        }

        (*jenv)->SetBooleanArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'B') {
        result = (*jenv)->NewByteArray(jenv, length);

        jbyte array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jbyte) mrb_fixnum(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetByteArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'C') {
        result = (*jenv)->NewCharArray(jenv, length);

        jchar array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jchar) mrb_fixnum(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetCharArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'S') {
        result = (*jenv)->NewShortArray(jenv, length);

        jshort array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jshort) mrb_fixnum(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetShortArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'I') {
        result = (*jenv)->NewIntArray(jenv, length);

        jint array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jint) mrb_fixnum(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetIntArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'J') {
        result = (*jenv)->NewLongArray(jenv, length);

        jlong array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jlong) mrb_fixnum(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetLongArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'F') {
        result = (*jenv)->NewFloatArray(jenv, length);

        jfloat array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jfloat) mrb_float(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetFloatArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'D') {
        result = (*jenv)->NewDoubleArray(jenv, length);

        jdouble array[length];

        for (i = 0; i < length; i++) {
            array[i] = (jdouble) mrb_float(mrb_ary_ref(mrb, arg, i));
        }

        (*jenv)->SetDoubleArrayRegion(jenv, result, 0, length, array);
    } else if (signature[0] == 'L') {
        char* className;
        int stringLen;

        className = malloc((stringLen = strstr(signature, ";") - signature));
        memcpy(className, signature + 1, stringLen - 1);
        className[stringLen - 1] = '\0';

        jclass arrayClass = (*jenv)->FindClass(jenv, className);

        result = (*jenv)->NewObjectArray(jenv, length, arrayClass, NULL);

        int isString = strstr(signature, "Ljava/lang/String") != NULL;

        for (i = 0; i < length; i++) {
            jobject object;

            if (isString) {
                object = (*jenv)->NewStringUTF(jenv, mrb_string_value_ptr(mrb, mrb_ary_ref(mrb, arg, i)));
            } else {
                mrb_value pointerString = mrb_ary_ref(mrb, arg, i);

                object = (jobject) floatToPointer(mrb, pointerString);
            }

            (*jenv)->SetObjectArrayElement(jenv, result, i, object);
        }
    }

    return result;
}

jvalue getJavaArg(mrb_state* mrb, char** signature, mrb_value arg) {
    jvalue result;
    int nextIndex = 1;

    switch ((*signature)[0]) {
        case 'Z':
            if (mrb_type(arg) == MRB_TT_TRUE) {
                result.z = (jboolean) 1;
            } else {
                result.z = (jboolean) 0;
            }

            break;
        case 'B':
            result.b = (jbyte) mrb_fixnum(arg);

            break;
        case 'C':
            result.c = (jchar) mrb_fixnum(arg);

            break;
        case 'S':
            result.s = (jshort) mrb_fixnum(arg);

            break;
        case 'I':
            result.i = (jint) mrb_fixnum(arg);

            break;
        case 'J':
            result.j = (jlong) mrb_fixnum(arg);

            break;
        case 'F':
            result.f = (jfloat) mrb_float(arg);

            break;
        case 'D':
            result.d = (jdouble) mrb_float(arg);

            break;
        case '[':
            result.l = (jobject) getArray(mrb, *signature + 1, arg);

            char* semiColon = strstr(*signature, ";");

            if (semiColon) {
                nextIndex += semiColon - (*signature);
            }

            break;
        case 'L':
            if (strstr(*signature, "Ljava/lang/String") != NULL) {
                result.l = (*jenv)->NewStringUTF(jenv, mrb_string_value_ptr(mrb, arg));
            } else {
                result.l = (jobject) floatToPointer(mrb, arg);
            }

            nextIndex += strstr(*signature, ";") - (*signature);

            break;
    }

    *signature += nextIndex;

    return result;
}

char* getRubyClass(char* javaClass) {
    int dotIndex = -1;
    char* string = javaClass;

    while ((string = strstr(javaClass, "/")) != NULL) {
        javaClass = string + 1;
    }

    char* result = malloc(strlen(javaClass) + 1);
    strcpy(result, javaClass);

    int len = strlen(result);

    if (result[len - 1] == ';') result[len - 1] = '\0';

    return result;
}

mrb_value allocate(mrb_state* mrb, char* rubyClass) {
    struct RClass* c = mrb_class_get(mrb, rubyClass);
    enum mrb_vtype type = MRB_TT_OBJECT;

    struct RObject* o = (struct RObject*) mrb_obj_alloc(mrb, type, c);
    o->c = c;

    return mrb_obj_value(o);
}

mrb_value getRubyArray(mrb_state* mrb, char* signature, jarray array) {
    jint i;
    jsize len = (*jenv)->GetArrayLength(jenv, array);
    mrb_value result = mrb_ary_new_capa(mrb, (mrb_int) len);

    if (signature[0] == 'Z') {
        jboolean* booleans = (*jenv)->GetBooleanArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            if (booleans[i]) {
                mrb_ary_set(mrb, result, (mrb_int) i, mrb_true_value());
            } else {
                mrb_ary_set(mrb, result, (mrb_int) i, mrb_false_value());
            }
        }
    } else if (signature[0] == 'B') {
        jbyte* bytes = (*jenv)->GetByteArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_fixnum_value((mrb_int) bytes[i]));
        }
    } else if (signature[0] == 'C') {
        jchar* chars = (*jenv)->GetCharArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_fixnum_value((mrb_int) chars[i]));
        }
    } else if (signature[0] == 'S') {
        jshort* shorts = (*jenv)->GetShortArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_fixnum_value((mrb_int) shorts[i]));
        }
    } else if (signature[0] == 'I') {
        jint* ints = (*jenv)->GetIntArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_fixnum_value((mrb_int) ints[i]));
        }
    } else if (signature[0] == 'J') {
        jlong* longs = (*jenv)->GetLongArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_fixnum_value((mrb_int) longs[i]));
        }
    } else if (signature[0] == 'F') {
        jfloat* floats = (*jenv)->GetFloatArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_float_value(mrb, (mrb_float) floats[i]));
        }
    } else if (signature[0] == 'D') {
        jdouble* doubles = (*jenv)->GetDoubleArrayElements(jenv, array, 0);

        for (i = 0; i < len; i++) {
            mrb_ary_set(mrb, result, (mrb_int) i, mrb_float_value(mrb, (mrb_float) doubles[i]));
        }
    } else if (signature[0] == 'L') {
        for (i = 0; i < len; i++) {
            mrb_value object;

            if (strstr(signature, "Ljava/lang/String") != NULL) {
                jstring javaString = (jstring) (*jenv)->GetObjectArrayElement(jenv, array, i);

                const char* string = (*jenv)->GetStringUTFChars(jenv, javaString, NULL);

                object = mrb_str_new_cstr(mrb, string);

                (*jenv)->ReleaseStringUTFChars(jenv, javaString, string);
            } else {
                object = allocate(mrb, getRubyClass(signature + 1));

                jobject javaObject = (*jenv)->GetObjectArrayElement(jenv, array, i);

                mrb_value pointer = pointerToFloat(mrb, (long) javaObject);

                mrb_funcall_argv(mrb, object, mrb_intern_lit(mrb, "pointer="), 1, &pointer);
            }

            mrb_ary_set(mrb, result, (mrb_int) i, object);
        }
    }

    return result;
}

mrb_value getRubyReturn(mrb_state* mrb, mrb_value self, char* signature, char* className, jmethodID methodID,
        jobject thisObject, jvalue* args) {
    mrb_value result;

    if (signature[0] == '\0') {
        jclass javaClass = (*jenv)->FindClass(jenv, className);
        jobject javaObject = (*jenv)->NewObjectA(jenv, javaClass, methodID, args);

        result = pointerToFloat(mrb, (long) javaObject);
    } else {
        if (signature[0] == 'V') {
            (*jenv)->CallVoidMethodA(jenv, thisObject, methodID, args);

            result = mrb_nil_value();
        } else if (signature[0] == 'Z') {
            jboolean boolean = (*jenv)->CallBooleanMethodA(jenv, thisObject, methodID, args);

            if (boolean) {
                result = mrb_true_value();
            } else {
                result = mrb_false_value();
            }
        } else if (signature[0] == 'B') {
            jbyte byte = (*jenv)->CallByteMethodA(jenv, thisObject, methodID, args);

            result = mrb_fixnum_value((mrb_int) byte);
        } else if (signature[0] == 'C') {
            jchar chr = (*jenv)->CallCharMethodA(jenv, thisObject, methodID, args);

            result = mrb_fixnum_value((mrb_int) chr);
        } else if (signature[0] == 'S') {
            jshort shrt = (*jenv)->CallShortMethodA(jenv, thisObject, methodID, args);

            result = mrb_fixnum_value((mrb_int) shrt);
        } else if (signature[0] == 'I') {
            jint in = (*jenv)->CallIntMethodA(jenv, thisObject, methodID, args);

            result = mrb_fixnum_value((mrb_int) in);
        } else if (signature[0] == 'J') {
            jlong lng = (*jenv)->CallLongMethodA(jenv, thisObject, methodID, args);

            result = mrb_fixnum_value((mrb_int) lng);
        } else if (signature[0] == 'F') {
            jfloat flt = (*jenv)->CallFloatMethodA(jenv, thisObject, methodID, args);

            result = mrb_float_value(mrb, (mrb_float) flt);
        } else if (signature[0] == 'D') {
            jdouble dbl = (*jenv)->CallDoubleMethodA(jenv, thisObject, methodID, args);

            result = mrb_float_value(mrb, (mrb_float) dbl);
        } else if (signature[0] == '[') {
            jarray array = (jarray) (*jenv)->CallObjectMethodA(jenv, thisObject, methodID, args);

            result = getRubyArray(mrb, signature + 1, array);
        } else if (signature[0] == 'L') {
            if (strstr(signature, "Ljava/lang/String") != NULL) {
                jstring javaString = (jstring) (*jenv)->CallObjectMethodA(jenv, thisObject, methodID, args);

                const char* string = (*jenv)->GetStringUTFChars(jenv, javaString, NULL);

                result = mrb_str_new_cstr(mrb, string);

                (*jenv)->ReleaseStringUTFChars(jenv, javaString, string);
            } else {
                mrb_value object = allocate(mrb, getRubyClass(signature + 1));

                jobject javaObject = (*jenv)->CallObjectMethodA(jenv, thisObject, methodID, args);

                mrb_value pointer = pointerToFloat(mrb, (long) javaObject);

                mrb_funcall_argv(mrb, object, mrb_intern_lit(mrb, "pointer="), 1, &pointer);

                result = object;
            }
        }
    }

    return result;
}

mrb_value getRubyStaticReturn(mrb_state* mrb, mrb_value self, char* signature, char* className, jmethodID methodID,
        jvalue* args) {
    mrb_value result;

    jclass javaClass = (*jenv)->FindClass(jenv, className);

    if (signature[0] == 'V') {
        (*jenv)->CallStaticVoidMethodA(jenv, javaClass, methodID, args);

        result = mrb_nil_value();
    } else if (signature[0] == 'Z') {
        jboolean boolean = (*jenv)->CallStaticBooleanMethodA(jenv, javaClass, methodID, args);

        if (boolean) {
            result = mrb_true_value();
        } else {
            result = mrb_false_value();
        }
    } else if (signature[0] == 'B') {
        jbyte byte = (*jenv)->CallStaticByteMethodA(jenv, javaClass, methodID, args);

        result = mrb_fixnum_value((mrb_int) byte);
    } else if (signature[0] == 'C') {
        jchar chr = (*jenv)->CallStaticCharMethodA(jenv, javaClass, methodID, args);

        result = mrb_fixnum_value((mrb_int) chr);
    } else if (signature[0] == 'S') {
        jshort shrt = (*jenv)->CallStaticShortMethodA(jenv, javaClass, methodID, args);

        result = mrb_fixnum_value((mrb_int) shrt);
    } else if (signature[0] == 'I') {
        jint in = (*jenv)->CallStaticIntMethodA(jenv, javaClass, methodID, args);

        result = mrb_fixnum_value((mrb_int) in);
    } else if (signature[0] == 'J') {
        jlong lng = (*jenv)->CallStaticLongMethodA(jenv, javaClass, methodID, args);

        result = mrb_fixnum_value((mrb_int) lng);
    } else if (signature[0] == 'F') {
        jfloat flt = (*jenv)->CallStaticFloatMethodA(jenv, javaClass, methodID, args);

        result = mrb_float_value(mrb, (mrb_float) flt);
    } else if (signature[0] == 'D') {
        jdouble dbl = (*jenv)->CallStaticDoubleMethodA(jenv, javaClass, methodID, args);

        result = mrb_float_value(mrb, (mrb_float) dbl);
    } else if (signature[0] == '[') {
        jarray array = (jarray) (*jenv)->CallStaticObjectMethodA(jenv, javaClass, methodID, args);

        result = getRubyArray(mrb, signature + 1, array);
    } else if (signature[0] == 'L') {
        if (strstr(signature, "Ljava/lang/String") != NULL) {
            jstring javaString = (jstring) (*jenv)->CallStaticObjectMethodA(jenv, javaClass, methodID, args);

            const char* string = (*jenv)->GetStringUTFChars(jenv, javaString, NULL);

            result = mrb_str_new_cstr(mrb, string);

            (*jenv)->ReleaseStringUTFChars(jenv, javaString, string);
        } else {
            mrb_value object = allocate(mrb, getRubyClass(signature + 1));

            jobject javaObject = (*jenv)->CallStaticObjectMethodA(jenv, javaClass, methodID, args);

            mrb_value pointer = pointerToFloat(mrb, (long) javaObject);

            mrb_funcall_argv(mrb, object, mrb_intern_lit(mrb, "pointer="), 1, &pointer);

            result = object;
        }
    }

    return result;
}

mrb_value require(mrb_state* mrb, mrb_value self) {
    mrb_value fileName;

    mrb_get_args(mrb, "o", &fileName, NULL);

    jclass mRubyStateClass = (*jenv)->FindClass(jenv, "MRubyState");
    jmethodID requireMethod = (*jenv)->GetMethodID(jenv, mRubyStateClass, "require", "(Ljava/lang/String;)Z");

    jvalue javaArgs[1];
    javaArgs[0].l = (*jenv)->NewStringUTF(jenv, mrb_string_value_ptr(mrb, fileName));

    jboolean result = (*jenv)->CallBooleanMethodA(jenv, this, requireMethod, javaArgs);

    if (result) {
        return mrb_true_value();
    } else {
        return mrb_false_value();
    }
}

mrb_value call(mrb_state* mrb, mrb_value self) {
    mrb_value* args;
    mrb_int len;

    mrb_get_args(mrb, "*", &args, &len);

    jmethodID methodID = (jmethodID) floatToPointer(mrb, args[0]);

    jobject thisObject = (jobject) floatToPointer(mrb, args[1]);

    char* signatureString = (char*) mrb_string_value_ptr(mrb, args[2]) + 1;
    char** signature = &signatureString;

    char* className = (char*) mrb_string_value_ptr(mrb, args[3]);

    jvalue javaArgs[len - 4];
    int i = 4;

    while ((*signature)[0] != ')') {
        javaArgs[i - 4] = getJavaArg(mrb, signature, args[i]);

        i++;
    }

    return getRubyReturn(mrb, self, (*signature) + 1, className, methodID, thisObject, javaArgs);
}

mrb_value call_static(mrb_state* mrb, mrb_value self) {
    mrb_value* args;
    mrb_int len;

    mrb_get_args(mrb, "*", &args, &len);

    jmethodID methodID = (jmethodID) floatToPointer(mrb, args[0]);

    char* signatureString = (char*) mrb_string_value_ptr(mrb, args[1]) + 1;
    char** signature = &signatureString;

    char* className = (char*) mrb_string_value_ptr(mrb, args[2]);

    jvalue javaArgs[len - 3];
    int i = 3;

    while ((*signature)[0] != ')') {
        javaArgs[i - 3] = getJavaArg(mrb, signature, args[i]);

        i++;
    }

    return getRubyStaticReturn(mrb, self, (*signature) + 1, className, methodID, javaArgs);
}

void throwRuntimeException(JNIEnv* env, const char* message) {
    jclass exceptionClass = (*env)->FindClass(env, "java/lang/RuntimeException");

    (*env)->ThrowNew(env, exceptionClass, message);
}


JNIEXPORT jlong JNICALL Java_MRubyState_getStatePointer(JNIEnv* env,
        jobject thisObject) {
    jenv = env;
    this = thisObject;
    mrb_state* mrb = mrb_open();

    mrb_load_string(mrb, (const char *) java_class_rb);

    struct RClass* javaMethod = mrb_class_get(mrb, "JavaMethod");

    mrb_define_singleton_method(mrb, mrb->top_self, "require", require, MRB_ARGS_REQ(1));

    mrb_define_class_method(mrb, javaMethod, "java_call", call, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, javaMethod, "java_call_static", call_static, MRB_ARGS_ANY());

    return (jlong) mrb;
}

JNIEXPORT void JNICALL Java_MRubyState_loadClassToState(JNIEnv* env, jobject thisObject, jlong mRubyState,
        jstring javaName, jstring rubyName) {
    mrb_state* mrb = (mrb_state*) mRubyState;

    const char* javaNameString = (*env)->GetStringUTFChars(env, javaName, NULL);
    const char* rubyNameString = (*env)->GetStringUTFChars(env, rubyName, NULL);

    struct RClass* javaClass = mrb_class_get(mrb, "JavaClass");
    struct RClass* rubyClass = mrb_define_class(mrb, rubyNameString, javaClass);

    mrb_value string = mrb_str_new_cstr(mrb, javaNameString);

    mrb_funcall_argv(mrb, mrb_obj_value(rubyClass), mrb_intern_lit(mrb, "java_class="), 1, &string);

    (*env)->ReleaseStringUTFChars(env, javaName, javaNameString);
    (*env)->ReleaseStringUTFChars(env, rubyName, rubyNameString);
}

JNIEXPORT void JNICALL Java_MRubyState_loadClassMethodsToState(JNIEnv* env, jobject thisObject, jlong mRubyState,
        jstring className, jstring rubyClassName, jobjectArray javaNames, jobjectArray rubyNames,
        jobjectArray javaSignatures, jbooleanArray isStatic) {
    mrb_state* mrb = (mrb_state*) mRubyState;
    const char* classNameString = (*env)->GetStringUTFChars(env, className, NULL);
    const char* rubyClassNameString = (*env)->GetStringUTFChars(env, rubyClassName, NULL);

    struct RClass* rubyClass = mrb_class_get(mrb, rubyClassNameString);

    jclass thisClass = (*env)->FindClass(env, classNameString);
    jboolean* isStaticArray = (*env)->GetBooleanArrayElements(env, isStatic, NULL);

    jsize length = (*env)->GetArrayLength(env, javaNames);

    int i;

    for (i = 0; i < length; i++) {
        jstring javaName = (jstring) (*env)->GetObjectArrayElement(env, javaNames, i);
        jstring rubyName = (jstring) (*env)->GetObjectArrayElement(env, rubyNames, i);
        jstring signature = (jstring) (*env)->GetObjectArrayElement(env, javaSignatures, i);

        const char* javaNameString = (*env)->GetStringUTFChars(env, javaName, NULL);
        const char* rubyNameString = (*env)->GetStringUTFChars(env, rubyName, NULL);
        const char* signatureString = (*env)->GetStringUTFChars(env, signature, NULL);

        jmethodID methodPointer;

        if (!isStaticArray[i]) {
            methodPointer = (*env)->GetMethodID(env, thisClass, javaNameString, signatureString);
        } else {
            methodPointer = (*env)->GetStaticMethodID(env, thisClass, javaNameString, signatureString);
        }

        mrb_value* args = malloc(sizeof(mrb_value) * 3);

        args[0] = pointerToFloat(mrb, (long) methodPointer);
        args[1] = mrb_str_new_cstr(mrb, rubyNameString);
        args[2] = mrb_str_new_cstr(mrb, signatureString);

        if (!isStaticArray[i]) {
            mrb_funcall_argv(mrb, mrb_obj_value(rubyClass), mrb_intern_lit(mrb, "set_instance_method"), 3, args);
        } else {
            mrb_funcall_argv(mrb, mrb_obj_value(rubyClass), mrb_intern_lit(mrb, "set_class_method"), 3, args);
        }

        (*env)->ReleaseStringUTFChars(env, javaName, javaNameString);
        (*env)->ReleaseStringUTFChars(env, rubyName, rubyNameString);
        (*env)->ReleaseStringUTFChars(env, signature, signatureString);
    }

    (*env)->ReleaseStringUTFChars(env, className, classNameString);
    (*env)->ReleaseStringUTFChars(env, rubyClassName, rubyClassNameString);
}

JNIEXPORT void JNICALL Java_MRubyState_loadString(JNIEnv* env, jobject thisObject, jlong mRubyState, jstring string,
        jstring fileName) {
    this = thisObject;
    mrb_state* mrb = (mrb_state*) mRubyState;
    mrbc_context* context = mrbc_context_new(mrb);

    context->capture_errors = 1;

    const char* fileNameString = (*env)->GetStringUTFChars(env, fileName, NULL);
    const char* mrubyString = (*env)->GetStringUTFChars(env, string, NULL);

    mrbc_filename(mrb, context, fileNameString);

    mrb_load_string_cxt(mrb, mrubyString, context);

    if (mrb->exc != NULL) {
        mrb_value message = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);

        throwRuntimeException(env, mrb_string_value_ptr(mrb, message));
    }

    (*env)->ReleaseStringUTFChars(env, fileName, fileNameString);
    (*env)->ReleaseStringUTFChars(env, string, mrubyString);
}

JNIEXPORT void JNICALL Java_MRubyState_close(JNIEnv* env, jobject thisObject, jlong mRubyState) {
    mrb_state* mrb = (mrb_state*) mRubyState;

    mrb_close(mrb);
}
