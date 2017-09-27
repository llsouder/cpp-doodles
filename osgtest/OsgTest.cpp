#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osg/Texture2D>
#include <osgDB/ReadFile> 
#include <osg/ShapeDrawable> 
#include <osg/animationpath>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <osgDB/ReadFile> 
#include <Windows.h>

#include "TimeManager.h"

typedef boost::chrono::steady_clock steady_clock_t;
typedef boost::chrono::steady_clock::time_point timepoint_t;
typedef boost::chrono::steady_clock::duration duration_t;

timepoint_t t1 = steady_clock_t::now();
boost::chrono::nanoseconds total(15000000); //15 milliseconds
float x, y;
int length = 50;
float angle = 0.0;
float angle_stepsize = 0.025;

void addPyrimid(osg::Group* root)
{
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

	// Declare and initialize a transform node.
	osg::PositionAttitudeTransform* pyramidTwoXForm =
		new osg::PositionAttitudeTransform();

}

void makeGround(osg::Group* root)
{
	const int CastsShadowTraversalMask = 0x2;

    osg::Geode* WallGeode = new osg::Geode();
    osg::Geometry* WallGeometry = new osg::Geometry();

    WallGeode->addDrawable(WallGeometry);
    //add the Node to the root Node
    root->addChild(WallGeode);
    //specify vertices
    osg::Vec3Array* WallVertices = new osg::Vec3Array;
    WallVertices->push_back( osg::Vec3(-250, -250, 0) ); // front left
    WallVertices->push_back( osg::Vec3(250, -250, 0) ); // front right
    WallVertices->push_back( osg::Vec3(250, 250, 0) ); // back right
    WallVertices->push_back( osg::Vec3(-250, 250, 0) ); // back left 
    WallGeometry->setVertexArray( WallVertices );

    //specify the kind of geometry we want to draw here
    osg::DrawElementsUInt* WallBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    //specify the order we want to draw the base of our geometry
    WallBase->push_back(3);
    WallBase->push_back(2);
    WallBase->push_back(1);
    WallBase->push_back(0);
    WallGeometry->addPrimitiveSet(WallBase);

    //Declare and load an array of Vec4 elements to store colors.
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 0 red
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 2 blue
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 3 white
    //colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 4 red

    WallGeometry->setColorArray(colors);
    WallGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    // Declare and initialize a transform node.
    osg::PositionAttitudeTransform* WallTwoXForm = new osg::PositionAttitudeTransform();
	WallGeode->setNodeMask(WallGeode->getNodeMask() & ~CastsShadowTraversalMask);

    // Use the 'addChild' method of the osg::Group class to
    // add the transform as a child of the root node and the
    // pyramid node as a child of the transform.
    root->addChild(WallTwoXForm);
}
void addSun(osg::Group* root) 
{
	osg::ref_ptr<osg::Group> lightGroup (new osg::Group);
	osg::ref_ptr<osg::StateSet> lightSS (root->getOrCreateStateSet());
	osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource;
	osg::ref_ptr<osg::LightSource> lightSource2 = new osg::LightSource;

	// create a local light.
	float xCenter = 100;
	float yCenter = 100;
	float zCenter = 500;


	osg::Vec4f lightPosition (osg::Vec4f(xCenter, yCenter, zCenter, 0.0f));
	osg::ref_ptr<osg::Light> myLight = new osg::Light;
	myLight->setLightNum(1);
	myLight->setPosition(lightPosition);
	myLight->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight->setSpecular(osg::Vec4(1,1,1,1));
	myLight->setConstantAttenuation(1.0f);
	myLight->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	lightSource1->setLight(myLight.get());

	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON); 
	lightSource1->setStateSetModes(*lightSS,osg::StateAttribute::ON);
	//osg::StateSet* lightSS (lightGroup->getOrCreateStateSet());

	lightGroup->addChild(lightSource1.get());

	//Light markers: small spheres
	osg::ref_ptr<osg::Geode> lightMarkerGeode (new osg::Geode);
	lightMarkerGeode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(xCenter,yCenter,zCenter),10.0f)));


	//Tuto 9: lighting code
	root->addChild(lightGroup.get());
	//Tuto 9: Adding the light marker geode
	root->addChild(lightMarkerGeode.get());
}
osg::ref_ptr<osg::AnimationPathCallback> makeRotateCallback(osg::Vec3 rot_axis, float time_in_sec, float total_degrees, osg::Vec3 stationary_pos)
{
	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
	path->setLoopMode( osg::AnimationPath::LOOP );
	//osg::AnimationPath::ControlPoint pointA(osg::Vec3(-1,0,0));
	//osg::AnimationPath::ControlPoint pointB(osg::Vec3( 1,0,0));
	unsigned int slices = 32;
	float delta_rot = total_degrees/ (float)slices-1.0f;
	float delta_time = time_in_sec/(float)slices;
	for(unsigned int i=0; i<slices; i++)
	{
		osg::AnimationPath::ControlPoint pointA(stationary_pos, osg::Quat(delta_rot*(float)i, rot_axis) );
		path->insert( delta_time*(float)i, pointA );
	}
	
	osg::ref_ptr<osg::AnimationPathCallback> cb = new osg::AnimationPathCallback( path );
	return cb;
}
void update_positions(osg::PositionAttitudeTransform* pyramidTwoXForm)
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
	pyramidTwoXForm->setPosition( pyramidTwoPosition );
	angle += angle_stepsize;

}

