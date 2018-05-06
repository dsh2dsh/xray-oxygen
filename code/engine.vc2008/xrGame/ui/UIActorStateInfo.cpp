////////////////////////////////////////////////////////////////////////////
//	Module 		: UIActorStateInfo.cpp
//	Created 	: 15.02.2008
//	Author		: Evgeniy Sokolov
//	Description : UI actor state window class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIActorStateInfo.h"
#include "UIProgressBar.h"
#include "UIProgressShape.h"
#include "UIScrollView.h"
#include "UIFrameWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "object_broker.h"

#include "UIHelper.h"
#include "ui_arrow.h"
#include "UIHudStatesWnd.h"

#include "../Level.h"
#include "../location_manager.h"
#include "../player_hud.h"
#include "UIMainIngameWnd.h"
#include "../UIGameCustom.h"

#include "../Actor.h"
#include "../ActorCondition.h"
#include "../EntityCondition.h"
#include "../CustomOutfit.h"
#include "../ActorHelmet.h"
#include "../Inventory.h"
#include "../Artefact.h"
#include "../string_table.h"
ui_actor_state_wnd::ui_actor_state_wnd()
{
}

ui_actor_state_wnd::~ui_actor_state_wnd()
{
	delete_data( m_hint_wnd );
}

void ui_actor_state_wnd::init_from_xml( CUIXml& xml, LPCSTR path )
{
	XML_NODE* stored_root = xml.GetLocalRoot();
	CUIXmlInit::InitWindow( xml, path, 0, this );

	XML_NODE* new_root = xml.NavigateToNode( path, 0 );
	xml.SetLocalRoot( new_root );

	m_hint_wnd = UIHelper::CreateHint( xml, "hint_wnd" );

	for ( int i = 0; i < stt_count; ++i )
	{
		m_state[i] = xr_new<ui_actor_state_item>();
		m_state[i]->SetAutoDelete( true );
		AttachChild( m_state[i] );
		m_state[i]->set_hint_wnd( m_hint_wnd );
	}
	m_state[stt_health]->init_from_xml( xml, "health_state");
	m_state[stt_bleeding]->init_from_xml( xml, "bleeding_state");
	m_state[stt_blood]->init_from_xml( xml, "blood");
	m_state[stt_radiation]->init_from_xml( xml, "radiation_state");
	m_state[stt_rad]->init_from_xml( xml, "rad");
	m_state[stt_fire]->init_from_xml( xml, "fire_sensor");
	m_state[stt_radia]->init_from_xml( xml, "radia_sensor");
	m_state[stt_acid ]->init_from_xml( xml, "acid_sensor");
	m_state[stt_psi]->init_from_xml( xml, "psi_sensor");
	m_state[stt_wound]->init_from_xml( xml, "wound_sensor");
	m_state[stt_fire_wound]->init_from_xml( xml, "fire_wound_sensor");
	m_state[stt_shock]->init_from_xml( xml, "shock_sensor");
	m_state[stt_power]->init_from_xml( xml, "power_sensor");

	xml.SetLocalRoot( stored_root );
}

