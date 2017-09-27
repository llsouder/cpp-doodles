#include <iomanip>
#include <utility>
#include <queue>
#include <string>

#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/cast.hpp>

#ifdef HAVE_GMTL

#include <gmtl/Vec.h>
#include <gmtl/gmtl.h>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>

#include <tgfframework/ecp/EcoCommunicator.h>
#include <tgfframework/ecp/IdealAircraft.h>
#include <tgfframework/airspace/OptimizedStereoMapProjection.h>
#include <tgfframework/airspace/Position.h>
#include <tgfframework/units/Units.h>
#include <tgfframework/util/ThreadSafeQueue.h>
#include <boost/signal.hpp>

/*
 * EcoSmoothCommunicator.h
 *
 *  Created on: Feb 19, 2014
 *      Author: lonnies
 */

#ifndef ECOSMOOTHCOMMUNICATOR_H_
#define ECOSMOOTHCOMMUNICATOR_H_

using namespace Tgf;
using namespace Airspace;

typedef boost::chrono::steady_clock steady_clock_t;
typedef boost::chrono::steady_clock::time_point timepoint_t;
typedef boost::chrono::steady_clock::duration duration_t;

/**
 * time of the update and the aircraft
 * so I can pull it out based on time.
 */
typedef std::pair<boost::uint64_t, boost::shared_ptr<Tgf::ECP::IdealAircraft>> acinfo_t;

namespace Tgf
{
  namespace ECP
  {
    class EcoSmoothCommunicator
    {
    private:

      boost::signal<void (const IdealAircraft&)> aircraftSignal;
      /**
       * ECO start message simulation time
       */
      boost::uint64_t sim_start;

      /**
       * subtract offset from steady clock to find simulation time
       */
      timepoint_t time_offset;


      /**
       * delay in milliseconds
       */
      boost::uint64_t m_delay;

      boost::asio::io_service io;
      boost::scoped_ptr<boost::thread> starttimer_thread;
      boost::scoped_ptr<boost::asio::deadline_timer> timer;

      /**
       * only way to know if we are waiting to start
       */
      bool running;

      /**
       * did we get a start message
       */
      bool start_msg_recv;

      struct AircraftData
       {
         std::queue<acinfo_t> queue;

         //where you were
         acinfo_t acinfo1;

         //where you want to be
         acinfo_t acinfo2;
       };

      typedef std::map<uint16_t, AircraftData*> aircraftDataMap_t;
      /**
       * all the aircraft in the simulation mapped by id
       */
      aircraftDataMap_t data;


    public:
      EcoSmoothCommunicator(int delay);
      virtual ~EcoSmoothCommunicator(){};


      /**
       * interpolate aircraft position and rotation and
       * fire an aircraft signal
       */
      std::list<boost::shared_ptr<IdealAircraft>> updateAllAircraft();

      /**
       * what sim time are we at?
       */
      boost::uint64_t ig_simTime();

      /**
       * store new position to pair with epoch
       */
      void aircraftUpdateReceived(const Tgf::ECP::IdealAircraft& iac);

      void startIG(const boost::system::error_code& error);
      void resetUpdate();

      void startSim(boost::uint64_t startTime);
      void pauseSim(boost::uint64_t time);

      /**
       * util to get position from IdealAircraft in gmtl::Vec3d format
       */
      gmtl::Vec3d getVecPosition(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac);
      gmtl::Vec3d getVecRotation(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac);

      Position convertFrom(gmtl::Vec3d pos);

      void getEulerFromQuat(osg::Quat q, double& roll, double& pitch, double& yaw);

      boost::signal<void (const IdealAircraft&)>& getAircraftSignal();
    };

  } /* namespace ECP */
} /* namespace Tgf */
#endif /* ECOSMOOTHCOMMUNICATOR_H_ */
#endif /* HAVE_GMTL */
