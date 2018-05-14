#include <algorithm>
#include <cstring>

#include "area.h"

Area::Area(const Point<size_t> size, const double initial_approx) : size(size) {
    size_t mem = (size.x + 2) * (size.y + 2) * (size.z + 2);
    area = new double[mem];
    std::fill_n(area, mem, initial_approx);
}

Area::~Area() {
    delete[] area;
}

double Area::get(const Point<int> pos) const noexcept {
    return area[pos.x * (size.y + 2) * (size.z + 2) + pos.y * (size.z + 2) +
                pos.z];
}

void Area::set(const double value, const Point<size_t> pos) noexcept {
    area[pos.x * (size.y + 2) * (size.z + 2) + pos.y * (size.z + 2) + pos.z] =
        value;
}

void Area::swapAreas(Area& area) noexcept {
    std::swap(this->area, area.area);
}

void Area::copyData(const Area& area) {
    size_t mem = (size.x + 2) * (size.y + 2) * (size.z + 2);
    memcpy(this->area, area.area, mem * sizeof(double));
}

double* Area::getFlatSlice(const size_t num) const {
    return area + (num * (size.y + 2) * (size.z + 2));
}
