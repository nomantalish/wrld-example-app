// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "WorldPinsFactory.h"
#include "Pin.h"

namespace ExampleApp
{
	namespace WorldPins
	{
		WorldPinsFactory::WorldPinsFactory()
			: m_lastId(0)
		{

		}

		WorldPinsFactory::~WorldPinsFactory()
		{

		}

		Eegeo::Pins::Pin* WorldPinsFactory::CreatePin(const Eegeo::Space::LatLong& location,
		        int iconIndex)
		{
			WorldPinItemModel::WorldPinItemModelId pinId = m_lastId ++;
			return Eegeo_NEW(Eegeo::Pins::Pin)(pinId, location, 0, iconIndex, NULL);
		}
	}
}
