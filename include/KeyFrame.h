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


#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "MapPoint.h"
#include "Thirdparty/DBoW2/DBoW2/BowVector.h"
#include "Thirdparty/DBoW2/DBoW2/FeatureVector.h"
#include "ORBVocabulary.h"
#include "Frame.h"
#include "KeyFrameDatabase.h"
#include "FeatureGrid.h"
#include<boost/thread.hpp>


namespace ORB_SLAM
{

class Map;
class KeyFrameDatabase;
class DatabaseResult;
const uchar DoubleWindowKF= 0x01;// if a keyframe is in double window, then its mbNotErase & DoubleWindowKF ==DoubleWindowKF
const uchar LoopCandidateKF= 0x02;// if a keyframe is a loop candidate, then its mbNotErase & LoopCandidateKF ==LoopCandidateKF
class KeyFrame: public Frame
{
public:
    KeyFrame(Frame &F, Map* pMap, KeyFrameDatabase* pKFDB);
    ~KeyFrame(){Release();}
    bool isKeyFrame() const {return true;}

    void Release();
    vector<MapPoint*> GetMapPointMatches();

    // Pose functions
    void SetPose(const Eigen::Matrix3d &Rcw,const Eigen::Vector3d &tcw);
    void SetPose(const Sophus::SE3d &Tcw_);
    Sophus::SE3d GetPose(bool left=true);
    void EraseMapPointMatch(const size_t &idx);
    Eigen::Vector3d GetCameraCenter();
    void AddMapPoint(MapPoint* pMP, const size_t &idx);
    bool isBad();

    Sophus::SE3d GetPoseInverse();
    Eigen::Matrix3d GetRotation();
    Eigen::Vector3d GetTranslation();

    // Calibration 
    Eigen::Matrix3d GetCalibrationMatrix() const;

    // Bag of Words Representation
    DBoW2::FeatureVector GetFeatureVector();
    DBoW2::BowVector GetBowVector();

    // Covisibility graph functions
    void AddConnection(KeyFrame* pKF, const int &weight);
    void EraseConnection(KeyFrame* pKF);
    void UpdateConnections();
    void UpdateBestCovisibles();
    std::set<KeyFrame *> GetConnectedKeyFrames();
    std::vector<KeyFrame* > GetVectorCovisibleKeyFrames();
    std::vector<KeyFrame*> GetBestCovisibilityKeyFrames(const int &N);
    std::vector<KeyFrame*> GetCovisiblesByWeight(const int &w);
    int GetWeight(KeyFrame* pKF);

    // Spanning tree functions
    void AddChild(KeyFrame* pKF);
    void EraseChild(KeyFrame* pKF);
    void ChangeParent(KeyFrame* pKF);
    std::set<KeyFrame*> GetChilds();
    KeyFrame* GetParent();
    bool hasChild(KeyFrame* pKF);

    // Loop Edges
    void AddLoopEdge(KeyFrame* pKF);
    std::set<KeyFrame*> GetLoopEdges();

    // MapPoint observation functions
    void EraseMapPointMatch(MapPoint* pMP);

    std::set<MapPoint*> GetMapPoints();
    int TrackedMapPoints();
    Map * GetMap(){ return mpMap;}
    MapPoint* GetMapPoint(const size_t &idx);   
    MapPoint* GetFeaturePoint(const size_t &idx);
    // KeyPoint functions
    int GetKeyPointScaleLevel(const size_t &idx) const;

    std::vector<cv::KeyPoint> GetKeyPointsUn() const;
    cv::Mat GetDescriptors(bool left=true);
    std::vector<size_t> GetFeaturesInArea(const float &x, const float  &y, const float  &r) const;

    // Image
    bool IsInImage(const float &x, const float &y) const;

    // Activate/deactivate erasable flags
    void SetNotErase(uchar enableWhichProtection);
    uchar GetNotErase();
    void SetErase(uchar disableWhichProtection);

    // Set/check erased
    void SetBadFlag();

