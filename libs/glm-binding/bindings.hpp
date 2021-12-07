/*
** $Id: bindings.hpp $
**
** Template system designed to relate the various glm types (e.g., scalars, vec,
** quaternions, and matrices) and their functions to Lua operations. This file
** is designed to be compatible with g++, clang++, and MSVC (prior to
** https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=vs-2019)
**
** For example, to execute glm::axisAngleMatrix:
**  (1) Ensure the top two elements on a Lua stack are compatible with vec<3>
**      and a numeric type;
**  (2) Retrieve those values from the Lua stack and convert them into the
**      corresponding GLM types;
**  (3) Invoke glm::axisAngleMatrix() for those converted values;
**  (4) Convert the result back into something that is compatible with Lua;
**  (5) Push that value onto the Lua stack.
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_LUA_BINDINGS_HPP
#define BINDING_LUA_BINDINGS_HPP

#define LUA_LIB
#if !defined(LUA_API_LINKAGE)
#if defined(LUA_C_LINKAGE)
  #define LUA_API_LINKAGE "C"
#else
  #define LUA_API_LINKAGE "C++"
#endif
#endif

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtx/type_trait.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/integer.hpp>

#include <lua.hpp>
#include <lglm.hpp>
extern LUA_API_LINKAGE {
  #include "lgrit_lib.h"
  #include "lapi.h"
  #include "lobject.h"
  #include "lgc.h"
  #include "lstate.h"
  #include "lvm.h"
  #include "lglm_core.h"
}

#if defined(LUAGLM_INCLUDE_GEOM)
  #include "allocator.hpp"
  #include "ext/geom/aabb.hpp"
  #include "ext/geom/line.hpp"
  #include "ext/geom/linesegment.hpp"
  #include "ext/geom/ray.hpp"
  #include "ext/geom/triangle.hpp"
  #include "ext/geom/sphere.hpp"
  #include "ext/geom/plane.hpp"
  #include "ext/geom/polygon.hpp"
#endif

/* Lua Definitions */
#define GLM_NAME(F) glm_##F
#define GLM_NAMESPACE(NAME) glm::NAME

/* Metatable name for polygon userdata. */
#define LUAGLM_POLYGON_META "GLM_POLYGON"

/* Macro for implicitly handling floating point drift where possible */
#if defined(LUAGLM_DRIFT)
  #define glm_drift_compensate(x) glm::normalize((x))
#else
  #define glm_drift_compensate(x) x
#endif

/* lua_gettop() macro */
#if !defined(_gettop)
#define _gettop(L) cast_int((L)->top - ((L)->ci->func + 1))
#define _isvalid(L, o) (!ttisnil(o) || o != &G(L)->nilvalue)
#endif

/* TValue -> glmVector */
#if !defined(glm_vvalue)
#define glm_mvalue(o) glm_constmat_boundary(mvalue_ref(o))
#define glm_vvalue(o) check_exp(ttisvector(o), glm_constvec_boundary(&vvalue_(o)))
#define glm_v2value(o) glm_vvalue(o).v2
#define glm_v3value(o) glm_vvalue(o).v3
#define glm_v4value(o) glm_vvalue(o).v4
#define glm_qvalue(o) glm_vvalue(o).q
#endif

/*
@@ LUAGLM_ALIGNED compensate for some SIMD library functions being broken in GLM.
*/
#if !defined(LUAGLM_ALIGNED)
#if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE && defined(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
  #define LUAGLM_ALIGNED
#endif
#endif

/*
@@ LUAGLM_FALLTHROUGH Macro for informing the compiler a fallthrough is intentional
*/
#if defined __has_cpp_attribute && __has_cpp_attribute(fallthrough)
  #define LUAGLM_FALLTHROUGH [[fallthrough]]
#elif __has_attribute(__fallthrough__)
  #define LUAGLM_FALLTHROUGH __attribute__((__fallthrough__))
#else
  #define LUAGLM_FALLTHROUGH do {} while (0)  /* FALLTHROUGH */
#endif

/*
@@ LUAGLM_UNREACHABLE: Unreachable code path reached.
*/
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
  #define LUAGLM_UNREACHABLE() __builtin_unreachable()
#elif defined(__clang__) || defined(__INTEL_COMPILER)
  #define LUAGLM_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
  #define LUAGLM_UNREACHABLE() __assume(false)
#else
  #define LUAGLM_UNREACHABLE() lua_assert(false)
#endif

/*
** index2value ported from lapi.c. Simplified to only operate on positive stack
** indices; see related function index2stack.
*/
static LUA_INLINE const TValue *glm_i2v(lua_State *L, int idx) {
  const StkId o = L->ci->func + idx;
  api_check(L, idx > 0, "positive indices only");
  api_check(L, idx <= L->ci->top - (L->ci->func + 1), "invalid index");
  return (o >= L->top) ? &G(L)->nilvalue : s2v(o);
}

/*
** {==================================================================
** Traits
** ===================================================================
*/

/*
** Common header for Lua Traits.
**
** @NOTE: inlining this declaration will come at a (significant) cost to the
**  size of the shared-object/static-library.
*/
#define LUA_TRAIT_QUALIFIER static GLM_INLINE

/*
** Override GLM_FORCE_INLINE for the 'heavier' binding functions. tointegerx and
** tonumberx are quite heavy due the requirement that the binding can be a
** complete superset of lmathlib.
**
** @TODO: Eventually include a compile-option.
**  #define LUA_TRAIT_QUALIFIER_NIL static GLM_INLINE
*/
#define LUA_TRAIT_QUALIFIER_NIL static GLM_NEVER_INLINE

/// <summary>
/// Forward declare a (function) parameter trait.
///
/// This structure is used to interact with a stack iterator to convert sequences
/// of parameters into GLM objects.
/// </summary>
template<typename T, bool FastPath = false>
struct gLuaTrait;

/// <summary>
/// A structure that interfaces with an active Lua state.
///
/// This structure serves two purposes:
/// (1) A simple stack iterator state.
/// (2) Uses SFINAE for "Push" operations: placing a value (or values) on top of
///   the Lua stack that represent the GLM object; returning the number of
///   values placed onto the stack.
///
/// A benefit to this approach is to allow the quick creation of geometric
/// structures that does not require additional userdata/metatable definitions.
///
/// @TODO:
///   1. An interface for generating random numbers to replace std::rand(). This
///     allows gLuaBase to invoke math_random instead of having to maintain
///     multiple random states.
///
///   2. Consider: https://en.cppreference.com/w/cpp/numeric/random
/// </summary>
struct gLuaBase {
  lua_State *L;  // Current lua state.
  int idx;  // Iteration pointer.
  int ltop;  // Number of function parameters (lazily cached)

  gLuaBase(lua_State *baseL, int baseIdx = 1)
    : L(baseL), idx(baseIdx), ltop(0) {
  }

  /// <summary>
  /// Lazy catching lua_gettop
  /// </summary>
  GLM_INLINE int top() {
    return (ltop == 0) ? ((ltop = _gettop(L))) : ltop;
  }

  /// <summary>
  /// Presumes the "top" field is cached: i.e., the mutable equivalent of this
  /// method has been invoked at least once.
  /// </summary>
  GLM_INLINE int top() const {
    return ltop;  // return (ltop == 0) ? _gettop(L) : ltop;
  }

  /// <summary>
  /// Reset the iterator
  /// </summary>
  GLM_INLINE gLuaBase &reset() {
    idx = 1;
    return *this;
  }

  /// <summary>
  /// Invalid the pointer index, i.e., set it greater than top.
  /// </summary>
  GLM_INLINE void invalidate() {
    idx = top() + 1;
  }

  /// <summary>
  /// Temporary math.random() hook
  ///
  /// @TODO: Remove this function when math.random/std:rand functions are unified
  /// and have proper bindings.
  /// </summary>
  lua_Number rand() {
    const int t = top();  // Get cached top;
    lua_checkstack(L, 3);
    if (lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE) == LUA_TTABLE) {  // [..., load_tab]
      if (lua_getfield(L, -1, LUA_MATHLIBNAME) == LUA_TTABLE) {  // [..., load_tab, math_tab]
        if (lua_getfield(L, -1, "random") == LUA_TFUNCTION) {  // [..., load_tab, math_tab, rand_func]
          lua_call(L, 0, 1);
          lua_Number result = lua_tonumber(L, -1);  // [..., load_tab, math_tab, result]
          lua_pop(L, 3);
          return result;
        }
      }
    }

