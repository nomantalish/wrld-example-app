// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "Types.h"
#include "AboutPage.h"
#include "IAboutPageViewModule.h"
#include "IMetricsService.h"
#include "BidirectionalBus.h"

namespace ExampleApp
{
    namespace AboutPage
    {
        namespace View
        {
            class AboutPageViewModule: public IAboutPageViewModule, private Eegeo::NonCopyable
            {
            private:
                AboutPageView* m_pView;
                AboutPageController* m_pController;

            public:
                AboutPageViewModule(IAboutPageViewModel& aboutPageViewModel, Metrics::IMetricsService& metricsService, ExampleAppMessaging::TMessageBus& messageBus);

                ~AboutPageViewModule();

                AboutPageController& GetAboutPageController() const;

                AboutPageView& GetAboutPageView() const;
            };
        }
    }
}