osgViewer::Viewer viewer;
osg::PositionAttitudeTransform* batmanXForm;

void viewer_loop()
{
	std::cout<<"viewer loop start"<<std::endl;
	while( !viewer.done() )
	{
		viewer.frame();
		//update_positions(batmanXForm);
	}
	std::cout<<"viewer loop done"<<std::endl;

}

int main()
{

	viewer.setUpViewInWindow(100,100, 500,500);
	const int ReceivesShadowTraversalMask = 0x1;

	const int CastsShadowTraversalMask = 0x2;

	osg::ref_ptr<osgShadow::ShadowedScene> root = new osgShadow::ShadowedScene;

	root->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	root->setCastsShadowTraversalMask(CastsShadowTraversalMask);

	osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
	root->setShadowTechnique(sm.get());

	int mapres = 1024;
	sm->setTextureSize(osg::Vec2s(mapres,mapres));

	//osg::Group* root = new osg::Group();

	//addPyrimid(root);
	makeGround(root);

	//osg::Node* batmanNode = osgDB::readNodeFile("C:\\Users\\Lonnie\\Desktop\\Marvin The Martian\\Marvin  The Martian.obj");
	osg::Node* batmanNode = osgDB::readNodeFile("E:\\Users\\Lonnie\\Desktop\\Batman\\Converted.osg");
	//osg::Node* batmanNode = osgDB::readNodeFile("E:\\Users\\Lonnie\\Desktop\\LockheedC130Hercules\\C130-Hercules.obj");
	//osg::Node* batmanNode = osgDB::readNodeFile("E:\\Users\\Lonnie\\Desktop\\Hyrule_Castle_Twilight_Princess_2.0\\models\\untitiled.dae");
	//osg::Node* batmanNode = osgDB::readNodeFile("E:\\Users\\Lonnie\\Desktop\\Batman\\gary.obj");
	//osg::Node* batmanNode = osgDB::readNodeFile("C:\\Users\\Lonnie\\Desktop\\OpenSceneGraph-Data-3.0.0\\cessna.osg");
	{
		batmanXForm = new osg::PositionAttitudeTransform();
		root->addChild(batmanXForm);
		batmanXForm->addChild(batmanNode);

		// Declare and initialize a Vec3 instance to change the
		// position of the model in the scene
		//osg::Quat rotation(osg::DegreesToRadians(90.0), osg::Vec3(0,0,1));
		//batmanXForm->setAttitude(rotation);
		osg::Vec3 batmanPosition(15,0,0);
		batmanXForm->setPosition( batmanPosition );

		//osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
		//path->setLoopMode( osg::AnimationPath::LOOP );
		//osg::AnimationPath::ControlPoint pointA(osg::Vec3(-1,0,0));
		//osg::AnimationPath::ControlPoint pointB(osg::Vec3( 1,0,0));
		//osg::AnimationPath::ControlPoint pointA(osg::Vec3(0,0,0), osg::Quat(0, osg::Vec3(0,0,1)) );
		//osg::AnimationPath::ControlPoint pointB(osg::Vec3(0,0,0), osg::Quat((osg::PI*2.0)-1.0f, osg::Vec3(0,0,1)));
		//path->insert( 0.0f, pointA );
		//path->insert( 5.0f, pointB );
		//osg::ref_ptr<osg::AnimationPathCallback> cb = new osg::AnimationPathCallback( path );

		osg::ref_ptr<osg::AnimationPathCallback> cb =makeRotateCallback(osg::Vec3(0,0,1), 10.0f, 2.0f*osg::PI, batmanXForm->getPosition());
		batmanXForm->setUpdateCallback( cb );
		//update_positions(pyramidTwoXForm);
	}


	// switch off lighting as we haven't assigned any normals.
	//root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	addSun(root);

	//The final step is to set up and enter a simulation loop.

	viewer.setSceneData( root );
	//viewer.run();
	viewer.realize();

	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.addEventHandler(new osgViewer::StatsHandler);
	// Init the GraphicsContext Traits. 
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits; 

	// Setup the traits parameters. 
	traits->samples                       = 4; // to make anti-aliased. 

	std::cout<<"main loop waiting..."<<std::endl;
	TimeManager tm;
	tm.startTime();
	viewer_loop();
	tm.stopTime();
	std::cin.ignore();
	return 0;
}
