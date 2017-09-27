/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This application is open source and may be redistributed and/or modified   
 * freely and without restriction, both in commericial and non commericial applications,
 * as long as this copyright notice is maintained.
 * 
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osgText/Text>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/AutoTransform>
#include <osg/BlendFunc>
#include <osg/PositionAttitudeTransform>
#include <osg/LightSource>
#include <osg/LOD>
#include <osg/Material>
#include <osg/io_utils>
#include <osg/Shader>
#include <osg/Program>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <iostream>
#include <osgSim/LightPoint>
#include <osgSim/LightPointNode>
#include <osgSim/BlinkSequence>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgTerrain/Layer>
#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgTerrain/GeometryTechnique>
#include <osgTerrain/Locator>
#include <tgfframework/osg-util/DeferredMaterialsLibrary.h>
#include <osg/Light>
#include <osg/LightSource>

#include <cfloat>
#include <stdexcept>

using namespace osg;
int uniqueLightNumber = 0;
int const LIGHTS = 3;

PositionAttitudeTransform *cubeTransform;
PositionAttitudeTransform *lightTransform[LIGHTS];
StateSet *lightStateSet;

Light *createLight(Vec4 color)
{	
	Light *light = new Light();
	// each light must have a unique number
	light->setLightNum(uniqueLightNumber++);
	// we set the light's position via a PositionAttitudeTransform object
	light->setPosition(Vec4(0.0, 0.0, 0.0, 1.0));
	light->setDiffuse(color);
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient( Vec4(0.0, 0.0, 0.0, 1.0));

	return light;
}

Material *createSimpleMaterial(Vec4 color)
{
	Material *material = new Material();
	material->setDiffuse(Material::FRONT,  Vec4(0.0, 0.0, 0.0, 1.0));
	material->setEmission(Material::FRONT, color);

	return material;
}


// Derive a class from NodeVisitor to find a node with a
//   specific name.
class FindNamedNode : public osg::NodeVisitor
{
public:
    FindNamedNode( const std::string& name )
      : osg::NodeVisitor( // Traverse all children.
                osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
        _name( name ) {}

    // This method gets called for every node in the scene
    //   graph. Check each node to see if its name matches
    //   out target. If so, save the node's address.
    virtual void apply( osg::Node& node )
    {
        if (node.getName() == _name)
        {
           std::cout<<"found it!"<<std::endl;
            _node = &node;
        }

        // Keep traversing the rest of the scene graph.
        traverse( node );
    }

    osg::Node* getNode() { return _node.get(); }

protected:
    std::string _name;
    osg::ref_ptr<osg::Node> _node;
};

int main(int argc, char** argv)
{
  osg::Group* root = new osg::Group();
  //osg::Sphere* sphere = new osg::Sphere(osg::Vec3(0,0,0), 1.0);
  //{
    //osg::ShapeDrawable* drawableUnit = new osg::ShapeDrawable(sphere);
    //drawableUnit->setColor(osg::Vec4(1.0, 0,0, 1.0));
    //osg::Geode* basicShapesGeode = new osg::Geode();
    //basicShapesGeode->addDrawable(drawableUnit);

    //root->addChild(basicShapesGeode);
  //}
  static osg::ref_ptr<osg::Node> defaultModel = new osg::Group;

  osg::Geode* geode = new osg::Geode;
  defaultModel->asGroup()->addChild(geode);

  osg::Geometry* geometry = new osg::Geometry;
  geode->addDrawable(geometry);

  osg::Vec3Array* vertex = new osg::Vec3Array;
  geometry->setVertexArray(vertex);

  osg::Vec3 v1(0, 0, 10);
  osg::Vec3 v2(0, 10, 0);
  osg::Vec3 v3(10, 0, 0);
  osg::Vec3 v4(0, -10, 0);
  osg::Vec3 v5(-10, 0, 0);

  vertex->push_back(v1);
  vertex->push_back(v2);
  vertex->push_back(v3);

  vertex->push_back(v1);
  vertex->push_back(v3);
  vertex->push_back(v4);

  vertex->push_back(v1);
  vertex->push_back(v4);
  vertex->push_back(v5);

  vertex->push_back(v1);
  vertex->push_back(v5);
  vertex->push_back(v2);

  vertex->push_back(v2);
  vertex->push_back(v5);
  vertex->push_back(v3);

  vertex->push_back(v4);
  vertex->push_back(v3);
  vertex->push_back(v5);

  osg::Material* mat = new osg::Material;
  mat->setSpecular(osg::Material::FRONT, osg::Vec4(1.0, 0.0, 0.0, 1.0));
  mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0, 0.0, 0.0, 1.0));
  mat->setAmbient(osg::Material::FRONT, osg::Vec4(1.0, 0.0, 0.0, 1.0));
  mat->setShininess(osg::Material::FRONT, 0.0);

  osg::StateSet *stateset = defaultModel->getOrCreateStateSet();
  stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
  stateset->setAttribute(mat);
  stateset->setTextureAttributeAndModes(0, new osg::Texture2D(), osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

  geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vertex->size()));
  geometry->dirtyDisplayList();
  geometry->dirtyBound();

  //defaultModel->setNodeMask(TranslucentMask);
  osgDeferred::IDeferredMaterialsLibrary::instance()->traverseAndDecorateTree(defaultModel);

