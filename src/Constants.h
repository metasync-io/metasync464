#pragma once
#include <cstdint>
static constexpr uint32_t CYCLE_RATE_MS = 600;
static constexpr uint32_t MAX_ACCEPTS_PER_CYCLE = 10;

static constexpr uint32_t MAX_PACKETS_PER_SECOND = 100;

static constexpr uint32_t EQUIPMENT_SLOT_HEAD = 0;
static constexpr uint32_t EQUIPMENT_SLOT_CAPE = 1;
static constexpr uint32_t EQUIPMENT_SLOT_AMULET = 2;
static constexpr uint32_t EQUIPMENT_SLOT_WEAPON = 3;
static constexpr uint32_t EQUIPMENT_SLOT_CHEST = 4;
static constexpr uint32_t EQUIPMENT_SLOT_SHIELD = 5;
static constexpr uint32_t EQUIPMENT_SLOT_LEGS = 7;
static constexpr uint32_t EQUIPMENT_SLOT_HANDS = 9;
static constexpr uint32_t EQUIPMENT_SLOT_FEET = 10;
static constexpr uint32_t EQUIPMENT_SLOT_RING = 12;
static constexpr uint32_t EQUIPMENT_SLOT_ARROWS = 13;

static constexpr uint32_t APPEARANCE_SLOT_CHEST = 0;
static constexpr uint32_t APPEARANCE_SLOT_ARMS = 1;
static constexpr uint32_t APPEARANCE_SLOT_LEGS = 2;
static constexpr uint32_t APPEARANCE_SLOT_HEAD = 3;
static constexpr uint32_t APPEARANCE_SLOT_HANDS = 4;
static constexpr uint32_t APPEARANCE_SLOT_FEET = 5;
static constexpr uint32_t APPEARANCE_SLOT_BEARD = 6;

static constexpr uint32_t MAX_PLAYERS = 2048;