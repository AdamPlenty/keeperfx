#include "globals.h"
#include "bflib_basics.h"
#include "map_data.h"
#include "player_data.h"
#include "dungeon_data.h"
#include "game_legacy.h"
#include "roomspace.h"
#include "engine_render.h"
#include "slab_data.h"
#include "power_hand.h"
#include "frontend.h"
#include "thing_physics.h"
#include "thing_navigate.h"
#include "packets.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/

/******************************************************************************/
#ifdef __cplusplus
}
#endif
/******************************************************************************/
void tag_cursor_blocks_dig(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y, TbBool full_slab)
{
    SYNCDBG(7,"Starting for player %d at subtile (%d,%d)",(int)plyr_idx,(int)stl_x,(int)stl_y);
    //_DK_tag_cursor_blocks_dig(plyr_idx, stl_x, stl_y, full_slab);
    struct PlayerInfo* player = get_player(plyr_idx);
    struct DungeonAdd* dungeonadd = get_dungeonadd(plyr_idx);
    struct Packet* pckt = get_packet_direct(player->packet_num);
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    TbBool allowed = false;
    if (render_roomspace.slab_count > 0 && full_slab) // if roomspace is not empty
    {
        allowed = true;
    }
    else if (subtile_is_diggable_for_player(plyr_idx, stl_x, stl_y, false)) // else if not using roomspace, is current slab diggable
    {
        allowed = true;
    }
    else if ((dungeonadd->one_click_lock_cursor) && ((pckt->control_flags & PCtr_LBtnHeld) != 0))
    {
        allowed = true;
    }
    unsigned char line_color = allowed;
    if (render_roomspace.untag_mode && allowed)
    {
        line_color = SLC_YELLOW;
    }
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && (game.small_map_state != 2) && ((pckt->control_flags & PCtr_MapCoordsValid) != 0))
    {
        map_volume_box.visible = 1;
        map_volume_box.color = line_color;
        map_volume_box.beg_x = (!full_slab ? (subtile_coord(stl_x, 0)) : subtile_coord(((render_roomspace.centreX - calc_distance_from_roomspace_centre(render_roomspace.width,0)) * STL_PER_SLB), 0));
        map_volume_box.beg_y = (!full_slab ? (subtile_coord(stl_y, 0)) : subtile_coord(((render_roomspace.centreY - calc_distance_from_roomspace_centre(render_roomspace.height,0)) * STL_PER_SLB), 0));
        map_volume_box.end_x = (!full_slab ? (subtile_coord(stl_x + 1, 0)) : subtile_coord((((render_roomspace.centreX + calc_distance_from_roomspace_centre(render_roomspace.width,(render_roomspace.width % 2 == 0))) + 1) * STL_PER_SLB), 0));
        map_volume_box.end_y = (!full_slab ? (subtile_coord(stl_y + 1, 0)) : subtile_coord((((render_roomspace.centreY + calc_distance_from_roomspace_centre(render_roomspace.height,(render_roomspace.height % 2 == 0))) + 1) * STL_PER_SLB), 0));
        map_volume_box.floor_height_z = floor_height_z;
        render_roomspace.is_roomspace_a_single_subtile = !full_slab;
    }
}

void tag_cursor_blocks_thing_in_hand(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y, TbBool is_special_digger, TbBool full_slab)
{
  SYNCDBG(7,"Starting");
  // _DK_tag_cursor_blocks_thing_in_hand(plyr_idx, stl_x, stl_y, is_special_digger, full_slab);
  MapSlabCoord slb_x = subtile_slab_fast(stl_x);
  MapSlabCoord slb_y = subtile_slab_fast(stl_y);  
  if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && (game.small_map_state != 2) )
    {
        map_volume_box.visible = true;
        map_volume_box.color = can_drop_thing_here(stl_x, stl_y, plyr_idx, is_special_digger);
        if (full_slab)
        {
            map_volume_box.beg_x = subtile_coord(slab_subtile(slb_x, 0), 0);
            map_volume_box.beg_y = subtile_coord(slab_subtile(slb_y, 0), 0);
            map_volume_box.end_x = subtile_coord(slab_subtile(slb_x, 0) + STL_PER_SLB, 0);
            map_volume_box.end_y = subtile_coord(slab_subtile(slb_y, 0) + STL_PER_SLB, 0);
        }
        else
        {
            map_volume_box.beg_x = subtile_coord(stl_x, 0);
            map_volume_box.beg_y = subtile_coord(stl_y, 0);
            map_volume_box.end_x = subtile_coord(stl_x + 1, 0);
            map_volume_box.end_y = subtile_coord(stl_y + 1, 0); 
        }
        map_volume_box.floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    }
}

