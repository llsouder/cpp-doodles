#include <iomanip>
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
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osg/ShapeDrawable> 
#include <boost/chrono.hpp>
#include <osgDB/ReadFile> 
#include <tgfframework/ecp/EcoCommunicator.h> 
#include <tgfframework/ecp/IdealAircraft.h> 
#include <tgfframework/airspace/OptimizedStereoMapProjection.h>
#include <tgfframework/airspace/Position.h>
#include <tgfframework/units/Units.h>
#include <gmtl/Vec.h>

using namespace Tgf;
using namespace Airspace;

typedef boost::chrono::steady_clock steady_clock_t;
typedef boost::chrono::steady_clock::time_point timepoint_t;
typedef boost::chrono::steady_clock::duration duration_t;

timepoint_t t1 = steady_clock_t::now();
boost::chrono::nanoseconds total(15000000); //15 milliseconds
float x, y;
int length = 500;
float angle = 0.0;
float angle_stepsize = 0.025;

class Updater
{
public:
int last; 
osg::PositionAttitudeTransform* batmanXForm;
OptimizedStereoMapProjection* m_stereoMap;
Updater():batmanXForm( new osg::PositionAttitudeTransform()),m_stereoMap(new OptimizedStereoMapProjection(Position(Units::Latitude(42.36248, Units::deg()),
								Longitude(-70.99905, Units::deg())))),last(steady_clock_t::now().time_since_epoch().count())
{
  //center of the bos_ground/flights/circle2.fpx flight
}

void update_positions()
{
	duration_t diff(steady_clock_t::now()-t1);
	if(diff<total)
	{
		return;
	}
	else
	{
		t1 = t1+total;
	}

	// calculate x, y from a vector with known length and angle
	x = length * cos (angle);
	y = length * sin (angle);
	//std::cout<<"x="<<x<<", y="<<y<<std::endl;
	osg::Vec3 pyramidTwoPosition(x,y,0);
	batmanXForm->setPosition( pyramidTwoPosition );
	angle += angle_stepsize;

}
void update_positions(const Tgf::ECP::IdealAircraft& iac)
{
  int current = steady_clock_t::now().time_since_epoch().count();
  std::cout<<std::setprecision(10)<<current-last<<","<<iac.getEntityID()<<", "<<iac.getLatitude().as(Units::deg())<<", "<<iac.getLongitude().as(Units::deg())<<std::endl;
  last=current;
  gmtl::Vec3d vec3d = getDBCoords(Position(iac.getLatitude(), iac.getLongitude(), iac.getAltitude()));
  osg::Vec3 pyramidTwoPosition(vec3d[0],vec3d[1],vec3d[2]);
  batmanXForm->setPosition( pyramidTwoPosition );
}
virtual gmtl::Vec3d getDBCoords(const Position &pos) const 
{
  Tgf::Units::Coordinate result = m_stereoMap->latLonToXYCoordinate(pos);
  return gmtl::Vec3d(result.getXCoordinate(Tgf::Units::m()),
            result.getYCoordinate(Tgf::Units::m()),
            pos.getAltitude().as(Tgf::Units::m()));
}
};


