/// @file   equipmentSlot.cpp
/// @brief  Implements the equipment slot class methods.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#include "equipmentSlot.hpp"

EquipmentSlot::EquipmentSlot() :
    equipmentSlot(None)
{
    // Nothing to do.
}

EquipmentSlot::EquipmentSlot(const unsigned int & _equipmentSlot) :
    equipmentSlot()
{
    if (_equipmentSlot == 1) equipmentSlot = Head;
    else if (_equipmentSlot == 2) equipmentSlot = Torso;
    else if (_equipmentSlot == 4) equipmentSlot = Back;
    else if (_equipmentSlot == 8) equipmentSlot = Legs;
    else if (_equipmentSlot == 16) equipmentSlot = Feet;
    else if (_equipmentSlot == 32) equipmentSlot = RightHand;
    else if (_equipmentSlot == 64) equipmentSlot = LeftHand;
    else equipmentSlot = None;
}

EquipmentSlot::EquipmentSlot(const Enum & _equipmentSlot) :
    equipmentSlot(_equipmentSlot)
{
    // Nothing to do.
}

EquipmentSlot::EquipmentSlot(const std::string & _equipmentSlot) :
    equipmentSlot()
{
    if (_equipmentSlot == "Head") equipmentSlot = Head;
    else if (_equipmentSlot == "Torso") equipmentSlot = Torso;
    else if (_equipmentSlot == "Back") equipmentSlot = Back;
    else if (_equipmentSlot == "Legs") equipmentSlot = Legs;
    else if (_equipmentSlot == "Feet") equipmentSlot = Feet;
    else if (_equipmentSlot == "RightHand") equipmentSlot = RightHand;
    else if (_equipmentSlot == "LeftHand") equipmentSlot = LeftHand;
    else equipmentSlot = None;
}

bool EquipmentSlot::isValid(const unsigned int & _equipmentSlot)
{
    return (_equipmentSlot == 1) || (_equipmentSlot == 2) || (_equipmentSlot == 4) ||
           (_equipmentSlot == 8) || (_equipmentSlot == 16) || (_equipmentSlot == 32) ||
           (_equipmentSlot == 64);
}

bool EquipmentSlot::isValid(const std::string & _equipmentSlot)
{
    if (_equipmentSlot == "Head") return true;
    if(_equipmentSlot == "Torso") return true;
    if(_equipmentSlot == "Back") return true;
    if(_equipmentSlot == "Legs") return true;
    if(_equipmentSlot == "Feet") return true;
    if(_equipmentSlot == "RightHand") return true;
    return (_equipmentSlot == "LeftHand");
}

std::string EquipmentSlot::toString() const
{
    if (equipmentSlot == Head) return "Head";
    if (equipmentSlot == Torso) return "Torso";
    if (equipmentSlot == Back) return "Back";
    if (equipmentSlot == Legs) return "Legs";
    if (equipmentSlot == Feet) return "Feet";
    if (equipmentSlot == RightHand) return "Right Hand";
    if (equipmentSlot == LeftHand) return "Left Hand";
    return "None";
}

unsigned int EquipmentSlot::toUInt() const
{
    return static_cast<unsigned int>(equipmentSlot);
}

bool EquipmentSlot::operator==(const EquipmentSlot & rhs) const
{
    return equipmentSlot == rhs.equipmentSlot;
}

bool EquipmentSlot::operator==(const EquipmentSlot::Enum & rhs) const
{
    return equipmentSlot == rhs;
}

bool EquipmentSlot::operator!=(const EquipmentSlot::Enum & rhs) const
{
    return equipmentSlot != rhs;
}