    lua_pop(L, _gettop(L) - t);  // Fallback to std::rand() if lmathlib has not been loaded
    return cast_num(std::rand()) / cast_num((RAND_MAX));
  }

  /// <summary>
  /// Return true if the current iteration pointer references a valid, and
  /// recyclable, data structure.
  /// </summary>
  GLM_INLINE bool can_recycle() {
#if defined(LUAGLM_RECYCLE)
    return (idx < 0 || idx <= top());
#else
    return false;
#endif
  }

  /// <summary>
  /// @HACK: luaL_typeerror that conveys noreturn information to the compiler.
  /// </summary>
  static l_noret typeerror(lua_State *L_, int arg, const char *tname) {
    luaL_typeerror(L_, arg, tname);

    // This code should never be reached given that a lngjmp or try/catch is
    // hidden underneath luaL_typeerror.
    LUAGLM_UNREACHABLE();
  }

  /// <summary>
  /// lua_tointeger with additional rules for casting booleans.
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER_NIL T tointegerx(lua_State *L_, int idx_) {
    const TValue *o = glm_i2v(L_, idx_);
    switch (ttypetag(o)) {
      case LUA_VTRUE: return static_cast<T>(1); break;
      case LUA_VFALSE: return static_cast<T>(0); break;
      case LUA_VNUMINT: return static_cast<T>(ivalue(o)); break;
      case LUA_VNUMFLT: return static_cast<T>(fltvalue(o)); break;
      default: {
#if defined(LUAGLM_TYPE_COERCION)
        return static_cast<T>(luaL_checkinteger(L_, idx_));
#else
        gLuaBase::typeerror(L_, idx_, GLM_STRING_INTEGER);
        return T(0);
#endif
      }
    }
  }

  /// <summary>
  /// lua_tonumber with additional rules for casting booleans.
  ///
  /// string coercion must exist for this binding to be a superset of lmathlib.
  /// As much of the luaL_checknumber logic is redundant, this should be
  /// optimized. However, luaV_tonumber_ is not an exported function.
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER_NIL T tonumberx(lua_State *L_, int idx_) {
    const TValue *o = glm_i2v(L_, idx_);
    switch (ttypetag(o)) {
      case LUA_VTRUE: return static_cast<T>(1); break;
      case LUA_VFALSE: return static_cast<T>(0); break;
      case LUA_VNUMINT: return static_cast<T>(ivalue(o)); break;
      case LUA_VNUMFLT: return static_cast<T>(fltvalue(o)); break;
      default: {
#if defined(LUAGLM_TYPE_COERCION)
        return static_cast<T>(luaL_checknumber(L_, idx_));
#else
        gLuaBase::typeerror(L_, idx_, GLM_STRING_NUMBER);
        return T(0);
#endif
      }
    }
  }

  /// <summary>
  /// Pull(gLuaBase) wrapper
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER int Pull(lua_State *L_, int idx_, T &v) {
    if (gLuaTrait<T>::Is(L_, idx_)) {
      gLuaBase LB_(L_, idx_);
      v = gLuaTrait<T>::Next(LB_);
      return 1;
    }
    return 0;
  }

  /// <summary>
  /// Push(gLuaBase) wrapper
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER int Push(lua_State *L, const T &v) {
    gLuaBase _LB(L, _gettop(L) + 1);
    return gLuaBase::Push(_LB, v);
  }

  /// <summary>
  /// Pushes a nil value onto the stack.
  /// </summary>
  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB) {
    luaL_pushfail(LB.L);
    return 1;
  }

  /// <summary>
  /// Pushes a boolean value with value b onto the stack.
  /// </summary>
  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB, bool b) {
    lua_pushboolean(LB.L, b);
    return 1;
  }

  /// <summary>
  /// Pushes integer-type with value 'v' (casted to a lua_Integer) onto the stack.
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, int>::type Push(const gLuaBase &LB, T v) {
    lua_pushinteger(LB.L, static_cast<lua_Integer>(v));
    return 1;
  }

  /// <summary>
  /// Pushes floating-point-type with value 'v' (casted to a lua_Number) onto the stack.
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, int>::type Push(const gLuaBase &LB, T v) {
    lua_pushnumber(LB.L, static_cast<lua_Number>(v));
    return 1;
  }

  /// <summary>
  /// Attempt to push the number as an integer; falling back to number otherwise
  /// </summary>
  LUA_TRAIT_QUALIFIER int PushNumInt(const gLuaBase &LB, lua_Number d) {
    lua_Integer n = 0;
    if (lua_numbertointeger(d, &n)) /* does 'd' fit in an integer? */
      lua_pushinteger(LB.L, n); /* result is integer */
    else
      lua_pushnumber(LB.L, d); /* result is float */
    return 1;
  }

  /// <summary>
  /// Attempt to push the vector as an glm::ivec; falling back to glm::vec otherwise.
  ///
  /// @NOTE: Future-proofing.
  /// </summary>
  template<glm::length_t L, typename T>
  LUA_TRAIT_QUALIFIER int PushNumInt(const gLuaBase &LB, const glm::vec<L, T> &v) {
    return Push(LB, v);
  }

  /// <summary>
  /// Pushes a zero-terminated string pointed to by s onto the stack.
  /// </summary>
  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB, const char *&s) {
    lua_pushstring(LB.L, s);
    return 1;
  }

  /// <summary>
  /// Convert one-or-more Lua values starting at idx into a suitable glm::vec<>
  /// structure; returning the number of stack values consumed to populate the
  /// vector.
  ///
  /// @NOTE: vec<1, ...> support removed from the binding as they were unused.
  /// The statement would simply be: Pull<T, FastPath>(LB, idx_, v.x)
  /// </summary>

  template<typename T>
  LUA_TRAIT_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, int>::type Push(const gLuaBase &LB, const glm::vec<1, T> &v) {
    lua_pushnumber(LB.L, static_cast<lua_Number>(v.x));
    return 1;
  }

  template<typename T>
  LUA_TRAIT_QUALIFIER typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, int>::type Push(const gLuaBase &LB, const glm::vec<1, T> &v) {
    lua_pushinteger(LB.L, static_cast<lua_Integer>(v.x));
    return 1;
  }

  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB, const glm::vec<1, bool> &v) {
    return gLuaBase::Push(LB, v.x);
  }

  template<glm::length_t L, typename T>
  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB, const glm::vec<L, T> &v) {
    return glm_pushvec(LB.L, glmVector(v), L);
  }

  /// <summary>
  /// Convert the provided glm::qua into a Lua suitable value(s).
  /// </summary>
  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB, const glm::qua<glm_Float> &q) {
    return glm_pushquat(LB.L, glm_drift_compensate(q));
  }

  template<glm::length_t C, glm::length_t R>
  static int Push(gLuaBase &LB, const glm::mat<C, R, glm_Float> &m) {
    if (LB.can_recycle()) {
      lua_State *L_ = LB.L;

      lua_lock(L_);
      const TValue *o = glm_i2v(L_, LB.idx);
      if (l_likely(ttismatrix(o))) {
        LB.idx++;

        glm_mat_boundary(mvalue_ref(o)) = m;
        setobj2s(L_, L_->top, o);  // lua_pushvalue
        api_incr_top(L_);
        lua_unlock(L_);
        return 1;
      }
      lua_unlock(L_);
    }

#if defined(LUAGLM_FORCED_RECYCLE)
    /* This library allocating memory is verboten! */
    return luaL_error(LB.L, "library configured to not allocate additional memory; use recycling mechanisms")
#else
    return glm_pushmat(LB.L, glmMatrix(m));
#endif
  }

  /// <summary>
  /// Helper for double-precision matrices: instead of casting the function
  /// arguments from double-to-float, cast the result.
  /// </summary>
  template<typename T, glm::length_t C, glm::length_t R>
  LUA_TRAIT_QUALIFIER typename std::enable_if<!std::is_same<T, glm_Float>::value, int>::type Push(gLuaBase &LB, const glm::mat<C, R, T> &m) {
    return Push(LB, glm::mat<C, R, glm_Float>(m));
  }

#if defined(LUAGLM_INCLUDE_GEOM)
  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::AABB<D, T> &v) {
    Push(LB, v.minPoint);
    Push(LB, v.maxPoint);
    return 2;
  }

  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::Line<D, T> &l) {
    Push(LB, l.pos);
    Push(LB, glm_drift_compensate(l.dir));
    return 2;
  }

  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::LineSegment<D, T> &l) {
    Push(LB, l.a);
    Push(LB, l.b);
    return 2;
  }

  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::Ray<D, T> &r) {
    Push(LB, r.pos);
    Push(LB, glm_drift_compensate(r.dir));
    return 2;
  }

  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::Triangle<D, T> &t) {
    Push(LB, t.a);
    Push(LB, t.b);
    Push(LB, t.c);
    return 3;
  }

  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::Sphere<D, T> &s) {
    Push(LB, s.pos);
    Push(LB, s.r);
    return 2;
  }

  template<glm::length_t D, typename T>
  LUA_TRAIT_QUALIFIER int Push(gLuaBase &LB, const glm::Plane<D, T> &p) {
    Push(LB, p.normal);
    Push(LB, p.d);
    return 2;
  }

  /// <summary>
  /// All mutable operations mutate the referenced Polygon userdata; simply
  /// push that userdata back onto the Lua stack.
  /// </summary>
  template<typename T>
  LUA_TRAIT_QUALIFIER int Push(const gLuaBase &LB, const glm::Polygon<3, T> &p) {
    if (l_likely(p.stack_idx >= 1)) {
      lua_pushvalue(LB.L, p.stack_idx);
      return 1;
    }

    // This code-path is not implemented for the time being. All polygons must
    // already exist on the Lua stack. Otherwise polygon_new will need to be
    // duplicated here.
    return luaL_error(LB.L, "not implemented");
  }
