/*
 * Batman.cpp
 *
 *  Created on: Jan 17, 2014
 *      Author: lonnies
 */

#include "Batman.h"
#include <iostream>

void addSun(osg::Group* root) 
{
	osg::ref_ptr<osg::Group> lightGroup (new osg::Group);
	osg::ref_ptr<osg::StateSet> lightSS (root->getOrCreateStateSet());
	osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource;

	// create a local light.
	float xCenter = 2500;
	float yCenter = 2500;
	float zCenter = 15000;


	osg::Vec4f lightPosition (osg::Vec4f(xCenter, yCenter, zCenter, 1.0f));
	osg::ref_ptr<osg::Light> myLight = new osg::Light;
	myLight->setLightNum(1);
	myLight->setPosition(lightPosition);
	myLight->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight->setSpecular(osg::Vec4(1,1,1,1));
	myLight->setConstantAttenuation(1.0f);
	myLight->setDirection(osg::Vec3(0.0f, 0.0f, 1.0f));
	lightSource1->setLight(myLight.get());

	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON); 
	lightSource1->setStateSetModes(*lightSS,osg::StateAttribute::ON);

	lightGroup->addChild(lightSource1.get());

	//Light markers: small spheres
	osg::ref_ptr<osg::Geode> lightMarkerGeode (new osg::Geode);
	lightMarkerGeode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(xCenter,yCenter,zCenter),10.0f)));


	//Tuto 9: lighting code
	root->addChild(lightGroup.get());
	//Tuto 9: Adding the light marker geode
	root->addChild(lightMarkerGeode.get());
}

    /**
     * Turn off batman's bones
     */
    class TurnoffBones : public osg::NodeVisitor
    {

    public:
      TurnoffBones() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ){}

      // This method gets called for every node in the scene
      //   graph. Check each node to see if its name matches
      //   out target. If so, save the node's address.
      virtual void apply( osg::Node& node )
      {
        std::string name = node.getName();
        if (name.find("Bip")!=std::string::npos || name.find("FcFX")!=std::string::npos || name.find("FcFx")!=std::string::npos)
        {
          node.setNodeMask(0x0);
        }
        // Keep traversing the rest of the scene graph.
        traverse( node );
      }


    };
    Manager::Manager()
    {
      const int ReceivesShadowTraversalMask = 0x1;
      const int CastsShadowTraversalMask = 0x2;

      root = new osgShadow::ShadowedScene;

      root->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
      root->setCastsShadowTraversalMask(CastsShadowTraversalMask);

      osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
      root->setShadowTechnique(sm.get());

      int mapres = 4096;
      sm->setTextureSize(osg::Vec2s(mapres,mapres));

      addSun(root);
      std::cout<<"Loading model"<<std::endl;
      batmanNode = osgDB::readNodeFile("Batman/Batman.osg");
      TurnoffBones tob;
      tob.apply(*batmanNode);
      batmanNode->setNodeMask(batmanNode->getNodeMask() & 0x2);
    }

    /**
     *  find the batman model. If one does not exist
     *  create a new one, add it to the sg and return it
     */ 
    boost::shared_ptr<Batman> Manager::getModel(boost::uint32_t id)
    {
      //std::cout<<"getting batman >> "<<id<<std::endl;
      auto iter = map.find(id);
      if(iter==map.end())
      {
        boost::shared_ptr<Batman> b(new Batman(batmanNode, id));
        map[id]=b;
        root->addChild(b->batmanXForm);
        return b;
      }
      else
        return iter->second;
    }

    Manager::~Manager()
    {
      //nothing for now
    }

    Batman::Batman(osg::Node* aBatmanNode, boost::uint32_t aId):id(aId)
    {
      batmanXForm = new osg::PositionAttitudeTransform();
      batmanNode = aBatmanNode;
      batmanXForm->addChild(batmanNode);
    }

    void Batman::setPosition(osg::Vec3 pos)
    {
      batmanXForm->setPosition( pos );
    }

    void Batman::setAttitude(osg::Quat rotation)
    {
      batmanXForm->setAttitude(rotation);
    }

    Batman::~Batman()
    {
      //nothing for now
    }
