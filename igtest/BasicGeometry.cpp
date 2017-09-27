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

#include <gmtl/Vec.h>
#include <gmtl/gmtl.h>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>

#include <tgfframework/ecp/EcoCommunicator.h> 
#include <tgfframework/ecp/IdealAircraft.h> 
#include <tgfframework/airspace/OptimizedStereoMapProjection.h>
#include <tgfframework/airspace/Position.h>
#include <tgfframework/units/Units.h>
#include <tgfframework/util/ThreadSafeQueue.h>

#include "Batman.h"

using namespace Tgf;
using namespace Airspace;

typedef boost::chrono::steady_clock steady_clock_t;
typedef boost::chrono::steady_clock::time_point timepoint_t;
typedef boost::chrono::steady_clock::duration duration_t;

typedef std::pair<boost::uint64_t, boost::shared_ptr<Tgf::ECP::IdealAircraft>> acinfo_t;

/**
 * Delayed interpolated updates of the aircraft.
 * Allow the simulation to get ahead of the ig then
 * use updates to interpolate smoothly.
 */
class Updater
{
private:

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
   * used to transform from simulation coords to DB coords
   */
  OptimizedStereoMapProjection* m_stereoMap;

  /**
   * only way to know if we are waiting to start
   */
  bool running;

  struct AircraftData
   {
     std::queue<acinfo_t> queue;
     acinfo_t acinfo1;
     acinfo_t acinfo2;
   };
  typedef std::map<uint16_t, AircraftData*> aircraftDataMap_t;
  aircraftDataMap_t data;


public:

  Manager manager;

  Updater(int delay):running(false), m_delay(delay), m_stereoMap(new OptimizedStereoMapProjection(Position(Units::Latitude(47.94434, Units::deg()), Longitude(-97.4008, Units::deg()))))
  {
    //center of the bos_ground/flights/circle2.fpx flight
  }

  /**
   * get the aircraft location, convert and put the results
   * into the transform to move the aircraft.
   */
  void update_position()
  {
    if(!running)
      return;

    if(data.empty())
    {
      //std::cout<<"EMPTY!!!!!"<<std::endl;
      return;
    }

    boost::uint64_t simTime = ig_simTime();

    //for each thing in data do this:
    for(auto dataIter=data.begin();dataIter!=data.end();dataIter++)
    {
      AircraftData* ad  = dataIter->second;
      //std::cout<<"ig simTime = "<<simTime/1000000<<std::endl;
      while(ad->acinfo2.first<simTime&&!ad->queue.empty())
      {
        //std::cout<<"      acinfo2 = "<<acinfo2.first/1000000<<std::endl;
        ad->acinfo1=ad->acinfo2;
        ad->acinfo2 = ad->queue.front();
        ad->queue.pop();
      }
      if(ad->queue.empty())
      {
        //std::cout<<"      WE EMPTIED the QUEUE"<<std::endl;
      }

      if(ad->acinfo2.first==0 || ad->acinfo1.first==0)
      {
        static int x=0;
        //std::cout<<x++<<" acinfos reset"<<std::endl;
        return;
      }
      else
      {
        //std::cout<<"**************** NOT acinfos reset ****************"<<std::endl;
      }

      boost::shared_ptr<Batman> b = manager.getModel(ad->acinfo1.second->getEntityID());
      //interpolate

      boost::uint64_t total = ad->acinfo2.first-ad->acinfo1.first;
      if(total==0)
        return;
      double tp = (simTime-ad->acinfo1.first)/(double)total;
      gmtl::Vec3d pos1 = getVecPosition(ad->acinfo1.second);
      gmtl::Vec3d pos2 = getVecPosition(ad->acinfo2.second);
      gmtl::Vec3d vec3d;
      gmtl::lerp( vec3d, tp, pos1, pos2);
      Position p = convertFrom(vec3d);
      vec3d = getDBCoords(p);
      osg::Vec3 pyramidTwoPosition(vec3d[0],vec3d[1],vec3d[2]);

      gmtl::Vec3d rot1 = getVecRotation(ad->acinfo1.second);
      gmtl::Vec3d rot2 = getVecRotation(ad->acinfo2.second);
      gmtl::Vec3d rot3d;
      gmtl::lerp( rot3d, tp, rot1, rot2);
      osg::Quat rotation(rot3d[2],  osg::Vec3(0,1,0),  //roll
          rot3d[1],  osg::Vec3(1,0,0),      //pitch
          -rot3d[0]+PI(),  osg::Vec3(0,0,1));  //yaw

      b->setAttitude(rotation);
      b->setPosition( pyramidTwoPosition );

      Tgf::ECP::UnifiedAircraft::Builder newdata;
      setVecPosition(newdata, vec3d);

      boost::shared_ptr<Tgf::ECP::UnifiedAircraft> olddata = boost::shared_polymorphic_downcast<Tgf::ECP::UnifiedAircraft>(ad->acinfo1.second->clone());
      olddata->merge(newdata);
      //if(ad->acinfo1.second->getAcid()=="USA6594")
      //{
      //  std::cout<<ad->acinfo1.second->getAcid()<<" "<<ad->acinfo1.second->getYaw().as(Units::deg())<<std::endl;
      //}
    }//end of data loop
  }

