#include <gmtl/gmtl.h>
#include <osg/Quat>
#include <osg/Vec3>
#include <osg/Matrix>

osg::Quat makeHPRQuat( double h, double p, double r )
{
  osg::Vec3 z( 0., 0., 1. );
  osg::Quat qHeading(osg::DegreesToRadians(h), z);
  osg::Vec3 x = qHeading * osg::Vec3( 1., 0., 0. );
  osg::Vec3 y = qHeading * osg::Vec3( 0., 1., 0. );
  //Rotate z and y axes by the pitch.
  osg::Quat qPitch( osg::DegreesToRadians(p), x);
  y = qPitch * y;
  z = qPitch * z;
  //  Rotate x and z axes by the roll.
  osg::Quat qRoll( osg::DegreesToRadians( r ), y );
  x = qRoll * x;
  z = qRoll * z;
  // Use x, y, and z axes to create an orientation matrix.
  osg::Matrix m( x[0], x[1], x[2], 0.,
        y[0], y[1], y[2], 0.,
        z[0], z[1], z[2], 0.,
          0.,   0.,   0., 1. );

  osg::Quat quat;
  quat.set(m);
  return(quat);
}

void getEulerFromQuat(osg::Quat q, double& roll, double& pitch, double& yaw)
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



int main(int argc, char *args[])
{
//float radians = gmtl::Math::deg2Rad( degrees );
//float degrees = gmtl::Math::rad2Deg( radians );
gmtl::Vec3d vecrot1;
vecrot1[0]=gmtl::Math::deg2Rad(0.0);
vecrot1[1]=gmtl::Math::deg2Rad(0.0);
vecrot1[2]=gmtl::Math::deg2Rad(0.0);

osg::Quat rot1 = makeHPRQuat(0,0,0);
osg::Quat rot1 = makeHPRQuat(0,0,0);


osg::Quat rot2 = makeHPRQuat(20,20,20);

//osg::Quat result;
//result.slerp(1., rot1, rot2);
gmtl::Vec3d resultVec;
getEulerFromQuat(rot2, resultVec[0], resultVec[1], resultVec[2]);

std::cout<<gmtl::Math::rad2Deg( resultVec[0] )<<std::endl;
std::cout<<gmtl::Math::rad2Deg( resultVec[1] )<<std::endl;
std::cout<<gmtl::Math::rad2Deg( resultVec[2] )<<std::endl;
return 0;
}
