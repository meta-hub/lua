/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_LINESEGMENT_HPP
#define EXT_GEOM_LINESEGMENT_HPP

#include "setup.hpp"
#include "line.hpp"
#include "triangle.hpp"

namespace glm {
  /// <summary>
  /// A line segment in 3D space is a finite line with a start and end point.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct LineSegment {
    using Point = vec<L, T, Q>;

    Point a;  // The starting point of this line segment.
    Point b;  // The end point of this line segment.

    LineSegment() GLM_DEFAULT_CTOR;

    LineSegment(T scalar)
      : a(scalar), b(scalar) {
    }

    LineSegment(const Point &begin, const Point &end)
      : a(begin), b(end) {
    }

    LineSegment(const LineSegment<L, T, Q> &line)
      : a(line.a), b(line.b) {
    }

    LineSegment<L, T, Q> &operator=(const LineSegment<L, T, Q> &line) {
      a = line.a;
      b = line.b;
      return *this;
    }

    GLM_FUNC_QUALIFIER Point dir() const {
      return normalize(b - a);
    }

    GLM_FUNC_QUALIFIER Point dir2() const {
      return b - a;
    }
  };

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Line<L, T, Q> toLine(const LineSegment<L, T, Q> &line) {
    return Line<L, T, Q>(line.a, line.dir());
  }

  // template<length_t L, typename T, qualifier Q>
  // GLM_GEOM_QUALIFIER Ray<L, T, Q> toRay(const LineSegment<L, T, Q> &line) {
  //   return Ray<L, T, Q>(line.a, line.dir());
  // }

  template<length_t L, typename T, qualifier Q>
  static LineSegment<L, T, Q> operator-(const LineSegment<L, T, Q> &line) {
    return LineSegment<L, T, Q>(-line.b, -line.a);
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const LineSegment<L, T, Q> &l1, const LineSegment<L, T, Q> &l2) {
    return l1.a == l2.a && l1.b == l2.b;
  }

  template<length_t L, typename T, qualifier Q>
  static LineSegment<L, T, Q> operator+(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &offset) {
    return LineSegment<L, T, Q>(line.a + offset, line.b + offset);
  }

  template<length_t L, typename T, qualifier Q>
  static LineSegment<L, T, Q> operator-(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &offset) {
    return LineSegment<L, T, Q>(line.a - offset, line.b - offset);
  }

  template<typename T, qualifier Q>
  static LineSegment<3, T, Q> operator*(const mat<3, 3, T, Q> &m, const LineSegment<3, T, Q> &line) {
    return LineSegment<3, T, Q>(m * line.a, m * line.b);
  }

  template<typename T, qualifier Q>
  static LineSegment<3, T, Q> operator*(const mat<3, 4, T, Q> &m, const LineSegment<3, T, Q> &line) {
    return LineSegment<3, T, Q>(m * line.a, m * line.b);
  }

  template<typename T, qualifier Q>
  static LineSegment<3, T, Q> operator*(const mat<4, 3, T, Q> &m, const LineSegment<3, T, Q> &line) {
    return LineSegment<3, T, Q>(transformPos(m, line.a), transformPos(m, line.b));
  }

  template<typename T, qualifier Q>
  static LineSegment<3, T, Q> operator*(const mat<4, 4, T, Q> &m, const LineSegment<3, T, Q> &line) {
    return LineSegment<3, T, Q>(transformPos(m, line.a), transformPos(m, line.b));
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<3, T, Q> operator*(const qua<T, Q> &q, const LineSegment<3, T, Q> &line) {
    return LineSegment<3, T, Q>(q * line.a, q * line.b);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, T eps = epsilon<T>()) {
    return all_equal(x.a, y.a, eps) && all_equal(x.b, y.b, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all_equal(x.a, y.a, eps) && all_equal(x.b, y.b, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, int MaxULPs) {
    return all_equal(x.a, y.a, MaxULPs) && all_equal(x.b, y.b, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all_equal(x.a, y.a, MaxULPs) && all_equal(x.b, y.b, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, T eps = epsilon<T>()) {
    return any_notequal(x.a, y.a, eps) || any_notequal(x.b, y.b, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any_notequal(x.a, y.a, eps) || any_notequal(x.b, y.b, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, int MaxULPs) {
    return any_notequal(x.a, y.a, MaxULPs) || any_notequal(x.b, y.b, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(LineSegment<L, T, Q> const &x, LineSegment<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any_notequal(x.a, y.a, MaxULPs) || any_notequal(x.b, y.b, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T length(const LineSegment<L, T, Q> &line) {
    return distance(line.a, line.b);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T length2(const LineSegment<L, T, Q> &line) {
    return distance2(line.a, line.b);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const LineSegment<L, T, Q> &line) {
    return all(isfinite(line.a)) && all(isfinite(line.b));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> getPoint(const LineSegment<L, T, Q> &line, T d) {
    return (T(1) - d) * line.a + d * line.b;
  }

  /// <summary>
  /// Returns the center point of this line segment; getPoint(line, T(0.5))
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> centerPoint(const LineSegment<L, T, Q> &line) {
    return (line.a + line.b) * T(0.5);
  }

  /// <summary>
  /// Reverses the direction of the line segment.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> reverse(const LineSegment<L, T, Q> &line) {
    return LineSegment<L, T, Q>(line.b, line.a);
  }

  /// <summary>
  /// Returns the (normalized) direction vector that points from line.a to line.b
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> dir(const LineSegment<L, T, Q> &line) {
    return normalize(line.dir2());
  }

  /// <summary>
  /// Compute an extreme point along the segment, i.e., the furthest point in a
  /// given direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &direction) {
    return dot(direction, line.dir2()) >= T(0) ? line.b : line.a;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &direction, T &projectionDistance) {
    vec<L, T, Q> point = extremePoint(line, direction);
    projectionDistance = dot(point, direction);
    return point;
  }

  /// <summary>
  /// Project the segment onto the given axis (direction), i.e., collapse the
  /// segment onto an axis.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER void projectToAxis(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &direction, T &outMin, T &outMax) {
    outMin = dot(direction, line.a);
    outMax = dot(direction, line.b);
    if (outMax < outMin) {
      const T temp = outMin;
      outMin = outMax;
      outMax = temp;
    }
  }

  // Computes the closest point on this segment to the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &point, T &d) {
    const vec<L, T, Q> dir = line.dir2();
    d = clamp(dot(point - line.a, dir) / length2(dir), T(0), T(1));
    return line.a + d * dir;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const Ray<L, T, Q> &ray, T &d, T &d2) {
    closestPoint(ray, line, d2, d);
    return getPoint(line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const Line<L, T, Q> &other, T &d, T &d2) {
    closestPointLineLine(other.pos, other.dir, line.a, line.dir2(), d2, d);
    if (d < T(0)) {
      d = T(0);
      closestPoint(other, line.a, d2);
      return line.a;
    }
    else if (d > T(1)) {
      d = T(1);
      closestPoint(other, line.b, d2);
      return line.b;
    }
    return getPoint(line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other, T &d, T &d2) {
    closestPointLineLine(line.a, line.dir2(), other.a, other.b - other.a, d, d2);

    if (d >= T(0) && d <= T(1) && d2 >= T(0) && d2 <= T(1))
      return line.a + d * line.dir2();
    else if (d >= T(0) && d <= T(1)) {
      const vec<L, T, Q> p = (d2 < T(0)) ? other.a : other.b;
      d2 = (d2 < T(0)) ? T(0) : T(1);
      return closestPoint(line, p, d);
    }
    else {
      const vec<L, T, Q> p = (d < T(0)) ? line.a : line.b;
      const vec<L, T, Q> p2 = (d2 < T(0)) ? other.a : other.b;
      d = (d < T(0)) ? T(0) : T(1);
      d2 = (d2 < T(0)) ? T(0) : T(1);

      T dt(0), dt2(0);
      const vec<L, T, Q> pt = closestPoint(line, p2, dt);
      const vec<L, T, Q> pt2 = closestPoint(other, p, dt2);
      if (distance2(pt, p2) <= distance2(pt2, p)) {
        d = dt;
        return pt;
      }
      else {
        d2 = dt2;
        return p;
      }
    }
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const Triangle<L, T, Q> &triangle, T &d, T &u, T &v) {
    closestPointTriangleSegment<L, T, Q, LineSegment<L, T, Q>>(triangle, line, u, v, d);
    return getPoint(line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &point) {
    T d(0);
    return closestPoint(line, point, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const Ray<L, T, Q> &ray) {
    T d(0), d2(0);
    return closestPoint(line, ray, d, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const Line<L, T, Q> &other) {
    T d(0), d2(0);
    return closestPoint(line, other, d, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other) {
    T d(0), d2(0);
    return closestPoint(line, other, d, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> closestPoint(const LineSegment<L, T, Q> &line, const Triangle<L, T, Q> &triangle) {
    T u, v, d(0);
    return closestPoint(line, triangle, d, u, v);
  }

  // Tests if the given object is fully contained on the segment.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &point, T distanceThreshold = epsilon<T>()) {
    T d = T(0);
    return distance2(closestPoint(line, point, d), point) <= distanceThreshold;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &rhs, T distanceThreshold = epsilon<T>()) {
    return contains(line, rhs.a, distanceThreshold) && contains(line, rhs.b, distanceThreshold);
  }

  // Computes the distance between the segment and the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &point, T &d) {
    return distance(closestPoint(line, point, d), point);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const LineSegment<L, T, Q> &line, const Ray<L, T, Q> &other, T &d, T &d2) {
    closestPoint(line, other, d, d2);
    return distance(getPoint(line, d), getPoint(other, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const LineSegment<L, T, Q> &line, const Line<L, T, Q> &other, T &d, T &d2) {
    return distance(getPoint(line, d2), closestPoint(other, line, d, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other, T &d, T &d2) {
    closestPoint(line, other, d, d2);
    return distance(getPoint(line, d), getPoint(other, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance2(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &point, T &d) {
    return distance2(closestPoint(line, point, d), point);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance2(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other, T &d, T &d2) {
    closestPoint(line, other, d, d2);
    return distance2(getPoint(line, d), getPoint(other, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance2(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &point) {
    T d = T(0);
    return distance2(line, point, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance2(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other) {
    T d(0), d2(0);
    return distance2(line, other, d, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const LineSegment<L, T, Q> &line, const Sphere<L, T, Q> &other) {
    T d = T(0);
    return max(T(0), distance(line, other.pos, d) - other.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const LineSegment<L, T, Q> &line, const Plane<L, T, Q> &plane) {
    const T aDist = signedDistance(plane, line.a);
    const T bDist = signedDistance(plane, line.b);
    if (aDist * bDist <= T(0))
      return T(0);  // Was an intersection
    return min(abs(aDist), abs(bDist));
  }

  /// Tests whether the segment and the given object intersect.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const LineSegment<L, T, Q> &line, const Sphere<L, T, Q> &sphere, T &d, T &d2) {
    return intersects(sphere, line, d, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &line, const AABB<L, T, Q> &aabb, T &d, T &d2) {
    return intersects(aabb, line, d, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &lineSegment, const Plane<L, T, Q> &plane, T &t) {
    return intersects(plane, lineSegment, t);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &lineSegment, const Triangle<L, T, Q> &triangle, T &d, T &u, T &v) {
    return intersects(triangle, lineSegment, u, v, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other, T &d, T &d2, T eps = epsilon<T>()) {
    return distance(line, other, d, d2) <= eps;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &line, const AABB<L, T, Q> &aabb) {
    return intersects(line, aabb);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &line, const Sphere<L, T, Q> &sphere) {
    return intersects(line, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &line, const Plane<L, T, Q> &plane) {
    const T aDist = signedDistance(plane, line.a);
    const T bDist = signedDistance(plane, line.b);
    return aDist * bDist <= T(0);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const LineSegment<L, T, Q> &line, const LineSegment<L, T, Q> &other, T eps = epsilon<T>()) {
    T d(0), d2(0);
    return distance(line, other, d, d2) <= eps;
  }

  namespace detail {
    // @LuaGLM
    template<glm::length_t L, typename T, qualifier Q>
    struct format_lua_string<LineSegment<L, T, Q>> {
      static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, const LineSegment<L, T, Q> &line) {
        char a[GLM_STRING_BUFFER];
        char b[GLM_STRING_BUFFER];

        format_lua_string<vec<L, T, Q>>::call(a, GLM_STRING_BUFFER, line.a);
        format_lua_string<vec<L, T, Q>>::call(b, GLM_STRING_BUFFER, line.b);
        return _vsnprintf(buff, buff_len, "segment(%s, %s)", a, b);
      }
    };

    template<glm::length_t L, typename T, qualifier Q>
    struct compute_to_string<LineSegment<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const LineSegment<L, T, Q> &line) {
        return detail::format("segment(%s, %s)",
          glm::to_string(line.a).c_str(),
          glm::to_string(line.b).c_str()
        );
      }
    };
  }
}

#endif
