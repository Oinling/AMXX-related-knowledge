/* 本插件由 AMXX-Studio 中文版自动生成*/
/* UTF-8 func by www.DT-Club.net */


#include <amxmodx>
#include <fakemeta>
#include <xs>


#define BEAM_NOISE      	0
#define BEAM_BRIGHTNESS     	255.0
#define BEAM_WIDTH      	7.5

#define   COLOR_I       {255.0, 0.0, 0.0}
#define   COLOR_II      {0.0, 0.0, 255.0}

/* <<< END >>> */

#define SNIPER_WEAPON_CS(%1)   %1 & (1 << CSW_SCOUT | 1 << CSW_AWP | 1 << CSW_G3SG1 | 1 << CSW_SG550)

#define None	0

enum _:Beam_Rendering_Mode
{
	beam_Hide,
	beam_Show
}

enum _:CStrike_Teams(<<= 1)
{
	team_Terrorists = 2,
	team_CTerrorists
}

enum _:Plugin_Data
{
	bit_In_Zoom,	
	_client_beam[32],
	_Sprite_Index
}

new g_plugin_data[Plugin_Data]

#define PLUGIN      "No Lag Laser 不卡的激光"
#define VERSION     "1.0"
#define AUTHOR      "Hui"

Beam_Create(const endIndex)
{
	static iEnt_Class
	new beam
	
	if(iEnt_Class || (iEnt_Class = engfunc(EngFunc_AllocString, "beam")))
	{
		beam = engfunc(EngFunc_CreateNamedEntity, iEnt_Class)
	}

	if(pev_valid(beam))
	{
		
		client_print(0,print_center,"Create Entity")
		
		set_pev(beam, pev_body, BEAM_NOISE)
		set_pev(beam, pev_renderamt, BEAM_BRIGHTNESS)   
		set_pev(beam, pev_flags, pev(beam, pev_flags) | FL_CUSTOMENTITY)
		set_pev(beam, pev_model, "sprites/laserbeam.spr")
		set_pev(beam, pev_modelindex, g_plugin_data[_Sprite_Index])
		set_pev(beam, pev_scale, BEAM_WIDTH)
		
		set_pev(beam, pev_skin, endIndex)
		//set_pev(beam, pev_skin, (endIndex | ((pev(beam, pev_skin) & (0x10000 - 0x2000)) << 0xd)))
		set_pev(beam, pev_rendermode, (pev(beam, pev_rendermode) & 0xF0) | 1 & 0x0F)
		set_pev(beam, pev_sequence, (pev(beam, pev_sequence) & 0x0FFF) | ((0 & 0xF) << 12))
		set_pev(beam, pev_aiment, endIndex)
		set_pev(beam, pev_framerate, 1.0)

		return beam
	}
	
	return None
}

public client_putinserver(id)
{
	if(!g_plugin_data[id])
	{
		client_print(0,print_center,"Create Entity")
		g_plugin_data[id] = Beam_Create(id | 0x1000)
		change_visibility(g_plugin_data[id], beam_Hide)
	}
}

