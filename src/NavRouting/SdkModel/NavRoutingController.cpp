// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "NavRoutingController.h"
#include "NavRoutingViewOpenMessage.h"
#include "SearchResultModel.h"
#include "NavRoutingStartLocationSetMessage.h"
#include "NavRoutingStartLocationClearedMessage.h"
#include "NavRoutingEndLocationSetMessage.h"
#include "NavRoutingEndLocationClearedMessage.h"
#include "NavRoutingRouteChangedMessage.h"
#include "NavRoutingRouteClearedMessage.h"
#include "NavRoutingCurrentDirectionSetMessage.h"
#include "NavRoutingCurrentDirectionUpdatedMessage.h"
#include "NavRoutingSelectedDirectionSetMessage.h"
#include "NavRoutingRemainingRouteDurationSetMessage.h"
#include "NavRoutingModeSetMessage.h"
#include "INavTurnByTurnModel.h"
#include "InteriorsModel.h"
#include "InteriorsFloorModel.h"
#include "IAlertBoxFactory.h"
#include "IWorldPinsService.h"
#include "NavRouteInteriorModelHelper.h"
#include "NavRoutingShowRerouteDialogMessage.h"

namespace ExampleApp
{
    namespace NavRouting
    {
        namespace SdkModel
        {
            NavRoutingController::NavRoutingController(INavRoutingModel& routingModel,
                                                       TurnByTurn::INavTurnByTurnModel& turnByTurnModel,
                                                       INavRoutingLocationFinder& locationFinder,
                                                       ExampleAppMessaging::TMessageBus& messageBus,
                                                       WorldPins::SdkModel::IWorldPinsService& worldPinsService)
            : m_routingModel(routingModel)
            , m_turnByTurnModel(turnByTurnModel)
            , m_locationFinder(locationFinder)
            , m_messageBus(messageBus)
            , m_worldPinsService(worldPinsService)
            , m_isRerouting(false)
            , m_waitingForRerouteResponse(false)
            , m_startLocationSetCallback(this, &NavRoutingController::OnStartLocationSet)
            , m_startLocationClearedCallback(this, &NavRoutingController::OnStartLocationCleared)
            , m_endLocationSetCallback(this, &NavRoutingController::OnEndLocationSet)
            , m_endLocationClearedCallback(this, &NavRoutingController::OnEndLocationCleared)
            , m_routeSetCallback(this, &NavRoutingController::OnRouteSet)
            , m_routeClearedCallback(this, &NavRoutingController::OnRouteCleared)
            , m_routeUpdatedCallback(this, &NavRoutingController::OnRouteUpdated)
            , m_currentDirectionSetCallback(this, &NavRoutingController::OnCurrentDirectionSet)
            , m_currentDirectionUpdatedCallback(this, &NavRoutingController::OnCurrentDirectionUpdated)
            , m_selectedDirectionSetCallback(this, &NavRoutingController::OnSelectedDirectionSet)
            , m_remainingRouteDurationSetCallback(this, &NavRoutingController::OnRemainingRouteDurationSet)
            , m_navRoutingModeSetCallback(this, &NavRoutingController::OnNavRoutingModeSet)
            , m_viewClosedMessageHandler(this, &NavRoutingController::OnNavWidgetViewClosed)
            , m_startEndLocationSwappedMessageHandler(this, &NavRoutingController::OnStartEndLocationSwapped)
            , m_startLocationClearClickedMessageHandler(this, &NavRoutingController::OnStartLocationClearClicked)
            , m_endLocationClearClickedMessageHandler(this, &NavRoutingController::OnEndLocationClearClicked)
            , m_startEndRoutingButtonClickedMessageHandler(this, &NavRoutingController::OnStartEndRoutingButtonClicked)
            , m_selectedDirectionChangedMessageHandler(this, &NavRoutingController::OnSelectedDirectionChanged)
            , m_rerouteDialogClosedMessageMessageHandler(this, &NavRoutingController::OnRerouteDialogClosed)
            , m_navigateToMessageHandler(this, &NavRoutingController::OnNavigationMessage)
            , m_shouldRerouteCallback(this, &NavRoutingController::OnShouldReroute)
            {
                m_routingModel.InsertStartLocationSetCallback(m_startLocationSetCallback);
                m_routingModel.InsertStartLocationClearedCallback(m_startLocationClearedCallback);
                m_routingModel.InsertEndLocationSetCallback(m_endLocationSetCallback);
                m_routingModel.InsertEndLocationClearedCallback(m_endLocationClearedCallback);
                m_routingModel.InsertRouteSetCallback(m_routeSetCallback);
                m_routingModel.InsertRouteClearedCallback(m_routeClearedCallback);
                m_routingModel.InsertRouteUpdatedCallback(m_routeUpdatedCallback);
                m_routingModel.InsertSelectedDirectionSetCallback(m_selectedDirectionSetCallback);
                m_routingModel.InsertCurrentDirectionSetCallback(m_currentDirectionSetCallback);
                m_routingModel.InsertCurrentDirectionUpdatedCallback(m_currentDirectionUpdatedCallback);
                m_routingModel.InsertRemainingRouteDurationSetCallback(m_remainingRouteDurationSetCallback);
                m_routingModel.InsertNavModeSetCallback(m_navRoutingModeSetCallback);
                m_messageBus.SubscribeNative(m_viewClosedMessageHandler);
                m_messageBus.SubscribeNative(m_startEndLocationSwappedMessageHandler);
                m_messageBus.SubscribeNative(m_startLocationClearClickedMessageHandler);
                m_messageBus.SubscribeNative(m_endLocationClearClickedMessageHandler);
                m_messageBus.SubscribeNative(m_startEndRoutingButtonClickedMessageHandler);
                m_messageBus.SubscribeNative(m_selectedDirectionChangedMessageHandler);
                m_messageBus.SubscribeNative(m_rerouteDialogClosedMessageMessageHandler);
                m_messageBus.SubscribeNative(m_navigateToMessageHandler);
                m_turnByTurnModel.InsertShouldRerouteCallback(m_shouldRerouteCallback);
            }

