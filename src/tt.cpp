/*
    Clarity
    Copyright (C) 2024 Joseph Pasfield

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

uint64_t index(uint64_t key, size_t size) {
    // this emits a single mul on both x64 and arm64
    return static_cast<uint64_t>((static_cast<unsigned __int128>(key) * static_cast<unsigned __int128>(size)) >> 64);
}

uint32_t getAgeDelta(const int age, const Transposition *entry) {
  return (MAX_AGE + age - entry->age) % MAX_AGE;
}


Transposition* TranspositionTable::getEntry(uint64_t zkey) {
    auto bucket = &table[index(zkey, size)];
    auto entryToReplace = &bucket->entries[0];
    if(entryToReplace->score != 0 && entryToReplace->flag != Undefined && shrink(zkey) != entryToReplace->zobristKey) {
        for(int i = 1; i < BUCKET_SIZE; i += 1) {
            auto entry = &bucket->entries[i];
            if((entryToReplace->score == 0 && entryToReplace->flag == Undefined) || shrink(zkey) == entry->zobristKey) {
                return entry;
            }
            const int lowestQuality = entryToReplace->depth - 8 * getAgeDelta(age, entryToReplace);
            const int currentQuality = entry->depth - 8 * getAgeDelta(age, entry);
            if(currentQuality < lowestQuality) {
                entryToReplace = entry;
            }
        }
    }
    return entryToReplace;
}

void TranspositionTable::setEntry(Transposition &entry, Transposition* oldEntry) {
    oldEntry->zobristKey = entry.zobristKey;
    oldEntry->bestMove = entry.bestMove;
    oldEntry->flag = entry.flag;
    oldEntry->score = entry.score;
    oldEntry->depth = entry.depth;
    oldEntry->staticEval = entry.staticEval;
    oldEntry->age = entry.age;
}

void TranspositionTable::clearTable(int threadCount) {
    const std::size_t chunks = (size + threadCount - 1) / threadCount;
    std::vector<std::thread> threads;
    for(int i = 0; i < threadCount; ++i) {
        threads.emplace_back([i, chunks, this]() {
            const std::size_t clearIndex = chunks * i;
            const std::size_t clearSize = std::min(chunks, size - clearIndex) * sizeof(TTBucket);
            std::memset(table.data() + clearIndex, 0, clearSize);
        });
    }
    for(auto &thread : threads) {
        thread.join();
    }
    age = 0;
}

void TranspositionTable::resize(size_t newSizeMB, int threadCount) {
    size_t newSizeB = newSizeMB * 1024 * 1024;
    size_t newSizeEntries = newSizeB / sizeof(TTBucket);
    size = newSizeEntries;
    table.resize(size, TTBucket());
    clearTable(threadCount);
}