#endif
};

/* }================================================================== */

/*
** {==================================================================
** Specializations
** ===================================================================
*/

/// <summary>
/// Forward declaration of the epsion_value_trait
/// </summary>
template<typename T, bool FastPath = false>
struct gLuaEpsilonTrait;

/// <summary>
/// Shared functions for parsing types from the Lua stack.
/// </summary>
template<typename T, typename ValueType = typename T::value_type>
struct gLuaAbstractTrait : glm::type<T> {

  /// <summary>
  /// Base type.
  /// </summary>
  using type = T;

  /// <summary>
  /// Base primitive type: equivalent to ::value_type in glm::vec and glm::mat
  /// structures.
  /// </summary>
  using value_type = ValueType;

  /// <summary>
  /// Trait of the primitive type.
  /// </summary>
  using value_trait = gLuaTrait<value_type>;

  /// <summary>
  /// Epsilon of value_type.
  /// </summary>
  using eps_trait = gLuaEpsilonTrait<value_type>;

  /// <summary>
  /// @CastBinding: Cast this trait to a new primitive.
  /// </summary>
  template<typename Type = T>
  using as_type = gLuaTrait<Type>;

  /// <summary>
  /// @SafeBinding: Cast this trait to one that type-checks its value on Next.
  /// </summary>
  using safe = gLuaTrait<T, false>;

  /// <summary>
  /// @UnsafeBinding: Cast this trait to one that pulls the value without
  /// additional type checking.
  ///
  /// @NOTE: The idiom 'Tr::fast::Is' + 'Tr::fast::Next' may only be used when
  /// its guaranteed the trait does not coerce types, e.g., string-to-float or
  /// table-to-vector/matrix.
  /// </summary>
  using fast = gLuaTrait<T, true>;

  /// <summary>
  /// Return a descriptive parameter literal for debugging/error messaging.
  /// </summary>
  /// static GLM_CONSTEXPR const char *Label()

  /// <summary>
  /// Zero initialize
  /// </summary>
  ///LUA_TRAIT_QUALIFIER GLM_CONSTEXPR T zero()

  /// <summary>
  /// Return true if the value starting at "idx" on the Lua stack corresponds
  /// to this type.
  /// </summary>
  /// LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {

  /// <summary>
  /// Given a current stack state, create a GLM object corresponding to the
  /// "type" this trait is capturing.
  ///
  /// @NOTE: This function will be invoked for each GLM bound parameter.
  ///   Force inlining by default (the compiler should be making this decision
  ///   anyway) will significantly increase the size of the shared object.
  ///   Instead, this function is beholden to GLM_FORCE_INLINE which is a
  ///   compile-time toggle.
  /// </summary>
  /// LUA_TRAIT_QUALIFIER T Next(gLuaBase &LB)
};

/// <summary>
/// gLuaTrait optimized for primitive types.
/// </summary>
template<typename T, bool FastPath = false>
struct gLuaPrimitive : gLuaAbstractTrait<T, T> {
  static GLM_CONSTEXPR glm::length_t length() { return 1; }

  static GLM_CONSTEXPR const char *Label() {
    GLM_IF_CONSTEXPR(std::is_same<T, bool>::value) return "bool";
    GLM_IF_CONSTEXPR(std::is_integral<T>::value) return GLM_STRING_INTEGER;
    GLM_IF_CONSTEXPR(std::is_floating_point<T>::value) return GLM_STRING_NUMBER;
    return "Unknown_Type";
  }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR T zero() {
    return T(0);
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    GLM_IF_CONSTEXPR(std::is_same<T, bool>::value) return ttisboolean(o);  // lua_isboolean(LB.L, idx);
    GLM_IF_CONSTEXPR(std::is_integral<T>::value) return ttisinteger(o) || ttisboolean(o);  // lua_isinteger(LB.L, idx)
    GLM_IF_CONSTEXPR(std::is_floating_point<T>::value) return lua_isnumber(L, idx);  // @TODO: isboolean
    lua_assert(false);
    return false;
  }

  LUA_TRAIT_QUALIFIER T Next(gLuaBase &LB) {
    GLM_IF_CONSTEXPR(FastPath) {
      const TValue *o = glm_i2v(LB.L, LB.idx++);
      GLM_IF_CONSTEXPR(std::is_same<T, bool>::value) return static_cast<T>(!l_isfalse(o));
      GLM_IF_CONSTEXPR(std::is_integral<T>::value) return static_cast<T>(ivalue(o));
      GLM_IF_CONSTEXPR(std::is_floating_point<T>::value) return static_cast<T>(fltvalue(o));
    }
    else {
      GLM_IF_CONSTEXPR(std::is_same<T, bool>::value) return static_cast<T>(lua_toboolean(LB.L, LB.idx++));
      GLM_IF_CONSTEXPR(std::is_integral<T>::value) return gLuaBase::tointegerx<T>(LB.L, LB.idx++);
      GLM_IF_CONSTEXPR(std::is_floating_point<T>::value) return gLuaBase::tonumberx<T>(LB.L, LB.idx++);
    }
    lua_assert(false);
    return zero();
  }
};

/// <summary>
/// Abstraction vector trait implementation.
/// </summary>
template<glm::length_t D, typename T, bool FastPath>
struct gLuaAbstractVector : gLuaAbstractTrait<glm::vec<D, T>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::vec<D, Type>>;  // @CastBinding

  /// <summary>
  /// Cast the vector trait to a compatible matrix type.
  ///
  /// This type is mainly a helper for matrix-multiplication operators.
  /// </summary>
  template<glm::length_t R>
  using rhs_mat_type = gLuaTrait<glm::mat<R, D, T>>;

  /// <summary>
  /// Alternative name: lhs_mat_type
  /// </summary>
  using row_type = gLuaTrait<glm::vec<D, T>>;

  /// <summary>
  /// Trait definition of glm::vec<D,T>::length()
  /// </summary>
  static GLM_CONSTEXPR glm::length_t length() {
    return D;
  }
};

/// <summary>
/// Generic trait for all types: POD/Structs/Classes.
/// </summary>
template<typename T, bool FastPath> struct gLuaTrait : gLuaAbstractTrait<T, T> {  };

// Explicit trait specializations types declared in fwd.hpp
template<bool FastPath> struct gLuaTrait<bool, FastPath> : gLuaPrimitive<bool, FastPath> { };
template<bool FastPath> struct gLuaTrait<char, FastPath> : gLuaPrimitive<char, FastPath> { };
template<bool FastPath> struct gLuaTrait<char16_t, FastPath> : gLuaPrimitive<char16_t, FastPath> { };
template<bool FastPath> struct gLuaTrait<char32_t, FastPath> : gLuaPrimitive<char32_t, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed char, FastPath> : gLuaPrimitive<signed char, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed short int, FastPath> : gLuaPrimitive<signed short int, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed int, FastPath> : gLuaPrimitive<signed int, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed long int, FastPath> : gLuaPrimitive<signed long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed long long int, FastPath> : gLuaPrimitive<signed long long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned char, FastPath> : gLuaPrimitive<unsigned char, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned short int, FastPath> : gLuaPrimitive<unsigned short int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned int, FastPath> : gLuaPrimitive<unsigned int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned long int, FastPath> : gLuaPrimitive<unsigned long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned long long int, FastPath> : gLuaPrimitive<unsigned long long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<float, FastPath> : gLuaPrimitive<float, FastPath> { };
template<bool FastPath> struct gLuaTrait<double, FastPath> : gLuaPrimitive<double, FastPath> { };

template<bool FastPath>
struct gLuaTrait<const char *, FastPath> : gLuaAbstractTrait<const char *, const char *> {
  static GLM_CONSTEXPR glm::length_t length() { return 1; }
  static GLM_CONSTEXPR const char *Label() { return "string"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR const char *zero() {
    return GLM_NULLPTR;
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisstring(o);
  }

  LUA_TRAIT_QUALIFIER const char* Next(gLuaBase &LB, size_t *len = GLM_NULLPTR) {
    GLM_IF_CONSTEXPR(FastPath) {
      const TValue *o = glm_i2v(LB.L, LB.idx++);
      if (len != GLM_NULLPTR)
        *len = vslen(o);
      return svalue(o);
    }
    else {
      return lua_tolstring(LB.L, LB.idx++, len);
    }
  }
};

/// <summary>
/// Trait for glm::qua types.
/// </summary>
template<typename T, bool FastPath>
struct gLuaTrait<glm::qua<T>, FastPath> : gLuaAbstractTrait<glm::qua<T>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::qua<T>>;  // @CastBinding

