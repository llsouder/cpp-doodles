/*
 * dateTimeControlsPlugin.cpp
 *
 *  Created on: Dec 12, 2011
 *      Author: schnurrj
 */


#include "Plugin.h"
#include "Controller.h"
#include "DateTimeControls.h"
#include "src/gui/ControllerMainWindow.h"

namespace Tgf {
namespace vaie {
namespace controller {


namespace {
    std::string getPluginDescription() {
        return "Creates a QDockWidget GUI component to allow the user to control the simulated date, time of day as well as \"fast time\" controls.";
    }
}


class DateTimeControlsPlugin : public Plugin {
    public:
        DateTimeControlsPlugin() : Plugin("Date Time Controls", getPluginDescription()) { }
        virtual ~DateTimeControlsPlugin() { }

        virtual void configure(ConfigurationController& controller) { }

        virtual void configure(ConfigurationController& controller, gui::ControllerMainWindow* mainWindow) {
            mainWindow->addPluginDockWidget(new gui::DateTimeControls(controller, mainWindow), Qt::BottomDockWidgetArea);
        }
};

}
}
}

VAIE_PLUGIN(Tgf::vaie::controller::DateTimeControlsPlugin)