void ui_actor_state_wnd::UpdateActorInfo( CInventoryOwner* owner )
{
	CActor* actor = smart_cast<CActor*>( owner );
	if ( !actor )
	{
		return;
	}

	float value = 0.0f;
	float text = 0.0f;
	float text_rad = 0.0f;
	float value_rad = 0.0f;
	float text_blood = 0.0f;
	float value_blood = 0.0f;
	
	value = actor->conditions().GetHealth();
	text = actor->conditions().GetHealth();
	value = floor(value * 55) / 55; // number of sticks in progress bar
	text = floor(value * 101); // if 100 max health is 99(maybe can fixed in game configs)
	m_state[stt_health]->set_progress(value);
	m_state[stt_health]->set_text(text);

	// show bleeding icon
	value_blood = actor->conditions().BleedingSpeed();
	text_blood = actor->conditions().BleedingSpeed();
	text_blood = floor(value_blood * 100);
	m_state[stt_blood]->set_text(text_blood);	
	m_state[stt_blood]->set_progress(value_blood);

	value = actor->conditions().BleedingSpeed();					
	m_state[stt_bleeding]->show_static(false, 1);
	m_state[stt_bleeding]->show_static(false, 2);
	m_state[stt_bleeding]->show_static(false, 3);
	if(!fis_zero(value, EPS))
	{
		if(value<0.35f)
			m_state[stt_bleeding]->show_static(true, 1);
		else if(value<0.7f)
			m_state[stt_bleeding]->show_static(true, 2);
		else 
			m_state[stt_bleeding]->show_static(true, 3);
	}

	// show radiation icon
	value_rad = actor->conditions().GetRadiation();
	text_rad = actor->conditions().GetRadiation();
	text_rad = floor(value_rad * 100);
	m_state[stt_rad]->set_text(text_rad);	
	m_state[stt_rad]->set_progress(value_rad);

	value = actor->conditions().GetRadiation();
	m_state[stt_radiation]->show_static(false, 1);
	m_state[stt_radiation]->show_static(false, 2);
	m_state[stt_radiation]->show_static(false, 3);
	if(!fis_zero(value, EPS))
	{
		if(value<0.35f)
			m_state[stt_radiation]->show_static(true, 1);
		else if(value<0.7f)
			m_state[stt_radiation]->show_static(true, 2);
		else 
			m_state[stt_radiation]->show_static(true, 3);
	}


	CCustomOutfit* outfit = actor->GetOutfit();
	PIItem itm = actor->inventory().ItemFromSlot(HELMET_SLOT);
	CHelmet* helmet = smart_cast<CHelmet*>(itm);

	m_state[stt_fire]->set_progress(0.0f);
	m_state[stt_radia]->set_progress(0.0f);
	m_state[stt_acid]->set_progress(0.0f);
	m_state[stt_psi]->set_progress(0.0f);
	m_state[stt_wound]->set_progress(0.0f);
	m_state[stt_fire_wound]->set_progress(0.0f);
	m_state[stt_shock]->set_progress(0.0f);
	m_state[stt_power]->set_progress(0.0f);
	m_state[stt_power]->set_progress(0.0f);

	float burn_value = 0.0f;
	float burn_text = 0.0f;
	float radi_value = 0.0f;
	float radi_text = 0.0f;
	float cmbn_value = 0.0f;
	float cmbn_text = 0.0f;
	float tele_value = 0.0f;
	float tele_text = 0.0f;
	float woun_value = 0.0f;
	float woun_text = 0.0f;
	float shoc_value = 0.0f;
	float shoc_text = 0.0f;
	float fwou_value = 0.0f;
	float fwou_text = 0.0f;
	float rspeed_value = 0.0f;
	float rspeed_text = 0.0f;
	
	CEntityCondition::BOOSTER_MAP cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
	CEntityCondition::BOOSTER_MAP::const_iterator it;
	it = cur_booster_influences.find(eBoostRadiationProtection);
	if(it!=cur_booster_influences.end())
		radi_value += it->second.fBoostValue;

	it = cur_booster_influences.find(eBoostChemicalBurnProtection);
	if(it!=cur_booster_influences.end())
		cmbn_value += it->second.fBoostValue;

	it = cur_booster_influences.find(eBoostTelepaticProtection);
	if(it!=cur_booster_influences.end())
		tele_value += it->second.fBoostValue;

	if(outfit)
	{
		burn_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeBurn);
		burn_text += outfit->GetDefHitTypeProtection(ALife::eHitTypeBurn);
		radi_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
		radi_text += outfit->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
		cmbn_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
		cmbn_text += outfit->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
		tele_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
		tele_text += outfit->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
		woun_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeWound);
		woun_text += outfit->GetDefHitTypeProtection(ALife::eHitTypeWound);
		shoc_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeShock);
		shoc_text += outfit->GetDefHitTypeProtection(ALife::eHitTypeShock);
		
		IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
		VERIFY(ikv);
		u16 spine_bone = ikv->LL_BoneID("bip01_spine");
		fwou_value += outfit->GetBoneArmor(spine_bone)*outfit->GetCondition();					
		fwou_text += outfit->GetBoneArmor(spine_bone)*outfit->GetCondition();
		if(!outfit->bIsHelmetAvaliable)
		{
			u16 spine_bone = ikv->LL_BoneID("bip01_head");
			fwou_value += outfit->GetBoneArmor(spine_bone)*outfit->GetCondition();
			fwou_text += outfit->GetBoneArmor(spine_bone)*outfit->GetCondition();
			}
	}
	if(helmet)
	{
		burn_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeBurn);
		burn_text += helmet->GetDefHitTypeProtection(ALife::eHitTypeBurn);
		radi_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
		radi_text += helmet->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
		cmbn_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
		cmbn_text += helmet->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
		tele_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
		tele_text += helmet->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
		woun_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeWound);
		woun_text += helmet->GetDefHitTypeProtection(ALife::eHitTypeWound);
		shoc_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeShock);
		shoc_text += helmet->GetDefHitTypeProtection(ALife::eHitTypeShock);
		
		IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
		VERIFY(ikv);
		u16 spine_bone = ikv->LL_BoneID("bip01_head");
		fwou_value += helmet->GetBoneArmor(spine_bone)*helmet->GetCondition();
		fwou_text += helmet->GetBoneArmor(spine_bone)*helmet->GetCondition();
		}
	
