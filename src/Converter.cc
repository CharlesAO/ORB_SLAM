/**
* This file is part of ORB-SLAM.
*
* Copyright (C) 2014 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <http://webdiis.unizar.es/~raulmur/orbslam/>
*
* ORB-SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM. If not, see <http://www.gnu.org/licenses/>.
*/


#include "Converter.h"
//#include "ros/ros.h"

namespace ORB_SLAM
{

std::vector<cv::Mat> Converter::toDescriptorVector(const cv::Mat &Descriptors)
{
    std::vector<cv::Mat> vDesc;
    vDesc.reserve(Descriptors.rows);
    for (int j=0;j<Descriptors.rows;j++)
        vDesc.push_back(Descriptors.row(j));

    return vDesc;
}

g2o::SE3Quat Converter::toSE3Quat(const cv::Mat &cvT)
{
    Eigen::Matrix<double,3,3> R;
    R << cvT.at<float>(0,0), cvT.at<float>(0,1), cvT.at<float>(0,2),
         cvT.at<float>(1,0), cvT.at<float>(1,1), cvT.at<float>(1,2),
         cvT.at<float>(2,0), cvT.at<float>(2,1), cvT.at<float>(2,2);

    Eigen::Matrix<double,3,1> t(cvT.at<float>(0,3), cvT.at<float>(1,3), cvT.at<float>(2,3));

    return g2o::SE3Quat(R,t);
}
g2o::SE3Quat Converter::toSE3Quat(const Sophus::SE3d &se3d)
{
   return g2o::SE3Quat(se3d.unit_quaternion(),se3d.translation());
}
Sophus::SE3d Converter::toSE3d(const g2o::SE3Quat &se3q)
{
   return Sophus::SE3d(se3q.rotation(),se3q.translation());
}

Sophus::Sim3d Converter::toSim3d(const g2o::Sim3 &se3q)
{
   return Sophus::Sim3d(Sophus::RxSO3d(se3q.scale(), Sophus::SO3d(se3q.rotation())),se3q.translation());
}

Eigen::Vector4d Converter::toScaleTrans(const g2o::Sim3 & se3q)
{
    Eigen::Vector4d v4;
    v4[0]= se3q.scale();
    v4.tail<3>()= se3q.translation();
    return v4;
}

Sophus::Sim3d Converter::toSim3d(const Sophus::SE3d &se3)
{
   return Sophus::Sim3d(Sophus::RxSO3d(1.0, Sophus::SO3d(se3.unit_quaternion())),se3.translation());
}
Sophus::SE3d Converter::toSE3d(const Sophus::Sim3d &sim3)
{
   return Sophus::SE3d(sim3.rotationMatrix(), sim3.translation()/sim3.scale());
}

Sophus::SE3d Converter::toSE3d(const cv::Mat &cvT)
{
    Eigen::Matrix<double,3,3> R;
    Eigen::Matrix<double,3,1> t;

    if(cvT.depth()==CV_32F)
    {
        R << cvT.at<float>(0,0), cvT.at<float>(0,1), cvT.at<float>(0,2),
                cvT.at<float>(1,0), cvT.at<float>(1,1), cvT.at<float>(1,2),
                cvT.at<float>(2,0), cvT.at<float>(2,1), cvT.at<float>(2,2);

        t << cvT.at<float>(0,3), cvT.at<float>(1,3), cvT.at<float>(2,3);
    }else{
        R << cvT.at<double>(0,0), cvT.at<double>(0,1), cvT.at<double>(0,2),
             cvT.at<double>(1,0), cvT.at<double>(1,1), cvT.at<double>(1,2),
             cvT.at<double>(2,0), cvT.at<double>(2,1), cvT.at<double>(2,2);

        t << cvT.at<double>(0,3), cvT.at<double>(1,3), cvT.at<double>(2,3);
    }

    return Sophus::SE3d(R,t);
}

cv::Mat Converter::toCvMat(const g2o::SE3Quat &SE3)
{
    Eigen::Matrix<double,4,4> eigMat = SE3.to_homogeneous_matrix();
    return toCvMat(eigMat);
}

cv::Mat Converter::toCvMat(const g2o::Sim3 &Sim3)
{
    Eigen::Matrix3d eigR = Sim3.rotation().toRotationMatrix();
    Eigen::Vector3d eigt = Sim3.translation();
    double s = Sim3.scale();
    return toCvSE3(s*eigR,eigt);
}

cv::Mat Converter::toCvMat(const Eigen::Matrix<double,4,4> &m)
{
    cv::Mat cvMat(4,4,CV_32F);
    for(int i=0;i<4;i++)
        for(int j=0; j<4; j++)
            cvMat.at<float>(i,j)=m(i,j);

    return cvMat.clone();
}
Sophus::SE3d Converter::toSE3d(const libviso2::Matrix& Tr)
{
    Eigen::Matrix<double,3,3> R;
    R << Tr.val[0][0], Tr.val[0][1],Tr.val[0][2],
         Tr.val[1][0], Tr.val[1][1],Tr.val[1][2],
         Tr.val[2][0], Tr.val[2][1],Tr.val[2][2];

    Eigen::Matrix<double,3,1> t(Tr.val[0][3], Tr.val[1][3],Tr.val[2][3]);

    return Sophus::SE3d(R,t);
}

libviso2::Matrix Converter::toViso2Matrix(const Sophus::SE3d & se3)
{
    libviso2::Matrix Tr(4,4);
    Eigen::Matrix<double,3,4> Rt= se3.matrix3x4();

    Tr.val[0][0] = Rt(0,0); Tr.val[0][1] = Rt(0,1); Tr.val[0][2] = Rt(0,2); Tr.val[0][3] = Rt(0,3);
    Tr.val[1][0] = Rt(1,0); Tr.val[1][1] = Rt(1,1); Tr.val[1][2] = Rt(1,2); Tr.val[1][3] = Rt(1,3);
    Tr.val[2][0] = Rt(2,0); Tr.val[2][1] = Rt(2,1); Tr.val[2][2] = Rt(2,2); Tr.val[2][3] = Rt(2,3);
    Tr.val[3][0] = 0;       Tr.val[3][1] = 0;       Tr.val[3][2] = 0;       Tr.val[3][3] = 1;
    return Tr;
}

cv::Mat Converter::toCvMat(const Eigen::Matrix3d &m)
{
    cv::Mat cvMat(3,3,CV_32F);
    for(int i=0;i<3;i++)
        for(int j=0; j<3; j++)
            cvMat.at<float>(i,j)=m(i,j);

    return cvMat.clone();
}

cv::Mat Converter::toCvMat(const Eigen::Matrix<double,3,1> &m)
{
    cv::Mat cvMat(3,1,CV_32F);
    for(int i=0;i<3;i++)
            cvMat.at<float>(i)=m(i);

    return cvMat.clone();
}

cv::Mat Converter::toCvSE3(const Eigen::Matrix<double,3,3> &R, const Eigen::Matrix<double,3,1> &t)
{
    cv::Mat cvMat = cv::Mat::eye(4,4,CV_32F);
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            cvMat.at<float>(i,j)=R(i,j);
        }
    }
    for(int i=0;i<3;i++)
    {
        cvMat.at<float>(i,3)=t(i);
    }

    return cvMat.clone();
}