  /**
   * what sim time are we at?
   */
  boost::uint64_t ig_simTime()
  {
    steady_clock_t::duration ig_time = steady_clock_t::now() - time_offset;
    return (ig_time.count());
  }
  /**
   * store new position to pair with epoch
   */
  void aircraftUpdateReceived(const Tgf::ECP::IdealAircraft& iac)
  {
    boost::shared_ptr<Tgf::ECP::IdealAircraft> tmp = iac.clone();
    acinfo_t tmppair = std::make_pair(tmp->getTimeStamp(), tmp);
    auto findItr = data.find(tmppair.second->getEntityID());
    AircraftData* ad;
    if(findItr==data.end())
    {
      //create new data
      ad = new AircraftData;
      data[tmppair.second->getEntityID()]=ad;
    }
    else
    {
      ad = findItr->second;
    }
    //std::cout<<"id = "<<tmppair.second->getEntityID()<<" "<<ad->queue.size()<<std::endl;
    ad->queue.push(tmppair);
  }

  void startIG(const boost::system::error_code& error)
  {
    timepoint_t timenow = steady_clock_t::now();
    time_offset = timenow-boost::chrono::nanoseconds(sim_start);

    std::cout<<"startIG!"<<time_offset<<std::endl;
    running=true;
  }

  void resetUpdate()
  {
    running=false;
    //clear the queue
    aircraftDataMap_t empty;
    std:swap(data, empty);
  }

  void startSim(boost::uint64_t startTime)
  {
    sim_start = startTime;
    resetUpdate();
    io.reset();
    // boost 1.49 or later, ASIO introduced the basic_waitable_timer
    std::cout<<"start timer:"<<steady_clock_t::now()<<std::endl;
    timer.reset(new boost::asio::deadline_timer(io, boost::posix_time::millisec(m_delay)));
    timer->async_wait(boost::bind(&Updater::startIG, this, _1));
    starttimer_thread.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &io)));
  }

  void pauseSim(boost::uint64_t time)
  {
    std::cout<<"PAUSED "<<time/1000000<<std::endl;
    resetUpdate();
  }

  void updateSim(boost::uint64_t time)
  {
    //std::cout<<"Update "<<time/1000000<<std::endl;
  }

  /**
   * convert from simulation position to the 3d DB coords.
   */
  gmtl::Vec3d getDBCoords(const Position &pos) const
  {
    Tgf::Units::Coordinate result = m_stereoMap->latLonToXYCoordinate(pos);
    return (gmtl::Vec3d(result.getXCoordinate(Tgf::Units::m()),
        result.getYCoordinate(Tgf::Units::m()),
        pos.getAltitude().as(Tgf::Units::m())));
  }

  /**
   * util to get position from IdealAircraft in gmtl::Vec3d format
   */
  gmtl::Vec3d getVecPosition(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac)
  {
    gmtl::Vec3d result;
    result[0] = iac->getLatitude().as(Units::deg());
    result[1] = iac->getLongitude().as(Units::deg());
    result[2] = iac->getAltitude().as(Units::m());
    return (result);
  }
  /**
    * util to get position from IdealAircraft in gmtl::Vec3d format
    */
   gmtl::Vec3d getVecRotation(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac)
   {
     gmtl::Vec3d result;
     result[0] = iac->getYaw().as(Units::rad());
     result[1] = iac->getPitch().as(Units::rad());
     result[2] = iac->getRoll().as(Units::rad());
     return (result);
   }
  /**
   * util to put position back into IdealAircraft from gmtl::Vec3d format
   */
  void setVecPosition(Tgf::ECP::UnifiedAircraft::Builder& iac, gmtl::Vec3d pos)
  {
    iac.latitude(Units::Latitude(pos[0], Units::deg()));
    iac.longitude(Units::Longitude(pos[1], Units::deg()));
    iac.altitude(Units::Altitude(pos[2], Units::m()));
  }

  /**
   * util to get position from IdealAircraft in gmtl::Vec3d format
   */
  Position getPosition(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac)
  {
    return ( Position(iac->getLatitude(), iac->getLongitude(), iac->getAltitude()) );
  }

  Position convertFrom(gmtl::Vec3d pos)
  {
    return (Position(Latitude(pos[0], Tgf::Units::deg()), Longitude(pos[1], Tgf::Units::deg()), Altitude(pos[2], Tgf::Units::m())) );
  }

};

int main(int argc, char* argv[])
{
  Tgf::ECP::EcoCommunicator eco(40);
  Updater updater(200);
  eco.getAircraftSignal().connect(boost::bind(&Updater::aircraftUpdateReceived, &updater, _1));
  eco.getSimStartDiscreteSignal().connect(boost::bind(&Updater::startSim, &updater, _1));
  eco.getSimTimeUpdateSignal().connect(boost::bind(&Updater::updateSim, &updater, _1));
  eco.getSimPauseDiscreteSignal().connect(boost::bind(&Updater::pauseSim, &updater, _1));
  std::cout<<"Loading Airport model"<<std::endl;
  //tims!  this was krdr/KRDR.osg instead of KRDR/KRDR... doh!
  osg::Node* airportNode = osgDB::readNodeFile("/vsim/Terrain/KRDR/KRDR.osg");

  osgViewer::Viewer viewer;
  viewer.addEventHandler (new osgViewer::StatsHandler ());
  viewer.setUpViewInWindow(100,100, 500,500);

  updater.manager.getModel(0);

  // switch off lighting as we haven't assigned any normals.
  //updater.manager.root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
  updater.manager.root->addChild(airportNode);
  airportNode->setNodeMask(airportNode->getNodeMask() & 0x2);

  //The final step is to set up and enter a simulation loop.
  //viewer.setSceneData( updater.manager.root );
  viewer.setSceneData(updater.manager.root );

  viewer.setCameraManipulator(new osgGA::TrackballManipulator());
  viewer.realize();
  // Init the GraphicsContext Traits.
  osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

  // Setup the traits parameters.
  traits->samples                       = 4; // to make anti-aliased.

  eco.startComms();
  //  app.exec();
  while( !viewer.done() )
  {
    viewer.frame();
    updater.update_position();
  }
  return (0);
}
