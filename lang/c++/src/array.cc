/** @file array.cc  Array.

@authors Copyright (c) 2018 Jaakko Keränen <jaakko.keranen@iki.fi>

@par License

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

<small>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</small>
*/

#include "c_plus/array"
#include <c_plus/array.h>

namespace i { namespace typeless {

Array::Array(Size elementSize) : _array(new_Array(elementSize)) {}

Array::Array(const Array &a) : _array(copy_Array(a._array)) {}

Array::Array(Array &&a) {
    _array = a._array;
    a._array = nullptr;
}

Array::~Array() {
    delete_Array(_array);
}

Array::Size Array::size() const {
    return size_Array(_array);
}

void *Array::data() {
    return data_Array(_array);
}

const void *Array::data() const {
    return constData_Array(_array);
}

void *Array::at(Size pos) {
    return at_Array(_array, pos);
}

const void *Array::at(Size pos) const {
    return constAt_Array(_array, pos);
}

Array &Array::operator=(const Array &a) {
    setN_Array(_array, 0, a.at(0), a.size());
    return *this;
}

void *Array::operator[](Size pos) {
    return at_Array(_array, pos);
}

const void *Array::operator[](Size pos) const {
    return constAt_Array(_array, pos);
}

}} // namespace i::typeless
