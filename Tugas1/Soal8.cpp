#include <iostream>
#include <cmath>

using namespace std;

const double PI = 3.1416;
double r;

double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double circle_circumference(double r) {
    return 2 * PI * r;
}

double circle_area(double r) {
    return PI * r * r;
}

int main() {
    double xCenter, yCenter, xPoint, yPoint;

    cout << "Masukkan koordinat pusat lingkaran (x1): ";
    cin >> xCenter;

    cout << "Masukkan koordinat pusat lingkaran (y1): ";
    cin >> yCenter;

    cout << "Masukkan koordinat titik pada lingkaran (x2): ";
    cin >> xPoint;

    cout << "Masukkan koordinat titik pada lingkaran (y2): ";
    cin >> yPoint;

    double r = distance(xCenter, yCenter, xPoint, yPoint);
    double diameter = 2 * r;
    double keliling = circle_circumference(r);
    double luas = circle_area(r);

    cout << "\n\n=== HASIL PERHITUNGAN LINGKARAN ===" << endl;
    cout << "Radius\t\t: " << r << endl;
    cout << "Diameter\t: " << diameter << endl;
    cout << "Keliling\t: " << keliling << endl;
    cout << "Luas\t\t: " << luas << endl;

    return 0;
}