    // Median MapPoint depth
    float ComputeSceneMedianDepth(int q = 2);
    void setExistingFeatures(FeatureGrid &fg);

  
public:
    static long unsigned int nNextKeyId;
    // long unsigned int mnId; //inherit from Frame
    long unsigned int mnFrameId; //keyframe id, 0,1,2 for keyframes

//    double mTimeStamp;//inherit from Frame

//    float mfGridElementWidthInv;//inherit from Frame
//    float mfGridElementHeightInv;//inherit from Frame

    // Variables used by the tracking
    long unsigned int mnTrackReferenceForFrame;
    long unsigned int mnFuseTargetForKF;

    // Variables used by the local mapping
    long unsigned int mnBALocalForKF;
    long unsigned int mnBAFixedForKF;

    // Variables used by the keyframe database
    long unsigned int mnLoopQuery;
    int mnLoopWords;
    float mLoopScore;
    long unsigned int mnRelocQuery;
    int mnRelocWords;
    float mRelocScore;

    FeatureGrid * mpFG; //feature grid to control distribution of newly created features
    // Calibration parameters
//    float fx, fy, cx, cy;//inherit from Frame

    //BoW
//    DBoW2::BowVector mBowVec;//inherit from Frame

    static bool weightComp( int a, int b){
        return a>b;
    }

    static bool lId(KeyFrame* pKF1, KeyFrame* pKF2){
        return pKF1->mnFrameId<pKF2->mnFrameId;
    }

protected:

    // SE3 Pose and camera center
//    cv::Mat mTcw; //transformation to camera from world frame, cvMat float //inherit from Frame
//    cv::Mat mOw; // camera center in world frame, cvMat float //inherit from Frame

    // Original image, undistorted image bounds, and calibration matrix
//    cv::Mat im;//inherit from Frame
//    int mnMinX;//inherit from Frame
//    int mnMinY;//inherit from Frame
//    int mnMaxX;//inherit from Frame
//    int mnMaxY;//inherit from Frame
//    cv::Mat mK;//inherit from Frame

    // KeyPoints, Descriptors, MapPoints vectors (all associated by an index)
//    std::vector<cv::KeyPoint> mvKeys;//inherit from Frame
//    std::vector<cv::KeyPoint> mvKeysUn;
//    cv::Mat mDescriptors;
//    std::vector<MapPoint*> mvpMapPoints;

//    std::vector<cv::KeyPoint> mvRightKeys;
//    std::vector<cv::KeyPoint> mvRightKeysUn;
//    cv::Mat mRightDescriptors;

    // BoW
    KeyFrameDatabase* mpKeyFrameDB;
//    ORBVocabulary* mpORBvocabulary;//inherit from Frame
//    DBoW2::FeatureVector mFeatVec;//inherit from Frame


    // Grid over the image to speed up feature matching
//    std::vector< std::vector <std::vector<size_t> > > mGrid; //inherit from Frame

    std::map<KeyFrame*,int> mConnectedKeyFrameWeights;
    std::vector<KeyFrame*> mvpOrderedConnectedKeyFrames;
    std::vector<int> mvOrderedWeights;

    // Spanning Tree and Loop Edges
    bool mbFirstConnection;
    KeyFrame* mpParent;
    std::set<KeyFrame*> mspChildrens;
    std::set<KeyFrame*> mspLoopEdges;

    // Erase flags
    uchar mbNotErase;
    bool mbToBeErased;

    Map* mpMap;
    boost::mutex mMutexPose;
    boost::mutex mMutexConnections;
    boost::mutex mMutexFeatures; // exclusive access to mvpMapPoints and fts_->point

private:
    KeyFrame();
    KeyFrame(const KeyFrame&);
    KeyFrame& operator= (const KeyFrame&);
};
Eigen::Matrix3d ComputeF12(KeyFrame *&pKF1, KeyFrame *&pKF2);

} //namespace ORB_SLAM

#endif // KEYFRAME_H