  static GLM_CONSTEXPR glm::length_t length() { return 4; }  // glm::qua<T>::length()
  static GLM_CONSTEXPR const char *Label() { return GLM_STRING_QUATERN; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::qua<T> zero() {
    return glm::qua<T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisquat(o);
  }

  LUA_TRAIT_QUALIFIER glm::qua<T> Next(gLuaBase &LB) {
    const TValue *o = glm_i2v(LB.L, LB.idx++);
    if (FastPath || l_likely(ttisquat(o))) {
      GLM_IF_CONSTEXPR(std::is_same<T, glm_Float>::value)
        return glm_drift_compensate(glm_qvalue(o));
      else {
        const glm::qua<glm_Float>& tq = glm_drift_compensate(glm_qvalue(o));
        return cast_quat(tq, T);
      }
    }
    gLuaBase::typeerror(LB.L, LB.idx - 1, GLM_STRING_QUATERN);
    return zero();
  }
};

/// <summary>
/// Trait for glm::vec<1, T> types.
/// </summary>
template<typename T, bool FastPath>
struct gLuaTrait<glm::vec<1, T>, FastPath> : gLuaAbstractVector<1, T, FastPath> {
  static GLM_CONSTEXPR const char *Label() { return GLM_STRING_VECTOR1; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::vec<1, T> zero() {
    return glm::vec<1, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    return gLuaTrait<T>::Is(L, idx);
  }

  LUA_TRAIT_QUALIFIER glm::vec<1, T> Next(gLuaBase &LB) {
    return glm::vec<1, T>(gLuaTrait<T>::Next(LB));
  }
};

/// <summary>
/// Trait for glm::vec<2, T> types.
/// </summary>
template<typename T, bool FastPath>
struct gLuaTrait<glm::vec<2, T>, FastPath> : gLuaAbstractVector<2, T, FastPath> {
  static GLM_CONSTEXPR const char *Label() { return GLM_STRING_VECTOR2; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::vec<2, T> zero() {
    return glm::vec<2, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisvector2(o);
  }

  LUA_TRAIT_QUALIFIER glm::vec<2, T> Next(gLuaBase &LB) {
    const TValue *o = glm_i2v(LB.L, LB.idx++);
    if (FastPath || l_likely(ttisvector2(o))) {
      const glm::vec<2, glm_Float> &_v = glm_v2value(o);
      GLM_IF_CONSTEXPR(!std::is_same<T, glm_Float>::value)
        return cast_vec2(_v, T);
      return _v;
    }

    gLuaBase::typeerror(LB.L, LB.idx - 1, Label());
    return zero();
  }
};

/// <summary>
/// Trait for glm::vec<3, T> types.
/// </summary>
template<typename T, bool FastPath>
struct gLuaTrait<glm::vec<3, T>, FastPath> : gLuaAbstractVector<3, T, FastPath> {
  static GLM_CONSTEXPR const char *Label() { return GLM_STRING_VECTOR3; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::vec<3, T> zero() {
    return glm::vec<3, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisvector3(o);
  }

  LUA_TRAIT_QUALIFIER glm::vec<3, T> Next(gLuaBase &LB) {
    const TValue *o = glm_i2v(LB.L, LB.idx++);
    if (FastPath || l_likely(ttisvector3(o))) {
      const glm::vec<3, glm_Float> &_v = glm_v3value(o);
      GLM_IF_CONSTEXPR(!std::is_same<T, glm_Float>::value)
        return cast_vec3(_v, T);
      return _v;
    }

    gLuaBase::typeerror(LB.L, LB.idx - 1, Label());
    return zero();
  }
};

/// <summary>
/// Trait for glm::vec<4, T> types.
/// </summary>
template<typename T, bool FastPath>
struct gLuaTrait<glm::vec<4, T>, FastPath> : gLuaAbstractVector<4, T, FastPath> {
  static GLM_CONSTEXPR const char *Label() { return GLM_STRING_VECTOR4; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::vec<4, T> zero() {
    return glm::vec<4, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisvector4(o);
  }

  LUA_TRAIT_QUALIFIER glm::vec<4, T> Next(gLuaBase &LB) {
    const TValue *o = glm_i2v(LB.L, LB.idx++);
    if (FastPath || l_likely(ttisvector4(o))) {
      const glm::vec<4, glm_Float> &_v = glm_v4value(o);
      GLM_IF_CONSTEXPR(!std::is_same<T, glm_Float>::value)
        return cast_vec4(_v, T);
      return _v;
    }

    gLuaBase::typeerror(LB.L, LB.idx - 1, Label());
    return zero();
  }
};

/// <summary>
/// Trait for glm::mat<C, R, T> types.
/// </summary>
template<glm::length_t C, glm::length_t R, typename T, bool FastPath>
struct gLuaTrait<glm::mat<C, R, T>, FastPath> : gLuaAbstractTrait<glm::mat<C, R, T>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::mat<C, R, Type>>;  // @CastBinding

  /// <summary>
  /// Lua type trait equivalent to glm::mat<C, R, T>::col_type
  /// </summary>
  using col_type = gLuaTrait<typename glm::mat<C, R, T>::col_type>;

  /// <summary>
  /// Lua type trait equivalent to glm::mat<C, R, T>::row_type
  /// </summary>
  using row_type = gLuaTrait<typename glm::mat<C, R, T>::row_type>;

  /// <summary>
  /// Right-hand matrix-operation type.
  /// </summary>
  template<glm::length_t RNext>
  using rhs_mat_type = gLuaTrait<glm::mat<RNext, C, T>>;

  /// <summary>
  /// Helpers for QR decomposition
  /// </summary>
  using q_type = gLuaTrait<glm::mat<(C < R ? C : R), R, T>>;
  using r_type = gLuaTrait<glm::mat<C, (C < R ? C : R), T>>;

  /// <summary>
  /// Trait definition of glm::mat<C, R, T>::length()
  /// </summary>
  static GLM_CONSTEXPR glm::length_t length() {
    return C;
  }

  static GLM_CONSTEXPR const char *Label() {
    switch (LUAGLM_MATRIX_TYPE(C, R)) {
      case LUAGLM_MATRIX_2x2: return GLM_STRING_MATRIX "2x2";
      case LUAGLM_MATRIX_2x3: return GLM_STRING_MATRIX "2x3";
      case LUAGLM_MATRIX_2x4: return GLM_STRING_MATRIX "2x4";
      case LUAGLM_MATRIX_3x2: return GLM_STRING_MATRIX "3x2";
      case LUAGLM_MATRIX_3x3: return GLM_STRING_MATRIX "3x3";
      case LUAGLM_MATRIX_3x4: return GLM_STRING_MATRIX "3x4";
      case LUAGLM_MATRIX_4x2: return GLM_STRING_MATRIX "4x2";
      case LUAGLM_MATRIX_4x3: return GLM_STRING_MATRIX "4x3";
      case LUAGLM_MATRIX_4x4: return GLM_STRING_MATRIX "4x4";
      default: {
        break;
      }
    }
    return GLM_STRING_MATRIX;
  }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::mat<C, R, T> zero() {
    return glm::mat<C, R, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return (ttismatrix(o) && mvalue_dims(o) == LUAGLM_MATRIX_TYPE(C, R));
  }

  LUA_TRAIT_QUALIFIER glm::mat<C, R, T> Next(gLuaBase &LB) {
    const TValue *o = glm_i2v(LB.L, LB.idx++);
    if (l_likely(FastPath || ttismatrix(o))) {
      const glmMatrix &mat = glm_mvalue(o);
      if (l_likely(FastPath || mat.dimensions == LUAGLM_MATRIX_TYPE(C, R))) {
        static const GLM_CONSTEXPR glm::length_t D = LUAGLM_MATRIX_TYPE(C, R);
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x2) return mat.m22;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x3) return mat.m23;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x4) return mat.m24;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x2) return mat.m32;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x3) return mat.m33;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x4) return mat.m34;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x2) return mat.m42;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x3) return mat.m43;
        GLM_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x4) return mat.m44;
      }
    }

    gLuaBase::typeerror(LB.L, LB.idx - 1, Label());
    return zero();
  }
};

// Specializations

using gLuaFloat = gLuaTrait<glm_Float>;
using gLuaNumber = gLuaTrait<glm_Number>;
using gLuaInteger = gLuaTrait<lua_Integer>;

/// <summary>
/// See @LUAGLM_NUMBER_ARGS.
/// </summary>
#if defined(LUAGLM_NUMBER_ARGS)
using gLuaFloatOnly = gLuaNumber;
#else
using gLuaFloatOnly = gLuaFloat;
#endif

