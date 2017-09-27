/*
 * DateTimeControls.h
 *
 *  Created on: Dec 12, 2011
 *      Author: schnurrj
 */

#ifndef DATETIMECONTROLS_H_
#define DATETIMECONTROLS_H_

#include <boost/signals.hpp>
#include "src/core/Controller.h"
#include <tgfframework/util/ThreadSafeQueue.h>
#include <QDockWidget>
#include "ui_DateTimeControls.h"

namespace Tgf {
namespace vaie {
namespace controller {
namespace gui {


class DateTimeControls : public QDockWidget, public Ui::DateTimeControls {
    Q_OBJECT

    private:
        std::vector<boost::signals::connection> controllerConnections;

        struct DateTimeStrings {
                std::string date;
                std::string time;
        };
        Util::ThreadSafeQueue<DateTimeStrings> dateTimeStrings;
        bool fastTimeEnabled;


        void initialize(InitializationController& controller);
        void update(UpdateController& controller, const boost::posix_time::time_duration &);
        void shutdown();
        void updateLocalTime(const boost::local_time::local_date_time& localTime);

    protected:
        void customEvent(QEvent*);

    public:
        DateTimeControls(ConfigurationController& controller, QWidget* parent = NULL, Qt::WindowFlags flags = 0);
        virtual ~DateTimeControls();

    public Q_SLOTS:
        void setDate();
        void fastTime();
};


}
}
}
}

#endif 
