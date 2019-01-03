#pragma once

#include "../xrRender/r__dsgraph_structure.h"
#include "../xrRender/r__occlusion.h"
#include "../xrRender/PSLibrary.h"
#include "../xrRender/xrRenderTarget.h"
#include "../xrRender/hom.h"
#include "../xrRender/detailmanager.h"
#include "../xrRender/modelpool.h"
#include "../xrRender/wallmarksengine.h"
#include "../xrRender/light_db.h"
#include "../xrRender/LightTrack.h"
#include "../xrRender/r_sun_cascades.h"
#include "../../xrEngine/irenderable.h"
#include "../../xrEngine/fmesh.h"
#include "../xrRender/dxGlowManager.h"
#include "../xrRender/ScreenshotManager.h"

class CRender	:	public R_dsgraph_structure
{
public: // базовые методы рендера от прошлого
    CRender();
    virtual ~CRender();
    
    virtual	void					create					();
    virtual	void					destroy					();
    virtual	void					reset_begin				();
    virtual	void					reset_end				();
    virtual	void					level_Load				(IReader*);
    virtual void					level_Unload			();

    virtual HRESULT					shader_compile			(
        LPCSTR							name,
        DWORD const*                    pSrcData,
        UINT                            SrcDataLen,
        LPCSTR                          pFunctionName,
        LPCSTR                          pTarget,
        DWORD                           Flags,
        void*&							result
    );

    virtual	void					Statistics				(CGameFont* F);

    virtual LPCSTR					getShaderPath			();
    virtual IRender_Sector*			getSector				(int id);
    virtual IRenderVisual*			getVisual				(int id);
    virtual IRender_Sector*			detectSector			(const Fvector& P);
    virtual IRender_Target*			getTarget				();

    virtual void					set_Transform			(Fmatrix* M );
    virtual void					set_HUD					(BOOL V );
    virtual BOOL					get_HUD					();
    virtual void					set_Invisible			(BOOL V );
    virtual void					flush					();	
    virtual void					set_Object				(IRenderable* O );
    virtual	void					add_Occluder			(Fbox2&	bb_screenspace );
    virtual void					add_Visual				(IRenderVisual*	V	);
    virtual void					add_Geometry			(IRenderVisual*	V	);