template<typename T = glm_Float> using gLuaVec1 = gLuaTrait<glm::vec<1, T>>;
template<typename T = glm_Float> using gLuaVec2 = gLuaTrait<glm::vec<2, T>>;
template<typename T = glm_Float> using gLuaVec3 = gLuaTrait<glm::vec<3, T>>;
template<typename T = glm_Float> using gLuaVec4 = gLuaTrait<glm::vec<4, T>>;
template<typename T = glm_Float> using gLuaQuat = gLuaTrait<glm::qua<T>>;

template<typename T = glm_Float> using gLuaMat2x2 = gLuaTrait<glm::mat<2, 2, T>>;
template<typename T = glm_Float> using gLuaMat2x3 = gLuaTrait<glm::mat<2, 3, T>>;
template<typename T = glm_Float> using gLuaMat2x4 = gLuaTrait<glm::mat<2, 4, T>>;
template<typename T = glm_Float> using gLuaMat3x2 = gLuaTrait<glm::mat<3, 2, T>>;
template<typename T = glm_Float> using gLuaMat3x3 = gLuaTrait<glm::mat<3, 3, T>>;
template<typename T = glm_Float> using gLuaMat3x4 = gLuaTrait<glm::mat<3, 4, T>>;
template<typename T = glm_Float> using gLuaMat4x2 = gLuaTrait<glm::mat<4, 2, T>>;
template<typename T = glm_Float> using gLuaMat4x3 = gLuaTrait<glm::mat<4, 3, T>>;
template<typename T = glm_Float> using gLuaMat4x4 = gLuaTrait<glm::mat<4, 4, T>>;

/// <summary>
/// See @LUAGLM_DRIFT
/// </summary>
#if defined(LUAGLM_DRIFT)
template<glm::length_t L, typename T, bool FastPath>
struct gLuaDir : gLuaTrait<glm::vec<L, T>, FastPath> {
  using safe = gLuaDir<L, T, false>;  // @SafeBinding
  using fast = gLuaDir<L, T, true>;  // @UnsafeBinding

  LUA_TRAIT_QUALIFIER glm::vec<L, T> Next(gLuaBase &LB) {
    const glm::vec<L, T> result = gLuaTrait<glm::vec<L, T>, FastPath>::Next(LB);
    return glm_drift_compensate(result);
  }
};
template<typename T = glm_Float> using gLuaDir2 = gLuaDir<2, T, false>;
template<typename T = glm_Float> using gLuaDir3 = gLuaDir<3, T, false>;
#else
template<typename T = glm_Float> using gLuaDir2 = gLuaTrait<glm::vec<2, T>>;
template<typename T = glm_Float> using gLuaDir3 = gLuaTrait<glm::vec<3, T>>;
#endif

/// <summary>
/// Specialization for floating point epsilon arguments (and/or default arguments).
/// </summary>
template<typename T, bool FastPath>
struct gLuaEpsilonTrait : gLuaTrait<T, FastPath> {
  using safe = gLuaEpsilonTrait<T, false>;  // @SafeBinding
  using fast = gLuaEpsilonTrait<T, true>;  // @UnsafeBinding

  static GLM_CONSTEXPR const char *Label() { return "epsilon"; }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return !_isvalid(L, o) || gLuaTrait<T>::Is(L, idx);
  }

  LUA_TRAIT_QUALIFIER T Next(gLuaBase &LB) {
    const TValue *o = glm_i2v(LB.L, LB.idx);
    if (!_isvalid(LB.L, o)) {
      LB.idx++;  // Skip the argument
      return glm::epsilon<T>();
    }
    return gLuaTrait<T, FastPath>::Next(LB);
  }
};

/// <summary>
/// @GLMAssert: Specialization for traits bounded at zero. Usage of this trait
/// often reflects an assert clause within the GLM implementation.
///
/// @NOTE: All of the assert statements within GLM relate to conditions around
/// zero or floats between [0, 1]. These trait implementations abuse that
/// shortcut.
/// </summary>
template<class Tr, bool Inclusive = true, bool IncludeEps = false>
struct gLuaBoundedBelow : gLuaTrait<typename Tr::type, false> {
  using safe = gLuaBoundedBelow<typename Tr::safe, Inclusive, IncludeEps>;  // @SafeBinding
  using fast = gLuaBoundedBelow<typename Tr::fast, Inclusive, IncludeEps>;  // @UnsafeBinding

  static GLM_CONSTEXPR const char *Label() { return Tr::Label(); }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR typename Tr::type zero() { return Tr::zero(); }
  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) { return Tr::Is(L, idx); }
  LUA_TRAIT_QUALIFIER typename Tr::type Next(gLuaBase &LB) {
    using T = typename Tr::type;
    const T min = T(0) + (IncludeEps ? std::numeric_limits<typename Tr::value_type>::epsilon() : 0);

    const T value = Tr::Next(LB);
    if ((Inclusive && !glm::all(glm::lessThanEqual(min, value))) || (!Inclusive && !glm::all(glm::lessThan(min, value))))
      luaL_argerror(LB.L, LB.idx - 1, "argument not in range");
    return value;
  }
};

/// <summary>
/// @GLMAssert: Specialization for arguments between zero and one.
/// </summary>
template<class Tr>
struct gLuaBoundedBetween : gLuaTrait<typename Tr::type, false> {
  using safe = gLuaBoundedBetween<typename Tr::safe>;  // @SafeBinding
  using fast = gLuaBoundedBetween<typename Tr::fast>;  // @UnsafeBinding

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR typename Tr::type zero() { return Tr::zero(); }
  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) { return Tr::Is(L, idx); }
  LUA_TRAIT_QUALIFIER typename Tr::type Next(gLuaBase &LB) {
    using T = typename Tr::type;

    const T value = Tr::Next(LB);
    if (!glm::all(glm::lessThanEqual(T(0), value)) || !glm::all(glm::greaterThanEqual(T(1), value)))
      luaL_argerror(LB.L, LB.idx - 1, "argument not in range");
    return value;
  }
};

/// <summary>
/// @GLMAssert: Specialization that ensures values cannot be zero (fmod)
/// </summary>
template<class Tr>
struct gLuaNotZero : gLuaTrait<typename Tr::type, false> {
  using safe = gLuaNotZero<typename Tr::safe>;  // @SafeBinding
  using fast = gLuaNotZero<typename Tr::fast>;  // @UnsafeBinding

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR typename Tr::type zero() { return Tr::zero(); }
  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) { return Tr::Is(L, idx); }
  LUA_TRAIT_QUALIFIER typename Tr::type Next(gLuaBase &LB) {
    using T = typename Tr::type;

    const T value = Tr::Next(LB);
    if (std::is_integral<T>::value && glm::any(glm::equal(T(0), value)))
      luaL_argerror(LB.L, LB.idx - 1, "zero");
    return value;
  }
};

/* }================================================================== */

/*
** {==================================================================
** Traits Functions.
** ===================================================================
*/

/*
** Generic Traits Operation:
**  (1) Extract data from Lua stack and convert into some glm suitable structure
**  (2) Execute the bound glm function: "R = F(...)"
**  (3) Convert the function result back into something suitable for Lua and it
**      onto the Lua stack: "gLuaBase::Push(LB, ...)"
**
** @NOTE: Must consider the evaluation of function arguments.
*/

#define _VA_NARGS_GLUE(x, y) x y
#define _VA_NARGS_RETURN_COUNT(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, count, ...) count
#define _VA_NARGS_EXPAND(args) _VA_NARGS_RETURN_COUNT args
#define _VA_NARGS_COUNT_MAX(...) _VA_NARGS_EXPAND((__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define _VA_NARGS_COUNT 10

#define _VA_NARGS_OVERLOAD_MACRO2(name, count) name##count
#define _VA_NARGS_OVERLOAD_MACRO1(name, count) _VA_NARGS_OVERLOAD_MACRO2(name, count)
#define _VA_NARGS_OVERLOAD_MACRO(name, count) _VA_NARGS_OVERLOAD_MACRO1(name, count)
#define VA_NARGS_CALL_OVERLOAD(name, ...) _VA_NARGS_GLUE(_VA_NARGS_OVERLOAD_MACRO(name, _VA_NARGS_COUNT_MAX(__VA_ARGS__)), (__VA_ARGS__))

/* Mapping Lua stack values to function parameters */
#define TRAITS_FUNC(...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, __VA_ARGS__)

/* fail */
#define TRAITS_FUNC1(LB, F) \
  return gLuaBase::Push(LB)

/* F() */
#define TRAITS_FUNC2(LB, F) \
  return gLuaBase::Push(LB, F())

/* F(a) */
#define TRAITS_FUNC3(LB, F, A)               \
  LUA_MLM_BEGIN                              \
  return gLuaBase::Push(LB, F(A::Next(LB))); \
  LUA_MLM_END

/* F(a, b) */
#define TRAITS_FUNC4(LB, F, A, B)         \
  LUA_MLM_BEGIN                           \
  const A::type __a = A::Next(LB);        \
  const B::type __b = B::Next(LB);        \
  return gLuaBase::Push(LB, F(__a, __b)); \
  LUA_MLM_END

