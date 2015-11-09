// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "InteriorsExplorerController.h"
#include "InteriorsExplorerViewModel.h"
#include "IInteriorsExplorerView.h"
#include "IMenuViewModel.h"
#include "IScreenControlViewModel.h"
#include "IMyPinCreationInitiationViewModel.h"
#include "ApplyScreenControl.h"
#include "WorldPinVisibility.h"

namespace ExampleApp
{
    namespace InteriorsExplorer
    {
        namespace View
        {
            InteriorsExplorerController::InteriorsExplorerController(IInteriorsExplorerView& view,
                                                                     InteriorsExplorerViewModel& viewModel,
                                                                     ExampleAppMessaging::TMessageBus& messageBus,
                                                                     MyPinCreation::View::IMyPinCreationInitiationViewModel& initiationViewModel,
                                                                     ExampleApp::Menu::View::IMenuViewModel& secondaryMenuViewModel,
                                                                     ExampleApp::Menu::View::IMenuViewModel& searchResultMenuViewModel,
                                                                     ScreenControl::View::IScreenControlViewModel& flattenViewModel,
                                                                     ScreenControl::View::IScreenControlViewModel& compassViewModel,
																	 ScreenControl::View::IScreenControlViewModel& watermarkViewModel)
            : m_view(view)
            , m_viewModel(viewModel)
            , m_messageBus(messageBus)
            , m_initiationViewModel(initiationViewModel)
            , m_secondaryMenuViewModel(secondaryMenuViewModel)
            , m_searchResultMenuViewModel(searchResultMenuViewModel)
            , m_flattenViewModel(flattenViewModel)
            , m_compassViewModel(compassViewModel)
            , m_watermarkViewModel(watermarkViewModel)
            , m_dismissedCallback(this, &InteriorsExplorerController::OnDismiss)
            , m_selectFloorCallback(this, &InteriorsExplorerController::OnSelectFloor)
            , m_stateChangedCallback(this, &InteriorsExplorerController::OnStateChanged)
            , m_floorSelectedCallback(this, &InteriorsExplorerController::OnFloorSelected)
            , m_viewStateCallback(this, &InteriorsExplorerController::OnViewStateChangeScreenControl)
            {
                m_messageBus.SubscribeUi(m_stateChangedCallback);
                m_messageBus.SubscribeUi(m_floorSelectedCallback);
                
                m_viewModel.InsertOnScreenStateChangedCallback(m_viewStateCallback);
                
                m_view.InsertDismissedCallback(m_dismissedCallback);
                m_view.InsertFloorSelectedCallback(m_selectFloorCallback);
            }
        
            InteriorsExplorerController::~InteriorsExplorerController()
            {
                m_view.RemoveDismissedCallback(m_dismissedCallback);
                m_view.RemoveFloorSelectedCallback(m_selectFloorCallback);
                
                m_viewModel.RemoveOnScreenStateChangedCallback(m_viewStateCallback);
                
                m_messageBus.UnsubscribeUi(m_stateChangedCallback);
                m_messageBus.UnsubscribeUi(m_floorSelectedCallback);
            }
            
            void InteriorsExplorerController::OnDismiss()
            {
                m_messageBus.Publish(InteriorsExplorerExitMessage());
                m_view.SetTouchEnabled(false);
            }
            
            void InteriorsExplorerController::OnSelectFloor(int& selected)
            {
                m_messageBus.Publish(InteriorsExplorerSelectFloorMessage(selected));
            }
            
            void InteriorsExplorerController::OnFloorSelected(const InteriorsExplorerFloorSelectedMessage& message)
            {
                std::stringstream content;
                content << "Floor " << (message.GetFloorName());
                m_view.SetFloorName(content.str());
                if(m_viewModel.IsFullyOnScreen())
                {
                    m_view.SetSelectedFloorIndex(message.GetFloorIndex());
                }
            }
            
            void InteriorsExplorerController::OnStateChanged(const InteriorsExplorerStateChangedMessage& message)
            {
                if(message.IsInteriorVisible())
                {
                    m_view.UpdateFloors(message.GetFloorShortNames(), message.GetSelectedFloorIndex());
                    m_view.SetTouchEnabled(true);
                    
                    OnFloorSelected(InteriorsExplorerFloorSelectedMessage(message.GetSelectedFloorIndex(), message.GetSelectedFloorName()));
                    
                    m_viewModel.AddToScreen();
                }
                else
                {
                    m_viewModel.RemoveFromScreen();
                }
            }
            
            void InteriorsExplorerController::OnViewStateChangeScreenControl(ScreenControl::View::IScreenControlViewModel &viewModel, float &state)
            {
                ScreenControl::View::Apply(m_viewModel, m_view);
            }
        }
    }
}
