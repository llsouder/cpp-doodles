/*
 * DateTimeControls.cpp
 *
 *  Created on: Dec 12, 2011
 *      Author: schnurrj
 */

#include "DateTimeControls.h"
#include "src/core/Configuration.h"
#include <tgfframework/util/TgfLogging.h>
#include <tgfframework/qt-util/CustomEvent.h>


CUSTOM_GUI_DATA_EVENT(LocalTimeUpdatedEvent, QEvent::User, boost::local_time::local_date_time)


namespace Tgf {
namespace vaie {
namespace controller {
namespace gui {


DateTimeControls::DateTimeControls(ConfigurationController& controller, QWidget* parent, Qt::WindowFlags flags)
 : QDockWidget(parent, flags),
   Ui::DateTimeControls(),
   fastTimeEnabled(false)
{
    setupUi(this);
    fastTimeLineEdit->setValidator(new QIntValidator());

    controller.getInitializeSignal().connect(boost::bind(&DateTimeControls::initialize, this, _1));
    controller.getUpdateSignal().connect(boost::bind(&DateTimeControls::update, this, _1, _2));
    controller.getLocalTimeUpdatedSignal().connect(boost::bind(&DateTimeControls::updateLocalTime, this, _1));
}

DateTimeControls::~DateTimeControls() { }

void DateTimeControls::customEvent(QEvent* event) {
    switch(event->type()) {
        case LocalTimeUpdatedEvent::ID : {
            std::string dateTimeString = to_iso_extended_string(dynamic_cast<LocalTimeUpdatedEvent*>(event)->getData().local_time());

            char dateBuffer[11];
            int dateLen = 0;

            char timeBuffer[10];
            int timeLen = 0;

            //
            // dateTimeString is in the format yyyy-mm-ddThh:mm:ss,fractal_seconds
            //
            // we want YYYY-MM-DD in dateBuffer
            // and HH:MM:SS int timeBuffer
            //
            // we need to skip the T which seperates the date and time and we
            // can ignore the fractal seconds
            //

            dateLen = dateTimeString.copy(dateBuffer, 10, 0);
            dateBuffer[dateLen] = '\0';

            timeLen = dateTimeString.copy(timeBuffer, 8, 11);
            timeBuffer[timeLen] = '\0';

            date_edit->setText(QString(dateBuffer));
            time_edit->setText(QString(timeBuffer));
        }
    }
}

void DateTimeControls::initialize(InitializationController& controller) {
    updateLocalTime(controller.getLocalTime());
}

void DateTimeControls::update(UpdateController& controller, const boost::posix_time::time_duration &dt) {
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    using namespace boost::local_time;

    bool setDateTime = false;
    DateTimeStrings strings;
    while(dateTimeStrings.dataAvailable()) {
        strings = dateTimeStrings.getData();
        setDateTime = true;
    }

    if(setDateTime) {
        date d;
        time_duration tod;

        try {
            d = from_simple_string(strings.date);
            tod = duration_from_string(strings.time);
        }
        catch(std::exception& e) {
            Util::Log::Error("vaie::controller::gui::DateTimeControls") << "Unable To Parse Date/Time: " << e.what() << std::endl;
            return;
        }

        controller.setLocalTime(local_date_time(ptime(d,tod),Configuration::GMT()));
    }

    if(fastTimeEnabled) {
        int minPerSec = fastTimeLineEdit->text().toInt();
        float timeRatio = (float)(minPerSec) * 60.0;
        long msec = dt.total_milliseconds() * timeRatio;
        try {
            controller.setLocalTime(controller.getLocalTime() + milliseconds(msec));
        }
        catch(std::exception& e) {
            Tgf::Util::Log::Error("vaie::controller::gui::DateTimeControls") << e.what() << std::endl;
            return;
        }
    }
}

void DateTimeControls::updateLocalTime(const boost::local_time::local_date_time& localTime) {
    QApplication::postEvent(this, new LocalTimeUpdatedEvent(localTime));
}

void DateTimeControls::setDate() {
    DateTimeStrings strings;
    strings.date = date_edit->text().toStdString();
    strings.time = time_edit->text().toStdString();

    dateTimeStrings.addData(strings);
}

void DateTimeControls::fastTime() {
    fastTimeEnabled = !fastTimeEnabled;
    updateInFastTimeButton->setText((fastTimeEnabled ? "Stop Fast Time" : "Start Fast Time"));
}


}
}
}
}