//fire burn protection progress bar
	{
		burn_value += actor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeBurn);
		float max_power = actor->conditions().GetZoneMaxPower(ALife::eHitTypeBurn);
		burn_value = floor(burn_value / max_power * 31) / 31; // number of sticks in progress bar
		burn_text = floor(burn_text / max_power * 100);
		m_state[stt_fire]->set_progress(burn_value);//0..1
		m_state[stt_fire]->set_text(burn_text);
	}
//radiation protection progress bar
	{
		radi_value += actor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeRadiation);
		float max_power = actor->conditions().GetZoneMaxPower(ALife::eHitTypeRadiation);
		radi_value = floor(radi_value / max_power * 31) / 31; // number of sticks in progress bar
		radi_text = floor(radi_text / max_power * 100);
		m_state[stt_radia]->set_progress(radi_value);//0..1
		m_state[stt_radia]->set_text(radi_text);
		}
//chemical burn protection progress bar
	{
		cmbn_value += actor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeChemicalBurn);
		float max_power = actor->conditions().GetZoneMaxPower(ALife::eHitTypeChemicalBurn);
		cmbn_value = floor(cmbn_value / max_power * 31) / 31; // number of sticks in progress bar
		cmbn_text = floor(cmbn_text / max_power * 100);
		m_state[stt_acid]->set_progress(cmbn_value);//0..1
		m_state[stt_acid]->set_text(cmbn_text);
		}
//telepatic protection progress bar
	{
		tele_value += actor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeTelepatic);
		float max_power = actor->conditions().GetZoneMaxPower(ALife::eHitTypeTelepatic);
		tele_value = floor(tele_value / max_power * 31) / 31; // number of sticks in progress bar  
		tele_text = floor(tele_text / max_power * 100);
		m_state[stt_psi]->set_progress(tele_value);//0..1
		m_state[stt_psi]->set_text(tele_text);
		}
//wound protection progress bar
	{
		float max_power = actor->conditions().GetMaxWoundProtection();
		woun_value = floor(woun_value / max_power * 31) / 31; // number of sticks in progress bar
		woun_text = floor(woun_text / max_power * 100);
		m_state[stt_wound]->set_progress(woun_value);//0..1
		m_state[stt_wound]->set_text(woun_text);
		}
//shock protection progress bar
	{
		shoc_value += actor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeShock);
		float max_power = actor->conditions().GetZoneMaxPower(ALife::eHitTypeShock);
		shoc_value = floor(shoc_value / max_power * 31) / 31; // number of sticks in progress bar  
		shoc_text = floor(shoc_text / max_power * 100);
		m_state[stt_shock]->set_progress(shoc_value);//0..1
		m_state[stt_shock]->set_text(shoc_text);
		}
//fire wound protection progress bar
	{
		float max_power = actor->conditions().GetMaxFireWoundProtection();
		fwou_value = floor(fwou_value / max_power * 31) / 31; // number of sticks in progress bar
		fwou_text = floor(fwou_text / max_power * 100);
		m_state[stt_fire_wound]->set_progress(fwou_value);
		m_state[stt_fire_wound]->set_text(fwou_text);
		}
//power restore speed progress bar
	{
		rspeed_value += actor->GetRestoreSpeed(ALife::ePowerRestoreSpeed);
		rspeed_text += actor->GetRestoreSpeed(ALife::ePowerRestoreSpeed);
		float max_power =actor->conditions().GetMaxPowerRestoreSpeed();
		rspeed_value = floor(rspeed_value / max_power * 31) / 31; // number of sticks in progress bar  
		rspeed_text = floor(rspeed_text / max_power * 100);
		m_state[stt_power]->set_progress(rspeed_value);//0..1
		m_state[stt_power]->set_text(rspeed_text);
		}
// -----------------------------------------------------------------------------------

	UpdateHitZone();
}

void ui_actor_state_wnd::update_round_states( CActor* actor, ALife::EHitType hit_type, EStateType stt_type )
{
	CCustomOutfit* outfit = actor->GetOutfit();
	PIItem itm = actor->inventory().ItemFromSlot(HELMET_SLOT);
	CHelmet* helmet = smart_cast<CHelmet*>(itm);
	float value = (outfit)? outfit->GetDefHitTypeProtection( hit_type ) : 0.0f;
	value += actor->GetProtection_ArtefactsOnBelt( hit_type );
	value += helmet?helmet->GetDefHitTypeProtection(ALife::eHitTypeShock):0.0f;
	
	float max_power = actor->conditions().GetZoneMaxPower( hit_type );
	value = value / max_power;
	m_state[stt_type]->set_arrow( value );
	m_state[stt_type]->set_text( value );
}

