/*
Please note additional terms under
the in LGPL-3.0 incorporated GNU GPL version 3 section 7. These
additional terms can be found in the corresponding LICENSE file
*/

#ifndef POWER_SASA_PARALLEL_H_
#define POWER_SASA_PARALLEL_H_

#include "power_diagram.h"
#include "power_sasa.h"
#include <boost/shared_ptr.hpp>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace POWERSASA
{

template<class PDFloat, class PDCoord>
class PowerSasaParallel
{
	typedef boost::shared_ptr<PowerSasa<PDFloat,PDCoord> > Psp;
	std::vector<Psp> sasa;
	std::vector<PDFloat> area, vol;
	bool with_sasa, with_vol;
	unsigned int size;
public:
	template<class Coordcontainer, class Floatcontainer>
	PowerSasaParallel(Coordcontainer const& coords, Floatcontainer const& radii, bool bsasa = 1, bool dsasa = 0, bool bvol=0, bool dvol=0)
	{
#ifdef _OPENMP
		sasa.resize(omp_get_max_threads());
#else
		sasa.resize(1);
#endif
		if(dsasa || dvol)
		{
			printf("derivatives of Sasa or Volume are not supported in PowerSasaParallel\n");
			abort();
		}
		with_sasa = bsasa;
		with_vol = bvol;
		if(with_sasa) area.resize(coords.size());
		if(with_vol) vol.resize(coords.size());
		size = coords.size();
		this->init(coords, radii);
	}
	const std::vector<PDFloat>& getSasa() const
	{
		return area;
	}
	const std::vector<PDFloat>& getVol() const
	{
		return vol;
	}
	POWER_DIAGRAM::PowerDiagram<PDFloat,PDCoord,3 >& get_power_diagram()  { return sasa[0]->get_power_diagram(); }
	template<class Coordcontainer, class Floatcontainer>
	void update_coords(Coordcontainer const& coords, Floatcontainer const& radii)
	{
#pragma omp single
		{
			if(with_sasa) area.resize(coords.size());
			if(with_vol) vol.resize(coords.size());
			size = coords.size();
		}
#ifdef _OPENMP
		Psp mysasa = sasa[omp_get_thread_num()];
        //printf("hello from thread %d",omp_get_thread_num());
#else
		Psp mysasa = sasa[0];
#endif
		mysasa->update_coords(coords,radii);
	}
	void calc_sasa_all()
	{
#ifdef _OPENMP
		Psp mysasa = sasa[omp_get_thread_num()];
#else
		Psp mysasa = sasa[0];
#endif
		if(with_sasa && !with_vol)
		{
			const std::vector<PDFloat>& tmp_area = mysasa->getSasa();
#pragma omp for
			for (unsigned int i = 0; i < size; ++i)
			{
				mysasa->calc_sasa_single(i);
				area[i] = tmp_area[i];
			}
		}
		else if(!with_sasa && with_vol)
		{
			const std::vector<PDFloat>& tmp_vol = mysasa->getVol();
#pragma omp for
			for (unsigned int i = 0; i < size; ++i)
			{
				mysasa->calc_sasa_single(i);
				vol[i] = tmp_vol[i];
			}
		}
		else if(with_sasa && with_vol)
		{
			const std::vector<PDFloat>& tmp_area = mysasa->getSasa();
			const std::vector<PDFloat>& tmp_vol = mysasa->getVol();
#pragma omp for
			for (unsigned int i = 0; i < size; ++i)
			{
				mysasa->calc_sasa_single(i);
				vol[i] = tmp_vol[i];
				area[i] = tmp_area[i];
			}
		}
	}
private:
	template<class Coordcontainer, class Floatcontainer>
	void init(Coordcontainer const& coords, Floatcontainer const& radii)
	{
		for(unsigned int i=0; i<sasa.size(); ++i)
		{
			sasa[i].reset(new POWERSASA::PowerSasa<PDFloat,PDCoord>(coords,radii, with_sasa, 0, with_vol, 0));
		}
	}

};

} // end of namespace

#endif /* POWER_SASA_PARALLEL_H_ */
