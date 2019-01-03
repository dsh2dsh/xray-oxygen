#pragma once

class CGammaControl
{
private:
	float	fGamma;
	float	fBrightness;
	float	fContrast;
	Fvector	cBalance;

	DXGI_GAMMA_CONTROL_CAPABILITIES GC;
	DXGI_GAMMA_CONTROL G;
	IDXGIOutput* pOutput;

public:
				CGammaControl	();

	IC  Fvector	GetBalance		() const						{ return cBalance; }
	IC	void	SetBalance		(float r, float g, float b)		{ cBalance.set(r, g, b); }
	IC	void	SetBalance		(Fvector &C)					{ SetBalance(C.x, C.y, C.z); }

	IC DXGI_GAMMA_CONTROL	GetLUT() const { return G; }

	IC	float	GetGamma		() const						{ return fGamma; }
	IC	float	GetBrightness	() const						{ return fBrightness; }
	IC	float	GetContrast		() const						{ return fContrast; }
	IC	void	SetGamma		(float val)						{ fGamma = val; }
	IC	void	SetBrightness	(float val)						{ fBrightness = val; }
	IC	void	SetContrast		(float val)						{ fContrast	= val; }

		void	GetIP			(float& G, float &B, float& C, Fvector& Balance);
		void	Update			();

private:
		void	GenLUT			();
};