void ui_actor_state_wnd::UpdateHitZone()
{
	CUIHudStatesWnd* wnd = CurrentGameUI()->UIMainIngameWnd->get_hud_states(); //некрасиво слишком
	VERIFY( wnd );
	if ( !wnd )
	{
		return;
	}
	wnd->UpdateZones();
}

void ui_actor_state_wnd::Draw()
{
	inherited::Draw();
	m_hint_wnd->Draw();
}

void ui_actor_state_wnd::Show( bool status )
{
	inherited::Show( status );
	ShowChildren( status );
}

/// =============================================================================================
ui_actor_state_item::ui_actor_state_item()
{
	m_static		= NULL;
	m_static2		= NULL;
	m_static3		= NULL;
	m_progress		= NULL;
	m_sensor		= NULL;
	m_arrow			= NULL;
	m_arrow_shadow	= NULL;
	m_magnitude		= 1.0f;
}

ui_actor_state_item::~ui_actor_state_item()
{
}

void ui_actor_state_item::init_from_xml( CUIXml& xml, LPCSTR path )
{
	CUIXmlInit::InitWindow( xml, path, 0, this);

	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* new_root = xml.NavigateToNode( path, 0 );
	xml.SetLocalRoot( new_root );

	LPCSTR hint_text = xml.Read( "hint_text", 0, "no hint" );
	set_hint_text_ST( hint_text );
	
	set_hint_delay( (u32)xml.ReadAttribInt( "hint_text", 0, "delay" ) );

	if ( xml.NavigateToNode( "state_progress", 0 ) )	
	{
		m_progress = UIHelper::CreateProgressBar( xml, "state_progress", this );
	}
	if ( xml.NavigateToNode( "progress_shape", 0 ) )	
	{
		m_sensor = xr_new<CUIProgressShape>();
		AttachChild( m_sensor );
		m_sensor->SetAutoDelete( true );
		CUIXmlInit::InitProgressShape( xml, "progress_shape", 0, m_sensor );
	}
	if ( xml.NavigateToNode( "arrow", 0 ) )	
	{
		m_arrow = xr_new<UI_Arrow>();
		m_arrow->init_from_xml( xml, "arrow", this );
	}
	if ( xml.NavigateToNode( "arrow_shadow", 0 ) )	
	{
		m_arrow_shadow = xr_new<UI_Arrow>();
		m_arrow_shadow->init_from_xml( xml, "arrow_shadow", this );
	}
	if ( xml.NavigateToNode( "icon", 0 ) )	
	{
		m_static = UIHelper::CreateStatic( xml, "icon", this );
		m_static->TextItemControl()->SetText("");
	}
	if ( xml.NavigateToNode( "icon2", 0 ) )	
	{
		m_static2 = UIHelper::CreateStatic( xml, "icon2", this );
		m_static2->TextItemControl()->SetText("");
	}
	if ( xml.NavigateToNode( "icon3", 0 ) )	
	{
		m_static3 = UIHelper::CreateStatic( xml, "icon3", this );
		m_static3->TextItemControl()->SetText("");
	}
	set_arrow( 0.0f );
	xml.SetLocalRoot( stored_root );
}


void ui_actor_state_item::set_text( float value )
{
	if ( !m_static )
	{
		return;
	}
	int v = (int)( value * m_magnitude + 0.49f );
	clamp( v, 0, 10000 );
	string32 text_res;
	xr_sprintf( text_res, sizeof(text_res), "%d", v );
	m_static->TextItemControl()->SetText( text_res );
}

void ui_actor_state_item::set_progress( float value )
{
	if ( !m_progress )
	{
		return;
	}
	m_progress->SetProgressPos( value );
}

void ui_actor_state_item::set_progress_shape( float value )
{
	if ( !m_sensor )
	{
		return;
	}
	m_sensor->SetPos( value );
}

void ui_actor_state_item::set_arrow( float value )
{
	if ( !m_arrow )
	{
		return;	
	}
	m_arrow->SetNewValue( value );
	if ( !m_arrow_shadow )
	{
		return;
	}
	m_arrow_shadow->SetPos( m_arrow->GetPos() );
}


void ui_actor_state_item::show_static( bool status, u8 number )
{
	switch(number)
	{
	case 1:
		if(!m_static)
			return;
		m_static->Show(status);
		break;
	case 2:
		if(!m_static2)
			return;
		m_static2->Show(status);
		break;
	case 3:
		if(!m_static3)
			return;
		m_static3->Show(status);
		break;
	default:
		break;
	}
}