// Declare and initialize a transform node.
   osg::PositionAttitudeTransform* posXForm =
      new osg::PositionAttitudeTransform();

//create a second sphere node
  //osg::ShapeDrawable* drawableUnit = new osg::ShapeDrawable(defaultModel);
  //drawableUnit->setColor(osg::Vec4(1.0, 0,1, 1.0));
  //osg::Geode* basicShapesGeode = new osg::Geode();
  //basicShapesGeode->addDrawable(drawableUnit);
// Use the 'addChild' method of the osg::Group class to
// add the transform as a child of the root node and the
// pyramid node as a child of the transform.
   root->addChild(posXForm);
   //posXForm->addChild(basicShapesGeode);
   posXForm->addChild(defaultModel);

// Declare and initialize a Vec3 instance to change the
// position of the tank model in the scene
   //osg::Vec3 sphereTwoPosition(15,0,0);
   //posXForm->setPosition( sphereTwoPosition );

  Vec4 lightColors[] = {Vec4(1.0, 0.0, 0.0, 1.0), Vec4(0.0, 1.0, 0.0, 1.0), Vec4(0.0, 0.0, 1.0, 1.0)};

  Geode *lightMarker[LIGHTS];
  LightSource *lightSource[LIGHTS];
  lightStateSet = root->getOrCreateStateSet();
 
   for (int i = 0; i < LIGHTS; i++) {
        lightMarker[i] = new Geode();
        lightMarker[i]->addDrawable(new ShapeDrawable(new Sphere(Vec3(), 1)));
        lightMarker[i]->getOrCreateStateSet()->setAttribute(createSimpleMaterial(lightColors[i]));
 
        lightSource[i] = new LightSource();
        lightSource[i]->setLight(createLight(lightColors[i]));
        lightSource[i]->setLocalStateSetModes(StateAttribute::ON);
        lightSource[i]->setStateSetModes(*lightStateSet, StateAttribute::ON);
 
        lightTransform[i] = new PositionAttitudeTransform();
        lightTransform[i]->addChild(lightSource[i]);
        lightTransform[i]->addChild(lightMarker[i]);
        lightTransform[i]->setPosition(Vec3(0, 0, 5));
        lightTransform[i]->setScale(Vec3(0.1,0.1,0.1));
 
        root->addChild(lightTransform[i]);
    }

  osgViewer::Viewer viewer;

  viewer.setSceneData( root );

  viewer.run();
}