public id_post_think(id)
{
	if(g_plugin_data[bit_In_Zoom] & 1 << (id - 1))
	{
		static 
		
		Float:flStartPos[3],
		Float:aim_start[3], 
		Float:aim_view_ofs[3],
		Float:aim_dest[3],
		Float:flOrigin[3],
		Float:flEndPos[3],
		Float:flMins[3],
		Float:flMaxs[3],
		iEntity,
		iBeamEntity
		
		iBeamEntity = g_plugin_data[id]
		
		if(pev_valid(iBeamEntity))
		{
			iEntity = pev(iBeamEntity, pev_skin) & 0xFFF
		
			switch(pev_valid(iEntity))
			{
				case None:
				{
					return
				}
				
				default:
				{
					pev(iEntity, pev_origin, flEndPos)
				}	
			}
			
			pev(id, pev_origin, aim_start)
			pev(id, pev_view_ofs, aim_view_ofs)
	
			xs_vec_add(aim_start, aim_view_ofs, aim_start)

			pev(id, pev_v_angle, aim_dest)
			engfunc(EngFunc_MakeVectors, aim_dest)
	
			global_get(glb_v_forward, aim_dest)
	
			xs_vec_mul_scalar(aim_dest, 9999.0, aim_dest)
			xs_vec_add(aim_start, aim_dest, aim_dest)

			engfunc(EngFunc_TraceLine, aim_start, aim_dest, 0, id, 0)
			get_tr2(0, TR_vecEndPos, flStartPos)
	
			pev(iBeamEntity, pev_origin, flOrigin)
	
			flMins[0] = floatmin(flStartPos[0], flEndPos[0])
			flMins[1] = floatmin(flStartPos[1], flEndPos[1])
			flMins[2] = floatmin(flStartPos[2], flEndPos[2])
	
			flMaxs[0] = floatmax(flStartPos[0], flEndPos[0])
			flMaxs[1] = floatmax(flStartPos[1], flEndPos[1])
			flMaxs[2] = floatmax(flStartPos[2], flEndPos[2])
	
			xs_vec_sub(flMins, flOrigin, flMins)
			xs_vec_sub(flMaxs, flOrigin, flMaxs)
	
			set_pev(iBeamEntity, pev_mins, flMins)
			set_pev(iBeamEntity, pev_maxs, flMaxs)
		
			xs_vec_sub(flMaxs, flMins, flMaxs)
		
		
			client_print(0,print_chat,"思考思考 实体大小：%.2f",flMaxs)
			
			set_pev(iBeamEntity, pev_size, flMaxs)
			engfunc(EngFunc_SetOrigin, iBeamEntity, flStartPos)
		}
	}
}

public plugin_precache()
{
	g_plugin_data[_Sprite_Index] = precache_model("sprites/laserbeam.spr")
}

public client_disconnect(id)
{
	if(pev_valid(g_plugin_data[id]))
	{
		engfunc(EngFunc_RemoveEntity, g_plugin_data[id])
	}
	
	g_plugin_data[id] = None
	g_plugin_data[bit_In_Zoom] &= ~(1 << (id - 1))
}

change_visibility(beam, mode)
{
	if(pev_valid(beam))
	{
		new effects = pev(beam, pev_effects)
	
		switch(mode)
		{
			case beam_Hide:
			{
				effects |= EF_NODRAW
			}
			
			case beam_Show:
			{
				effects &= ~EF_NODRAW
			}
		}
	
		set_pev(beam, pev_effects, effects)
	}
}

public client_dead()
{
	new id = read_data(2)

	g_plugin_data[bit_In_Zoom] &= ~(1 << (id - 1))
	change_visibility(g_plugin_data[id], beam_Hide)
}

public zoom_event(id)
{
	
	
	new beam = g_plugin_data[id]

	
	if(pev_valid(beam))
	{
		client_print(id,print_chat,"Test")
			
		g_plugin_data[bit_In_Zoom] |= 1 << (id - 1)  
		set_pev(beam, pev_rendercolor, (1 << _:get_user_team(id)) & team_Terrorists ? COLOR_I : COLOR_II)      
	}
		
	change_visibility(beam, beam_Show)
}

public plugin_init()
{
	register_plugin(PLUGIN, VERSION, AUTHOR);
	
	register_event("DeathMsg", "client_dead", "a")
	register_event("CurWeapon", "zoom_event", "be", "1=1")
	
	//register_forward(FM_AddToFullPack, "hide_beam", 1)
	register_forward(FM_PlayerPostThink, "id_post_think", 1)
}

public hide_beam(entState, e, ent, host, iHostFlags, iPlayer, pSet)
{
	if(g_plugin_data[bit_In_Zoom] & 1 << (host - 1))
	{
		if(ent ^ g_plugin_data[host]) return	
		
		//set_es(entState, ES_RenderAmt, beam_Hide)
	}
}