    virtual void					add_StaticWallmark		(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    
    virtual void					add_StaticWallmark		(IWallMarkArray *pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    virtual void					clear_static_wallmarks	();
    
    virtual void					add_SkeletonWallmark	(const Fmatrix* xf, IKinematics* obj, IWallMarkArray *pArray, const Fvector& start, const Fvector& dir, float size);

    virtual IRender_ObjectSpecific*	ros_create				(IRenderable* parent);
    virtual void					ros_destroy				(IRender_ObjectSpecific* &);

    virtual IRender_Light*			light_create			();
    virtual void					light_destroy			(IRender_Light* p_);
    virtual IRender_Glow*			glow_create				();
    virtual void					glow_destroy			(IRender_Glow* p_);

    virtual IRenderVisual*			model_CreateParticles	(LPCSTR name);
    virtual IRenderVisual*			model_Create			(LPCSTR name, IReader*	data=0);
    virtual IRenderVisual*			model_CreateChild		(LPCSTR name, IReader*	data);
    virtual IRenderVisual*			model_Duplicate			(IRenderVisual*	V);
    virtual void					model_Delete			(IRenderVisual* &	V, BOOL bDiscard);
    virtual void					model_Logging			(BOOL bEnable);
    virtual void					models_Prefetch			();
    virtual void					models_Clear			(BOOL b_complete);

    virtual BOOL					occ_visible				(vis_data&	V);
    virtual BOOL					occ_visible				(Fbox&		B);
    virtual BOOL					occ_visible				(sPoly&		P);

    virtual void					Calculate				();
    virtual void					Render					();
    virtual void                    BeforeWorldRender       (); // Перед рендерингом мира
    virtual void                    AfterWorldRender        (); // После рендеринга мира (до UI)

    virtual void					Screenshot				(ScreenshotMode mode = SM_NORMAL, LPCSTR name = nullptr);

    virtual void					rmNear					();
    virtual void					rmFar					();
    virtual void					rmNormal				();
    virtual u32						memory_usage			();
    virtual u32                     active_phase            ();

    virtual void		_BCL		OnFrame				    (); // pureFrame

public: // остатки прошлого рендера зависимости без которых нескомпилится пустой код
    struct		_stats		{
        u32		l_total,	l_visible;
        u32		l_shadowed,	l_unshadowed;
        s32		s_used,		s_merged,	s_finalclip;
        u32		o_queries,	o_culled;
        u32		ic_total,	ic_culled;
    } stats;

    R_occlusion													HWOCC;

    // HW-occlusion culling
    IC u32							occq_begin					(u32&	ID		)	{ return HWOCC.occq_begin	(ID);	}
    IC void							occq_end					(u32&	ID		)	{ HWOCC.occq_end	(ID);			}
    IC R_occlusion::occq_result		occq_get					(u32&	ID		)	{ return HWOCC.occq_get		(ID);	}

    struct		_options	{
        u32		ssao_blur_on		: 1;
        u32		ssao_opt_data		: 1;
        u32		ssao_half_data		: 1;
        u32		ssao_hbao			: 1;
        u32		ssao_ultra			: 1;
        u32		hbao_vectorized		: 1;

        u32		smapsize			: 16;
        u32		depth16				: 1;
        u32		mrt					: 1;
        u32		mrtmixdepth			: 1;
        u32		fp16_filter			: 1;
        u32		fp16_blend			: 1;
        u32		albedo_wo			: 1;						// work-around albedo on less capable HW
        u32		HW_smap				: 1;
        u32		HW_smap_PCF			: 1;

        //Sunshafts
        u32     sunshaft_mrmnwar    : 1;
        u32     sunshaft_screenspace: 1;

        u32		HW_smap_FORMAT		: 32;

        u32		nvdbt				: 1;

        u32		distortion			: 1;
        u32		distortion_enabled	: 1;

        u32		sunfilter			: 1;
        u32		sjitter				: 1;
        u32		noshadows			: 1;
        u32		Tshadows			: 1;						// transluent shadows
        u32		disasm				: 1;
        u32		advancedpp			: 1;	//	advanced post process (DOF, SSAO, volumetrics, etc.)
        u32		volumetricfog		: 1;

        u32		dx10_msaa			: 1;	//	DX10.0 path
        u32		dx10_msaa_hybrid	: 1;	//	DX10.0 main path with DX10.1 A-test msaa allowed
        u32		dx10_msaa_opt	    : 1;	//	DX10.1 path
        u32		dx10_sm4_1			: 1;	//	DX10.1 path
        u32		dx10_msaa_alphatest	: 2;	//	A-test mode
        u32		dx10_msaa_samples	: 4;

        u32		dx10_minmax_sm		: 2;
        u32		dx10_minmax_sm_screenarea_threshold;

        u32		dx11_enable_tessellation : 1;

        u32		forcegloss			: 1;
        u32		forceskinw			: 1;
        float	forcegloss_v		;
    }			o;

    ShaderElement*					rimp_select_sh_static		(dxRender_Visual	*pVisual, float cdist_sq){ return nullptr; }
    ShaderElement*					rimp_select_sh_dynamic		(dxRender_Visual	*pVisual, float cdist_sq){ return nullptr; }
    ICF void						apply_object				(IRenderable*	O) {}
    IC void							apply_lmaterial				() {}

    CHOM														HOM;
    IRender_Portal*					getPortal					(int id){ return nullptr; }

    void                            add_leafs_Dynamic	(dxRender_Visual *pVisual);
    void                            add_leafs_Static(dxRender_Visual *pVisual);
    
    enum
    {
        PHASE_NORMAL	= 0,	// E[0]
        PHASE_SMAP		= 1,	// E[1]
    };

    CRenderTarget*												Target;			// Render-target

    BOOL                            add_Dynamic(dxRender_Visual *pVisual, u32 planes);
    void							add_Static					(dxRender_Visual*pVisual, u32 planes);

    xrXRC														Sectors_xrc;
    CDB::MODEL*													rmPortals;
    
    xr_vector<IRender_Portal*>									Portals;
    xr_vector<IRender_Sector*>									Sectors;

    void CRender::render_lights(light_Package& LP);

};

extern CRender						RImplementation;