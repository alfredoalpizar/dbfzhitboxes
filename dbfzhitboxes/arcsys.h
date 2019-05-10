#pragma once
#include <stdint.h>

#define USENEWPATCHOFFSETS 1

struct asw_hithurtbox
{
	int type;
	float x, y, w, h;
};

enum asw_entity_status
{
	knocked_down = 0x4,
	dragon_rush = 0x8,
};

class asw_entity
{
public:
	int angle() const
	{
		return *(int*)((char*)(this) + 0x3AC);
	}

	float angle_rads() const
	{
		return (float)(angle()) / 1000.F * 3.1415926F / 180.F;
	}

	int scale_x() const
	{
#if USENEWPATCHOFFSETS
		return *(int*)((char*)(this) + 0x3BC);
#else
		return *(int*)((char*)(this) + 0x3B8);
#endif
	}

	int scale_y() const
	{
#if USENEWPATCHOFFSETS
		return *(int*)((char*)(this) + 0x3C0);
#else
		return *(int*)((char*)(this) + 0x3BC);
#endif
	}

	int direction() const
	{
		return *(int*)((char*)(this) + 0x39C);
	}

	bool is_counterhit() const
	{
		return (*(int*)((char*)(this) + 0x388) & 256) != 0;
	}

	bool is_invuln() const
	{
		return (*(int*)((char*)(this) + 0xD28) & 2) != 0;
	}

	int hithurtbox_count() const
	{
		// Seems Valid
		return *(int*)((char*)(this) + 0x104) + *(int*)((char*)(this) + 0x108);
	}

	asw_hithurtbox *hithurtbox_data() const
	{
		return *(asw_hithurtbox**)((char*)(this) + 0x78);
	}

	asw_entity *owner() const
	{
		return *(asw_entity**)((char*)(this) + 0x258);
	}

	asw_entity *attached() const
	{
		return *(asw_entity**)((char*)(this) + 0x2C0);
	}

	int pushbox_front_offset() const
	{
		int front_offset = *(int*)((char*)(this) + 0x524);
		if (front_offset != 0)
		{
			int tempInt = front_offset;
		}
		return front_offset;
	}

	uint8_t status_flags() const
	{
		return *(uint8_t*)((char*)(this) + 0x391);
	}

	bool is_knocked_down() const
	{
		return (status_flags() & asw_entity_status::knocked_down) != 0;
	}

	bool can_air_jump() const
	{
		return (*(int*)((char*)(this) + 0x3834)) > 0;
	}

	int prev_pos_y() const
	{
		return *(int*)((char*)(this) + 0x4DC);
	}

	bool can_ground_jump() const
	{
		// Need to have been on the ground for at least a frame
		return get_pos_y() == 0 && prev_pos_y() == 0;
	}

	bool can_any_jump() const
	{
		return can_ground_jump() || can_air_jump();
	}

	int get_pos_x() const;
	int get_pos_y() const;
	int pushbox_width() const;
	int pushbox_top() const;
	int pushbox_bottom() const;
	bool is_active() const;
};

class asw_engine
{
public:
	static constexpr float coord_scale = .43F;

	int entity_count() const
	{
#if USENEWPATCHOFFSETS
		return *(int*)((char*)(this) + 0x4C8);
#else
		return *(int*)((char*)(this) + 0x590);
#endif
	}

	asw_entity **entity_list() const
	{
#if USENEWPATCHOFFSETS
		return (asw_entity**)((char*)(this) + 0x1198);
#else
		return (asw_entity**)((char*)(this) + 0x1260);
#endif
	}

	int ComboDamage() const
	{
		return *(int*)((char*)(this) + 0xC0);
	}

	int LastComboDamage() const
	{
		return *(int*)((char*)(this) + 0x6249D4); // Needs more testing
	}

	int HitCount() const
	{
		return *(int*)((char*)(this) + 0x188);
	}

	bool in_combo() const
	{
		return HitCount() > 0;
	}

	static asw_engine *get();
};