            NavRoutingController::~NavRoutingController()
            {
                m_turnByTurnModel.RemoveShouldRerouteCallback(m_shouldRerouteCallback);
                m_messageBus.UnsubscribeNative(m_navigateToMessageHandler);
                m_messageBus.UnsubscribeNative(m_rerouteDialogClosedMessageMessageHandler);
                m_messageBus.UnsubscribeNative(m_selectedDirectionChangedMessageHandler);
                m_messageBus.UnsubscribeNative(m_startEndRoutingButtonClickedMessageHandler);
                m_messageBus.UnsubscribeNative(m_endLocationClearClickedMessageHandler);
                m_messageBus.UnsubscribeNative(m_startLocationClearClickedMessageHandler);
                m_messageBus.UnsubscribeNative(m_startEndLocationSwappedMessageHandler);
                m_messageBus.UnsubscribeNative(m_viewClosedMessageHandler);
                m_routingModel.RemoveNavModeSetCallback(m_navRoutingModeSetCallback);
                m_routingModel.RemoveRemainingRouteDurationSetCallback(m_remainingRouteDurationSetCallback);
                m_routingModel.RemoveCurrentDirectionUpdatedCallback(m_currentDirectionUpdatedCallback);
                m_routingModel.RemoveCurrentDirectionSetCallback(m_currentDirectionSetCallback);
                m_routingModel.RemoveSelectedDirectionSetCallback(m_selectedDirectionSetCallback);
                m_routingModel.RemoveRouteUpdatedCallback(m_routeUpdatedCallback);
                m_routingModel.RemoveRouteClearedCallback(m_routeClearedCallback);
                m_routingModel.RemoveRouteSetCallback(m_routeSetCallback);
                m_routingModel.RemoveEndLocationClearedCallback(m_endLocationClearedCallback);
                m_routingModel.RemoveEndLocationSetCallback(m_endLocationSetCallback);
                m_routingModel.RemoveStartLocationClearedCallback(m_startLocationClearedCallback);
                m_routingModel.RemoveStartLocationSetCallback(m_startLocationSetCallback);
            }

            void NavRoutingController::OnStartLocationSet(const NavRoutingLocationModel& startLocation)
            {
                m_messageBus.Publish(NavRoutingStartLocationSetMessage(startLocation));
            }

            void NavRoutingController::OnStartLocationCleared()
            {
                m_messageBus.Publish(NavRoutingStartLocationClearedMessage());
            }

            void NavRoutingController::OnEndLocationSet(const NavRoutingLocationModel& endLocation)
            {
                m_messageBus.Publish(NavRoutingEndLocationSetMessage(endLocation));
            }

            void NavRoutingController::OnEndLocationCleared()
            {
                m_messageBus.Publish(NavRoutingEndLocationClearedMessage());
            }

            void NavRoutingController::OnRouteSet(const NavRoutingRouteModel& routeModel)
            {
                m_messageBus.Publish(NavRoutingRouteChangedMessage(routeModel, true));
                if (m_isRerouting)
                {
                    m_turnByTurnModel.Start(routeModel.GetSourceRouteData());
                    m_isRerouting = false;
                }
                else
                {
                    m_routingModel.SetNavMode(Ready);
                }
                m_routingModel.SetRemainingRouteDuration(routeModel.GetDuration());
                m_worldPinsService.DeselectPin();
            }

            void NavRoutingController::OnRouteCleared()
            {
                m_messageBus.Publish(NavRoutingRouteClearedMessage());
                m_routingModel.SetNavMode(NotReady);
                m_routingModel.SetRemainingRouteDuration(0);
            }

