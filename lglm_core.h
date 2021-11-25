/*
** $Id: lglm_core.h $
** Internal definitions for GLM objects
** See Copyright Notice in lua.h
*/

#ifndef lglm_core_h
#define lglm_core_h

#include "luaconf.h"
#include "lua.h"
#include "lobject.h"
#include "ltm.h"

/*
@@ LUAGLM_LIBVERSION Version number of the included GLM library. This value is
**  manually redefined so it can be used by the strictly-C defined portions of
**  this runtime.
**
** Temporary until 820a2c0e625f26000c688d841836bb10483be34d is remedied.
*/
#if !defined(LUAGLM_LIBVERSION)
  #define LUAGLM_LIBVERSION 999
#endif

/* Handle should-be-deprecated-instead-of-removed GLM_FORCE_QUAT_DATA_WXYZ */
#if defined(GLM_FORCE_QUAT_DATA_WXYZ)
  #undef GLM_FORCE_QUAT_DATA_XYZW
#endif

/*
@@ LUAGLM_QUAT_WXYZ Quaternion layout (i.e., wxyz vs xyzw)
*/
#if LUAGLM_LIBVERSION < 999
  #if defined(GLM_FORCE_QUAT_DATA_WXYZ)
    #define LUAGLM_QUAT_WXYZ 1
  #else
    #define LUAGLM_QUAT_WXYZ 0
  #endif
#elif defined(GLM_FORCE_QUAT_DATA_XYZW)
  #define LUAGLM_QUAT_WXYZ 0
#else
  #define LUAGLM_QUAT_WXYZ 1
#endif

/*
** This value should be greater than: (MAXNUMBER2STR * 16) + 64:
**  [d]mat4x4((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))
*/
#define GLM_STRING_BUFFER 1024

/*
** Return the vector variant (tag) associated with 'dimensions'. Note, this
** function does not sanitize the input: assumes [2, 4].
*/
static LUA_INLINE lu_byte glm_variant (grit_length_t dimensions) {
  return cast_byte(makevariant(LUA_TVECTOR, (dimensions - 2) /* & 0x3 */));
}

/*
** Return the vector dimensions associated with the variant (tag). Note, this
** function does not sanitize the input: 'rtt' must be equal to
* makevariant(LUA_TVECTOR, X) for some 'X'
*/
static LUA_INLINE grit_length_t glm_dimensions (lu_byte rtt) {
  const grit_length_t dims = cast(grit_length_t, (rtt & 0x30) >> 4); /* variant bits 4-5 */
  return (rtt == LUA_VQUAT) ? 4 : (2 + dims); /* quat uses 3rd bit. */
}

/*
** {==================================================================
** Internal vector functions
** ===================================================================
*/

/* Fast path equivalent macros. */
#define glmVec_fastgeti(T, I, S) (vecgeti((T), (I), (S)) != LUA_TNONE)
#define glmVec_fastgets(T, K, S) \
  ((tsslen((K)) == 1) && (vecgets((T), getstr((K)), (S)) != LUA_TNONE))

/* Helper function for generalized vector int-access. */
static LUA_INLINE int vecgeti (const TValue *obj, lua_Integer n, StkId res) {
  grit_length_t _n = cast(grit_length_t, n);
  if (l_likely(_n >= 1 && _n <= glm_dimensions(ttypetag(obj)))) {  /* Accessing vectors is 0-based */
#if LUAGLM_QUAT_WXYZ  /* quaternion has WXYZ layout */
    if (ttypetag(obj) == LUA_VQUAT) _n = (_n % 4) + 1;
#endif

    /*
    ** @NOTE: This approach assumes a packed x,y,z,w struct. This was changed
    ** from a switch statement after profiling access times.
    */
    setfltvalue(s2v(res), cast_num((&(vvalue_(obj).x))[_n - 1]));
    return LUA_TNUMBER;
  }
  return LUA_TNONE;
}

/* Helper function for generalized vector string-access. */
static LUA_INLINE int vecgets (const TValue *obj, const char *k, StkId res) {
  grit_length_t _n = 0;
  switch (*k) {
    case 'x': case 'r': case '1': _n = 1; break;
    case 'y': case 'g': case '2': _n = 2; break;
    case 'z': case 'b': case '3': _n = 3; break;
    case 'w': case 'a': case '4': _n = 4; break;
    case 'n': {  /* Dimension fields takes priority over metamethods */
      setivalue(s2v(res), cast(lua_Integer, glm_dimensions(ttypetag(obj))));
      return LUA_TNUMBER;
    }
    default: {
      break;
    }
  }

  if (l_likely(_n >= 1 && _n <= glm_dimensions(ttypetag(obj)))) {
    /* @TODO: Avoid taking the address of a 'TValue' field */
#if LUAGLM_QUAT_WXYZ  /* quaternion has WXYZ layout */
    if (ttypetag(obj) == LUA_VQUAT) _n = (_n % 4) + 1;
#endif
    setfltvalue(s2v(res), cast_num((&(vvalue_(obj).x))[_n - 1]));
    return LUA_TNUMBER;
  }
  return LUA_TNONE;
}

/* rawgeti variant for vector types */
LUAI_FUNC int glmVec_rawgeti (const TValue *obj, lua_Integer n, StkId res);

