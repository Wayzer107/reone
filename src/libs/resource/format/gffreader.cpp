/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "reone/resource/format/gffreader.h"

#include "reone/resource/exception/format.h"
#include "reone/system/logutil.h"

namespace reone {

namespace resource {

void GffReader::load() {
    _gff.ignore(8); // signature

    _structOffset = _gff.readUint32();
    _structCount = _gff.readUint32();
    _fieldOffset = _gff.readUint32();
    _fieldCount = _gff.readUint32();
    _labelOffset = _gff.readUint32();
    _labelCount = _gff.readUint32();
    _fieldDataOffset = _gff.readUint32();
    _fieldDataCount = _gff.readUint32();
    _fieldIndicesOffset = _gff.readUint32();
    _fieldIncidesCount = _gff.readUint32();
    _listIndicesOffset = _gff.readUint32();
    _listIndicesCount = _gff.readUint32();

    _root = std::move(readStruct(0));
}

std::unique_ptr<Gff> GffReader::readStruct(int idx) {
    _gff.seek(_structOffset + 12ll * idx);

    uint32_t type = _gff.readUint32();
    uint32_t dataOffset = _gff.readUint32();
    uint32_t fieldCount = _gff.readUint32();

    auto fields = std::vector<Gff::Field>();

    if (fieldCount == 1) {
        fields.push_back(readField(dataOffset));
    } else {
        std::vector<uint32_t> indices(readFieldIndices(dataOffset, fieldCount));
        for (auto &idx : indices) {
            fields.push_back(readField(idx));
        }
    }

    return std::make_unique<Gff>(type, std::move(fields));
}

Gff::Field GffReader::readField(int idx) {
    _gff.seek(_fieldOffset + 12ll * idx);

    uint32_t type = _gff.readUint32();
    uint32_t labelIndex = _gff.readUint32();
    uint32_t dataOrDataOffset = _gff.readUint32();

    Gff::Field field;
    field.type = static_cast<Gff::FieldType>(type);
    field.label = readLabel(labelIndex);

    switch (field.type) {
    case Gff::FieldType::Byte:
    case Gff::FieldType::Word:
    case Gff::FieldType::Dword:
        field.uintValue = dataOrDataOffset;
        break;
    case Gff::FieldType::Char:
    case Gff::FieldType::Short:
    case Gff::FieldType::Int:
        field.intValue = *reinterpret_cast<int *>(&dataOrDataOffset);
        break;
    case Gff::FieldType::Dword64:
        field.uint64Value = readQWordFieldData(dataOrDataOffset);
        break;
    case Gff::FieldType::Int64: {
        uint64_t tmp = readQWordFieldData(dataOrDataOffset);
        field.int64Value = *reinterpret_cast<int64_t *>(&tmp);
        break;
    }
    case Gff::FieldType::Float:
        field.floatValue = *reinterpret_cast<float *>(&dataOrDataOffset);
        break;
    case Gff::FieldType::Double: {
        uint64_t tmp = readQWordFieldData(dataOrDataOffset);
        field.doubleValue = *reinterpret_cast<double *>(&tmp);
        break;
    }
    case Gff::FieldType::CExoString:
        field.strValue = readStringFieldData(dataOrDataOffset);
        break;
    case Gff::FieldType::ResRef:
        field.strValue = readResRefFieldData(dataOrDataOffset);
        break;
    case Gff::FieldType::CExoLocString: {
        LocString locString(readCExoLocStringFieldData(dataOrDataOffset));
        field.intValue = locString.strRef;
        field.strValue = locString.subString;
        break;
    }
    case Gff::FieldType::Void:
        field.data = readByteArrayFieldData(dataOrDataOffset);
        break;
    case Gff::FieldType::Struct:
        field.children.push_back(readStruct(dataOrDataOffset));
        break;
    case Gff::FieldType::List: {
        std::vector<uint32_t> list(readList(dataOrDataOffset));
        for (auto &item : list) {
            field.children.push_back(readStruct(item));
        }
        break;
    }
    case Gff::FieldType::Orientation: {
        ByteArray data(readByteArrayFieldData(dataOrDataOffset, 4 * sizeof(float)));
        auto floatData = reinterpret_cast<float *>(&data[0]);
        field.quatValue = glm::quat(floatData[0], floatData[1], floatData[2], floatData[3]);
        break;
    }
    case Gff::FieldType::Vector: {
        ByteArray data(readByteArrayFieldData(dataOrDataOffset, 3 * sizeof(float)));
        field.vecValue = glm::make_vec3(reinterpret_cast<float *>(&data[0]));
        break;
    }
    case Gff::FieldType::StrRef:
        field.intValue = readStrRefFieldData(dataOrDataOffset);
        break;
    default:
        throw FormatException("Unsupported field type: " + std::to_string(type));
    }

    return std::move(field);
}

std::string GffReader::readLabel(int idx) {
    uint32_t off = _labelOffset + 16 * idx;
    return _gff.readCStringAt(off, 16);
}

std::vector<uint32_t> GffReader::readFieldIndices(uint32_t off, int count) {
    return _gff.readUint32ArrayAt(_fieldIndicesOffset + off, count);
}

uint64_t GffReader::readQWordFieldData(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(_fieldDataOffset + off);
    uint64_t val = _gff.readUint64();
    _gff.seek(pos);

    return val;
}

std::string GffReader::readStringFieldData(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(_fieldDataOffset + off);

    uint32_t size = _gff.readUint32();
    std::string s(_gff.readCString(size));
    _gff.seek(pos);

    return std::move(s);
}

std::string GffReader::readResRefFieldData(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(_fieldDataOffset + off);

    uint8_t size = _gff.readByte();
    std::string s(_gff.readCString(size));
    _gff.seek(pos);

    return std::move(s);
}

GffReader::LocString GffReader::readCExoLocStringFieldData(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(_fieldDataOffset + off);

    uint32_t size = _gff.readUint32();
    int32_t ref = _gff.readInt32();
    uint32_t count = _gff.readUint32();

    LocString loc;
    loc.strRef = ref;

    if (count > 0) {
        int32_t type = _gff.readInt32();
        uint32_t ssSize = _gff.readUint32();
        loc.subString = _gff.readCString(ssSize);

        if (count > 1) {
            warn("GFF: more than one substring in CExoLocString, ignoring");
        }
    }

    _gff.seek(pos);

    return std::move(loc);
}

int32_t GffReader::readStrRefFieldData(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(_fieldDataOffset + off);

    uint32_t size = _gff.readUint32();
    int32_t ref = _gff.readInt32();

    _gff.seek(pos);

    return ref;
}

ByteArray GffReader::readByteArrayFieldData(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(_fieldDataOffset + off);

    uint32_t size = _gff.readUint32();
    ByteArray arr(_gff.readBytes(size));
    _gff.seek(pos);

    return std::move(arr);
}

ByteArray GffReader::readByteArrayFieldData(uint32_t off, int size) {
    return _gff.readBytesAt(_fieldDataOffset + off, size);
}

std::vector<uint32_t> GffReader::readList(uint32_t off) {
    size_t pos = _gff.position();
    _gff.seek(static_cast<size_t>(_listIndicesOffset) + off);

    uint32_t count = _gff.readUint32();
    std::vector<uint32_t> arr(_gff.readUint32Array(count));
    _gff.seek(pos);

    return std::move(arr);
}

} // namespace resource

} // namespace reone
