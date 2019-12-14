add_library(
	ITKSnap
    ColorWheel.cxx
    QColorButtonWidget.cxx
    QtHelper.cpp
    qaesencryption.cpp
)

qt5_use_modules(ITKSnap  Core Gui Widgets)