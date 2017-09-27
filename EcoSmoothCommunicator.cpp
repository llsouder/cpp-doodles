0/*
 * EcoSmoothCommunicator.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: lonnies
 */

#ifdef HAVE_GMTL

#include "EcoSmoothCommunicator.h"

namespace Tgf
{
  namespace ECP
  {
    /**
     * delay argument is in milliseconds.
     */
    EcoSmoothCommunicator::EcoSmoothCommunicator(int delay_ms):running(false),
        start_msg_recv(false),
        m_delay(delay_ms)
    {

    }

    /**
     * what sim time are we at?
     */
    boost::uint64_t EcoSmoothCommunicator::ig_simTime()
    {
      steady_clock_t::duration ig_time = steady_clock_t::now() - time_offset;
      return ig_time.count();
    }

    /**
     * store new position to pair with epoch
     */
    void EcoSmoothCommunicator::aircraftUpdateReceived(const Tgf::ECP::IdealAircraft& iac)
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
      if(!start_msg_recv)
        startSim(tmp->getTimeStamp());
    }

    void EcoSmoothCommunicator::startIG(const boost::system::error_code& error)
    {
      timepoint_t timenow = steady_clock_t::now();
      time_offset = timenow-boost::chrono::nanoseconds(sim_start);

      std::cout<<"startIG!"<<time_offset<<std::endl;
      running=true;
    }

    void EcoSmoothCommunicator::resetUpdate()
    {
      running=false;
      start_msg_recv=false;
      //clear the queue
      aircraftDataMap_t empty;
      std:swap(data, empty);
    }

    void EcoSmoothCommunicator::startSim(boost::uint64_t startTime)
    {
      sim_start = startTime;
      resetUpdate();
      start_msg_recv=true;
      io.reset();
      // boost 1.49 or later, ASIO introduced the basic_waitable_timer
      std::cout<<"start timer:"<<steady_clock_t::now()<<std::endl;
      timer.reset(new boost::asio::deadline_timer(io, boost::posix_time::millisec(m_delay)));
      timer->async_wait(boost::bind(&EcoSmoothCommunicator::startIG, this, _1));
      starttimer_thread.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &io)));
    }

    void EcoSmoothCommunicator::pauseSim(boost::uint64_t time)
    {
      std::cout<<"PAUSED "<<time/1000000<<std::endl;
      resetUpdate();
    }

    /**
     * util to get position from IdealAircraft in gmtl::Vec3d format
     */
    gmtl::Vec3d EcoSmoothCommunicator::getVecPosition(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac)
    {
      gmtl::Vec3d result;
      result[0] = iac->getLatitude().as(Units::deg());
      result[1] = iac->getLongitude().as(Units::deg());
      result[2] = iac->getAltitude().as(Units::m());
      return (result);
    }

    gmtl::Vec3d EcoSmoothCommunicator::getVecRotation(boost::shared_ptr<Tgf::ECP::IdealAircraft> iac)
    {
      gmtl::Vec3d result;
      result[0] = iac->getYaw().as(Units::deg());
      result[1] = iac->getPitch().as(Units::deg());
      result[2] = iac->getRoll().as(Units::deg());
      return (result);
    }
    Position EcoSmoothCommunicator::convertFrom(gmtl::Vec3d pos)
    {
      return ( Position(Latitude(pos[0], Tgf::Units::deg()), Longitude(pos[1], Tgf::Units::deg()), Altitude(pos[2], Tgf::Units::m())) );
    }

    //    P.S: getQuatFromEuler is not neccessary. OSG's Matrix class has a public method 'makeRotate(angle,axis)' which
    //
    //    does the conversion

    void EcoSmoothCommunicator::getEulerFromQuat(osg::Quat q, double& roll, double& pitch, double& yaw)
    {
      double limit = 0.499999;

      double sqx = q.x()*q.x();
      double sqy = q.y()*q.y();
      double sqz = q.z()*q.z();

      double t = q.x()*q.y() + q.z()*q.w();

      if (t>limit) // gimbal lock ?
      {
        yaw = 2 * atan2(q.x(),q.w());
        pitch = osg::PI_2;
        roll = 0;
      }
      else if (t<-limit)
      {
        yaw = -2 * atan2(q.x(),q.w());
        pitch = - osg::PI_2;
        roll = 0;
      }
      else
      {
        yaw = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz);
        pitch = asin(2*t);
        roll = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz);
      }
    }

    boost::signal<void (const IdealAircraft&)>& EcoSmoothCommunicator::getAircraftSignal()
    {
      return aircraftSignal;
    }
    /**
     * interpolate aircraft position and rotation and
     * fire an aircraft signal
     */
    std::list<boost::shared_ptr<IdealAircraft>> EcoSmoothCommunicator::updateAllAircraft()
    {
      std::list<boost::shared_ptr<IdealAircraft>> list;

      if(!running)
        return list;

      if(data.empty())
      {
        //std::cout<<"EMPTY!!!!!"<<std::endl;
        return list;
      }
      gmtl::Vec3d newPos;

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
          return list;
        }
        else
        {
          //std::cout<<"**************** NOT acinfos reset ****************"<<std::endl;
        }

        //interpolate
        gmtl::Vec3d pos1 = getVecPosition(ad->acinfo1.second);
        gmtl::Vec3d pos2 = getVecPosition(ad->acinfo2.second);
        boost::uint64_t total = ad->acinfo2.first-ad->acinfo1.first;
        if(total==0)
          continue;
        //time percent between 0.0 - 1.0
        double tp = (simTime-ad->acinfo1.first)/(double)total;

        gmtl::lerp( newPos, tp, pos1, pos2);

        gmtl::Vec3d rot1 = getVecRotation(ad->acinfo1.second);
        gmtl::Vec3d rot2 = getVecRotation(ad->acinfo2.second);
        gmtl::Vec3d rot;
        rot[0]=rot1[0]+((rot2[0]-rot1[0])*tp);
        rot[1]=rot1[1]+((rot2[1]-rot1[1])*tp);
        rot[2]=rot1[2]+((rot2[2]-rot1[2])*tp);


        Tgf::ECP::UnifiedAircraft::Builder newdata;
        boost::shared_ptr<Tgf::ECP::UnifiedAircraft> olddata = boost::shared_polymorphic_downcast<Tgf::ECP::UnifiedAircraft>(ad->acinfo1.second->clone());
        newdata.quality(3);
        newdata.latitude(Units::Latitude(newPos[0], Units::deg()));
        newdata.longitude(Units::Longitude(newPos[1], Units::deg()));
        newdata.altitude(Units::Altitude(newPos[2], Units::m()));
        newdata.yaw(Units::Angle(rot[0], Units::deg()));
        newdata.pitch(Units::Angle(rot[1], Units::deg()));
        newdata.roll(Units::Angle(rot[2], Units::deg()));
        olddata->merge(newdata);
        list.push_back(olddata);
        aircraftSignal(*olddata);

        // b->setAttitude(result);
        // b->setPosition( pyramidTwoPosition );
      }//end of data loop
      return list;
    }
    ///////////////////////
  } /* namespace ECP */
} /* namespace Tgf */
#endif /* HAVE_GMTL */