/* F(a, b, c)) */
#define TRAITS_FUNC5(LB, F, A, B, C)           \
  LUA_MLM_BEGIN                                \
  const A::type __a = A::Next(LB);             \
  const B::type __b = B::Next(LB);             \
  const C::type __c = C::Next(LB);             \
  return gLuaBase::Push(LB, F(__a, __b, __c)); \
  LUA_MLM_END

/* F(a, b, c, d) */
#define TRAITS_FUNC6(LB, F, A, B, C, D) \
  LUA_MLM_BEGIN                         \
  const A::type __a = A::Next(LB);      \
  const B::type __b = B::Next(LB);      \
  const C::type __c = C::Next(LB);      \
  const D::type __d = D::Next(LB);      \
  return gLuaBase::Push(LB, F(          \
    __a, __b, __c, __d                  \
  ));                                   \
  LUA_MLM_END

/* F(a, b, c, d, e) */
#define TRAITS_FUNC7(LB, F, A, B, C, D, E) \
  LUA_MLM_BEGIN                            \
  const A::type __a = A::Next(LB);         \
  const B::type __b = B::Next(LB);         \
  const C::type __c = C::Next(LB);         \
  const D::type __d = D::Next(LB);         \
  const E::type __e = E::Next(LB);         \
  return gLuaBase::Push(LB, F(             \
    __a, __b, __c, __d, __e                \
  ));                                      \
  LUA_MLM_END

/* F(a, b, c, d, e, g) */
#define TRAITS_FUNC8(LB, F, A, B, C, D, E, G) \
  LUA_MLM_BEGIN                               \
  const A::type __a = A::Next(LB);            \
  const B::type __b = B::Next(LB);            \
  const C::type __c = C::Next(LB);            \
  const D::type __d = D::Next(LB);            \
  const E::type __e = E::Next(LB);            \
  const G::type __g = G::Next(LB);            \
  return gLuaBase::Push(LB, F(                \
    __a, __b, __c, __d, __e, __g              \
  ));                                         \
  LUA_MLM_END

/* F(a, b, c, d, e, g, h) */
#define TRAITS_FUNC9(LB, F, A, B, C, D, E, G, H) \
  LUA_MLM_BEGIN                                  \
  const A::type __a = A::Next(LB);               \
  const B::type __b = B::Next(LB);               \
  const C::type __c = C::Next(LB);               \
  const D::type __d = D::Next(LB);               \
  const E::type __e = E::Next(LB);               \
  const G::type __g = G::Next(LB);               \
  const H::type __h = H::Next(LB);               \
  return gLuaBase::Push(LB, F(                   \
    __a, __b, __c, __d, __e, __g, __h            \
  ));                                            \
  LUA_MLM_END

/* F(a, b, c, d, e, g, h, i) */
#define TRAITS_FUNC10(LB, F, A, B, C, D, E, G, H, I) \
  LUA_MLM_BEGIN                                      \
  const A::type __a = A::Next(LB);                   \
  const B::type __b = B::Next(LB);                   \
  const C::type __c = C::Next(LB);                   \
  const D::type __d = D::Next(LB);                   \
  const E::type __e = E::Next(LB);                   \
  const G::type __g = G::Next(LB);                   \
  const H::type __h = H::Next(LB);                   \
  const I::type __i = I::Next(LB);                   \
  return gLuaBase::Push(LB, F(                       \
    __a, __b, __c, __d, __e, __g, __h, __i           \
  ));                                                \
  LUA_MLM_END

/*
** Place values onto the Lua stack in an order-of-evaluation independent
** fashion; returning the number of values placed onto the Lua stack.
*/
#define TRAITS_PUSH(...) VA_NARGS_CALL_OVERLOAD(TRAITS_PUSH, __VA_ARGS__)

#define TRAITS_PUSH1(LB) \
  return gLuaBase::Push(LB)

#define TRAITS_PUSH2(LB, A) \
  return gLuaBase::Push(LB, (A))

#define TRAITS_PUSH3(LB, A, B)             \
  LUA_MLM_BEGIN                            \
  const int __a = gLuaBase::Push(LB, (A)); \
  const int __b = gLuaBase::Push(LB, (B)); \
  return __a + __b;                        \
  LUA_MLM_END

#define TRAITS_PUSH4(LB, A, B, C)          \
  LUA_MLM_BEGIN                            \
  const int __a = gLuaBase::Push(LB, (A)); \
  const int __b = gLuaBase::Push(LB, (B)); \
  const int __c = gLuaBase::Push(LB, (C)); \
  return __a + __b + __c;                  \
  LUA_MLM_END

#define TRAITS_PUSH5(LB, A, B, C, D)       \
  LUA_MLM_BEGIN                            \
  const int __a = gLuaBase::Push(LB, (A)); \
  const int __b = gLuaBase::Push(LB, (B)); \
  const int __c = gLuaBase::Push(LB, (C)); \
  const int __d = gLuaBase::Push(LB, (D)); \
  return __a + __b + __c + __d;            \
  LUA_MLM_END

#define TRAITS_PUSH6(LB, A, B, C, D, E)    \
  LUA_MLM_BEGIN                            \
  const int __a = gLuaBase::Push(LB, (A)); \
  const int __b = gLuaBase::Push(LB, (B)); \
  const int __c = gLuaBase::Push(LB, (C)); \
  const int __d = gLuaBase::Push(LB, (D)); \
  const int __e = gLuaBase::Push(LB, (E)); \
  return __a + __b + __c + __d + __e;      \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** Argument Layouts
** ===================================================================
*/

/*
** Argument Layout: In most cases, the first argument to a glm::function is
** sufficient in template argument deduction. Moreover, that argument, or that
** trait, is often repeated. For example, consider a binary operation glm::dot.
** in which the vector/quaternion trait/type is repeated once.
**
** LAYOUT_*(F, Traits, ...) are defined by:
**    F - the glm::function being wrapped.
**    Tr - the first/deducing argument trait.
**    ... - Any trailing traits (types) consistent across all templates of the
**          same glm::function.
*/

/* Trait repetition */
#define LAYOUT_UNARY(LB, F, Tr, ...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, ##__VA_ARGS__)
#define LAYOUT_BINARY(LB, F, Tr, ...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_TERNARY(LB, F, Tr, ...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_QUATERNARY(LB, F, Tr, ...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_QUINARY(LB, F, Tr, ...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_SENARY(LB, F, Tr, ...) VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, Tr::safe, Tr::safe, ##__VA_ARGS__)

/* trait + eps op */
#define LAYOUT_BINARY_EPS(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::eps_trait, ##__VA_ARGS__)

/* trait + trait::primitive op */
#define LAYOUT_BINARY_SCALAR(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__)

/* trait + trait + eps op */
#define LAYOUT_TERNARY_EPS(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::eps_trait, ##__VA_ARGS__)

/* trait + trait + trait::primitive op */
#define LAYOUT_TERNARY_SCALAR(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__)

/* trait + trait + trait + trait + trait::primitive op */
#define LAYOUT_QUINARY_SCALAR(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, Tr::value_trait, ##__VA_ARGS__)

/* trait + trait<int> op */
#define LAYOUT_VECTOR_INT(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::as_type<int>, ##__VA_ARGS__)

/* trait + trait + trait + trait::primitive + trait::primitive op */
#define LAYOUT_BARYCENTRIC(LB, F, Tr, ...) \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::value_trait, Tr::value_trait, ##__VA_ARGS__)

/* unary or binary operator depending on the state of the Lua stack */
#define LAYOUT_UNARY_OR_BINARY(LB, F, Tr, ...)                               \
  LUA_MLM_BEGIN                                                              \
  if (Tr::Is((LB).L, (LB).idx + 1))                                          \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__); \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, ##__VA_ARGS__);             \
  LUA_MLM_END

/* trait + {nil || trait::primitive} op */
#define LAYOUT_UNARY_OPTIONAL(LB, F, Tr, ...)                                     \
  LUA_MLM_BEGIN                                                                   \
  if (lua_isnoneornil((LB).L, (LB).idx + 1))                                      \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, ##__VA_ARGS__);                \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); \
  LUA_MLM_END

/* unary or ternary operator depending on state of Lua stack */
#define LAYOUT_UNARY_OR_TERNARY(LB, F, Tr, ...)                                      \
  LUA_MLM_BEGIN                                                                      \
  if (lua_isnoneornil((LB).L, (LB).idx + 1))                                         \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, ##__VA_ARGS__);                   \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__); \
  LUA_MLM_END

/* trait + {trait || trait::primitive} op */
#define LAYOUT_BINARY_OPTIONAL(LB, F, Tr, ...)                                      \
  LUA_MLM_BEGIN                                                                     \
  if (Tr::value_trait::Is((LB).L, (LB).idx + 1))                                    \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__);          \
  LUA_MLM_END