TbBool tag_cursor_blocks_sell_area(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y, TbBool full_slab)
{
    SYNCDBG(7,"Starting");
    // _DK_tag_cursor_blocks_sell_area(plyr_idx, stl_x, stl_y, full_slab);
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    struct SlabMap *slb;
    slb = get_slabmap_block(slb_x, slb_y);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    TbBool allowed = false;
    if (render_roomspace.slab_count > 0 && full_slab)
    {
        allowed = true; // roomspace selling support is basic, this makes roomspace selling work over any slabtype
    }
    else if (floor_height_z == 1)
    {
        if ( ( ((subtile_is_sellable_room(plyr_idx, stl_x, stl_y)) || ( (slabmap_owner(slb) == plyr_idx) && ( (slab_is_door(slb_x, slb_y))
            || ((!full_slab) ? (subtile_has_trap_on(stl_x, stl_y)) : (slab_has_trap_on(slb_x, slb_y))) ) ) ) )
            && ( slb->kind != SlbT_ENTRANCE && slb->kind != SlbT_DUNGHEART ) )
        {
            allowed = true;
        }
    }
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && game.small_map_state != 2 )
    {
        map_volume_box.visible = 1;
        map_volume_box.color = allowed;
        map_volume_box.beg_x = (!full_slab ? (subtile_coord(stl_x, 0)) : subtile_coord(((render_roomspace.centreX - calc_distance_from_roomspace_centre(render_roomspace.width,0)) * STL_PER_SLB), 0));
        map_volume_box.beg_y = (!full_slab ? (subtile_coord(stl_y, 0)) : subtile_coord(((render_roomspace.centreY - calc_distance_from_roomspace_centre(render_roomspace.height,0)) * STL_PER_SLB), 0));
        map_volume_box.end_x = (!full_slab ? (subtile_coord(stl_x + 1, 0)) : subtile_coord((((render_roomspace.centreX + calc_distance_from_roomspace_centre(render_roomspace.width,(render_roomspace.width % 2 == 0))) + 1) * STL_PER_SLB), 0));
        map_volume_box.end_y = (!full_slab ? (subtile_coord(stl_y + 1, 0)) : subtile_coord((((render_roomspace.centreY + calc_distance_from_roomspace_centre(render_roomspace.height,(render_roomspace.height % 2 == 0))) + 1) * STL_PER_SLB), 0));
        map_volume_box.floor_height_z = floor_height_z;
        render_roomspace.is_roomspace_a_single_subtile = !full_slab;
    }
    return allowed;
}

TbBool tag_cursor_blocks_place_door(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    SYNCDBG(7,"Starting");
    // return _DK_tag_cursor_blocks_place_door(a1, a2, a3);
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    struct SlabMap *slb;
    slb = get_slabmap_block(slb_x, slb_y);
    TbBool allowed = false;
    char Orientation;
    TbBool Check = false;
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    if (floor_height_z == 1)
    {
        Orientation = find_door_angle(stl_x, stl_y, plyr_idx);
        if (gameadd.place_traps_on_subtiles)
        {
            switch(Orientation)
            {
                case 0:
                {
                    Check = (!slab_middle_row_has_trap_on(slb_x, slb_y) );
                    break;
                }
                case 1:
                {
                    Check = (!slab_middle_column_has_trap_on(slb_x, slb_y) );
                    break;
                }
            }
        }
        if ( ( (slabmap_owner(slb) == plyr_idx) && (slb->kind == SlbT_CLAIMED) )
            && (Orientation != -1)
            && ( ( (gameadd.place_traps_on_subtiles) ? (Check) : (!slab_has_trap_on(slb_x, slb_y) ) ) && (!slab_has_door_thing_on(slb_x, slb_y) ) )
            )
        {
            allowed = true;
        }
    }
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && game.small_map_state != 2 )
    {
        map_volume_box.visible = 1;
        map_volume_box.beg_x = subtile_coord(slab_subtile(slb_x, 0), 0);
        map_volume_box.beg_y = subtile_coord(slab_subtile(slb_y, 0), 0);
        map_volume_box.end_x = subtile_coord(slab_subtile(slb_x, STL_PER_SLB), 0);
        map_volume_box.end_y = subtile_coord(slab_subtile(slb_y, STL_PER_SLB), 0);
        map_volume_box.floor_height_z = floor_height_z;
        map_volume_box.color = allowed;
        render_roomspace.is_roomspace_a_box = true;
        render_roomspace.render_roomspace_as_box = true;
        render_roomspace.is_roomspace_a_single_subtile = false;
    }
    return allowed;
}

