if(${IOS})
set(headers ${headers} ios_src/NavRouting/View
)
set(currsources
  ios_src/NavRouting/View/INavWidgetViewModule.h
  ios_src/NavRouting/View/NavWidgetView.h
  ios_src/NavRouting/View/NavWidgetView.mm
  ios_src/NavRouting/View/NavWidgetViewIncludes.h
  ios_src/NavRouting/View/NavWidgetViewModule.h
  ios_src/NavRouting/View/NavWidgetViewModule.mm
  ios_src/NavRouting/View/CMakeLists.txt
)
set(sources ${sources} ${currsources})
source_group(\\ios_src\\NavRouting\\View FILES ${currsources})
include(ios_src/NavRouting/View/NavRoutingRerouteDialog/CMakeLists.txt)
endif(${IOS})
