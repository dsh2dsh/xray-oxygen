#include "StdAfx.h"
#include "../xrEngine/IGame_Actor.h"
#include "../xrEngine/IGame_Level.h"
#include "UIMMShniaga.h"
#include "UICursor.h"
#include "UIStatic.h"
#include "UIScrollView.h"
#include "UIXmlInit.h"
#include "MMsound.h"
#include <math.h>
#include "MainMenu.h"

ENGINE_API extern string_path g_last_saved_game;

CUIMMShniaga::CUIMMShniaga()
{
	m_sound			= xr_new<CMMSound>();

	m_view			= xr_new<CUIScrollView>();	AttachChild(m_view);
	m_shniaga		= xr_new<CUIStatic>();	AttachChild(m_shniaga);
	m_magnifier		= xr_new<CUIStatic>();	m_shniaga->AttachChild(m_magnifier);	m_magnifier->SetPPMode();
	m_mag_pos		= 0;

	m_selected		= NULL;

	m_start_time	= 0;
	m_origin		= 0;
	m_destination	= 0;
	m_run_time		= 0;

	m_flags.zero	();	

	m_selected_btn	= -1;
	m_page			= epi_none;
}

CUIMMShniaga::~CUIMMShniaga()
{
	xr_delete(m_magnifier);
	xr_delete(m_shniaga);
	xr_delete(m_view);
	xr_delete(m_sound);

	for (CUITextWnd* it : m_buttons)
		xr_delete(it);
	for (CUITextWnd* it : m_buttons_new)
		xr_delete(it);

	m_buttons.clear();
	m_buttons_new.clear();
}

void CUIMMShniaga::InitShniaga(CXml& xml_doc, LPCSTR path)
{
	string256 _path;

	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	xr_strconcat( _path, path, ":shniaga:magnifire");
	CUIXmlInit::InitStatic(xml_doc, _path, 0, m_magnifier);
	m_mag_pos = m_magnifier->GetWndPos().x;
	xr_strconcat( _path, path, ":shniaga");
	CUIXmlInit::InitStatic(xml_doc, _path, 0, m_shniaga);
	xr_strconcat( _path, path, ":buttons_region");
	CUIXmlInit::InitScrollView(xml_doc, _path, 0, m_view);
	xr_strconcat( _path, path, ":shniaga:magnifire:y_offset");
	m_offset = xml_doc.ReadFlt(_path, 0, 0);

	if (!g_pGameLevel || !g_pGameLevel->bReady)
	{
		auto valid_saved_gameS = [](IReader &stream) -> bool
		{
			if (stream.length() < 8)
				return					(false);

			if (stream.r_u32() != u32(-1))
				return					(false);

			if (stream.r_u32() < 0x0006)
				return					(false);

			return						(true);
		};

		auto valid_saved_game = [valid_saved_gameS](LPCSTR saved_game_name) -> bool
		{
			string_path					file_name;

			xr_strconcat(file_name, saved_game_name, ".scop");
			FS.update_path(file_name, "$game_saves$", file_name);

			if (!FS.exist(file_name))
				return					(false);

			IReader						*stream = FS.r_open(file_name);
			bool						result = valid_saved_gameS(*stream);
			FS.r_close(stream);
			return						(result);
		};


		if (!*g_last_saved_game || !valid_saved_game(g_last_saved_game))
			CreateList(m_buttons, xml_doc, "menu_main");
		else
			CreateList(m_buttons, xml_doc, "menu_main_last_save");

		CreateList(m_buttons_new, xml_doc, "menu_new_game");
	}
	else
	{
		if (g_actor && !g_actor->g_Alive())
			CreateList(m_buttons, xml_doc, "menu_main_single_dead");
		else
			CreateList(m_buttons, xml_doc, "menu_main_single");
	}

	ShowMain();

	m_sound->Init(xml_doc, "menu_sound");
	m_sound->music_Play();
}

void CUIMMShniaga::OnDeviceReset()
{
}