TbBool tag_cursor_blocks_place_room(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y, TbBool full_slab)
{
    SYNCDBG(7,"Starting");
    //return _DK_tag_cursor_blocks_place_room(plyr_idx, stl_x, stl_y, full_slab);
    MapSlabCoord slb_x;
    MapSlabCoord slb_y;
    struct SlabMap* slb;
    slb_x = subtile_slab_fast(stl_x);
    slb_y = subtile_slab_fast(stl_y);
    struct PlayerInfo *player;
    player = get_player(plyr_idx);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    TbBool allowed = false;
    if(can_build_roomspace(plyr_idx, player->chosen_room_kind, render_roomspace) > 0)
    {
        allowed = true;
    }
    else
    {
        slb = get_slabmap_block(slb_x, slb_y);
        SYNCDBG(7,"Cannot build %s on %s slabs centered at (%d,%d)", room_code_name(player->chosen_room_kind), slab_code_name(slb->kind), (int)slb_x, (int)slb_y);
    }
    
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && (game.small_map_state != 2))
    {
        map_volume_box.visible = 1;
        map_volume_box.color = allowed;
        map_volume_box.beg_x = (!full_slab ? (subtile_coord(stl_x, 0)) : subtile_coord(((render_roomspace.centreX - calc_distance_from_roomspace_centre(render_roomspace.width,0)) * STL_PER_SLB), 0));
        map_volume_box.beg_y = (!full_slab ? (subtile_coord(stl_y, 0)) : subtile_coord(((render_roomspace.centreY - calc_distance_from_roomspace_centre(render_roomspace.height,0)) * STL_PER_SLB), 0));
        map_volume_box.end_x = (!full_slab ? (subtile_coord(stl_x + 1, 0)) : subtile_coord((((render_roomspace.centreX + calc_distance_from_roomspace_centre(render_roomspace.width,(render_roomspace.width % 2 == 0))) + 1) * STL_PER_SLB), 0));
        map_volume_box.end_y = (!full_slab ? (subtile_coord(stl_y + 1, 0)) : subtile_coord((((render_roomspace.centreY + calc_distance_from_roomspace_centre(render_roomspace.height,(render_roomspace.height % 2 == 0))) + 1) * STL_PER_SLB), 0));
        map_volume_box.floor_height_z = floor_height_z;
        render_roomspace.is_roomspace_a_single_subtile = !full_slab;
    }
    return allowed;
}

void tag_cursor_blocks_place_terrain(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    SYNCDBG(7,"Starting");
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && game.small_map_state != 2 )
    {
        map_volume_box.visible = true;
        map_volume_box.beg_x = subtile_coord(slab_subtile(slb_x, 0), 0);
        map_volume_box.beg_y = subtile_coord(slab_subtile(slb_y, 0), 0);
        map_volume_box.end_x = subtile_coord(slab_subtile(slb_x, STL_PER_SLB), 0);
        map_volume_box.end_y = subtile_coord(slab_subtile(slb_y, STL_PER_SLB), 0);
        map_volume_box.floor_height_z = floor_height_z;
        map_volume_box.color = SLC_GREEN;
    }
}