/* trait + trait + {trait || trait::primitive} op */
#define LAYOUT_TERNARY_OPTIONAL(LB, F, Tr, ...)                                               \
  LUA_MLM_BEGIN                                                                               \
  if (Tr::value_trait::Is((LB).L, (LB).idx + 2))                                              \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__); \
  VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);          \
  LUA_MLM_END

/* A binary integer layout that sanitizes the second argument (division/modulo zero) */
#define LAYOUT_MODULO(LB, F, Tr, ...)                                                            \
  LUA_MLM_BEGIN                                                                                  \
  if (Tr::value_trait::Is((LB).L, (LB).idx + 1))                                                 \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, gLuaNotZero<Tr::value_trait>, ##__VA_ARGS__); \
  else                                                                                           \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, Tr, gLuaNotZero<Tr::safe>, ##__VA_ARGS__);        \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** Common Argument Parsers
** ===================================================================
*/

/* Invalid glm structure configurations */
#define GLM_INVALID_MAT_DIMENSIONS ("invalid " GLM_STRING_MATRIX " dimensions")

/*
** Generalized int16_t, int32_t, etc. definition
**
** @NOTE: Due to the nature of storing most/all data as floating point types,
**  bitfield operations on vectors may be inconsistent with float -> int -> float
**  casting.
**
**  Therefore, all INTEGER_VECTOR definitions are considered unsafe when the
**  function isn't explicitly operating on lua_Integer types.
*/
#define PARSE_VECTOR_TYPE(LB, F, IType, FType, VType, ILayout, FLayout, VLayout, ...)  \
  LUA_MLM_BEGIN                                                                        \
  const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                       \
  switch (ttypetag(_tv)) {                                                             \
    case LUA_VFALSE: case LUA_VTRUE: /* @BoolCoercion */                               \
    case LUA_VNUMINT: ILayout(LB, F, gLuaTrait<IType>, ##__VA_ARGS__); break;          \
    case LUA_VSHRSTR: case LUA_VLNGSTR: /* @StringCoercion */                          \
    case LUA_VNUMFLT: FLayout(LB, F, gLuaTrait<FType>, ##__VA_ARGS__); break;          \
    case LUA_VVECTOR2: VLayout(LB, F, gLuaVec2<VType>::fast, ##__VA_ARGS__); break;    \
    case LUA_VVECTOR3: VLayout(LB, F, gLuaVec3<VType>::fast, ##__VA_ARGS__); break;    \
    case LUA_VVECTOR4: VLayout(LB, F, gLuaVec4<VType>::fast, ##__VA_ARGS__); break;    \
    default:                                                                           \
      break;                                                                           \
  }                                                                                    \
  return luaL_typeerror((LB).L, (LB).idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR); \
  LUA_MLM_END

/* Vector definition where the lua_Number operation takes priority */
#define PARSE_NUMBER_VECTOR(LB, F, FLayout, VLayout, ...) \
  PARSE_VECTOR_TYPE(LB, F, gLuaNumber::value_type, gLuaNumber::value_type, gLuaVec3<>::value_type, FLayout, FLayout, VLayout, ##__VA_ARGS__)

/* Vector definition where lua_Integer and lua_Number operations takes priority */
#define PARSE_INTEGER_NUMBER_VECTOR(LB, F, ILayout, FLayout, VLayout, ...) \
  PARSE_VECTOR_TYPE(LB, F, gLuaInteger::value_type, gLuaNumber::value_type, gLuaVec3<>::value_type, ILayout, FLayout, VLayout, ##__VA_ARGS__)

/* glm::function defined over the vector & quaternion space: vec1, vec2, vec3, vec4, quat */
#define PARSE_NUMBER_VECTOR_QUAT(LB, F, FLayout, VLayout, QLayout, ...)                 \
  LUA_MLM_BEGIN                                                                         \
  const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                        \
  switch (ttypetag(_tv)) {                                                              \
    case LUA_VFALSE: case LUA_VTRUE: /* @BoolCoercion */                                \
    case LUA_VSHRSTR: case LUA_VLNGSTR: /* @StringCoercion */                           \
    case LUA_VNUMINT:  /* @IntCoercion */                                               \
    case LUA_VNUMFLT: FLayout(LB, F, gLuaNumber, ##__VA_ARGS__); break;                 \
    case LUA_VVECTOR2: VLayout(LB, F, gLuaVec2<>::fast, ##__VA_ARGS__); break;          \
    case LUA_VVECTOR3: VLayout(LB, F, gLuaVec3<>::fast, ##__VA_ARGS__); break;          \
    case LUA_VVECTOR4: VLayout(LB, F, gLuaVec4<>::fast, ##__VA_ARGS__); break;          \
    case LUA_VQUAT: QLayout(LB, F, gLuaQuat<>::fast, ##__VA_ARGS__); break;             \
    default:                                                                            \
      break;                                                                            \
  }                                                                                     \
  return luaL_typeerror((LB).L, (LB).idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN); \
  LUA_MLM_END

/*
** A glm::function that defined over any NxM matrix
**
** @NOTE: This parser does not throw an error if the Value is not a known matrix
**  value. This is to simplify the EQUAL/HASH operations.
*/
#define PARSE_MATRIX(LB, Value, F, ArgLayout, ...)                                      \
  LUA_MLM_BEGIN                                                                         \
  switch (mvalue_dims(Value)) {                                                         \
    case LUAGLM_MATRIX_2x2: ArgLayout(LB, F, gLuaMat2x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_2x3: ArgLayout(LB, F, gLuaMat2x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_2x4: ArgLayout(LB, F, gLuaMat2x4<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x2: ArgLayout(LB, F, gLuaMat3x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x3: ArgLayout(LB, F, gLuaMat3x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x4: ArgLayout(LB, F, gLuaMat3x4<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x2: ArgLayout(LB, F, gLuaMat4x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x3: ArgLayout(LB, F, gLuaMat4x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x4: ArgLayout(LB, F, gLuaMat4x4<>::fast, ##__VA_ARGS__); break; \
    default:                                                                            \
      break;                                                                            \
  }                                                                                     \
  LUA_MLM_END

/* A glm::function that operates only on NxN matrices */
#define PARSE_SYMMETRIC_MATRIX(LB, F, ArgLayout, ...)                                     \
  LUA_MLM_BEGIN                                                                           \
  const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                          \
  if (l_likely(ttismatrix(_tv))) {                                                        \
    switch (mvalue_dims(_tv)) {                                                           \
      case LUAGLM_MATRIX_2x2: ArgLayout(LB, F, gLuaMat2x2<>::fast, ##__VA_ARGS__); break; \
      case LUAGLM_MATRIX_3x3: ArgLayout(LB, F, gLuaMat3x3<>::fast, ##__VA_ARGS__); break; \
      case LUAGLM_MATRIX_4x4: ArgLayout(LB, F, gLuaMat4x4<>::fast, ##__VA_ARGS__); break; \
      default:                                                                            \
        return luaL_typeerror((LB).L, (LB).idx, GLM_INVALID_MAT_DIMENSIONS);              \
    }                                                                                     \
  }                                                                                       \
  return luaL_typeerror((LB).L, (LB).idx, GLM_STRING_SYMMATRIX);                          \
  LUA_MLM_END

/*
** a glm::function that operates on rotation matrices
**
** @NOTE: All geometric objects must support multiplication operations for
**  quaternions, mat3x3, mat3x4, mat4x3, and mat4x4.
*/
#define PARSE_ROTATION_MATRIX(LB, F, ArgLayout, ...)                                        \
  LUA_MLM_BEGIN                                                                             \
  const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                            \
  switch (ttypetag(_tv)) {                                                                  \
    case LUA_VQUAT: ArgLayout(LB, F, gLuaQuat<>, ##__VA_ARGS__); break;                     \
    case LUA_VMATRIX: {                                                                     \
      switch (mvalue_dims(_tv)) {                                                           \
        case LUAGLM_MATRIX_3x3: ArgLayout(LB, F, gLuaMat3x3<>::fast, ##__VA_ARGS__); break; \
        case LUAGLM_MATRIX_3x4: ArgLayout(LB, F, gLuaMat3x4<>::fast, ##__VA_ARGS__); break; \
        case LUAGLM_MATRIX_4x3: ArgLayout(LB, F, gLuaMat4x3<>::fast, ##__VA_ARGS__); break; \
        case LUAGLM_MATRIX_4x4: ArgLayout(LB, F, gLuaMat4x4<>::fast, ##__VA_ARGS__); break; \
        default:                                                                            \
          return luaL_typeerror((LB).L, (LB).idx, GLM_INVALID_MAT_DIMENSIONS);              \
      }                                                                                     \
    }                                                                                       \
    default:                                                                                \
      break;                                                                                \
  }                                                                                         \
  return luaL_typeerror((LB).L, (LB).idx, GLM_STRING_QUATERN " or " GLM_STRING_MATRIX);     \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** Generic Function API
** ===================================================================
*/

/* Common function declaration for all Lua-binded GLM functions. */
#define GLM_BINDING_QUALIFIER(NAME) static int GLM_NAME(NAME)(lua_State *L)

/*
** Exception handling wrapper for generic API functions.
**
** Generic try/catch blocks are used to avoid any lingering std::logic_error and
** std::runtime_error exceptions that could be thrown by GLM.
**
** @NOTE: This toggle isn't required for versions of Lua compiled with C++ so
**    long as LUAI_TRY handles exceptions via try/catch. Although, the flags
**    LUA_USE_LONGJMP & LUA_CPP_EXCEPTIONS can change that functionality.
*/
#if defined(LUAGLM_SAFELIB)
  #define GLM_BINDING_BEGIN         \
    gLuaBase LB(L);                 \
    /* Ensure LB.top() is cached */ \
    const int __top = LB.top();     \
    try {

  #define GLM_BINDING_END                 \
    }                                     \
    catch (const std::exception &e) {     \
      lua_settop(L, __top);               \
      lua_pushstring(L, e.what());        \
    }                                     \
    catch (...) {                         \
      lua_settop(L, __top);               \
      lua_pushstring(L, "GLM exception"); \
    }                                     \
    return lua_error(L);
#else
  #define GLM_BINDING_BEGIN gLuaBase LB(L);
  #define GLM_BINDING_END
#endif

/* GLM function that corresponds to one unique set of function parameters */
#define TRAITS_DEFN(Name, F, ...)                              \
  GLM_BINDING_QUALIFIER(Name) {                                \
    GLM_BINDING_BEGIN                                          \
    VA_NARGS_CALL_OVERLOAD(TRAITS_FUNC, LB, F, ##__VA_ARGS__); \
    GLM_BINDING_END                                            \
  }

/* A GLM function where the first argument (Tr) is sufficient in template argument deduction; */
#define TRAITS_LAYOUT_DEFN(Name, F, ArgLayout, Tr, ...) \
  GLM_BINDING_QUALIFIER(Name) {                         \
    GLM_BINDING_BEGIN                                   \
    ArgLayout(LB, F, Tr, ##__VA_ARGS__);                \
    GLM_BINDING_END                                     \
  }

/*
** A operation defined for two traits; often 2D/3D or 3D/4D vectors.
**
** Allows @UnsafeBinding when A/B are non-coerced types.
*/
#define TRAITS_BINARY_LAYOUT_DEFN(Name, F, ArgLayout, A, B, ...)            \
  GLM_BINDING_QUALIFIER(Name) {                                             \
    GLM_BINDING_BEGIN                                                       \
    if (A::Is((LB).L, (LB).idx)) ArgLayout(LB, F, A, ##__VA_ARGS__);        \
    if (B::Is((LB).L, (LB).idx)) ArgLayout(LB, F, B, ##__VA_ARGS__);        \
    return luaL_error((LB).L, "%s or %s expected", A::Label(), B::Label()); \
    GLM_BINDING_END                                                         \
  }

/* Vector definition where the lua_Number operation takes priority */
#define NUMBER_VECTOR_DEFN(Name, F, ArgLayout, ...)                  \
  GLM_BINDING_QUALIFIER(Name) {                                      \
    GLM_BINDING_BEGIN                                                \
    PARSE_NUMBER_VECTOR(LB, F, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                  \
  }

/* Vector definition where lua_Integer and lua_Number operations takes priority */
#define INTEGER_NUMBER_VECTOR_DEFN(Name, F, ArgLayout, ...)                             \
  GLM_BINDING_QUALIFIER(Name) {                                                         \
    GLM_BINDING_BEGIN                                                                   \
    PARSE_INTEGER_NUMBER_VECTOR(LB, F, ArgLayout, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                                     \
  }

#define INTEGER_NUMBER_VECTOR_DEFNS(Name, F, ILayout, FLayout, VLayout, ...)      \
  GLM_BINDING_QUALIFIER(Name) {                                                   \
    GLM_BINDING_BEGIN                                                             \
    PARSE_INTEGER_NUMBER_VECTOR(LB, F, ILayout, FLayout, VLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                               \
  }

/* glm::function defined over the vector & quaternion space: vec1, vec2, vec3, vec4, quat */
#define NUMBER_VECTOR_QUAT_DEFN(Name, F, ArgLayout, ...)                             \
  GLM_BINDING_QUALIFIER(Name) {                                                      \
    GLM_BINDING_BEGIN                                                                \
    PARSE_NUMBER_VECTOR_QUAT(LB, F, ArgLayout, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                                  \
  }

#define NUMBER_VECTOR_QUAT_DEFNS(Name, F, FLayout, VLayout, QLayout, ...)      \
  GLM_BINDING_QUALIFIER(Name) {                                                \
    GLM_BINDING_BEGIN                                                          \
    PARSE_NUMBER_VECTOR_QUAT(LB, F, FLayout, VLayout, QLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                            \
  }

/* A glm::function that defined over any quaternions */
#define QUAT_DEFN(Name, F, ArgLayout, ...)        \
  GLM_BINDING_QUALIFIER(Name) {                   \
    GLM_BINDING_BEGIN                             \
    ArgLayout(LB, F,  gLuaQuat<>, ##__VA_ARGS__); \
    GLM_BINDING_END                               \
  }

/* A glm::function that defined over any NxM matrix */
#define MATRIX_DEFN(Name, F, ArgLayout, ...)                \
  GLM_BINDING_QUALIFIER(Name) {                             \
    GLM_BINDING_BEGIN                                       \
    const TValue *_m = glm_i2v(LB.L, LB.idx);               \
    if (l_likely(ttismatrix(_m)))                           \
      PARSE_MATRIX(LB, _m, F, ArgLayout, ##__VA_ARGS__);    \
    return luaL_typeerror(LB.L, LB.idx, GLM_STRING_MATRIX); \
    GLM_BINDING_END                                         \
  }

/* A glm::function that operates only on NxN matrices */
#define SYMMETRIC_MATRIX_DEFN(Name, F, ArgLayout, ...)       \
  GLM_BINDING_QUALIFIER(Name) {                              \
    GLM_BINDING_BEGIN                                        \
    PARSE_SYMMETRIC_MATRIX(LB, F, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                          \
  }

/* a glm::function that operates on rotation matrices */
#define ROTATION_MATRIX_DEFN(Name, F, ArgLayout, ...)       \
  GLM_BINDING_QUALIFIER(Name) {                             \
    GLM_BINDING_BEGIN                                       \
    PARSE_ROTATION_MATRIX(LB, F, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                         \
  }

/* Generalized int16_t, int32_t, etc. function definition */
#define INTEGER_VECTOR_DEFN(Name, F, ArgLayout, IType, ...)                                        \
  GLM_BINDING_QUALIFIER(Name) {                                                                    \
    GLM_BINDING_BEGIN                                                                              \
    PARSE_VECTOR_TYPE(LB, F, IType, IType, IType, ArgLayout, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                                                \
  }

/* }================================================================== */

/*
** {==================================================================
** Function Bindings
** ===================================================================
*/

/* @COMPAT max ULPs parameters for scalar numbers introduced in 0.9.9.3 */
#if GLM_VERSION >= 993
  #define _TR_EQUAL_ULPS(LB, F, A, B, Val)                                        \
    else if (ttisinteger(Val)) {                                                  \
      return gLuaBase::Push((LB), F((A), (B), gLuaTrait<int>::fast::Next((LB)))); \
    }
#else
  #define _TR_EQUAL_ULPS(LB, F, A, B, Val)
#endif

/*
** Generic equals layout
**
** Allows @UnsafeBinding when Tr_Row is a non-coerced type.
*/
#define LAYOUT_GENERIC_EQUAL(LB, F, Tr, Tr_Row)                                                          \
  LUA_MLM_BEGIN                                                                                          \
  const Tr::type __a = Tr::Next(LB);                                                                     \
  const Tr::safe::type __b = Tr::safe::Next(LB);                                                         \
  const TValue *_tv3 = glm_i2v((LB).L, (LB).idx);                                                        \
  if (!_isvalid((LB).L, _tv3)) /* <Tr, Tr> */                                                            \
    return gLuaBase::Push(LB, F(__a, __b));                                                              \
  else if (ttisfloat(_tv3)) /* <Tr, Tr, eps> */                                                          \
    return gLuaBase::Push(LB, F(__a, __b, Tr::eps_trait::fast::Next(LB)));                               \
  else if (Tr_Row::Is((LB).L, (LB).idx)) /* <Tr, Tr, vec> */                                             \
    return gLuaBase::Push(LB, F(__a, __b, Tr_Row::Next(LB)));                                            \
  _TR_EQUAL_ULPS(LB, F, __a, __b, _tv3) /* <Tr, Tr, ULPs> */                                             \
  return luaL_typeerror((LB).L, (LB).idx, "expected none, " GLM_STRING_NUMBER " or " GLM_STRING_VECTOR); \
  LUA_MLM_END

/* }================================================================== */

#endif