            void NavRoutingController::OnRouteUpdated(const NavRoutingRouteModel& routeModel)
            {
                m_messageBus.Publish(NavRoutingRouteChangedMessage(routeModel, false));
            }

            void NavRoutingController::OnCurrentDirectionSet(const int& directionIndex)
            {
                m_messageBus.Publish(NavRoutingCurrentDirectionSetMessage(directionIndex));
            }

            void NavRoutingController::OnCurrentDirectionUpdated(const NavRoutingDirectionModel& directionModel)
            {
                m_messageBus.Publish(NavRoutingCurrentDirectionUpdatedMessage(directionModel));
            }

            void NavRoutingController::OnSelectedDirectionSet(const int& directionIndex)
            {
                m_messageBus.Publish(NavRoutingSelectedDirectionSetMessage(directionIndex));
            }

            void NavRoutingController::OnRemainingRouteDurationSet(const double& seconds)
            {
                m_messageBus.Publish(NavRoutingRemainingRouteDurationSetMessage(seconds));
            }

            void NavRoutingController::OnNavRoutingModeSet(const NavRoutingMode& mode)
            {
                m_messageBus.Publish(NavRoutingModeSetMessage(mode));
            }

            void NavRoutingController::OnNavWidgetViewClosed(const NavRoutingViewClosedMessage& message)
            {
                m_routingModel.ClearRoute();
            }

            void NavRoutingController::OnStartEndLocationSwapped(const NavRoutingViewStartEndLocationSwappedMessage& message)
            {
                NavRoutingLocationModel startLocation, endLocation;

                m_routingModel.TryGetStartLocation(startLocation);
                m_routingModel.TryGetEndLocation(endLocation);

                NavRoutingLocationModel tempLocation = startLocation;
                startLocation = endLocation;
                endLocation = tempLocation;

                m_routingModel.SetStartLocation(startLocation);
                m_routingModel.SetEndLocation(endLocation);

                m_routingModel.ClearRoute();
            }

            void NavRoutingController::OnStartLocationClearClicked(const NavRoutingStartLocationClearClickedMessage& message)
            {
                m_routingModel.ClearStartLocation();
            }

            void NavRoutingController::OnEndLocationClearClicked(const NavRoutingEndLocationClearClickedMessage& message)
            {
                m_routingModel.ClearEndLocation();
            }

            void NavRoutingController::OnStartEndRoutingButtonClicked(const NavRoutingStartEndRoutingButtonClickedMessage& message)
            {
                switch (m_routingModel.GetNavMode())
                {
                    case NavRoutingMode::Active:
                        m_turnByTurnModel.Stop();
                        break;
                    case NavRoutingMode::Ready:
                    {
                        NavRoutingRouteModel routeModel;
                        if(m_routingModel.TryGetRoute(routeModel))
                        {
                            m_turnByTurnModel.Start(routeModel.GetSourceRouteData());
                        }
                        else
                        {
                            m_routingModel.SetNavMode(NavRoutingMode::NotReady);
                        }
                        break;
                    }
                    default:
                        m_routingModel.SetNavMode(NavRoutingMode::NotReady);
                        break;
                }
            }

            void NavRoutingController::OnSelectedDirectionChanged(const NavRoutingSelectedDirectionChangedMessage& message)
            {
                m_routingModel.SetSelectedDirection(message.GetDirectionIndex());
            }

            void NavRoutingController::OnRerouteDialogClosed(const NavRoutingRerouteDialogClosedMessage& message)
            {
                if (message.GetShouldReroute())
                {
                    NavRoutingLocationModel startLocation;

                    if (m_locationFinder.TryGetCurrentLocation(startLocation))
                    {
                        m_routingModel.SetStartLocation(startLocation);
                        m_isRerouting = true;
                    }
                }

                m_turnByTurnModel.Stop();
                m_waitingForRerouteResponse = false;
            }

            void NavRoutingController::OnNavigationMessage(const NavigateToMessage& message)
            {
                NavRoutingLocationModel startLocation, endLocation;
                
                if (!m_locationFinder.TryGetCurrentLocation(startLocation))
                {
                    return;
                }

                if(!m_locationFinder.TryGetLocationFromNavigationMessage(message, endLocation))
                {
                    return;
                }
                
                m_routingModel.SetStartLocation(startLocation);
                m_routingModel.SetEndLocation(endLocation);

                OpenViewWithModel(m_routingModel);
            }

            void NavRoutingController::OpenViewWithModel(INavRoutingModel& routingModel)
            {
                m_messageBus.Publish(NavRoutingViewOpenMessage(routingModel));
            }

            void NavRoutingController::OnShouldReroute()
            {
                if (m_waitingForRerouteResponse)
                {
                    return;
                }

                std::string message = "You seem to be heading away from your destination. Do you want to end navigation?";
                m_messageBus.Publish(NavRoutingShowRerouteDialogMessage(message));
                m_waitingForRerouteResponse = true;
            }
        }
    }
}
