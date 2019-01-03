#include "stdafx.h"
#include "r4_extended.h"

CRender						RImplementation;

CRender::CRender( ) 
{
    
}

CRender::~CRender( ) 
{
    
}

void					CRender::create( )
{
    
}

void					CRender::destroy( )
{
    
}

void					CRender::reset_begin( )
{
    
}

void					CRender::reset_end( )
{
    
}

void					CRender::level_Load( IReader* )
{
    
}

void					CRender::level_Unload( )
{
    
}

HRESULT					CRender::shader_compile( LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result )
{
    return -1134;
}

void					CRender::Statistics( CGameFont* F )
{
    
}

LPCSTR					CRender::getShaderPath( )
{
    return "unknown";
}

IRender_Sector*			CRender::getSector( int id )
{
    return nullptr;
}

IRenderVisual*			CRender::getVisual( int id )
{
    return nullptr;
}

IRender_Sector*			CRender::detectSector( const Fvector& P )
{
    return nullptr;
}

IRender_Target*			CRender::getTarget( )
{
    return nullptr;
}

void					CRender::set_Transform( Fmatrix* M )
{

}

void					CRender::set_HUD( BOOL V )
{

}

BOOL					CRender::get_HUD( )
{
    return FALSE;
}

void					CRender::set_Invisible( BOOL V )
{
    
}

void					CRender::flush( )
{
    
}

void					CRender::set_Object( IRenderable* O )
{
    
}

void					CRender::add_Occluder( Fbox2&	bb_screenspace )
{
    
}

void					CRender::add_Visual( IRenderVisual*	V )
{
    
}

void					CRender::add_Geometry( IRenderVisual*	V )
{
    
}

void					CRender::add_StaticWallmark( const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V )
{
    
}

void					CRender::add_StaticWallmark( IWallMarkArray *pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V )
{
    
}

void					CRender::clear_static_wallmarks( )
{
    
}

void					CRender::add_SkeletonWallmark( const Fmatrix* xf, IKinematics* obj, IWallMarkArray *pArray, const Fvector& start, const Fvector& dir, float size )
{
    
}

IRender_ObjectSpecific*	CRender::ros_create( IRenderable* parent )
{
    return nullptr;
}

void					CRender::ros_destroy( IRender_ObjectSpecific* & )
{
    
}

IRender_Light*			CRender::light_create( )
{
    return nullptr;
}

void					CRender::light_destroy( IRender_Light* p_ )
{
    
}

IRender_Glow*			CRender::glow_create( )
{
    return nullptr;
}

void					CRender::glow_destroy( IRender_Glow* p_ )
{
    
}

IRenderVisual*			CRender::model_CreateParticles( LPCSTR name )
{
    return nullptr;
}

IRenderVisual*			CRender::model_Create( LPCSTR name, IReader*	data)
{
    return nullptr;
}

IRenderVisual*			CRender::model_CreateChild( LPCSTR name, IReader*	data )
{
    return nullptr;
}

IRenderVisual*			CRender::model_Duplicate( IRenderVisual*	V )
{
    return nullptr;
}

void					CRender::model_Delete( IRenderVisual* &	V, BOOL bDiscard )
{

}

void					CRender::model_Logging( BOOL bEnable )
{

}

void					CRender::models_Prefetch( )
{

}

void					CRender::models_Clear( BOOL b_complete )
{

}

BOOL					CRender::occ_visible( vis_data&	V )
{
    return FALSE;
}

BOOL					CRender::occ_visible( Fbox&		B )
{
    return FALSE;
}

BOOL					CRender::occ_visible( sPoly&		P )
{
    return FALSE;
}

void					CRender::Calculate( )
{

}

void					CRender::Render( )
{

}

void                    CRender::BeforeWorldRender( )
{

}

void                    CRender::AfterWorldRender( )
{

}

void					CRender::Screenshot( ScreenshotMode mode, LPCSTR name)
{

}

void					CRender::rmNear( )
{

}

void					CRender::rmFar( )
{

}

void					CRender::rmNormal( )
{

}

u32						CRender::memory_usage( )
{
    return 0;
}

u32                     CRender::active_phase( )
{
    return 0;
}


void	                CRender::OnFrame( )     
{
    
}