void CUIMMShniaga::CreateList(xr_vector<CUITextWnd*>& lst, CXml& xml_doc, LPCSTR path)
{
	CGameFont* pF;
	u32	color;
	float button_height				= xml_doc.ReadAttribFlt("button", 0, "h");
	R_ASSERT						(button_height);

	CUIXmlInit::InitFont			(xml_doc, path, 0, color, pF);
	R_ASSERT						(pF);

	int nodes_num					= xml_doc.GetNodesNum(path, 0, "btn");

	XML_NODE* tab_node				= xml_doc.NavigateToNode(path,0);
	xml_doc.SetLocalRoot			(tab_node);

	CUITextWnd* st;

	for (int i = 0; i < nodes_num; ++i)
	{		
		st							= xr_new<CUITextWnd>();
		st->SetWndPos				(Fvector2().set(0,0));
		st->SetWndSize				(Fvector2().set(m_view->GetDesiredChildWidth(), button_height));
		st->SetFont					(pF);
		st->SetTextComplexMode		(false);
		st->SetTextST				(xml_doc.ReadAttrib	("btn", i, "caption"));
		st->SetTextColor			(color);
		st->SetTextAlignment		(CGameFont::alCenter);
		st->SetVTextAlignment		(valCenter);
		st->SetWindowName			(xml_doc.ReadAttrib("btn", i, "name"));
		st->SetMessageTarget		(this);


		lst.push_back(st);
	}
	xml_doc.SetLocalRoot(xml_doc.GetRoot());

}

void CUIMMShniaga::SetPage		(enum_page_id page_id, LPCSTR xml_file, LPCSTR xml_path)
{
	VERIFY(m_page != page_id);
	xr_vector<CUITextWnd*>* lst = nullptr;
	switch (page_id)
	{
	case epi_main:		lst = &m_buttons; break;
	case epi_new_game:	lst = &m_buttons_new; break;
	}

	for (CUITextWnd* it : *lst)
		xr_delete(it);
	lst->clear();

	CXml tmp_xml;
	tmp_xml.Load	(CONFIG_PATH, UI_PATH, xml_file);
	CreateList		(*lst, tmp_xml, xml_path);
}

void CUIMMShniaga::ShowPage(enum_page_id page_id)
{
	switch (page_id)
	{
	case epi_main: ShowMain(); break;
	case epi_new_game: ShowNewGame(); break;
	}
}

void CUIMMShniaga::ShowMain()
{
	m_page = epi_main;
	m_view->Clear();
	for (u32 i = 0; i<m_buttons.size(); i++)
		m_view->AddWindow(m_buttons[i], false);

	SelectBtn(m_buttons[0]);
}

void CUIMMShniaga::ShowNewGame()
{
	m_page = epi_new_game;
    m_view->Clear();
	for (size_t i = 0; i<m_buttons_new.size(); i++)
		m_view->AddWindow(m_buttons_new[i], false);

	SelectBtn(m_buttons_new[0]);
}

bool CUIMMShniaga::IsButton(CUIWindow* st)
{
	for (u32 i = 0; i<m_buttons.size(); ++i)
		if (m_buttons[i] == st)
			return true;

	for (u32 i = 0; i<m_buttons_new.size(); ++i)
		if (m_buttons_new[i] == st)
			return true;

	return false;
}

void CUIMMShniaga::SendMessageToWnd(CUIWindow* pWnd, s16 msg, void* pData){
	CUIWindow::SendMessageToWnd(pWnd, msg, pData);
	if (IsButton(pWnd))
	{
		if (msg == WINDOW_FOCUS_RECEIVED)
		{
			SelectBtn(pWnd);
		}
	}
}

void CUIMMShniaga::SelectBtn(int btn)
{
	m_view->Update		();

	R_ASSERT(btn >= 0);
	if (epi_main == m_page)
        m_selected = m_buttons[btn];
	else if (epi_new_game == m_page)
		m_selected = m_buttons_new[btn];
	
	m_selected_btn = btn;
	ProcessEvent(E_Begin);
}

void CUIMMShniaga::SelectBtn(CUIWindow* btn)
{
	R_ASSERT(m_page >= 0);
	switch (m_page)
	{
	case epi_main:
		for (size_t i = 0; i < m_buttons.size(); ++i)
		{
			if (m_buttons[i] == btn)
			{ 
				SelectBtn((int)i); 
				return; 
			}
		}
		break;
	case epi_new_game:
		for (size_t i = 0; i < m_buttons_new.size(); ++i)
		{
			if (m_buttons_new[i] == btn)
			{
				SelectBtn((int)i);
				return;
			}
		}
		break;
	}
}

void CUIMMShniaga::Draw()
{
	CUIWindow::Draw();
}

