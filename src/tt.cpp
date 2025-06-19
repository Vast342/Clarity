/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "tt.h"

// all the functions for the transposition table
// keeping this code during the search rewrite because I did it so badly last time I'm not repeating that

uint64_t index(uint64_t key, size_t size) {
    // this emits a single mul on both x64 and arm64
    return static_cast<uint64_t>((static_cast<unsigned __int128>(key) * static_cast<unsigned __int128>(size)) >> 64);
}

Transposition* TranspositionTable::getEntry(uint64_t zkey) {
    return &table[index(zkey, size)];
}

void TranspositionTable::setEntry(uint64_t zkey, Transposition &entry) {
    table[index(zkey, size)] = entry;
}

void TranspositionTable::clearTable() {
    std::fill(table.begin(), table.end(), Transposition());
}

void TranspositionTable::resize(size_t newSizeMB) {
    size_t newSizeB = newSizeMB * 1024 * 1024;
    size_t newSizeEntries = newSizeB / sizeof(Transposition);
    size = newSizeEntries;
    table.resize(newSizeEntries, Transposition());
    clearTable();
}
