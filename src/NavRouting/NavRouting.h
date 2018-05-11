// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "ICallback.h"
#include "RouteData.h"

#include <vector>

namespace ExampleApp
{
    namespace NavRouting
    {
        namespace SdkModel
        {
            struct NavRouteDrawingVertexData;
            
            class NavRoutingModule;
            class NavRouteDrawingController;
            class INavRoutingServiceController;
            class NavRoutingServiceController;
            
            typedef Eegeo::Helpers::ICallback1<const std::vector<Eegeo::Routes::Webservice::RouteData>> RoutesReceivedCallback;
        }

        namespace View
        {
            class INavWidgetView;
            class INavWidgetViewModel;
            class NavUIModule;
            class NavWidgetController;
            class NavWidgetOpenableControl;
            class NavWidgetViewModel;
        }
    }
}