TbBool tag_cursor_blocks_place_thing(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    SYNCDBG(7,"Starting");
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    long height = get_floor_height(stl_x, stl_y);
    unsigned char colour;
    if (map_is_solid_at_height(stl_x, stl_y, height, height))
    {
        colour = SLC_RED;
    }
    else if (map_pos_is_lava(stl_x, stl_y))
    {
        colour = SLC_YELLOW;
    }
    else
    {
        colour = SLC_GREEN;
    }
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && game.small_map_state != 2 )
    {
        map_volume_box.visible = true;
        map_volume_box.beg_x = subtile_coord(stl_x, 0);
        map_volume_box.beg_y = subtile_coord(stl_y, 0);
        map_volume_box.end_x = subtile_coord(stl_x + 1, 0);
        map_volume_box.end_y = subtile_coord(stl_y + 1, 0);
        map_volume_box.floor_height_z = floor_height_z;
        map_volume_box.color = colour;
        render_roomspace.is_roomspace_a_single_subtile = true;
    }
    return (colour != SLC_RED);
}

TbBool tag_cursor_blocks_order_creature(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y, struct Thing* creatng)
{
    SYNCDBG(7,"Starting");
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    unsigned char colour;
    struct Coord3d pos;
    pos.x.stl.num = stl_x;
    pos.y.stl.num = stl_y;
    pos.z.val = get_floor_height(stl_x, stl_y);
    if (!creature_can_navigate_to(creatng, &pos, NavRtF_Default))
    {
        colour = SLC_RED;
    }
    else
    {
        colour = SLC_GREEN;
    }
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && game.small_map_state != 2 )
    {
        map_volume_box.visible = true;
        map_volume_box.beg_x = subtile_coord(stl_x, 0);
        map_volume_box.beg_y = subtile_coord(stl_y, 0);
        map_volume_box.end_x = subtile_coord(stl_x + 1, 0);
        map_volume_box.end_y = subtile_coord(stl_y + 1, 0);
        map_volume_box.floor_height_z = floor_height_z;
        map_volume_box.color = colour;
        render_roomspace.is_roomspace_a_single_subtile = true;
    }
    return (colour != SLC_RED);
}

TbBool tag_cursor_blocks_steal_slab(PlayerNumber plyr_idx, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    SYNCDBG(7,"Starting");
    MapSlabCoord slb_x = subtile_slab_fast(stl_x);
    MapSlabCoord slb_y = subtile_slab_fast(stl_y);
    struct SlabMap* slb = get_slabmap_block(slb_x, slb_y);
    struct SlabAttr* slbattr = get_slab_attrs(slb);
    int floor_height_z = floor_height_for_volume_box(plyr_idx, slb_x, slb_y);
    unsigned char colour;
    struct DungeonAdd* dungeonadd = get_dungeonadd(plyr_idx);
    if ( ( ( ((slbattr->category == SlbAtCtg_FortifiedGround) || (slbattr->category == SlbAtCtg_FortifiedWall) ) && (slabmap_owner(slb) != dungeonadd->chosen_player) ) )
        || ( (slbattr->category == SlbAtCtg_FriableDirt) || ( (slbattr->category == SlbAtCtg_Unclaimed) && (slbattr->is_safe_land) && (!slab_is_liquid(slb_x, slb_y) ) ) ) )
    {
        colour = SLC_GREEN;
    }
    else
    {
        colour = SLC_RED;
    }
    if ((is_my_player_number(plyr_idx)) && (!game_is_busy_doing_gui_for_player(plyr_idx)) && game.small_map_state != 2 )
    {
        map_volume_box.visible = true;
        map_volume_box.beg_x = subtile_coord(slab_subtile(slb_x, 0), 0);
        map_volume_box.beg_y = subtile_coord(slab_subtile(slb_y, 0), 0);
        map_volume_box.end_x = subtile_coord(slab_subtile(slb_x, STL_PER_SLB), 0);
        map_volume_box.end_y = subtile_coord(slab_subtile(slb_y, STL_PER_SLB), 0);
        map_volume_box.floor_height_z = floor_height_z;
        map_volume_box.color = colour;
    }
    return (colour != SLC_RED);
}
/******************************************************************************/