/* getfield variant for vector types */
LUAI_FUNC int glmVec_rawgets (const TValue *obj, const char *k, StkId res);

/* rawget variant for matrix types */
LUAI_FUNC int glmVec_rawget (const TValue *obj, TValue *key, StkId res);

/* OP_GETTABLE variant for vector types. */
LUAI_FUNC void glmVec_get (lua_State *L, const TValue *obj, TValue *key, StkId res);

/* OP_GETI variant for vector types. */
LUAI_FUNC void glmVec_geti (lua_State *L, const TValue *obj, lua_Integer key, StkId res);

/* Place the magnitude of the matrix (o) at the specified stack index (ra) */
LUAI_FUNC void glmVec_objlen (const TValue *obj, StkId res);

/* luaV_equalobj variant for matrix types */
LUAI_FUNC int glmVec_equalObj (lua_State *L, const TValue *o1, const TValue *o2, int rtt);

/* Attempt to concatenate a number (or vector) to another  */
LUAI_FUNC int glmVec_concat (const TValue *obj, const TValue *value, StkId res);

/* converts a vector to a string. */
LUAI_FUNC int glmVec_tostr (const TValue *obj, char *buff, size_t len);

/* Check whether key 'k1' is equal to the key in node 'n2'. */
LUAI_FUNC int glmVec_equalKey (const TValue *k1, const Node *n2, int rtt);

/* Compute the hash of the vector key: The key comes broken (tag in 'ktt' and value in 'vkl') */
LUAI_FUNC size_t glmVec_hash (const TValue *obj);

/* Return true if each vector component is finite */
LUAI_FUNC int glmVec_isfinite (const TValue *obj);

/*
** Pops a key from the stack and pushes a <key, value> pair from the vector at
** the given stack index, the "next" pair after the given key.
**
** If there are no more elements in the vector, then returns 0 and pushes
** nothing.
*/
LUAI_FUNC int glmVec_next (const TValue *obj, StkId key);

/* trybinTM handler for GLM objects */
LUAI_FUNC int glm_trybinTM (lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event);

/* }================================================================== */

/*
** {==================================================================
** Internal matrix functions
** ===================================================================
*/

/* Fast path equivalent macros. */
#define glmMat_fastgeti(T, I, S) (glmMat_vmgeti((T), (I), (S)) != LUA_TNONE)

/* Create a new collectible matrix object, linking it to the allgc list */
LUAI_FUNC GCMatrix *glmMat_new (lua_State *L);

/* rawgeti variant for matrix types */
LUAI_FUNC int glmMat_rawgeti (const TValue *obj, lua_Integer n, StkId res);

/*
** glmMat_rawgeti: That does not set 'res' to nil on invalid access.
**
** @NOTE: matgeti is an inlined function that could be directly used by lvm.c.
** However, certain GLM configurations will implicitly align glm::vec3. This
** logic does not exist within the C boundary of the runtime at the moment.
**
** This should incur a ~5% hit on the throughput of matrix accessing, e.g.,
** 305657101.637 Mi/s vs. 285964300.217 Mi/
*/
LUAI_FUNC int glmMat_vmgeti (const TValue *obj, lua_Integer n, StkId res);

/* rawget variant for matrix types */
LUAI_FUNC int glmMat_rawget (const TValue *obj, TValue *key, StkId res);

/* lua_rawset variant for matrix types */
LUAI_FUNC void glmMat_rawset (lua_State *L, const TValue *obj, TValue *key, TValue *val);

/* OP_GETTABLE variant for matrix types */
LUAI_FUNC void glmMat_get (lua_State *L, const TValue *obj, TValue *key, StkId res);

/* OP_GETI variant for vector types. */
LUAI_FUNC void glmMat_geti (lua_State *L, const TValue *obj, lua_Integer key, StkId res);

/* lua_settable variant for matrix types */
LUAI_FUNC void glmMat_set (lua_State *L, const TValue *obj, TValue *key, TValue *val);

/* OP_SETI variant for vector types. */
LUAI_FUNC void glmMat_seti (lua_State *L, const TValue *obj, lua_Integer key, TValue *val);

/* Place the magnitude of the matrix (o) at the specified stack index (ra) */
LUAI_FUNC void glmMat_objlen (const TValue *obj, StkId res);

/* converts a vector to a string. */
LUAI_FUNC int glmMat_tostr (const TValue *obj, char *buff, size_t len);

/*
** Pops a key from the stack and pushes a <key, value> pair from the matrix at
** the given stack index, the "next" pair after the given key.
**
** If there are no more elements in the vector, then returns 0 and pushes
** nothing.
*/
LUAI_FUNC int glmMat_next (const TValue *obj, StkId key);

/* luaV_equalobj variant for matrix types */
LUAI_FUNC int glmMat_equalObj (lua_State *L, const TValue *o1, const TValue *o2);

/* }================================================================== */

/*
** {==================================================================
** Miscellaneous
** ===================================================================
*/

/*
** Jenkins' one-at-a-time hash.
**
** It is the assume the string is properly delimited.
**
** @TODO: Change API to use lua_Unsigned
** @TODO: Compile-time option to allow 32-bit or 64-bit hashing.
*/
LUAI_FUNC lua_Integer luaO_HashString (const char* string, size_t length, int ignore_case);

/* }================================================================== */

#endif
