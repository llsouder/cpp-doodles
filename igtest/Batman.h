/*
 * Batman.h
 *
 *  Created on: Jan 17, 2014
 *      Author: lonnies
 */

#ifndef BATMAN_H_
#define BATMAN_H_

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/integer.hpp>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osg/ShapeDrawable> 
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osg/Quat>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osg/Quat>

    class Batman
    {
    private:
      boost::uint32_t id;

    public:
      Batman(osg::Node* aBatmanNode, boost::uint32_t aId);
      void setPosition(osg::Vec3 position);
      void setAttitude(osg::Quat rotation);
      void setInitalRotation(osg::Quat rot);

      osg::Node* batmanNode;
      osg::PositionAttitudeTransform* batmanXForm;
      virtual ~Batman();
    };

    typedef std::map<boost::uint32_t, boost::shared_ptr<Batman> > mgrmap_t;

    class Manager
    {
    private:
      osg::Node* batmanNode;
      mgrmap_t map;
    public:
     // osg::Group* root;
      osg::ref_ptr<osgShadow::ShadowedScene> root;
      Manager();
      boost::shared_ptr<Batman> getModel(boost::uint32_t id);
      virtual ~Manager();
    };

#endif /* BATMAN_H_ */