int main()
{
      Tgf::ECP::EcoCommunicator eco(40);
      eco.startComms();
      Updater updater;
      eco.getAircraftSignal().connect(boost::bind(&Updater::update_positions, &updater, _1));
      osgViewer::Viewer viewer;
      viewer.addEventHandler (new osgViewer::StatsHandler ()); 
	viewer.setUpViewInWindow(100,100, 500,500);
	const int ReceivesShadowTraversalMask = 0x1;

	const int CastsShadowTraversalMask = 0x2;

	osg::ref_ptr<osgShadow::ShadowedScene> root = new osgShadow::ShadowedScene;

	root->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	root->setCastsShadowTraversalMask(CastsShadowTraversalMask);

	osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
	root->setShadowTechnique(sm.get());


	//osg::Group* root = new osg::Group();
	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();

	//Associate the pyramid geometry with the pyramid geode
	//   Add the pyramid geode to the root node of the scene graph.

	pyramidGeode->addDrawable(pyramidGeometry);
	root->addChild(pyramidGeode);

	//Declare an array of vertices. Each vertex will be represented by
	//a triple -- an instances of the vec3 class. An instance of
	//osg::Vec3Array can be used to store these triples. Since
	//osg::Vec3Array is derived from the STL vector class, we can use the
	//push_back method to add array elements. Push back adds elements to
	//the end of the vector, thus the index of first element entered is
	//zero, the second entries index is 1, etc.
	//Using a right-handed coordinate system with 'z' up, array
	//elements zero..four below represent the 5 points required to create
	//a simple pyramid.

	osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
	pyramidVertices->push_back( osg::Vec3( 0, 0, 0) ); // front left
	pyramidVertices->push_back( osg::Vec3(10, 0, 0) ); // front right
	pyramidVertices->push_back( osg::Vec3(10,10, 0) ); // back right
	pyramidVertices->push_back( osg::Vec3( 0,10, 0) ); // back left
	pyramidVertices->push_back( osg::Vec3( 5, 5,10) ); // peak

	//Associate this set of vertices with the geometry associated with the
	//geode we added to the scene.

	pyramidGeometry->setVertexArray( pyramidVertices );

	//Next, create a primitive set and add it to the pyramid geometry.
	//Use the first four points of the pyramid to define the base using an
	//instance of the DrawElementsUint class. Again this class is derived
	//from the STL vector, so the push_back method will add elements in
	//sequential order. To ensure proper backface cullling, vertices
	//should be specified in counterclockwise order. The arguments for the
	//constructor are the enumerated type for the primitive
	//(same as the OpenGL primitive enumerated types), and the index in
	//the vertex array to start from.

	osg::DrawElementsUInt* pyramidBase =
		new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	pyramidBase->push_back(3);
	pyramidBase->push_back(2);
	pyramidBase->push_back(1);
	pyramidBase->push_back(0);
	pyramidGeometry->addPrimitiveSet(pyramidBase);

	//Repeat the same for each of the four sides. Again, vertices are
	//specified in counter-clockwise order.

	osg::DrawElementsUInt* pyramidFaceOne =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceOne->push_back(0);
	pyramidFaceOne->push_back(1);
	pyramidFaceOne->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

	osg::DrawElementsUInt* pyramidFaceTwo =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceTwo->push_back(1);
	pyramidFaceTwo->push_back(2);
	pyramidFaceTwo->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

	osg::DrawElementsUInt* pyramidFaceThree =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceThree->push_back(2);
	pyramidFaceThree->push_back(3);
	pyramidFaceThree->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

	osg::DrawElementsUInt* pyramidFaceFour =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceFour->push_back(3);
	pyramidFaceFour->push_back(0);
	pyramidFaceFour->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

	//Declare and load an array of Vec4 elements to store colors.

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 4 red

	//The next step is to associate the array of colors with the geometry,
	//assign the color indices created above to the geometry and set the
	//binding mode to _PER_VERTEX.

	pyramidGeometry->setColorArray(colors);
	pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//Now that we have created a geometry node and added it to the scene
	//we can reuse this geometry. For example, if we wanted to put a
	//second pyramid 15 units to the right of the first one, we could add
	//this geode as the child of a transform node in our scene graph.

	// Use the 'addChild' method of the osg::Group class to
	// add the transform as a child of the root node and the
	// pyramid node as a child of the transform.
	//osg::Node* batmanNode = osgDB::readNodeFile("C:\\Users\\Lonnie\\Desktop\\Marvin The Martian\\Marvin  The Martian.obj");
	osg::Node* batmanNode = osgDB::readNodeFile("Batman/Batman.osg");
	//osg::Node* batmanNode = osgDB::readNodeFile("C:\\Users\\Lonnie\\Desktop\\OpenSceneGraph-Data-3.0.0\\cessna.osg");

	root->addChild(updater.batmanXForm);
	updater.batmanXForm->addChild(batmanNode);

	// Declare and initialize a Vec3 instance to change the
	// position of the model in the scene

	osg::Vec3 batmanPosition(15,0,0);
	updater.batmanXForm->setPosition( batmanPosition );

	// switch off lighting as we haven't assigned any normals.
	root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//The final step is to set up and enter a simulation loop.

	viewer.setSceneData( root );
	//viewer.run();

	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();
	// Init the GraphicsContext Traits. 
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits; 

	// Setup the traits parameters. 
	traits->samples                       = 4; // to make anti-aliased. 
	viewer.addEventHandler(new osgViewer::StatsHandler);

	while( !viewer.done() )
	{
		viewer.frame();
		//update_positions();
	}

	return 0;
}
