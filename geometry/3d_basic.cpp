#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include <queue>
#include <numeric>
#include <functional>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <thread>
#include <tuple>
#include <utility>

using namespace std;

const double eps = 5e-10;

// implements Complete Pivoting (slow)
struct GaussianElimination {
    // Solve matrix * (x) = companion
    vector<vector<double>> matrix;
    vector<double> companion;

    // columns are swapped. Assuming id is input order. So to access 0-th elt in x, companion[id_to_index[0]]
    vector<int> id_to_index;
    vector<int> index_to_id;

    explicit GaussianElimination(vector<vector<double>> matrix) : matrix(std::move(matrix)) {
        companion.resize(this->matrix.size());
        if (this->matrix.empty()) return;
        id_to_index.resize(this->matrix[0].size());
        iota(id_to_index.begin(), id_to_index.end(), 0);
        index_to_id = id_to_index;
    }

    GaussianElimination(vector<vector<double>> matrix, vector<double> companion)
            : matrix(std::move(matrix)), companion(std::move(companion)) {
        if (this->matrix.empty()) return;
        id_to_index.resize(this->matrix[0].size());
        iota(id_to_index.begin(), id_to_index.end(), 0);
        index_to_id = id_to_index;
    }

    // returns rank
    int eliminate() {
        auto n = (int) matrix.size();
        if (n == 0) return 0;
        auto m = (int) matrix[0].size();
        int rank = 0;
        for (; rank < n && rank < m; rank++) {
            int best_row = rank, best_col = rank;
            double magnitude = -1;
            for (int r = rank; r < n; r++) {
                for (int c = rank; c < m; c++) {
                    double v = fabs(matrix[r][c]);
                    if (v > magnitude) {
                        magnitude = v;
                        tie(best_row, best_col) = make_pair(r, c);
                    }
                }
            }

            if (magnitude < eps) {
                // no more pivots
                return rank;
            }

            if (rank != best_row) { /* row swap */
                matrix[rank].swap(matrix[best_row]);
                swap(companion[rank], companion[best_row]);
            }
            if (rank != best_col) { /* column swap */
                for (int r = 0; r < n; r++) {
                    swap(matrix[r][rank], matrix[r][best_col]);
                }
                swap(index_to_id[rank], index_to_id[best_col]);
                id_to_index[index_to_id[rank]] = rank;
                id_to_index[index_to_id[best_col]] = best_col;
            }

            // now [rank][rank] is the greatest abs
            for (int r = 0; r < n; r++) {
                if (r == rank) continue;
                double scale = -matrix[r][rank] / matrix[rank][rank];
                matrix[r][rank] = 0;
                for (int c = rank + 1; c < m; c++) {
                    matrix[r][c] += scale * matrix[rank][c];
                }
                companion[r] += scale * companion[rank];
            }
            {
                double scale = 1 / matrix[rank][rank];
                matrix[rank][rank] = 1;
                for (int c = rank + 1; c < m; c++) {
                    matrix[rank][c] *= scale;
                }
                companion[rank] *= scale;
            }
        }
        return rank;
    }
};

bool epsequal(double a, double b) {
    return fabs(a-b) < eps;
}

struct Point3 {
    double x, y, z;

    Point3() : x(0), y(0), z(0) {}
    Point3(double x, double y, double z) : x(x), y(y), z(z) {}
    Point3(const Point3 &other) = default;
    Point3(Point3 &&other) = default;
    Point3 &operator =(const Point3 &other) = default;
    Point3 &operator =(Point3 &&other) = default;
    Point3 operator *(const double &scala) const { return Point3(x * scala, y * scala, z * scala); }
    Point3 operator /(const double &scala) const { return Point3(x / scala, y / scala, z / scala); }
    Point3 operator -(const Point3 &other) const { return Point3(x - other.x, y - other.y, z - other.z); }
    Point3 operator +(const Point3 &other) const { return Point3(x + other.x, y + other.y, z + other.z); }
    bool operator ==(const Point3 &other) const {
        return epsequal(x, other.x) && epsequal(y, other.y) && epsequal(z, other.z);
    }
    bool operator !=(const Point3 &other) const { return !(*this == other); }
    double length() const { return hypot(x, hypot(y, z)); }

    // this x other
    Point3 cross(const Point3 &other) const {
        return Point3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
        );
    }

    double inner(const Point3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Point3 unit() const {
        double len = length();
        return (len < eps) ? Point3(0, 0, 0) : Point3(x / len, y / len, z / len);
    }

    // calculate angle between this - (0,0,0) - other
    // returns [0, pi]
    double angleOrigin(const Point3 &other) const {
        double cosval = unit().inner(other.unit());
        return acos(max(-1.0, min(1.0, cosval)));
    }

    bool collinear(const Point3 &q, const Point3 &r) const {
        return (q - *this).cross(r - *this).length() < eps;
    }
};
Point3 operator *(const double &scala, const Point3 &p) { return Point3(scala * p.x, scala * p.y, scala * p.z); }

struct Line {
    Point3 p; /* some point on the line */
    Point3 dir; /* unit vector */

    Line() : p(), dir() {}
    Line(Point3 p, Point3 dir) : p(p), dir(dir.unit()) {}

    Line(const Line &other) = default;
    Line(Line &&other) = default;
    Line &operator =(const Line &other) = default;
    Line &operator =(Line &&other) = default;

    // project q on the line
    Point3 project(const Point3 &q) const {
        double shadow = (q - p).inner(dir);
        return p + dir * shadow;
    }