cv::Mat Converter::toCvSE3(const Sophus::SE3d &T )
{
    return toCvSE3(T.rotationMatrix(), T.translation());
}

Eigen::Matrix<double,3,1> Converter::toVector3d(const cv::Mat &cvVector)
{
    Eigen::Matrix<double,3,1> v;
    v << cvVector.at<float>(0), cvVector.at<float>(1), cvVector.at<float>(2);

    return v;
}

Eigen::Matrix<double,3,1> Converter::toVector3d(const cv::Point3f &cvPoint)
{
    Eigen::Matrix<double,3,1> v;
    v << cvPoint.x, cvPoint.y, cvPoint.z;

    return v;
}

Eigen::Matrix<double,2,1> Converter::toVector2d(const cv::Mat &cvVector)
{
    assert((cvVector.rows==2 && cvVector.cols==1) || (cvVector.cols==2 && cvVector.rows==1));

    Eigen::Matrix<double,2,1> v;
    v << cvVector.at<float>(0), cvVector.at<float>(1);

    return v;
}

Eigen::Matrix<double,3,3> Converter::toMatrix3d(const cv::Mat &cvMat3)
{
    Eigen::Matrix<double,3,3> M;

    M << cvMat3.at<float>(0,0), cvMat3.at<float>(0,1), cvMat3.at<float>(0,2),
         cvMat3.at<float>(1,0), cvMat3.at<float>(1,1), cvMat3.at<float>(1,2),
         cvMat3.at<float>(2,0), cvMat3.at<float>(2,1), cvMat3.at<float>(2,2);

    return M;
}

std::vector<float> Converter::toQuaternion(const cv::Mat &M)
{
    Eigen::Matrix<double,3,3> eigMat = toMatrix3d(M);
    Eigen::Quaterniond q(eigMat);

    std::vector<float> v(4);
    v[0] = q.x();
    v[1] = q.y();
    v[2] = q.z();
    v[3] = q.w();

    return v;
}
cv::Mat Converter::SkewSymmetricMatrix(const cv::Mat &v)
{
    return (cv::Mat_<float>(3,3) <<             0, -v.at<float>(2), v.at<float>(1),
                                  v.at<float>(2),               0,-v.at<float>(0),
                                 -v.at<float>(1),  v.at<float>(0),              0);
}
} //namespace ORB_SLAM
