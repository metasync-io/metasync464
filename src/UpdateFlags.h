#pragma once

#include <stdint.h>

enum class UpdateFlag : uint32_t
{

	None = 0,

	Chat = 0x1,

	Hit2 = 0x2,

	FaceCoordinate = 0x4,

	FaceEntity = 0x8,

	Animation = 0x20,

	Appearance = 0x40,

	ForcedChat = 0x80,

	Hit = 0x100,

	Graphics = 0x200,

	ResetMovement = 1 << 16,

	NeedsPlacement = 1 << 17,

	// Combat system flags
	CombatTarget = 1 << 18,
	SpecialEnergy = 1 << 19,
	EquipmentChange = 1 << 20,

	All = Appearance | Animation | Graphics | FaceEntity | FaceCoordinate | NeedsPlacement | EquipmentChange

};

inline UpdateFlag operator|(UpdateFlag a, UpdateFlag b) {

	return static_cast<UpdateFlag>(
		static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
	);

}

inline UpdateFlag& operator|=(UpdateFlag& a, UpdateFlag b) {

	a = a | b;

	return a;

}

inline UpdateFlag operator&(UpdateFlag a, UpdateFlag b) {

	return static_cast<UpdateFlag>(
		static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
	);

}

inline bool hasFlag(UpdateFlag flags, UpdateFlag flag) {

	return static_cast<uint32_t>(flags & flag) != 0;

}