void CUIMMShniaga::Update()
{
	if (m_start_time > Device.dwTimeContinual - m_run_time)
	{
		Fvector2 pos = m_shniaga->GetWndPos();
		pos.y = this->pos(m_origin, m_destination, Device.dwTimeContinual - m_start_time);
		m_shniaga->SetWndPos(pos);		
	}
	else ProcessEvent(E_Stop);

	if (m_start_time > Device.dwTimeContinual - m_run_time*10/100)
		ProcessEvent(E_Finilize);

	ProcessEvent(E_Update);
		
	CUIWindow::Update();
}


bool CUIMMShniaga::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
	Fvector2 pos = UI().GetUICursor().GetCursorPosition();
    Frect r;
	m_magnifier->GetAbsoluteRect(r);
	if (WINDOW_LBUTTON_DOWN == mouse_action && r.in(pos.x, pos.y))
	{
		OnBtnClick();
	}

	return CUIWindow::OnMouseAction(x,y,mouse_action);
}

void CUIMMShniaga::OnBtnClick()
{
	if (0 == xr_strcmp("btn_new_game", m_selected->WindowName()))
		ShowNewGame();
	else if (0 == xr_strcmp("btn_new_back", m_selected->WindowName()))
		ShowMain();
	else
		GetMessageTarget()->SendMessageToWnd(m_selected, BUTTON_CLICKED);
}

bool CUIMMShniaga::OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
{

	if (WINDOW_KEY_PRESSED == keyboard_action)
	{
		switch (dik)
		{
			case VK_UP:
				if (m_selected_btn > 0)
					SelectBtn(m_selected_btn - 1);
				return true;
			case VK_DOWN:
				if (m_selected_btn < BtnCount() - 1)
					SelectBtn(m_selected_btn + 1);
				return true;
			case VK_RETURN:
				OnBtnClick();
				return true;
			case VK_ESCAPE:
				if (m_page != epi_main)
					ShowMain();
				return true;
		}
	}


	return CUIWindow::OnKeyboardAction(dik, keyboard_action);
}

int CUIMMShniaga::BtnCount()
{
	R_ASSERT(-1);
	switch (m_page)
	{
	case epi_main:		return (int)m_buttons.size();
	case epi_new_game:  return (int)m_buttons_new.size();
	default: return		-1;
	}
}

float CUIMMShniaga::pos(float x1, float x2, u32 t)
{
	float x = 0;

	if (t != u32(-1) && t <= m_run_time)
		x = log(1 + (t*10.0f) / m_run_time) / log(11.0f);
	else if (t > m_run_time)
		x = 1;

	x *= abs(x2 - x1);

	if (x2 - x1 < 0)
		return x1 - x;
	else
		return x1 + x;
}

bool b_shniaganeed_pp = true;
void CUIMMShniaga::SetVisibleMagnifier(bool f)
{
	b_shniaganeed_pp = f;
	Fvector2 pos = m_magnifier->GetWndPos();
	if (f)
		pos.x = m_mag_pos;
	else
		pos.x = 1025;
	m_magnifier->SetWndPos(pos);
}

void CUIMMShniaga::ProcessEvent(EVENT ev)
{
	switch (ev){
		case E_Begin:
			{
				// init whell sound
				m_sound->whell_Play();

                // calculate moving params
				m_start_time = Device.dwTimeContinual;
				m_origin = m_shniaga->GetWndPos().y;
				m_destination = m_selected->GetWndPos().y - m_magnifier->GetWndPos().y;
				m_destination += m_offset;
				m_run_time = u32((log(1 + abs(m_origin - m_destination))/log(GetHeight()))*300);
				if (m_run_time < 100)
					m_run_time = 100;

                // reset flags
				m_flags.set(fl_SoundFinalized,	FALSE);
				m_flags.set(fl_MovingStoped,	FALSE);
			}	break;
		case E_Finilize:
			if (!m_flags.test(fl_SoundFinalized))
			{
				m_sound->whell_Click();
				
				m_flags.set(fl_SoundFinalized, TRUE);
			}	break;
		case E_Stop:		
			if (!m_flags.test(fl_MovingStoped))
			{
				m_sound->whell_Stop();	


				Fvector2 pos = m_shniaga->GetWndPos();
				pos.y = m_destination;
				m_shniaga->SetWndPos(pos);		

                m_flags.set(fl_MovingStoped, TRUE);
			}	break;
		case E_Update:		m_sound->music_Update();
			break;
									
	}
}