    // rotate q around line (right hand rotation: thumb=dir)
    Point3 rotateAround(const Point3 &q, const double angle) const {
        Point3 o = project(q);
        Point3 noTurn = q - o;
        Point3 quarterTurn = dir.cross(noTurn);
        double c = cos(angle), s = sin(angle);
        return o + ((c * noTurn) + (s * quarterTurn));
    }

    double distance(const Point3 &q) const {
        Point3 d = q - p;
        return dir.cross(d).length();
    }

    double distance(const Line& other) const {
        Point3 n = dir.cross(other.dir).unit();
        if (n == Point3(0, 0, 0)) {
            // parallel
            return distance(other.p);
        }
        return fabs(n.inner(p - other.p));
    }

    // returns the nearest point in the current line, or (nan, nan, nan)
    Point3 nearest(const Line &other) const {
        // form a plane from other and <d1 x d2>
        Point3 n = dir.cross(other.dir).unit();
        if (n == Point3(0, 0, 0)) {
            // parallel
            return Point3(nan(""), nan(""), nan(""));
        }
        Point3 normal = n.cross(other.dir);
        return p + ((other.p - p).inner(normal) / dir.inner(normal)) * dir;
    }
};

struct Plane {
    // normal.inner(p) + d = 0
    Point3 normal; // not necessarily unit
    double d;

    Plane() : normal(), d(0) {}
    Plane(double a, double b, double c, double d) : normal(a, b, c), d(d) {}
    Plane(const Point3 &normal, double d) : normal(normal), d(d) {}
    Plane(const Point3 &normal, const Point3 &p) : normal(normal), d(-normal.inner(p)) {}
    Plane(const Point3 &p, const Point3 &q, const Point3 &r) {
        normal = (q-p).cross(r-p).unit();
        d = -normal.inner(p);
    }
    Plane(const Line &l, const Point3 &p) {
        normal = l.dir.cross(p - l.p).unit();
        d = -normal.inner(p);
    }

    // return some point on the plane
    Point3 some() const {
        return -d * normal / normal.inner(normal);
    }

    double distance(const Point3 &p) const {
        return (p.inner(normal) + d) / normal.length();
    }

    // project a point on the plane (caution: not vector)
    Point3 project(const Point3 &p) const {
        return p - ((p.inner(normal) + d) / normal.inner(normal)) * normal;
    }

    // degenerate case if line.dir.inner(normal) == 0 -> line in the plane, line parallel to the plane
    // if it's degenerate, return (nan, nan, nan)
    Point3 intersect(const Line &line) const {
        double D = line.dir.inner(normal);
        if (epsequal(D, 0)) {
            return Point3(nan(""), nan(""), nan(""));
        }
        return line.p - ((line.p.inner(normal) + d) / D) * line.dir;
    }

    // returns Line() if not solvable -> two cases, identical or parallel
    Line intersect(const Plane &A) const {
        vector<vector<double>> matrix({
                vector<double>({normal.x, normal.y, normal.z}),
                vector<double>({A.normal.x, A.normal.y, A.normal.z})
        });
        vector<double> companion({-d, -A.d});
        GaussianElimination ge(move(matrix), move(companion));
        int rank = ge.eliminate();
        if (rank < 2) {
            return Line();
        }
        double xs[3] = {0, 0, 0};
        for (int i = 0; i < 3; i++) {
            int ind = ge.id_to_index[i];
            if (ind < 2) {
                xs[i] = ge.companion[ind];
            }
        }
        return Line(Point3(xs[0], xs[1], xs[2]), normal.cross(A.normal));
    }
};

void print(Point3 p) {
    printf("%.25f %.25f %.25f\n", p.x, p.y, p.z);
}

int main() {
    Point3 p(2,3,4);
    printf("%.25f == 2*sqrt(71.0 / 7.0) = %.25f \n", Line(Point3(1,1,1), Point3(1,4,5)).distance(Point3(5,3,13)), 2*sqrt(71/7.));
    printf("%.25f == 14/sqrt(3419) = %.25f \n", Line(Point3(1,1,1), Point3(1,4,5)).distance(Line(Point3(5,3,13), Point3(6,3,17))), 14 / sqrt(3419.));
    Plane A(Point3(-1, 1, 0), 4);
    Point3 q = p + Point3(3,4,12);
    printf("%.25f == 0\n", A.distance(A.project(q)));
    print(A.some());
    printf("%.25f == 0\n", A.distance(A.some()));

    {
        Line line(p, q-p);
        Point3 intersection = A.intersect(line);
        print(intersection);
        printf("%.25f == 0\n", line.distance(intersection));
        printf("%.25f == 0\n", A.distance(intersection));

        double angle = M_PI / 3;
        Point3 orig = Point3(-2, 3, -4);
        Point3 rotated = line.rotateAround(orig, angle); // rotate 60 degrees
        Point3 projected_orig = line.project(orig);
        print(projected_orig);
        print(rotated);
        printf("%.25f == 0 (rotation)\n", line.distance(orig) - line.distance(rotated));
        printf("%.25f == %.25f (in radian)\n", (orig - projected_orig).angleOrigin(rotated - projected_orig), angle);
    }

    {
        Plane B(Point3(2, 5, 6), -3);
        Plane C(Point3(3, 7, 1), 2);
        Line intersection_line = B.intersect(C);
        print(intersection_line.p);
        printf("%.25f == 0\n", B.distance(intersection_line.p));
        printf("%.25f == 0\n", C.distance(intersection_line.p));
        print(intersection_line.p + intersection_line.dir);
        printf("%.25f == 0\n", B.distance(intersection_line.p + intersection_line.dir));
        printf("%.25f == 0\n", C.distance(intersection_line.p + intersection_line.dir));
    }
    return 0;
}
